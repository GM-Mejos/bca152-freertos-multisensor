#ifndef RTOS_OBJECTS_H
#define RTOS_OBJECTS_H

#include <stdint.h>
#include <stdbool.h>
#include "sensors.h"
#include "input.h"

#ifndef UNIT_TESTING
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#endif

// System Event Group bits
#define EVENT_ACTIVE              (1 << 0)
#define EVENT_MOTION              (1 << 1)
#define EVENT_ALARM               (1 << 2)

#define EVENT_SYSTEM_ACTIVE_BIT   EVENT_ACTIVE
#define EVENT_MOTION_TRIGGER_BIT  EVENT_MOTION
#define EVENT_ALARM_ACTIVE_BIT    EVENT_ALARM

#ifndef UNIT_TESTING
// Queues
extern QueueHandle_t displayQueue;
extern QueueHandle_t alarmQueue;
extern QueueHandle_t navQueue;
extern QueueHandle_t g_telemetryQueue;
extern QueueHandle_t g_navQueue;

// Synchronization primitives
extern EventGroupHandle_t g_systemEvents;
extern SemaphoreHandle_t serialMutex;
extern SemaphoreHandle_t g_i2cMutex;

void init_rtos_objects(void);
void initRtosObjects(void);
#endif

#endif // RTOS_OBJECTS_H