#include "vec_math.h"
#include <string.h>


vec2d_t vec_between_points(const vec2d_t* p1, const vec2d_t* p2) {
	vec2d_t tmp = {
			.x = p1->x - p2->x,
			.y = p1->y - p2->y,
	};
	return tmp;
}


float vec_norm_res_len_sq(vec2d_t* vec, float* len_squared) {
	*len_squared = vec->x * vec->x + vec->y * vec->y;
	float len = sqrtf(*len_squared);
	vec->x /= len;
	vec->y /= len;
	return len;
}

void vec_translate(vec2d_t* position, const vec2d_t* translation) {
	position->x += translation->x;
	position->y += translation->y;
}


float vec_len_sq(const vec2d_t* vec) {
	return vec->x * vec->x + vec->y * vec->y;
}

vec2d_t vec_scaled(const vec2d_t* vec, const float scale) {
	vec2d_t tmp = {
			.x = vec->x * scale,
			.y = vec->y * scale,
	};
	return tmp;
}


vec2d_t vec_get_normalized(const vec2d_t* vec) {
	float len = sqrtf(vec_len_sq(vec));
	vec2d_t tmp = {
			.x = vec->x / len,
			.y = vec->y / len,
	};
	return tmp;
}

vec2d_t vec_get_reflected(const vec2d_t* vec, vec2d_t* normal) {
    // Calculate the dot product of the vector and the normal
    float dotProduct = vec->x * normal->x + vec->y * normal->y;

    // Calculate the reflected vector using the dot product and the normal
    vec2d_t reflectedVector = {
		.x = vec->x - 2 * dotProduct * normal->x,
		.y = vec->y - 2 * dotProduct * normal->y
    };
    return reflectedVector;
}

void vec_negate(vec2d_t* vec) {
	vec->x = -vec->x;
	vec->y = -vec->y;
}

float vec_get_dot_product(const vec2d_t* vec1, const vec2d_t* vec2) {
    float dot = (vec1->x * vec2->x) + (vec1->y * vec2->y);
    return dot;
}

vec2d_t vec_add(const vec2d_t* v1, const vec2d_t* v2) {
	vec2d_t tmp = {
			.x = v1->x + v2->x,
			.y = v1->y + v2->y,
	};
	return tmp;
}

float vec_get_distance(const vec2d_t* bpos, const vec2d_t* b2pos) {
	vec2d_t tmp = {
			.x = bpos->x - b2pos->x,
			.y = bpos->y - b2pos->y,
	};

	float dist_squared = tmp.x * tmp.x + tmp.y * tmp.y;
	return sqrtf(dist_squared);
}






