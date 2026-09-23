#include <unity.h>
#include "alarm.h"
#include "input.h"

void setUp(void) {}
void tearDown(void) {}

void test_temperature_normal(void) {
    TEST_ASSERT_EQUAL(AlarmState::NORMAL, evaluateTemperature(24.0f));
    TEST_ASSERT_EQUAL(AlarmState::NORMAL, evaluateTemperature(18.0f));
    TEST_ASSERT_EQUAL(AlarmState::NORMAL, evaluateTemperature(30.0f));
}

void test_temperature_low(void) {
    TEST_ASSERT_EQUAL(AlarmState::LOW_TEMPERATURE, evaluateTemperature(17.9f));
    TEST_ASSERT_EQUAL(AlarmState::LOW_TEMPERATURE, evaluateTemperature(0.0f));
    TEST_ASSERT_EQUAL(AlarmState::LOW_TEMPERATURE, evaluateTemperature(-5.0f));
}

void test_temperature_high(void) {
    TEST_ASSERT_EQUAL(AlarmState::HIGH_TEMPERATURE, evaluateTemperature(30.1f));
    TEST_ASSERT_EQUAL(AlarmState::HIGH_TEMPERATURE, evaluateTemperature(45.0f));
}

void test_display_navigation_cycle(void) {
    DisplayMode mode = DisplayMode::TEMPERATURE;
    mode = getNextDisplayMode(mode);
    TEST_ASSERT_EQUAL(DisplayMode::HUMIDITY, mode);
    mode = getNextDisplayMode(mode);
    TEST_ASSERT_EQUAL(DisplayMode::LIGHT, mode);
    mode = getNextDisplayMode(mode);
    TEST_ASSERT_EQUAL(DisplayMode::MOTION, mode);
    mode = getNextDisplayMode(mode);
    TEST_ASSERT_EQUAL(DisplayMode::TEMPERATURE, mode);
}

void test_display_navigation_reverse(void) {
    DisplayMode mode = DisplayMode::TEMPERATURE;
    mode = getPreviousDisplayMode(mode);
    TEST_ASSERT_EQUAL(DisplayMode::MOTION, mode);
    mode = getPreviousDisplayMode(mode);
    TEST_ASSERT_EQUAL(DisplayMode::LIGHT, mode);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_temperature_normal);
    RUN_TEST(test_temperature_low);
    RUN_TEST(test_temperature_high);
    RUN_TEST(test_display_navigation_cycle);
    RUN_TEST(test_display_navigation_reverse);
    return UNITY_END();
}

#ifndef UNIT_TESTING
extern "C" void app_main(void) {
    main(0, NULL);
}
#endif