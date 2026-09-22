#ifndef UNIT_TESTING
#include "rtos_objects.h"
#include "sensors.h"
#include "input.h"

QueueHandle_t displayQueue = NULL;
QueueHandle_t alarmQueue = NULL;
QueueHandle_t navQueue = NULL;
EventGroupHandle_t g_systemEvents = NULL;
SemaphoreHandle_t serialMutex = NULL;

void init_rtos_objects(void) {
    // Separate queues for DisplayTask and AlarmTask to prevent destructive read competition
    displayQueue = xQueueCreate(1, sizeof(SensorData));
    alarmQueue   = xQueueCreate(1, sizeof(SensorData));

    // Queue for rotary encoder navigation events
    navQueue     = xQueueCreate(5, sizeof(NavDirection));

    // Mutex to serialize UART terminal output and prevent race conditions
    serialMutex  = xSemaphoreCreateMutex();

    // Event group for state synchronization
    g_systemEvents = xEventGroupCreate();
    xEventGroupSetBits(g_systemEvents, EVENT_ACTIVE);
}
#endif