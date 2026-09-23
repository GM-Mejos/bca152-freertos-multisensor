#include "rtos_objects.h"

#ifndef UNIT_TESTING
#include <stdio.h>
#include "esp_log.h"

static const char *TAG = "RTOS_OBJ";

QueueHandle_t displayQueue = NULL;
QueueHandle_t alarmQueue = NULL;
QueueHandle_t navQueue = NULL;
QueueHandle_t g_telemetryQueue = NULL;
QueueHandle_t g_navQueue = NULL;

EventGroupHandle_t g_systemEvents = NULL;
SemaphoreHandle_t serialMutex = NULL;
SemaphoreHandle_t g_i2cMutex = NULL;

void init_rtos_objects(void) {
    // Queues: display and alarm receive latest SensorData (length 1 for overwrite)
    displayQueue = xQueueCreate(1, sizeof(SensorData));
    alarmQueue = xQueueCreate(1, sizeof(SensorData));
    navQueue = xQueueCreate(10, sizeof(NavDirection));

    g_telemetryQueue = displayQueue;
    g_navQueue = navQueue;

    g_systemEvents = xEventGroupCreate();
    serialMutex = xSemaphoreCreateMutex();
    g_i2cMutex = serialMutex;

    if (g_systemEvents != NULL) {
        xEventGroupSetBits(g_systemEvents, EVENT_ACTIVE);
    }

    ESP_LOGI(TAG, "RTOS Queues, Mutexes, and Event Groups initialized successfully.");
}

void initRtosObjects(void) {
    init_rtos_objects();
}
#endif