#include "alarm.h"

AlarmState evaluateTemperature(float temp) {
    if (temp < TEMP_THRESHOLD_LOW) {
        return AlarmState::LOW_TEMPERATURE;
    } else if (temp > TEMP_THRESHOLD_HIGH) {
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
#include "esp_rom_sys.h"
#include "rtos_objects.h"

// Buzzer is on GPIO 14 in diagram.json
#define BUZZER_PIN GPIO_NUM_14
static const char *TAG = "ALARM_TASK";

void vAlarmTask(void *pvParameters) {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << BUZZER_PIN);
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config(&io_conf);
    gpio_set_level(BUZZER_PIN, 0);

    SensorData data = { 24.0f, 50.0f, 0, false };

    while (1) {
        if (xQueueReceive(alarmQueue, &data, portMAX_DELAY) == pdTRUE) {
            AlarmState state = evaluateTemperature(data.temperature);
            if (state != AlarmState::NORMAL) {
                xEventGroupSetBits(g_systemEvents, EVENT_ALARM);

                if (xSemaphoreTake(serialMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
                    if (state == AlarmState::HIGH_TEMPERATURE) {
                        ESP_LOGW(TAG, "ALARM ACTIVE: HIGH TEMP (%.1f C > %.1f C)!", data.temperature, TEMP_THRESHOLD_HIGH);
                    } else {
                        ESP_LOGW(TAG, "ALARM ACTIVE: LOW TEMP (%.1f C < %.1f C)!", data.temperature, TEMP_THRESHOLD_LOW);
                    }
                    xSemaphoreGive(serialMutex);
                }

                // 1 kHz square-wave oscillation so Wokwi piezo buzzer produces sound
                for (int i = 0; i < 150; i++) {
                    gpio_set_level(BUZZER_PIN, 1);
                    esp_rom_delay_us(500);
                    gpio_set_level(BUZZER_PIN, 0);
                    esp_rom_delay_us(500);
                }
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