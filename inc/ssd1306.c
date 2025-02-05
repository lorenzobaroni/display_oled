#include "ssd1306.h"
#include <stdio.h>
#include <string.h>

// Comandos básicos do SSD1306
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_CHARGEPUMP 0x8D
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_SEGREMAP 0xA1
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETCONTRAST 0x81
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_DEACTIVATE_SCROLL 0x2E

void ssd1306_command(ssd1306_t *display, uint8_t cmd) {
    uint8_t buffer[2] = {0x00, cmd};
    i2c_write_blocking(display->i2c, display->address, buffer, 2, false);
}

void ssd1306_init(ssd1306_t *display, uint8_t address, uint8_t width, uint8_t height) {
    display->address = address;
    display->width = width;
    display->height = height;
    display->i2c = i2c_default;

    ssd1306_command(display, SSD1306_DISPLAYOFF);
    ssd1306_command(display, SSD1306_SETDISPLAYCLOCKDIV);
    ssd1306_command(display, 0x80);
    ssd1306_command(display, SSD1306_SETMULTIPLEX);
    ssd1306_command(display, height - 1);
    ssd1306_command(display, SSD1306_SETDISPLAYOFFSET);
    ssd1306_command(display, 0x00);
    ssd1306_command(display, SSD1306_SETSTARTLINE | 0x00);
    ssd1306_command(display, SSD1306_CHARGEPUMP);
    ssd1306_command(display, 0x14);
    ssd1306_command(display, SSD1306_MEMORYMODE);
    ssd1306_command(display, 0x00);
    ssd1306_command(display, SSD1306_SEGREMAP);
    ssd1306_command(display, SSD1306_COMSCANDEC);
    ssd1306_command(display, SSD1306_SETCOMPINS);
    ssd1306_command(display, 0x12);
    ssd1306_command(display, SSD1306_SETCONTRAST);
    ssd1306_command(display, 0xCF);
    ssd1306_command(display, SSD1306_SETPRECHARGE);
    ssd1306_command(display, 0xF1);
    ssd1306_command(display, SSD1306_SETVCOMDETECT);
    ssd1306_command(display, 0x40);
    ssd1306_command(display, SSD1306_DISPLAYALLON_RESUME);
    ssd1306_command(display, SSD1306_NORMALDISPLAY);
    ssd1306_command(display, SSD1306_DISPLAYON);
}

void ssd1306_clear(ssd1306_t *display) {
    uint8_t buffer[display->width * display->height / 8];
    memset(buffer, 0x00, sizeof(buffer));
    i2c_write_blocking(display->i2c, display->address, buffer, sizeof(buffer), false);
}

void ssd1306_update(ssd1306_t *display) {
    uint8_t buffer[1] = {0x40};
    i2c_write_blocking(display->i2c, display->address, buffer, 1, true);
}

void ssd1306_draw_char(ssd1306_t *display, int x, int y, char c, int color) {
    uint8_t data[6] = {0x40, c, c, c, c, c};
    i2c_write_blocking(display->i2c, display->address, data, 6, false);
}
