#pragma once

#define ABS(_x) ((_x > 0) ? _x : -_x)

#define MIN(_a, _b) ((_a < _b) ? _a : _b)
#define MAX(_a, _b) ((_a > _b) ? _a : _b)

#define CONSTRAIN(_x, _min, _max) (MIN(MAX(_x, _min), _max))

#ifndef option_t
typedef enum option_t {
	OPTION_NONE = 0,
	OPTION_SOME = 1,
} option_t;
#endif


