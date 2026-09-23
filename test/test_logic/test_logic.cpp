#include <unity.h>
#include "alarm.h"
#include "input.h"
#include "system_state.h"

void setUp(void) {}
void tearDown(void) {}

// Temperature Alarm Tests
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

// Display Navigation Tests
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

// System State Machine Tests (Section 43)
void test_state_active_no_timeout(void) {
    TEST_ASSERT_EQUAL(SystemState::ACTIVE, evaluateSystemState(SystemState::ACTIVE, false, 5));
}

void test_state_active_timeout_reached(void) {
    TEST_ASSERT_EQUAL(SystemState::INACTIVE, evaluateSystemState(SystemState::ACTIVE, false, 15));
}

void test_state_inactive_no_motion(void) {
    TEST_ASSERT_EQUAL(SystemState::INACTIVE, evaluateSystemState(SystemState::INACTIVE, false, 20));
}

void test_state_inactive_motion_restores_active(void) {
    TEST_ASSERT_EQUAL(SystemState::ACTIVE, evaluateSystemState(SystemState::INACTIVE, true, 20));
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_temperature_normal);
    RUN_TEST(test_temperature_low);
    RUN_TEST(test_temperature_high);
    RUN_TEST(test_display_navigation_cycle);
    RUN_TEST(test_display_navigation_reverse);
    RUN_TEST(test_state_active_no_timeout);
    RUN_TEST(test_state_active_timeout_reached);
    RUN_TEST(test_state_inactive_no_motion);
    RUN_TEST(test_state_inactive_motion_restores_active);
    return UNITY_END();
}

#ifndef UNIT_TESTING
extern "C" void app_main(void) {
    main(0, NULL);
}
#endif