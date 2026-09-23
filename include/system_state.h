#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

#include <stdint.h>
#include <stdbool.h>

enum class SystemState {
    ACTIVE,
    INACTIVE
};

constexpr uint32_t INACTIVITY_TIMEOUT_SECONDS = 15;

extern SystemState g_systemState;

SystemState evaluateSystemState(SystemState currentState, bool motionDetected, uint32_t elapsedInactive, uint32_t timeoutLimit = 15);

#ifndef UNIT_TESTING
void vStateTask(void *pvParameters);
#endif

#endif // SYSTEM_STATE_H