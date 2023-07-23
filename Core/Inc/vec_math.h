#pragma once
#include <stdint.h>


typedef struct vec2d_t {
	float x;
	float y;
} vec2d_t;


vec2d_t vec_between_points(const vec2d_t* p1, const vec2d_t* p2);
float vec_norm_res_len_sq(vec2d_t* vec, float* len_squared);
void vec_translate(vec2d_t* position, const vec2d_t* translation);
float vec_len_sq(const vec2d_t* vec);
vec2d_t vec_scaled(const vec2d_t* vec, const float scale);
vec2d_t vec_get_normalized(const vec2d_t* vec);
vec2d_t vec_get_reflected(const vec2d_t* vec, vec2d_t* normal);
void vec_negate(vec2d_t* vec);
//#define vec_scaled(_vec, _s) ((vec2d_t){_vec->x * _s, _vec->y * _s})
