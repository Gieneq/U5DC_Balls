#include "graphics_res.h"
#include "math.h"

#define ABS(_a) ((_a < 0) ? -_a : _a)

static void graphics_res_build_ball_image();

uint32_t ball_image[BALL_IMAGE_PIXELS] = {0};

static void graphics_res_build_ball_image() {
	const float radius_outer_sq   = powf(0.5F  * BALL_IMAGE_WIDTH, 2.0F);
	const float radius_inner_sq   = powf(0.35F * BALL_IMAGE_WIDTH, 2.0F);
	const uint32_t color_outer = 0xFF00FF00;
	const uint32_t color_inner = 0xFF0000FF;
	const uint32_t color_bg    = 0x00000000;

	const float center_x = 0.5F  * BALL_IMAGE_WIDTH;
	const float center_y = 0.5F  * BALL_IMAGE_WIDTH;

	for(int iy=0; iy<BALL_IMAGE_WIDTH; ++iy) {
		for(int ix=0; ix<BALL_IMAGE_WIDTH; ++ix) {
			int current_index = iy * BALL_IMAGE_WIDTH + ix;
			float radius_sq = powf(center_x - ix, 2.0F) + powf(center_y - iy, 2.0F);
			if(radius_sq > radius_outer_sq) {
				ball_image[current_index] = color_bg;
			}
			else if(radius_sq > radius_inner_sq) {
				ball_image[current_index] = color_outer;
			}
			else {
				ball_image[current_index] = color_inner;
			}
		}
	}
}

void graphics_res_init() {
	graphics_res_build_ball_image();
}
