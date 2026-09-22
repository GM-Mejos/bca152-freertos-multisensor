#include "system_state.h"
#include <stdint.h>
#include <stdbool.h>

SystemState g_systemState = SystemState::ACTIVE;

// Pure testable decision logic (compiled for Native Tests and ESP32)
SystemState evaluateSystemState(SystemState currentState, bool motionDetected, uint32_t elapsedTimeMs, uint32_t timeoutMs) {
    if (currentState == SystemState::ACTIVE) {
        if (motionDetected) {
            return SystemState::ACTIVE;
        }
        if (elapsedTimeMs >= timeoutMs) {
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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rtos_objects.h"

void systemStateInit(void) {
    // Initialization if needed
}

void systemStateTask(void *pvParameters) {
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

#endif // UNIT_TESTING