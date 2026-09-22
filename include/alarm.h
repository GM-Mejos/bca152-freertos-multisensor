#ifndef ALARM_H
#define ALARM_H

#define TEMP_LOW_LIMIT  18.0f
#define TEMP_HIGH_LIMIT 30.0f
#define BUZZER_PIN      25

enum class AlarmState {
    NORMAL,
    LOW_TEMPERATURE,
    HIGH_TEMPERATURE
};

// Pure testable decision logic (Section 30)
AlarmState evaluateTemperature(float temperature);

void alarmInit(void);
void alarmTask(void *pvParameters);
void vAlarmTask(void *pvParameters);

#endif // ALARM_H