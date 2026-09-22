#ifndef UNIT_TESTING
#include "input.h"
#include "rtos_objects.h"
#include "driver/gpio.h"

#define ENC_CLK_PIN GPIO_NUM_25
#define ENC_DT_PIN  GPIO_NUM_26
#define ENC_SW_PIN  GPIO_NUM_27

void inputTask(void *pvParameters) {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << ENC_CLK_PIN) | (1ULL << ENC_DT_PIN) | (1ULL << ENC_SW_PIN);
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    int lastClk = gpio_get_level(ENC_CLK_PIN);

    while (1) {
        EventBits_t bits = xEventGroupGetBits(g_systemEvents);
        if (bits & EVENT_ACTIVE) {
            int currentClk = gpio_get_level(ENC_CLK_PIN);
            if (currentClk != lastClk && currentClk == 0) { // Falling edge
                NavDirection dir = (gpio_get_level(ENC_DT_PIN) != currentClk) 
                                   ? NavDirection::NEXT 
                                   : NavDirection::PREVIOUS;
                xQueueSend(navQueue, &dir, 0);
            }
            lastClk = currentClk;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
#endif