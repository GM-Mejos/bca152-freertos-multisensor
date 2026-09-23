#ifndef SENSORS_H
#define SENSORS_H

#include <stdbool.h>

struct SensorData {
    float temperature;
    float humidity;
    int lightLevel;
    bool motionDetected;
};

#ifndef UNIT_TESTING
void vSensorTask(void *pvParameters);
#endif

#endif // SENSORS_H