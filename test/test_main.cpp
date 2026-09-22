#include <unity.h>
#include "alarm.h"
#include "system_state.h"

void setUp(void) {}
void tearDown(void) {}

// =============================================================================
// CATEGORY 1: Temperature Alarm Decision Logic (Section 43: 5 Required Tests)
// =============================================================================

void test_temp_below_lower_threshold(void) {
    // Below 18.0 °C must trigger LOW_TEMP
    TEST_ASSERT_EQUAL(AlarmState::LOW_TEMP, evaluateTemperature(17.9f));
}

void test_temp_exactly_lower_threshold(void) {
    // Exactly 18.0 °C is valid normal operation
    TEST_ASSERT_EQUAL(AlarmState::NORMAL, evaluateTemperature(18.0f));
}

void test_temp_normal_range(void) {
    // Normal ambient room temperature
    TEST_ASSERT_EQUAL(AlarmState::NORMAL, evaluateTemperature(24.5f));
}

void test_temp_exactly_upper_threshold(void) {
    // Exactly 30.0 °C is valid normal operation
    TEST_ASSERT_EQUAL(AlarmState::NORMAL, evaluateTemperature(30.0f));
}

void test_temp_above_upper_threshold(void) {
    // Above 30.0 °C must trigger HIGH_TEMP
    TEST_ASSERT_EQUAL(AlarmState::HIGH_TEMP, evaluateTemperature(30.1f));
}

// =============================================================================
// CATEGORY 2: Display Navigation Logic (Section 43: 4 Required Tests)
// =============================================================================

void test_nav_clockwise_step(void) {
    // Sequential clockwise navigation
    TEST_ASSERT_EQUAL(DisplayMode::HUMIDITY, getNextDisplayMode(DisplayMode::TEMPERATURE));
}

void test_nav_clockwise_wraparound(void) {
    // Wraparound from end back to beginning
    TEST_ASSERT_EQUAL(DisplayMode::TEMPERATURE, getNextDisplayMode(DisplayMode::MOTION));
}

void test_nav_counterclockwise_step(void) {
    // Sequential reverse navigation
    TEST_ASSERT_EQUAL(DisplayMode::HUMIDITY, getPreviousDisplayMode(DisplayMode::LIGHT));
}

void test_nav_counterclockwise_wraparound(void) {
    // Wraparound from beginning back to end
    TEST_ASSERT_EQUAL(DisplayMode::MOTION, getPreviousDisplayMode(DisplayMode::TEMPERATURE));
}

// =============================================================================
// CATEGORY 3: System State Machine Logic (Section 43: 4 Required Tests)
// =============================================================================

void test_state_active_no_timeout(void) {
    // System remains ACTIVE while within timeout period
    TEST_ASSERT_EQUAL(SystemState::ACTIVE, 
                      evaluateSystemState(SystemState::ACTIVE, false, 5000, 15000));
}

void test_state_active_timeout_reached(void) {
    // System transitions to INACTIVE when inactivity duration hits threshold
    TEST_ASSERT_EQUAL(SystemState::INACTIVE, 
                      evaluateSystemState(SystemState::ACTIVE, false, 15000, 15000));
}

void test_state_inactive_no_motion(void) {
    // System remains INACTIVE if no motion is observed
    TEST_ASSERT_EQUAL(SystemState::INACTIVE, 
                      evaluateSystemState(SystemState::INACTIVE, false, 20000, 15000));
}

void test_state_inactive_motion_restores_active(void) {
    // Any motion immediately restores system to ACTIVE
    TEST_ASSERT_EQUAL(SystemState::ACTIVE, 
                      evaluateSystemState(SystemState::INACTIVE, true, 20000, 15000));
}

// =============================================================================
// Test Suite Entry Point
// =============================================================================
int main(int argc, char **argv) {
    UNITY_BEGIN();

    // Category 1: Alarm Logic Tests
    RUN_TEST(test_temp_below_lower_threshold);
    RUN_TEST(test_temp_exactly_lower_threshold);
    RUN_TEST(test_temp_normal_range);
    RUN_TEST(test_temp_exactly_upper_threshold);
    RUN_TEST(test_temp_above_upper_threshold);

    // Category 2: Navigation Tests
    RUN_TEST(test_nav_clockwise_step);
    RUN_TEST(test_nav_clockwise_wraparound);
    RUN_TEST(test_nav_counterclockwise_step);
    RUN_TEST(test_nav_counterclockwise_wraparound);

    // Category 3: System State Machine Tests
    RUN_TEST(test_state_active_no_timeout);
    RUN_TEST(test_state_active_timeout_reached);
    RUN_TEST(test_state_inactive_no_motion);
    RUN_TEST(test_state_inactive_motion_restores_active);

    return UNITY_END();
}