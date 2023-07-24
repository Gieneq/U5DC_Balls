#include "balls_simulation.h"
#include <main.h>
#include <string.h>
#include "graphics.h"
#include "graphics_res.h"
#include <stdbool.h>
#include "rng.h"
extern uint32_t ball_image[BALL_IMAGE_PIXELS];

//static void balls_simulation_apply_constrains(ball_obj_t* ball);
//static void balls_simulation_spread(ball_obj_t* b1, ball_obj_t* b2);

#define MAX_BALLS_COUNT 128
static ball_obj_t balls[MAX_BALLS_COUNT];
static int balls_count = 0;

#define SIMULATION_TIME_SCALE (8.0F)

#define ITERATIONS_PER_UPDATE (5)

#define CONSTR_CX 0.0F
#define CONSTR_CY 0.0F
#define CONSTR_RADIUS 240.0F

#define GRAVITY_CONST 9.81F
static const vec2d_t gravity_force = {0.0F, -GRAVITY_CONST};
static const vec2d_t downwards_norm = {0.0F, -1.0F};
static void solve_ball_to_balls_collision(const int ball_idx);

static void balls_add(float start_x, float start_y) {
	ball_obj_t tmp_ball = {
			.pos = {start_x, start_y},
			.radius = 25.0F,
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

//	balls_add(0, -(240-25)); //todo inverse axis
//	balls_add(-240+25 + 0.1F, 0);
//	balls_add(240-75, 0);
//
//	balls_add(-240+45, 40);


	balls_add(1, 120);
	balls_add(0, -220);

//	balls_simulation_generate_ball();
//	balls_add(180, 122-10);
}

#define DAMPING_FACTOR 0.99F /* Reduction of velocity on collision */
#define TRANSLATION_EPSILON 0.01F
#define TRANSLATION_EPSILON_SQARED (TRANSLATION_EPSILON*TRANSLATION_EPSILON)
#define TRANSLATION_MAX_ITERATIONS 8


void balls_simulation_update(float time_sec, float delta_time_sec) {
	static const float const_epsilon = 0.005F;
	time_sec *= SIMULATION_TIME_SCALE;
	delta_time_sec *= SIMULATION_TIME_SCALE;

	for(int i=0; i<balls_count; ++i) {
		ball_obj_t* ball1_ref = &balls[i];
		const float b1_radius = ball1_ref->radius;
		vec2d_t* b1_pos = &ball1_ref->pos;
		vec2d_t* b1_vel = &ball1_ref->vel;

		const float b1_dist_sq = vec_get_len_sq(b1_pos);
		const float constr_radius_inner = CONSTR_RADIUS - b1_radius;
//		const float constr_radius_inner_sq = constr_radius_inner*constr_radius_inner;
		const float constr_radius_inner_minepsilon = constr_radius_inner - const_epsilon;
		const float constr_radius_inner_minepsilon_sq = constr_radius_inner_minepsilon * constr_radius_inner_minepsilon;
		const float constr_radius_inner_plusepsilon = constr_radius_inner + const_epsilon;
		const float constr_radius_inner_plusepsilon_sq = constr_radius_inner_plusepsilon * constr_radius_inner_plusepsilon;

		const bool is_touching_constrain = ((b1_dist_sq > constr_radius_inner_minepsilon_sq) && (b1_dist_sq < constr_radius_inner_plusepsilon_sq));
		const bool is_outside_constrain = (b1_dist_sq >= constr_radius_inner_plusepsilon_sq);

		if(is_touching_constrain == true) {
			/*
			 * Only touching.
			 * Apply force, sliding
			 */

			const vec2d_t const_outside_norm = vec_get_normalized(b1_pos);
			float gravity_dot = vec_get_dot_product(&const_outside_norm, &downwards_norm);
			if(gravity_dot < 0.0F) {
				/* Apply only gravity downwards */
				const vec2d_t acceleration = {
						.x = 0,
						.y = -GRAVITY_CONST
				};
				const vec2d_t b1_d_vel = vec_get_scaled(&acceleration, delta_time_sec);
				vec_add(b1_vel, &b1_d_vel);

				/* Calculate position */
				const vec2d_t b1_d_pos = vec_get_scaled(b1_vel, delta_time_sec);
				vec_add(b1_pos, &b1_d_pos);
			}
			else {
				/* Apply sliding */
				vec2d_t sliding_tangent_norm;
				if(const_outside_norm.x > 0) {
					/* Rotate clockwise */
					sliding_tangent_norm = get_rotated_clockwise_90n(&const_outside_norm, 1);
				}
				else {
					/* Rotate counter clockwise */
					sliding_tangent_norm = get_rotated_clockwise_90n(&const_outside_norm, 4-1);
				}
				float sliding_dot = vec_get_dot_product(&sliding_tangent_norm, &downwards_norm);

				/* Calculate velocity */
				const vec2d_t acceleration = {
						.x = sliding_dot * GRAVITY_CONST * sliding_tangent_norm.x,
						.y = sliding_dot * GRAVITY_CONST * sliding_tangent_norm.y
				};
				const vec2d_t b1_d_vel = vec_get_scaled(&acceleration, delta_time_sec);
				vec_add(b1_vel, &b1_d_vel);

				/* Calculate position */
				const vec2d_t b1_d_pos = vec_get_scaled(b1_vel, delta_time_sec);
				vec_add(b1_pos, &b1_d_pos);
			}


		}
		else if(is_outside_constrain == true) {
			/*
			 * In invalid position.
			 * Move back to valid position along radius.
			 * Reflect velocity vector.
			 */

			const vec2d_t b1_to_0 = vec_get_negated(b1_pos);
			const vec2d_t b1_to_0_norm = vec_get_normalized(&b1_to_0);
			const float dist_to_constrain = sqrtf(b1_dist_sq) - constr_radius_inner_minepsilon;
			const vec2d_t trans_to_constrain = vec_get_scaled(&b1_to_0_norm, dist_to_constrain);
			vec_add(b1_pos, &trans_to_constrain);

			/* Reflect velocity vector with damping */
			vec2d_t b1_reflected_vel = vec_get_reflected(b1_vel, &b1_to_0_norm);
			b1_reflected_vel = vec_get_scaled(&b1_reflected_vel, DAMPING_FACTOR);
			*b1_vel = b1_reflected_vel;
		}
		else {
			/* Not touching constrains at all */

			/* Calculate velocity */
			const vec2d_t acceleration = gravity_force;
			const vec2d_t b1_d_vel = vec_get_scaled(&acceleration, delta_time_sec);
			vec_add(b1_vel, &b1_d_vel);

			/* Calculate position */
			const vec2d_t b1_d_pos = vec_get_scaled(b1_vel, delta_time_sec);
			vec_add(b1_pos, &b1_d_pos);
		}
	}

	static const int iterations = 1;
	for(int n=0; n<iterations; ++n) {
		for(int i=0; i<balls_count; ++i) {
			/* Check collisions with other balls */
			solve_ball_to_balls_collision(i);
		}
	}
}


static void solve_ball_to_balls_collision(const int ball_idx) {
	ball_obj_t* ball1_ref = &balls[ball_idx];
	const float b1_radius = ball1_ref->radius;
	vec2d_t* b1_pos = &ball1_ref->pos;
	vec2d_t* b1_vel = &ball1_ref->vel;

	/* Collide with the rest */
	for(int j=0; j<balls_count; ++j) {
		if(ball_idx == j) {
			continue;
		}

		/* Other ball */
		ball_obj_t* ball2_ref = &balls[j];
		const float b2_radius = ball2_ref->radius;
		vec2d_t* b2_pos = &ball2_ref->pos;
		vec2d_t* b2_vel = &ball2_ref->vel;

		/* Check distance, if colliding, spread */
		const vec2d_t b1_to_b2 = vec_get_subtracted(b2_pos, b1_pos);
		const float balls_dist = sqrtf(vec_get_len_sq(&b1_to_b2));
		const float balls_radius_sum = b1_radius + b2_radius;
		if(balls_radius_sum > balls_dist) {
			/* Colliding - spread them */
			float offset_dist = balls_radius_sum - balls_dist;
			float offset_half_dist = offset_dist / 2.0F;
			const vec2d_t b1_to_b2_norm = vec_get_normalized(&b1_to_b2);
			const vec2d_t b2_to_b1_norm = vec_get_negated(&b1_to_b2_norm);

			const vec2d_t b1_translation = vec_get_scaled(&b2_to_b1_norm, offset_half_dist);
			const vec2d_t b2_translation = vec_get_scaled(&b1_to_b2_norm, offset_half_dist);
			vec_add(b1_pos, &b1_translation);
			vec_add(b2_pos, &b2_translation);

			/* Reflect velocity */

			vec2d_t vel_sum = vec_get_added(b1_vel, b2_vel);
			float half_vel_value = sqrtf(vec_get_len_sq(&vel_sum)) / 2.0F;
			half_vel_value *= DAMPING_FACTOR;
			*b1_vel = vec_get_scaled(&b2_to_b1_norm, half_vel_value);
			*b2_vel = vec_get_scaled(&b1_to_b2_norm, half_vel_value);
		}
	}
}




void balls_simulation_draw() {
	/* Draw center */
	gfx_draw_hline(25, LCD_WIDTH-25, LCD_HEIGHT/2, 0xFFFFFF00);
	gfx_draw_vline(25, LCD_HEIGHT-25, LCD_WIDTH/2, 0xFFFFFF00);

	for(int i=0; i<balls_count; ++i) {
		ball_obj_t* ball_ref = &balls[i];

		/* (0,0) in center, reflect y axis */
		const int16_t draw_x = (int16_t)(ball_ref->pos.x + (LCD_WIDTH  / 2.0F) - ball_ref->radius);
		const int16_t draw_y = (int16_t)((0.0F - ball_ref->pos.y) + (LCD_HEIGHT / 2.0F) - ball_ref->radius);
		/* Ball */
		gfx_draw_bitmap_blocking(ball_image, (uint16_t)draw_x, (uint16_t)draw_y, 50, 50);
	}
}


void balls_simulation_generate_ball() {
	const uint32_t ampl = 2*(240-25);
	uint32_t raw_x;
	uint32_t raw_y;
	HAL_RNG_GenerateRandomNumber(&hrng, &raw_x);
	HAL_RNG_GenerateRandomNumber(&hrng, &raw_y);
	float x = ((float)(raw_x % ampl)) - (ampl / 2.0F);
	float y = ((float)(raw_y % ampl)) - (ampl / 2.0F);
	balls_add(x, y);
}
