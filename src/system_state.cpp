#include "system_state.h"

SystemState g_systemState = SystemState::ACTIVE;

SystemState evaluateSystemState(SystemState currentState, bool motionDetected, uint32_t elapsedInactive, uint32_t timeoutLimit) {
    if (currentState == SystemState::ACTIVE) {
        if (motionDetected) {
            return SystemState::ACTIVE;
        }
        if (elapsedInactive >= timeoutLimit) {
            return SystemState::INACTIVE;
        }
        return SystemState::ACTIVE;
    } else { // INACTIVE
        if (motionDetected) {
            return SystemState::ACTIVE;
        }
        return SystemState::INACTIVE;
    }
}

#ifndef UNIT_TESTING
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rtos_objects.h"

void vStateTask(void *pvParameters) {
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
#endif