#include "alarm.h"

AlarmState evaluateTemperature(float temp) {
    if (temp < 18.0f) {
        return AlarmState::LOW_TEMPERATURE;
    } else if (temp > 30.0f) {
        return AlarmState::HIGH_TEMPERATURE;
    }
    return AlarmState::NORMAL;
}

#ifndef UNIT_TESTING
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "rtos_objects.h"

void vAlarmTask(void *pvParameters) {
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
#endif