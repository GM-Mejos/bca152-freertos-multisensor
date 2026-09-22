#include "alarm.h"

// Pure hardware-independent decision logic (Part VIII, Section 30)
AlarmState evaluateTemperature(float temp) {
    if (temp < TEMP_THRESHOLD_LOW) {
        return AlarmState::LOW_TEMP;
    } else if (temp > TEMP_THRESHOLD_HIGH) {
        return AlarmState::HIGH_TEMP;
    }
    return AlarmState::NORMAL;
}

#ifndef UNIT_TESTING
#include "rtos_objects.h"
#include "sensors.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define BUZZER_PIN GPIO_NUM_14

static const char *TAG = "ALARM_TASK";

void vAlarmTask(void *pvParameters) {
    // Configure buzzer output pin
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << BUZZER_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
    gpio_set_level(BUZZER_PIN, 0);

    SensorData data;
    while (1) {
        // Block until new sensor reading arrives via queue (Part V & VIII)
        if (xQueueReceive(alarmQueue, &data, portMAX_DELAY) == pdTRUE) {
            EventBits_t bits = xEventGroupGetBits(g_systemEvents);
            bool isActive = (bits & EVENT_ACTIVE) != 0;
            AlarmState state = evaluateTemperature(data.temperature);

            if (isActive && state != AlarmState::NORMAL) {
                gpio_set_level(BUZZER_PIN, 1);
                xEventGroupSetBits(g_systemEvents, EVENT_ALARM);
                if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE) {
                    ESP_LOGW(TAG, "[ALARM ACTIVE] Temp: %.2f C is outside normal limits!", data.temperature);
                    xSemaphoreGive(serialMutex);
                }
            } else {
                gpio_set_level(BUZZER_PIN, 0);
                xEventGroupClearBits(g_systemEvents, EVENT_ALARM);
            }
        }
    }
}
#endif