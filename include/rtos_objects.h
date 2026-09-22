#ifndef RTOS_OBJECTS_H
#define RTOS_OBJECTS_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "sensors.h" // SensorData is defined here

// Event Group Bits
#define EVENT_ACTIVE (1 << 0)
#define EVENT_MOTION (1 << 1)
#define EVENT_ALARM  (1 << 2)

#define EVENT_ACTIVE_BIT EVENT_ACTIVE
#define EVENT_MOTION_BIT EVENT_MOTION
#define EVENT_ALARM_BIT  EVENT_ALARM

// Global RTOS Primitive Handles
extern QueueHandle_t displayQueue;
extern QueueHandle_t alarmQueue;
extern QueueHandle_t navQueue;
extern EventGroupHandle_t g_systemEvents;
extern SemaphoreHandle_t serialMutex;

#define g_sensorAlarmQueue   alarmQueue
#define g_sensorDisplayQueue displayQueue
#define g_serialMutex        serialMutex

void init_rtos_objects(void);
void safePrint(const char *format, ...);

#endif // RTOS_OBJECTS_H