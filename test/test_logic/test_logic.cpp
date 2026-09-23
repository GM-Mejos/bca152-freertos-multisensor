#include <unity.h>
#include "alarm.h"
#include "input.h"
#include "system_state.h"

void setUp(void) {}
void tearDown(void) {}

// --- Temperature Alarm Logic Tests (5 required) ---
void test_temp_below_lower_threshold(void) {
    TEST_ASSERT_TRUE(evaluateTemperature(17.9f) == AlarmState::LOW_TEMPERATURE);
}

void test_temp_exactly_lower_threshold(void) {
    TEST_ASSERT_TRUE(evaluateTemperature(18.0f) == AlarmState::NORMAL);
}

void test_temp_normal_value(void) {
    TEST_ASSERT_TRUE(evaluateTemperature(24.0f) == AlarmState::NORMAL);
}

void test_temp_exactly_upper_threshold(void) {
    TEST_ASSERT_TRUE(evaluateTemperature(30.0f) == AlarmState::NORMAL);
}

void test_temp_above_upper_threshold(void) {
    TEST_ASSERT_TRUE(evaluateTemperature(30.1f) == AlarmState::HIGH_TEMPERATURE);
}

// --- Display Navigation Logic Tests (4 required) ---
void test_display_forward_transition(void) {
    TEST_ASSERT_TRUE(getNextDisplayMode(DisplayMode::TEMPERATURE) == DisplayMode::HUMIDITY);
}

void test_display_reverse_transition(void) {
    TEST_ASSERT_TRUE(getPreviousDisplayMode(DisplayMode::LIGHT) == DisplayMode::HUMIDITY);
}

void test_display_forward_wraparound(void) {
    TEST_ASSERT_TRUE(getNextDisplayMode(DisplayMode::MOTION) == DisplayMode::TEMPERATURE);
}

void test_display_reverse_wraparound(void) {
    TEST_ASSERT_TRUE(getPreviousDisplayMode(DisplayMode::TEMPERATURE) == DisplayMode::MOTION);
}

// --- System State Logic Tests (4 required) ---
void test_state_active_no_timeout(void) {
    TEST_ASSERT_TRUE(evaluateSystemState(SystemState::ACTIVE, false, 5000, 15000) == SystemState::ACTIVE);
}

void test_state_active_timeout_reached(void) {
    TEST_ASSERT_TRUE(evaluateSystemState(SystemState::ACTIVE, false, 15000, 15000) == SystemState::INACTIVE);
}

void test_state_inactive_no_motion(void) {
    TEST_ASSERT_TRUE(evaluateSystemState(SystemState::INACTIVE, false, 20000, 15000) == SystemState::INACTIVE);
}

void test_state_inactive_motion_restores_active(void) {
    TEST_ASSERT_TRUE(evaluateSystemState(SystemState::INACTIVE, true, 20000, 15000) == SystemState::ACTIVE);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    
    // Temperature Alarm Tests (5)
    RUN_TEST(test_temp_below_lower_threshold);
    RUN_TEST(test_temp_exactly_lower_threshold);
    RUN_TEST(test_temp_normal_value);
    RUN_TEST(test_temp_exactly_upper_threshold);
    RUN_TEST(test_temp_above_upper_threshold);

    // Display Navigation Tests (4)
    RUN_TEST(test_display_forward_transition);
    RUN_TEST(test_display_reverse_transition);
    RUN_TEST(test_display_forward_wraparound);
    RUN_TEST(test_display_reverse_wraparound);

    // System State Tests (4)
    RUN_TEST(test_state_active_no_timeout);
    RUN_TEST(test_state_active_timeout_reached);
    RUN_TEST(test_state_inactive_no_motion);
    RUN_TEST(test_state_inactive_motion_restores_active);

    return UNITY_END();
}