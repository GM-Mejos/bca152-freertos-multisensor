#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "rtos_objects.h"
#include "sensors.h"
#include "display.h"
#include "input.h"
#include "motion.h"
#include "alarm.h"

static const char *TAG = "APP_MAIN";

extern "C" void app_main(void) {
    init_rtos_objects();

    if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE) {
        ESP_LOGI(TAG, "=========================================");
        ESP_LOGI(TAG, "   BCA152 FreeRTOS Multisensor System    ");
        ESP_LOGI(TAG, "=========================================");
        xSemaphoreGive(serialMutex);
    }

    // Task Priorities: Motion=3, Input=3, Sensor=2, Alarm=2, Display=1
    xTaskCreate(vMotionTask,  "MotionTask",  3072, NULL, 3, NULL);
    xTaskCreate(inputTask,    "InputTask",   2048, NULL, 3, NULL);
    xTaskCreate(vSensorTask,  "SensorTask",  3072, NULL, 2, NULL);
    xTaskCreate(vAlarmTask,   "AlarmTask",   2048, NULL, 2, NULL);
    xTaskCreate(vDisplayTask, "DisplayTask", 3072, NULL, 1, NULL);
}