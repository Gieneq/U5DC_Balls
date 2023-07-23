#pragma once
#include "bsp_defs.h"


#define BALL_IMAGE_WIDTH 50
#define BALL_IMAGE_HEIGHT 50
#define BALL_IMAGE_PIXELS (BALL_IMAGE_WIDTH * BALL_IMAGE_HEIGHT)
#define BALL_IMAGE_BYTES (BALL_IMAGE_PIXELS * 4)

extern uint32_t ball_image[BALL_IMAGE_PIXELS];
void graphics_res_init();
