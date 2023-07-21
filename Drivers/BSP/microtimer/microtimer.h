#pragma once
#include "bsp_defs.h"
#include <stdint.h>

bsp_result_t microtimer_init();
void microtimer_start();
uint32_t microtimer_stop();
uint32_t microtimer_get_us();

void microperformance_start_idle();
void microperformance_start_update();
void microperformance_end_update();
void microperformance_start_draw();
void microperformance_end_loop();

float microperformance_get_usage();


static inline void microdelay(uint32_t micros) {
	uint32_t microdelay_last_time = microtimer_get_us();
	while((microtimer_get_us() - microdelay_last_time) < micros) {
		;//__NOP;
	}
}
