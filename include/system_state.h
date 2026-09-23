#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

#include <stdint.h>

enum class SystemState {
    ACTIVE,
    INACTIVE
};

constexpr uint32_t INACTIVITY_TIMEOUT_SECONDS = 15;

SystemState evaluateSystemState(SystemState currentState, bool motionDetected, uint32_t elapsedInactiveSeconds);

#ifndef UNIT_TESTING
void vStateTask(void *pvParameters);
#endif

#endif // SYSTEM_STATE_H