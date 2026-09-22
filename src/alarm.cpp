#include "alarm.h"
#include "rtos_objects.h"
#include "sensors.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

// Override buzzer pin to match diagram.json GPIO 14
#ifdef BUZZER_PIN
#undef BUZZER_PIN
#endif
#define BUZZER_PIN GPIO_NUM_14

static const char *TAG = "ALARM_TASK";

AlarmState evaluateTemperature(float temp) {
    if (temp < TEMP_LOW_LIMIT) {
        return AlarmState::LOW_TEMPERATURE;
    } else if (temp > TEMP_HIGH_LIMIT) {
        return AlarmState::HIGH_TEMPERATURE;
    }
    return AlarmState::NORMAL;
}

#ifndef UNIT_TESTING
void vAlarmTask(void *pvParameters) {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << BUZZER_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
    gpio_set_level(BUZZER_PIN, 0);

    SensorData data = { 24.0f, 50.0f, 0, false };
    bool alarmActive = false;

    while (1) {
        if (xQueueReceive(alarmQueue, &data, pdMS_TO_TICKS(50)) == pdTRUE) {
            AlarmState state = evaluateTemperature(data.temperature);
            alarmActive = (state != AlarmState::NORMAL);

            if (alarmActive) {
                xEventGroupSetBits(g_systemEvents, EVENT_ALARM);
                if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE) {
                    ESP_LOGW(TAG, "ALARM ACTIVE! Temp: %.1f C (Buzzer ON)", data.temperature);
                    xSemaphoreGive(serialMutex);
                }
            } else {
                xEventGroupClearBits(g_systemEvents, EVENT_ALARM);
                gpio_set_level(BUZZER_PIN, 0);
            }
        }

        if (alarmActive) {
            gpio_set_level(BUZZER_PIN, 1);
            vTaskDelay(pdMS_TO_TICKS(2));
            gpio_set_level(BUZZER_PIN, 0);
            vTaskDelay(pdMS_TO_TICKS(2));
        } else {
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}

void alarmTask(void *pvParameters) {
    vAlarmTask(pvParameters);
}

void alarmInit(void) {
    // Initialized within task
}
#endif