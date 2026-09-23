#include "alarm.h"

AlarmState evaluateTemperature(float temp) {
    if (temp < 18.0f) {
        return AlarmState::LOW_TEMPERATURE;
    } else if (temp > 30.0f) {
        return AlarmState::HIGH_TEMPERATURE;
    }
    return AlarmState::NORMAL;
}

#ifndef UNIT_TESTING
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "rtos_objects.h"

#define BUZZER_PIN GPIO_NUM_25
static const char *TAG = "ALARM_TASK";

void vAlarmTask(void *pvParameters) {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << BUZZER_PIN);
    gpio_config(&io_conf);
    gpio_set_level(BUZZER_PIN, 0);

    SensorData data = { 24.0f, 50.0f, 0, false };

    while (1) {
        if (xQueueReceive(alarmQueue, &data, portMAX_DELAY) == pdTRUE) {
            AlarmState state = evaluateTemperature(data.temperature);
            if (state != AlarmState::NORMAL) {
                xEventGroupSetBits(g_systemEvents, EVENT_ALARM);
                // Beep pattern for alarm condition
                gpio_set_level(BUZZER_PIN, 1);
                vTaskDelay(pdMS_TO_TICKS(100));
                gpio_set_level(BUZZER_PIN, 0);
            } else {
                xEventGroupClearBits(g_systemEvents, EVENT_ALARM);
                gpio_set_level(BUZZER_PIN, 0);
            }
        }
    }
}

void alarm_task(void *pvParameters) {
    vAlarmTask(pvParameters);
}
#endif