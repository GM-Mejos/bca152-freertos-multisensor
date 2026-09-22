#ifndef SENSORS_H
#define SENSORS_H

#ifndef UNIT_TESTING
#include "driver/gpio.h"
#define DHT_PIN GPIO_NUM_4
#endif

struct SensorData {
    float temperature;
    float humidity;
    int lightLevel;
    bool motionDetected;
};

#ifndef UNIT_TESTING
bool read_dht22(float *temp, float *humidity);
void vSensorTask(void *pvParameters);
#endif

#endif // SENSORS_H