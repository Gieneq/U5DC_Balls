#pragma once
#include "vec_math.h"

typedef struct ball_obj_t {
	vec2d_t pos;
	vec2d_t vel;
	float radius;
} ball_obj_t;


void balls_simulation_init();
void balls_simulation_update(float time_sec, float delta_time_sec);
void balls_simulation_draw();
void balls_simulation_generate_ball();
