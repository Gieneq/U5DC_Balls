#include "microtimer.h"
#include "tim.h"

extern TIM_HandleTypeDef htim3;
#define TIM_MICROTIMER htim3

static volatile uint32_t microtimer_last_time;

#define PERFORMANCE_REFRESH_INTERVAL_US 1000000UL

static volatile float estimated_usage;

static volatile uint32_t performance_acc_work_time;
static volatile uint32_t performance_acc_loop_time;

static volatile uint32_t performance_start_work_time;
static volatile uint32_t performance_work_duration;
static volatile uint32_t performance_loop_duration;

bsp_result_t microtimer_init() {
	MX_TIM3_Init();
	HAL_TIM_Base_MspInit(&TIM_MICROTIMER);
	HAL_TIM_Base_Start(&htim3);
	microtimer_last_time = __HAL_TIM_GET_COUNTER(&TIM_MICROTIMER);

	/* Performance */
	performance_acc_work_time = 0;
	performance_acc_loop_time = 0;

	return BSP_OK;
}

void microtimer_start() {
	microtimer_last_time = __HAL_TIM_GET_COUNTER(&TIM_MICROTIMER);
}

uint32_t microtimer_stop() {
	return (__HAL_TIM_GET_COUNTER(&TIM_MICROTIMER) - microtimer_last_time);
}

uint32_t microtimer_get_us() {
	return __HAL_TIM_GET_COUNTER(&TIM_MICROTIMER);
}



void microperformance_start_work() {
	performance_start_work_time = microtimer_get_us();
}

void microperformance_end_work() {
	performance_work_duration = microtimer_get_us() - performance_start_work_time;
	performance_acc_work_time += performance_work_duration;
}

void microperformance_end_loop() {
	performance_loop_duration = microtimer_get_us() - performance_start_work_time;
	performance_acc_loop_time += performance_loop_duration;

	if(performance_acc_loop_time > PERFORMANCE_REFRESH_INTERVAL_US) {
		/* Refresh final value */
		estimated_usage = 100.0F * performance_acc_work_time / performance_acc_loop_time;
		performance_acc_work_time = 0;
		performance_acc_loop_time = 0;
	}
}

float microperformance_get_usage() {
	return estimated_usage;
}
