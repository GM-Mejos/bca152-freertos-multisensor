#include "system_state.h"

SystemState evaluateSystemState(SystemState currentState, bool motionDetected, uint32_t elapsedInactiveSeconds) {
    if (currentState == SystemState::ACTIVE) {
        if (motionDetected) {
            return SystemState::ACTIVE;
        }
        if (elapsedInactiveSeconds >= INACTIVITY_TIMEOUT_SECONDS) {
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