#ifndef INPUT_H
#define INPUT_H

#define ENCODER_CLK 18
#define ENCODER_DT  19
#define ENCODER_SW  5

// Rotary encoder navigation event directions
enum class NavDirection {
    NEXT,
    PREVIOUS,
    CW = 0,
    CCW = 1,
    UP = 0,
    DOWN = 1
};

#ifndef DISPLAY_MODE_DEFINED
#define DISPLAY_MODE_DEFINED
enum class DisplayMode {
    TEMPERATURE,
    HUMIDITY,
    LIGHT,
    MOTION
};
#endif

// Pure testable navigation logic
inline DisplayMode getNextDisplayMode(DisplayMode current) {
    switch (current) {
        case DisplayMode::TEMPERATURE: return DisplayMode::HUMIDITY;
        case DisplayMode::HUMIDITY:    return DisplayMode::LIGHT;
        case DisplayMode::LIGHT:       return DisplayMode::MOTION;
        case DisplayMode::MOTION:      return DisplayMode::TEMPERATURE;
        default:                       return DisplayMode::TEMPERATURE;
    }
}

inline DisplayMode getPreviousDisplayMode(DisplayMode current) {
    switch (current) {
        case DisplayMode::TEMPERATURE: return DisplayMode::MOTION;
        case DisplayMode::HUMIDITY:    return DisplayMode::TEMPERATURE;
        case DisplayMode::LIGHT:       return DisplayMode::HUMIDITY;
        case DisplayMode::MOTION:      return DisplayMode::LIGHT;
        default:                       return DisplayMode::TEMPERATURE;
    }
}

// Aliases to satisfy both nextDisplayMode and getNextDisplayMode callers
inline DisplayMode nextDisplayMode(DisplayMode current) {
    return getNextDisplayMode(current);
}

inline DisplayMode previousDisplayMode(DisplayMode current) {
    return getPreviousDisplayMode(current);
}

extern DisplayMode g_currentDisplayMode;

void inputInit(void);
void inputTask(void *pvParameters);

#endif // INPUT_H