#ifndef INPUT_H
#define INPUT_H

#ifndef UNIT_TESTING
#include "driver/gpio.h"
// Pin assignments matching diagram.json
#define ENCODER_CLK GPIO_NUM_25
#define ENCODER_DT  GPIO_NUM_26
#define ENCODER_SW  GPIO_NUM_27
#endif

enum class NavDirection : int {
    PREVIOUS = -1,
    NEXT = 1
};

enum class DisplayMode {
    TEMPERATURE,
    HUMIDITY,
    LIGHT,
    MOTION
};

DisplayMode getNextDisplayMode(DisplayMode current);
DisplayMode getPreviousDisplayMode(DisplayMode current);

#ifndef UNIT_TESTING
void vInputTask(void *pvParameters);
#endif

#endif // INPUT_H