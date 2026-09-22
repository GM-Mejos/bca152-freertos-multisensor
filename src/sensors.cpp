#ifndef UNIT_TESTING
#include "sensors.h"
#include "rtos_objects.h"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "hal/adc_types.h"
#include "esp_timer.h"
#include "esp_rom_sys.h"
#include "esp_log.h"

#define DHT_PIN GPIO_NUM_4
static const char *TAG = "SENSOR_TASK";

static adc_oneshot_unit_handle_t s_adc1_handle = NULL;

static void init_adc(void) {
    adc_oneshot_unit_init_cfg_t init_config1 = {};
    init_config1.unit_id = ADC_UNIT_1;
    init_config1.ulp_mode = ADC_ULP_MODE_DISABLE;
    adc_oneshot_new_unit(&init_config1, &s_adc1_handle);

    adc_oneshot_chan_cfg_t config = {};
    config.atten = ADC_ATTEN_DB_12;
    config.bitwidth = ADC_BITWIDTH_DEFAULT;
    adc_oneshot_config_channel(s_adc1_handle, ADC_CHANNEL_6, &config);
}

bool read_dht22(float *temp, float *humidity) {
    uint8_t data[5] = {0};

    // Host trigger sequence
    gpio_set_direction(DHT_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(DHT_PIN, 0);
    esp_rom_delay_us(20000); // 20ms low pulse
    gpio_set_level(DHT_PIN, 1);
    esp_rom_delay_us(30);

    // Await sensor response
    gpio_set_direction(DHT_PIN, GPIO_MODE_INPUT);

    int timeout = 10000;
    while (gpio_get_level(DHT_PIN) == 1) { if (--timeout == 0) return false; }
    timeout = 10000;
    while (gpio_get_level(DHT_PIN) == 0) { if (--timeout == 0) return false; }
    timeout = 10000;
    while (gpio_get_level(DHT_PIN) == 1) { if (--timeout == 0) return false; }

    // Read 40 data bits
    for (int i = 0; i < 40; i++) {
        timeout = 10000;
        while (gpio_get_level(DHT_PIN) == 0) { if (--timeout == 0) return false; }
        int64_t start = esp_timer_get_time();
        timeout = 10000;
        while (gpio_get_level(DHT_PIN) == 1) { if (--timeout == 0) return false; }
        int64_t dur = esp_timer_get_time() - start;
        if (dur > 40) {
            data[i / 8] |= (1 << (7 - (i % 8)));
        }
    }

    // Checksum verification
    if (data[4] != ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
        return false;
    }

    *humidity = ((data[0] << 8) | data[1]) * 0.1f;
    int16_t rawT = ((data[2] & 0x7F) << 8) | data[3];
    if (data[2] & 0x80) rawT = -rawT;
    *temp = rawT * 0.1f;
    return true;
}

void vSensorTask(void *pvParameters) {
    gpio_set_direction(DHT_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(DHT_PIN, GPIO_PULLUP_ONLY);

    init_adc();

    TickType_t lastWakeTime = xTaskGetTickCount();
    SensorData data = { 24.0f, 50.0f, 0, false };

    while (1) {
        // Periodic execution with zero drift (Section 22)
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(2000));

        float t, h;
        if (read_dht22(&t, &h)) {
            data.temperature = t;
            data.humidity = h;
        }

        // Ambient light acquisition (0-100%) via ESP-IDF v5 oneshot ADC
        int rawLdr = 2048;
        if (s_adc1_handle != NULL) {
            adc_oneshot_read(s_adc1_handle, ADC_CHANNEL_6, &rawLdr);
        }
        data.lightLevel = (int)((4095 - rawLdr) * 100 / 4095);

        EventBits_t bits = xEventGroupGetBits(g_systemEvents);
        data.motionDetected = (bits & EVENT_MOTION) != 0;

        if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI(TAG, "Temp: %.1f C | Hum: %.1f %% | Light: %d %%",
                     data.temperature, data.humidity, data.lightLevel);
            xSemaphoreGive(serialMutex);
        }

        // Broadcast to queues
        xQueueOverwrite(displayQueue, &data);
        xQueueOverwrite(alarmQueue, &data);
    }
}
#endif