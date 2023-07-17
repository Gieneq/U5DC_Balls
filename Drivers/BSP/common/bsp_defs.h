#pragma once
#include <stdint.h>

typedef enum state_t {
	STATE_OFF = 1,
	STATE_ON
} state_t;

#define STATE_TOGGLE(_state) (_state ^= 1);

typedef enum bsp_result_t {
	BSP_OK = 0,
	BSP_ERROR
} bsp_result_t;
