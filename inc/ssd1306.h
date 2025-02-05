#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>
#include "hardware/i2c.h"

#define SSD1306_ADDR 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Estrutura para armazenar o estado do display
typedef struct {
    uint8_t address;
    uint8_t width;
    uint8_t height;
    i2c_inst_t *i2c;
} ssd1306_t;

void ssd1306_init(ssd1306_t *display, uint8_t address, uint8_t width, uint8_t height);
void ssd1306_clear(ssd1306_t *display);
void ssd1306_update(ssd1306_t *display);
void ssd1306_draw_char(ssd1306_t *display, int x, int y, char c, int color);

#endif
