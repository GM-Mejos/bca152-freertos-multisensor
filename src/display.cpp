#ifndef UNIT_TESTING
#include "display.h"
#include "rtos_objects.h"
#include "sensors.h"
#include "input.h"
#include "system_state.h"
#include "driver/i2c.h"
#include <string.h>
#include <stdio.h>

#define I2C_SDA_PIN GPIO_NUM_21
#define I2C_SCL_PIN GPIO_NUM_22
#define SSD1306_ADDR 0x3C

static const uint8_t font5x7[][5] = {
    {0x00,0x00,0x00,0x00,0x00}, // Space
    {0x00,0x00,0x5F,0x00,0x00}, // !
    {0x00,0x07,0x00,0x07,0x00}, // "
    {0x14,0x7F,0x14,0x7F,0x14}, // #
    {0x24,0x2A,0x7F,0x2A,0x12}, // $
    {0x23,0x13,0x08,0x64,0x62}, // %
    {0x36,0x49,0x55,0x22,0x50}, // &
    {0x00,0x05,0x03,0x00,0x00}, // '
    {0x00,0x1C,0x22,0x41,0x00}, // (
    {0x00,0x41,0x22,0x1C,0x00}, // )
    {0x08,0x2A,0x1C,0x2A,0x08}, // *
    {0x08,0x08,0x3E,0x08,0x08}, // +
    {0x00,0x50,0x30,0x00,0x00}, // ,
    {0x08,0x08,0x08,0x08,0x08}, // -
    {0x00,0x60,0x60,0x00,0x00}, // .
    {0x20,0x10,0x08,0x04,0x02}, // /
    {0x3E,0x51,0x49,0x45,0x3E}, // 0
    {0x00,0x42,0x7F,0x40,0x00}, // 1
    {0x42,0x61,0x51,0x49,0x46}, // 2
    {0x21,0x41,0x45,0x4B,0x31}, // 3
    {0x18,0x14,0x12,0x7F,0x10}, // 4
    {0x27,0x45,0x45,0x45,0x39}, // 5
    {0x3C,0x4A,0x49,0x49,0x30}, // 6
    {0x01,0x71,0x09,0x05,0x03}, // 7
    {0x36,0x49,0x49,0x49,0x36}, // 8
    {0x06,0x49,0x49,0x29,0x1E}, // 9
    {0x00,0x36,0x36,0x00,0x00}, // :
    {0x00,0x56,0x36,0x00,0x00}, // ;
    {0x00,0x08,0x14,0x22,0x41}, // <
    {0x14,0x14,0x14,0x14,0x14}, // =
    {0x41,0x22,0x14,0x08,0x00}, // >
    {0x02,0x01,0x51,0x09,0x06}, // ?
    {0x32,0x49,0x79,0x41,0x3E}, // @
    {0x7E,0x11,0x11,0x11,0x7E}, // A
    {0x7F,0x49,0x49,0x49,0x36}, // B
    {0x3E,0x41,0x41,0x41,0x22}, // C
    {0x7F,0x41,0x41,0x22,0x1C}, // D
    {0x7F,0x49,0x49,0x49,0x41}, // E
    {0x7F,0x09,0x09,0x01,0x01}, // F
    {0x3E,0x41,0x41,0x51,0x32}, // G
    {0x7F,0x08,0x08,0x08,0x7F}, // H
    {0x00,0x41,0x7F,0x41,0x00}, // I
    {0x20,0x40,0x41,0x3F,0x01}, // J
    {0x7F,0x08,0x14,0x22,0x41}, // K
    {0x7F,0x40,0x40,0x40,0x40}, // L
    {0x7F,0x02,0x04,0x02,0x7F}, // M
    {0x7F,0x04,0x08,0x10,0x7F}, // N
    {0x3E,0x41,0x41,0x41,0x3E}, // O
    {0x7F,0x09,0x09,0x01,0x01}, // P
    {0x3E,0x41,0x51,0x21,0x5E}, // Q
    {0x7F,0x09,0x19,0x29,0x46}, // R
    {0x46,0x49,0x49,0x49,0x31}, // S
    {0x01,0x01,0x7F,0x01,0x01}, // T
    {0x3F,0x40,0x40,0x40,0x3F}, // U
    {0x1F,0x20,0x40,0x20,0x1F}, // V
    {0x7F,0x20,0x18,0x20,0x7F}, // W
    {0x63,0x14,0x08,0x14,0x63}, // X
    {0x03,0x04,0x78,0x04,0x03}, // Y
    {0x61,0x51,0x49,0x45,0x43}  // Z
};

static uint8_t s_buffer[1024];

static void oledCmd(uint8_t cmd) {
    i2c_cmd_handle_t link = i2c_cmd_link_create();
    i2c_master_start(link);
    i2c_master_write_byte(link, (SSD1306_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(link, 0x00, true);
    i2c_master_write_byte(link, cmd, true);
    i2c_master_stop(link);
    i2c_master_cmd_begin(I2C_NUM_0, link, pdMS_TO_TICKS(10));
    i2c_cmd_link_delete(link);
}

static void drawChar(int x, int page, char c) {
    if (c >= 'a' && c <= 'z') c = c - 'a' + 'A';
    if (c < 32 || c > 90) return;
    int idx = c - 32;
    for (int i = 0; i < 5; i++) {
        if (x + i < 128) s_buffer[page * 128 + x + i] = font5x7[idx][i];
    }
}

static void drawText(int x, int page, const char *str) {
    while (*str) {
        drawChar(x, page, *str);
        x += 6;
        str++;
    }
}

static void flushScreen(void) {
    for (uint8_t p = 0; p < 8; p++) {
        oledCmd(0xB0 + p);
        oledCmd(0x00);
        oledCmd(0x10);
        i2c_cmd_handle_t link = i2c_cmd_link_create();
        i2c_master_start(link);
        i2c_master_write_byte(link, (SSD1306_ADDR << 1) | I2C_MASTER_WRITE, true);
        i2c_master_write_byte(link, 0x40, true);
        i2c_master_write(link, &s_buffer[p * 128], 128, true);
        i2c_master_stop(link);
        i2c_master_cmd_begin(I2C_NUM_0, link, pdMS_TO_TICKS(10));
        i2c_cmd_link_delete(link);
    }
}

void vDisplayTask(void *pvParameters) {
    i2c_config_t conf = {};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_SDA_PIN;
    conf.scl_io_num = I2C_SCL_PIN;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 400000;
    i2c_param_config(I2C_NUM_0, &conf);
    i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0);

    oledCmd(0xAE);
    oledCmd(0x20); oledCmd(0x00);
    oledCmd(0x8D); oledCmd(0x14);
    oledCmd(0xAF);

    DisplayMode currentMode = DisplayMode::TEMPERATURE;
    SensorData data = { 24.0f, 50.0f, 0, false };
    NavDirection dir;
    char strBuf[32];

    while (1) {
        while (xQueueReceive(navQueue, &dir, 0) == pdTRUE) {
            currentMode = (dir == NavDirection::NEXT) 
                          ? getNextDisplayMode(currentMode) 
                          : getPreviousDisplayMode(currentMode);
        }

        xQueueReceive(displayQueue, &data, 0);

        EventBits_t bits = xEventGroupGetBits(g_systemEvents);
        bool isActive = (bits & EVENT_ACTIVE) != 0;

        memset(s_buffer, 0, sizeof(s_buffer));

        if (!isActive) {
            drawText(35, 3, "SLEEP MODE");
        } else {
            drawText(16, 0, "ROOM MONITOR");
            drawText(0, 1, "---------------------");

            switch (currentMode) {
                case DisplayMode::TEMPERATURE:
                    drawText(0, 3, "MODE: TEMPERATURE");
                    snprintf(strBuf, sizeof(strBuf), "TEMP: %.1f C", data.temperature);
                    drawText(0, 5, strBuf);
                    break;
                case DisplayMode::HUMIDITY:
                    drawText(0, 3, "MODE: HUMIDITY");
                    snprintf(strBuf, sizeof(strBuf), "HUM : %.1f %%", data.humidity);
                    drawText(0, 5, strBuf);
                    break;
                case DisplayMode::LIGHT:
                    drawText(0, 3, "MODE: LIGHT");
                    snprintf(strBuf, sizeof(strBuf), "LDR : %d %%", data.lightLevel);
                    drawText(0, 5, strBuf);
                    break;
                case DisplayMode::MOTION:
                    drawText(0, 3, "MODE: MOTION");
                    snprintf(strBuf, sizeof(strBuf), "PIR : %s", data.motionDetected ? "DETECTED" : "CLEAR");
                    drawText(0, 5, strBuf);
                    break;
            }
        }

        flushScreen();
        vTaskDelay(pdMS_TO_TICKS(150));
    }
}
#endif