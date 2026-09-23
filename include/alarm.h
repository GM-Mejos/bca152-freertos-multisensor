#ifndef ALARM_H
#define ALARM_H

enum class AlarmState {
    NORMAL,
    LOW_TEMPERATURE,
    HIGH_TEMPERATURE
};

constexpr float TEMP_THRESHOLD_LOW = 18.0f;
constexpr float TEMP_THRESHOLD_HIGH = 30.0f;

AlarmState evaluateTemperature(float temp);

#ifndef UNIT_TESTING
void vAlarmTask(void *pvParameters);
#endif

#endif // ALARM_H