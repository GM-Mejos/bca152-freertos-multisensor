#include <cstdio>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" void app_main(void) {
    std::printf("\n=========================================\n");
    std::printf("  BCA152 FreeRTOS Multisensor Initialized\n");
    std::printf("=========================================\n");
    std::fflush(stdout);

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}