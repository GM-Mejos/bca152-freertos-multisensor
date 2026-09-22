#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

#include <stdint.h>
#include <stdbool.h>

enum class SystemState {
    ACTIVE,
    INACTIVE
};

extern SystemState g_systemState; // Export system state variable

SystemState evaluateSystemState(SystemState currentState, bool motionDetected, uint32_t elapsedTimeMs, uint32_t timeoutMs = 10000);

void systemStateInit(void);
void systemStateTask(void *pvParameters);

#endif // SYSTEM_STATE_H