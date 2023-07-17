#pragma once
#include "bsp_defs.h"

#define LCD_FRAMEBUFFER0_SIZE 184320
extern uint32_t lcd_framebuffer0[LCD_FRAMEBUFFER0_SIZE];

bsp_result_t graphics_init();
void gfx_draw_fillrect(uint32_t x_pos, uint32_t y_pos, uint32_t width, uint32_t height, uint32_t color);
void gfx_fillscreen(uint32_t color);
void gfx_clearscreen();
