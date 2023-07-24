#pragma once
#include <stdint.h>


typedef struct vec2d_t {
	float x;
	float y;
} vec2d_t;


vec2d_t vec_between_points(const vec2d_t* p1, const vec2d_t* p2);
float vec_norm_res_len_sq(vec2d_t* vec, float* len_squared);
void vec_add(vec2d_t* position, const vec2d_t* translation);
float vec_get_len_sq(const vec2d_t* vec);
vec2d_t vec_get_scaled(const vec2d_t* vec, const float scale);
vec2d_t vec_get_normalized(const vec2d_t* vec);
vec2d_t vec_get_reflected(const vec2d_t* vec, const vec2d_t* normal);
void vec_negate(vec2d_t* vec);
vec2d_t vec_get_negated(const vec2d_t* vec);
float vec_get_dot_product(const vec2d_t* vec1, const vec2d_t* vec2);
vec2d_t vec_get_added(const vec2d_t* v1, const vec2d_t* v2);

float vec_get_distance(const vec2d_t* bpos, const vec2d_t* b2pos);

vec2d_t get_rotated_clockwise_90n(const vec2d_t* vec, const int n);
vec2d_t vec_get_subtracted(const vec2d_t* v1, const vec2d_t* v2);
