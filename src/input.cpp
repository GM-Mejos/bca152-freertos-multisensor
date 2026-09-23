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
#include "rtos_objects.h"

void vInputTask(void *pvParameters) {
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
#endif