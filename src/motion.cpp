#ifndef UNIT_TESTING
#include "motion.h"
#include "rtos_objects.h"
#include "system_state.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define PIR_PIN GPIO_NUM_13
static const char *TAG = "MOTION_TASK";

void vMotionTask(void *pvParameters) {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << PIR_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    gpio_config(&io_conf);

    uint32_t inactiveElapsedMs = 0;
    const uint32_t checkIntervalMs = 200;
    const uint32_t timeoutLimitMs = 15000; // 15-second inactivity timeout

    while (1) {
        int pirLevel = gpio_get_level(PIR_PIN);
        bool motion = (pirLevel == 1);

        if (motion) {
            xEventGroupSetBits(g_systemEvents, EVENT_MOTION);
            inactiveElapsedMs = 0;
        } else {
            xEventGroupClearBits(g_systemEvents, EVENT_MOTION);
            inactiveElapsedMs += checkIntervalMs;
        }

        SystemState nextState = evaluateSystemState((SystemState)g_systemState, motion, inactiveElapsedMs, timeoutLimitMs);
        if (nextState != g_systemState) {
            g_systemState = nextState;
            if (g_systemState == SystemState::ACTIVE) {
                xEventGroupSetBits(g_systemEvents, EVENT_ACTIVE);
                if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE) {
                    ESP_LOGI(TAG, "State Transition: [ACTIVE] - Motion detected");
                    xSemaphoreGive(serialMutex);
                }
            } else {
                xEventGroupClearBits(g_systemEvents, EVENT_ACTIVE);
                if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE) {
                    ESP_LOGI(TAG, "State Transition: [INACTIVE] - Timeout reached");
                    xSemaphoreGive(serialMutex);
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(checkIntervalMs));
    }
}
#endif