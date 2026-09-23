#include "input.h"

DisplayMode getNextDisplayMode(DisplayMode current) {
    switch (current) {
        case DisplayMode::TEMPERATURE: return DisplayMode::HUMIDITY;
        case DisplayMode::HUMIDITY:    return DisplayMode::LIGHT;
        case DisplayMode::LIGHT:       return DisplayMode::MOTION;
        case DisplayMode::MOTION:      return DisplayMode::TEMPERATURE;
        default:                       return DisplayMode::TEMPERATURE;
    }
}

DisplayMode getPreviousDisplayMode(DisplayMode current) {
    switch (current) {
        case DisplayMode::TEMPERATURE: return DisplayMode::MOTION;
        case DisplayMode::HUMIDITY:    return DisplayMode::TEMPERATURE;
        case DisplayMode::LIGHT:       return DisplayMode::HUMIDITY;
        case DisplayMode::MOTION:      return DisplayMode::LIGHT;
        default:                       return DisplayMode::TEMPERATURE;
    }
}

#ifndef UNIT_TESTING
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "rtos_objects.h"

static const char *TAG = "INPUT_TASK";

void vInputTask(void *pvParameters) {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << ENCODER_CLK) | (1ULL << ENCODER_DT) | (1ULL << ENCODER_SW);
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config(&io_conf);

    int lastClk = gpio_get_level(ENCODER_CLK);
    int lastSw = 1;

    while (1) {
        int currentClk = gpio_get_level(ENCODER_CLK);
        if (currentClk != lastClk && currentClk == 0) { // Falling edge on CLK
            int dtVal = gpio_get_level(ENCODER_DT);
            NavDirection dir = (dtVal != 0) ? NavDirection::NEXT : NavDirection::PREVIOUS;

            if (navQueue != NULL) {
                xQueueSend(navQueue, &dir, 0);
            }

            // Turning the encoder restores active state if timed out
            if (g_systemEvents != NULL) {
                xEventGroupSetBits(g_systemEvents, EVENT_ACTIVE);
            }

            if (xSemaphoreTake(serialMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
                ESP_LOGI(TAG, "Rotary Encoder Turned: %s", (dir == NavDirection::NEXT) ? "NEXT" : "PREVIOUS");
                xSemaphoreGive(serialMutex);
            }
        }
        lastClk = currentClk;

        int currentSw = gpio_get_level(ENCODER_SW);
        if (currentSw == 0 && lastSw == 1) { // Button pressed
            if (g_systemEvents != NULL) {
                xEventGroupSetBits(g_systemEvents, EVENT_ACTIVE);
            }
            if (xSemaphoreTake(serialMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
                ESP_LOGI(TAG, "Encoder Button Pressed -> System Active");
                xSemaphoreGive(serialMutex);
            }
        }
        lastSw = currentSw;

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
#endif