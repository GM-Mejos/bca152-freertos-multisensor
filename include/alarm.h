#ifndef ALARM_H
#define ALARM_H

enum class AlarmState {
    NORMAL,
    LOW_TEMP,
    HIGH_TEMP
};

constexpr float TEMP_THRESHOLD_LOW = 18.0f;
constexpr float TEMP_THRESHOLD_HIGH = 30.0f;

// Hardware-independent decision logic (Part VIII, Section 30)
AlarmState evaluateTemperature(float temp);

#ifndef UNIT_TESTING
void vAlarmTask(void *pvParameters);
#endif

#endif // ALARM_H