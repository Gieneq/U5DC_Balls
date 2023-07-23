#include "balls_simulation.h"
#include <main.h>
#include <string.h>
#include "graphics.h"
#include "graphics_res.h"

extern uint32_t ball_image[BALL_IMAGE_PIXELS];

//static void balls_simulation_apply_constrains(ball_obj_t* ball);
//static void balls_simulation_spread(ball_obj_t* b1, ball_obj_t* b2);

#define MAX_BALLS_COUNT 16
static ball_obj_t balls[MAX_BALLS_COUNT];
static int balls_count = 0;

#define SIMULATION_TIME_SCALE (8.0F)

#define ITERATIONS_PER_UPDATE (5)

#define CONSTR_CX 0.0F
#define CONSTR_CY 0.0F
#define CONSTR_RADIUS 240.0F

#define GRAVITY_CONST 9.81F
static const vec2d_t gravity_force = {0.0F, GRAVITY_CONST};

//static void balls_simulation_apply_constrains(ball_obj_t* ball) {
//
//}
//
//static void balls_simulation_calculate() {
//
//}
static void balls_add(float start_x, float start_y) {
	ball_obj_t tmp_ball = {
			.pos = {start_x, start_y},
			.radius = 50.0F,
			.vel = {0.0F, 0.0F}
	};

	int new_index = balls_count;
	memcpy(&balls[new_index], &tmp_ball, sizeof(ball_obj_t));
	balls_count++;
}

void balls_simulation_init() {
	for(int i=0; i<MAX_BALLS_COUNT; ++i) {
		ball_obj_t* ball_ref = &balls[i];
		memset(ball_ref, 0, sizeof(ball_obj_t));
	}
	balls_count = 0;

	balls_add(0, -(240-25)); //todo inverse axis
	balls_add(-240+25, 0);
	balls_add(240-75, 0);
}

void balls_simulation_update(float time_sec, float delta_time_sec) {
	time_sec *= SIMULATION_TIME_SCALE;
	delta_time_sec *= SIMULATION_TIME_SCALE;
	//todo several iterations
	for(int i=0; i<balls_count; ++i) {
		ball_obj_t* ball1_ref = &balls[i];
		vec2d_t* bpos = &ball1_ref->pos;
		vec2d_t* bvel = &ball1_ref->vel;

		vec2d_t delta_vel = vec_scaled(&gravity_force, delta_time_sec);
		vec_translate(bvel, &delta_vel);

		vec2d_t delta_pos = vec_scaled(bvel, delta_time_sec);
		vec_translate(bpos, &delta_pos);


		/* Constraints */
		float dist_from_center = sqrtf(vec_len_sq(bpos));

		if((dist_from_center + ball1_ref->radius) > CONSTR_RADIUS) {
			/* Ball out of constrain */

			/* Clip position */
			vec2d_t norm_toward_center = vec_get_normalized(bpos);
			vec_negate(&norm_toward_center);

			float dist_otside = dist_from_center + ball1_ref->radius - CONSTR_RADIUS;
			vec2d_t translation = {
					.x = norm_toward_center.x * dist_otside,
					.y = norm_toward_center.y * dist_otside,
			};
			vec_translate(bpos, &translation);

			/* Reflect velocity */
			vec2d_t reflected_vel = vec_get_reflected(bvel, &norm_toward_center);
			bvel->x = reflected_vel.x;
			bvel->y = reflected_vel.y;
		}
	}
}

void balls_simulation_draw() {
	for(int i=0; i<balls_count; ++i) {
		ball_obj_t* ball_ref = &balls[i];

		const int16_t draw_x = (int16_t)(ball_ref->pos.x + (LCD_WIDTH  / 2.0F) - ball_ref->radius / 2.0F);
		const int16_t draw_y = (int16_t)(ball_ref->pos.y + (LCD_HEIGHT / 2.0F) - ball_ref->radius / 2.0F);
		/* Ball */
		gfx_draw_bitmap_blocking(ball_image, (uint16_t)draw_x, (uint16_t)draw_y, 50, 50);
	}
}
