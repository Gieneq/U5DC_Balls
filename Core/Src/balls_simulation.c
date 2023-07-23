#include "balls_simulation.h"
#include <main.h>
#include <string.h>
#include "graphics.h"
#include "graphics_res.h"
#include <stdbool.h>
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

	balls_add(0, 100);
	balls_add(0, -200);
}

#define DAMPING_FACTOR 0.9F /* Reduction of velocity on collision */
#define TRANSLATION_EPSILON 0.01F
#define TRANSLATION_EPSILON_SQARED (TRANSLATION_EPSILON*TRANSLATION_EPSILON)
#define TRANSLATION_MAX_ITERATIONS 8

void balls_simulation_update(float time_sec, float delta_time_sec) {
	time_sec *= SIMULATION_TIME_SCALE;
	delta_time_sec *= SIMULATION_TIME_SCALE;
	//todo several iterations

	for(int i=0; i<balls_count; ++i) {
		ball_obj_t* ball1_ref = &balls[i];
		vec2d_t* bpos = &ball1_ref->pos;
		vec2d_t* bvel = &ball1_ref->vel;


		/* Check if colliding with constrains in small range */
//		float actual_dist_from_center = sqrtf(vec_len_sq(bpos));
//		bool is_touching = (actual_dist_from_center < (CONSTR_RADIUS - ball1_ref->radius))

//		if(bs_is_touching_edge(bpos, ball1_ref->radius)) {
		vec2d_t delta_vel = vec_scaled(&gravity_force, delta_time_sec);
		vec_translate(bvel, &delta_vel);
//		}

		/*
		 * Total position translation in this step. Not applying transltion yet */
		vec2d_t delta_pos = vec_scaled(bvel, delta_time_sec);

		/*
		 * Recursively apply collision and constrains check
		 * Add to ball position and modify velocity
		 * with reflections and damping.
		 */

		while(vec_len_sq(&delta_pos) > TRANSLATION_EPSILON_SQARED) {
			/* Suspected position in this step */
			vec2d_t th_pos = vec_add(bpos, &delta_pos);

			/*
			 * Check if it is valid location.
			 * If not find valid spot, modify rest of path
			 * and reflect velocity.
			 */

			/* Constraints for th_pos */
			float dist_from_center = sqrtf(vec_len_sq(&th_pos));
			float constr_radius_trimmed = CONSTR_RADIUS - ball1_ref->radius;

			if(dist_from_center > constr_radius_trimmed) {
				/*
				 * Ball's center outside constrain circle.
				 * Find common point of const_trimmed_circle and
				 * strait of balls movement.
				 * Use circle equation: x^2 + y^2 = R^2
				 * and parametrized line equation p = p0 + t*v
				 * Solve t and choose t > 0
				 */

				/* Start from bpos as 'p', with delta_pos as 'v'
				 *				float a = (vx^2 + vy^2)
				 *				float b = 2 * (px * vx + py * vy)
				 *				float c = (px^2 + py^2 - R^2)
				 *				float t = (-b ± √(b^2 - 4ac)) / 2a
				 * Solution when
				 */

				float coof_a = delta_pos.x * delta_pos.x + delta_pos.y * delta_pos.y;
				float coof_b = 2.0F * vec_get_dot_product(bpos, &delta_pos);
				float coof_c = vec_len_sq(bpos) - constr_radius_trimmed*constr_radius_trimmed;
				float delta = coof_b * coof_b - 4.0F * coof_a * coof_c;

				if(delta >= 0.0F) {
					/* Solution exists */
					float t = 0.0F;
					if(delta > 0.0F) {
						float sqrt_delta = sqrtf(delta);
						float t1 = (-coof_b - sqrt_delta) / (2.0F * coof_a);
						float t2 = (-coof_b + sqrt_delta) / (2.0F * coof_a);
						t = (t1 > t2) ? t1 : t2; //find positive, well... in this way
					}
					else {
						/* delta = 0.0F */
						t = (-coof_b - 0) / (2.0F * coof_a);
					}

					/* t is parameter of ball hitting edge */
					vec2d_t transl_to_cross_circle = {
							.x = t * delta_pos.x,
							.y = t * delta_pos.y,
					};

					/*
					 * Move ball, shorted rest of remaining path,
					 * reflect velocity
					 */

					vec_translate(bpos, &transl_to_cross_circle);
					delta_pos.x -= transl_to_cross_circle.x;
					delta_pos.y -= transl_to_cross_circle.y;

					/* Reflect velocity */
					vec2d_t dir_to_ball = vec_get_normalized(bpos);
					vec2d_t reflection_normal = {
							-dir_to_ball.x,
							-dir_to_ball.y
					};
					vec2d_t reflected_velocity = vec_get_reflected(bvel, &reflection_normal);
					*bvel = vec_scaled(&reflected_velocity, DAMPING_FACTOR);

					/* Change delta position according to new velocity */
					//todo use time to solve steps
					float delta_pos_len = sqrtf(vec_len_sq(&delta_pos));
					float velocity_len = sqrtf(vec_len_sq(bvel));
					delta_pos.x = delta_pos_len * bvel->x / velocity_len;
					delta_pos.y = delta_pos_len * bvel->y / velocity_len;

				}
			}
			else {
				/*
				 * No collicion with border.
				 * But can collide with other balls.
				 */
				bool found_collision = false;


				/* Collide with the rest */
				for(int j=0; j<balls_count; ++j) {
					if(i == j) {
						continue;
					}

					/* Other ball */
					ball_obj_t* ball2_ref = &balls[j];
					vec2d_t* b2pos = &ball2_ref->pos;
					vec2d_t* b2vel = &ball2_ref->vel;

					/* Check distance, if colliding, spread */
					float b2b_dist = vec_get_distance(bpos, b2pos); //todo can improve
					float b2b_radius_sum = ball1_ref->radius + ball2_ref->radius;
					if(b2b_radius_sum > b2b_dist) {
						/* colliding - spread them */
						vec2d_t b2b_1_to_2_half = {
								.x = (b2pos->x - bpos->x) / 2.0F,
								.y = (b2pos->y - bpos->y) / 2.0F,
						};
						vec2d_t b2b_2_to_1_half = {
								.x = -b2b_1_to_2_half.x,
								.y = -b2b_1_to_2_half.y
						};

						//////// hacky, no all movement in this step
						bpos->x -= b2b_1_to_2_half.x;
						bpos->y -= b2b_1_to_2_half.y;
						delta_pos.x = 0;
						delta_pos.y = 0;

						//////

						b2pos->x -= b2b_2_to_1_half.x;
						b2pos->y -= b2b_2_to_1_half.y;

						/* Reflect velocity */
						vec2d_t b1_refl_norm = vec_get_normalized(&b2b_2_to_1_half);
						vec2d_t b2_refl_norm = {
								.x = -b1_refl_norm.x,
								.y = -b1_refl_norm.y,
						};

						*bvel = vec_get_reflected(bvel, &b1_refl_norm); //meh
						*b2vel = vec_get_reflected(b2vel, &b2_refl_norm);

						found_collision = true;
						break; //not rly cuz can have some movement to do
					}

				}


				if(found_collision == false) {
					vec_translate(bpos, &delta_pos);
					delta_pos.x = 0.0F;
					delta_pos.y = 0.0F;
				}
			}
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
