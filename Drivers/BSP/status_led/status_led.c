#include "status_led.h"
#include "tim.h"
#include "main.h"
#include "main.h"
#include "utils.h"
#include "math.h"

extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim16;

#define TIME_INCREMENT 10
#define STATUS_LED_BASE_INTERVAL_MS 2000
#define STATUS_LED_PWM_MAX (100-1)
#define SAMPLED_SIZE_SIZE (STATUS_LED_BASE_INTERVAL_MS / TIME_INCREMENT)

#define TIM_INTERVAL htim7
#define TIM_PWM htim16
#define TIM_PWM_CHANNEL TIM_CHANNEL_1

#define TIM_PWM_SET(_pwm_value) TIM_PWM.Instance->CCR1 = _pwm_value

#define LED_ERROR_PORT LED_RED_GPIO_Port
#define LED_ERROR_PIN LED_RED_Pin

typedef enum status_led_effect_t {
	STATUS_LED_EFFECT_OFF = 0,
	STATUS_LED_EFFECT_BLINK,
	STATUS_LED_EFFECT_PULSE,
	STATUS_LED_EFFECT_TWINKLE,
	STATUS_LED_EFFECT_ERROR,
} status_led_effect_t;

static status_led_effect_t status_led_effect;
static int led_time_interval;
static int led_on_time_interval;
static int led_time_counter;
static state_t led_last_state;
static int iteration_idx;
static uint16_t sampled_sine[SAMPLED_SIZE_SIZE];



void TIM7_IRQHandler(void) {
	/* Execute every 10ms */
	__HAL_TIM_CLEAR_IT(&TIM_INTERVAL, TIM_IT_UPDATE);

	if(status_led_effect == STATUS_LED_EFFECT_BLINK) {
		if((led_time_counter += TIME_INCREMENT) > led_time_interval) {
			led_time_counter -= led_time_interval;
			STATE_TOGGLE(led_last_state);
			TIM_PWM_SET((led_last_state == STATE_OFF) ? 0 : STATUS_LED_PWM_MAX);
		}
	}

	else if(status_led_effect == STATUS_LED_EFFECT_PULSE) {
		led_time_counter += TIME_INCREMENT;
		++iteration_idx;
		if (led_time_counter > led_time_interval) {
			led_time_counter -= led_time_interval;
			TIM_PWM.Instance->CCR1 = 0;
			iteration_idx = 0;
		}
		TIM_PWM_SET(sampled_sine[CONSTRAIN(iteration_idx, 0, SAMPLED_SIZE_SIZE)]);
	}

	else if(status_led_effect == STATUS_LED_EFFECT_TWINKLE) {
		led_time_counter += TIME_INCREMENT;

		if (led_time_counter < led_on_time_interval) {
			TIM_PWM_SET((uint32_t)(1.0F * led_time_counter * STATUS_LED_PWM_MAX / led_on_time_interval));
		}
		else if (led_time_counter > led_time_interval) {
			led_time_counter -= led_time_interval;
			TIM_PWM_SET(0);
		}
		else {
			TIM_PWM_SET(0);
		}
	}

	else if(status_led_effect == STATUS_LED_EFFECT_ERROR) {
		if((led_time_counter += TIME_INCREMENT) > led_time_interval) {
			led_time_counter -= led_time_interval;
			HAL_GPIO_TogglePin(LED_ERROR_PORT, LED_ERROR_PIN);
		}
	}
}


bsp_result_t status_led_init() {
	for(int i=0; i<SAMPLED_SIZE_SIZE; ++i) {
		sampled_sine[i] = (uint16_t)(STATUS_LED_PWM_MAX * (1.0F + sinf(2.0F * 3.1415F * i / SAMPLED_SIZE_SIZE))/2.0F);
		sampled_sine[i] = CONSTRAIN(sampled_sine[i], 0, STATUS_LED_PWM_MAX);
	}

	MX_TIM7_Init();
	MX_TIM16_Init();

	HAL_TIM_Base_MspInit(&htim7);
	HAL_TIM_Base_MspInit(&htim16);

	HAL_StatusTypeDef ret = HAL_OK;


	ret = HAL_TIM_PWM_Start(&TIM_PWM, TIM_PWM_CHANNEL);
	if(ret != HAL_OK) {
		return BSP_ERROR;
	}

	ret = HAL_TIM_Base_Start_IT(&TIM_INTERVAL);
	if(ret != HAL_OK) {
		return BSP_ERROR;
	}

	status_led_pulse();
	return BSP_OK;
}

void status_led_off() {
	status_led_effect = STATUS_LED_EFFECT_OFF;
	led_time_counter = 0;
	HAL_GPIO_WritePin(LED_ERROR_PORT, LED_ERROR_PIN, GPIO_PIN_RESET);
	TIM_PWM_SET(0);
	led_last_state = STATE_OFF;
}

void status_led_blink() {
	status_led_effect = STATUS_LED_EFFECT_BLINK;
	led_time_counter = 0;
	led_time_interval = STATUS_LED_BASE_INTERVAL_MS / 2;
}

void status_led_pulse() {
	status_led_effect = STATUS_LED_EFFECT_PULSE;
	led_time_interval = STATUS_LED_BASE_INTERVAL_MS;
	led_on_time_interval = led_time_interval / 2;
	led_time_counter = 0;
	iteration_idx = 0;
}

void status_led_twinkle() {
	status_led_effect = STATUS_LED_EFFECT_TWINKLE;
	led_time_counter = 0;
	led_time_interval = STATUS_LED_BASE_INTERVAL_MS;
	led_on_time_interval = STATUS_LED_BASE_INTERVAL_MS / 8;
}

void status_led_error() {
	status_led_effect = STATUS_LED_EFFECT_ERROR;
	led_time_counter = 0;
	HAL_GPIO_WritePin(LED_ERROR_PORT, LED_ERROR_PIN, GPIO_PIN_SET);
	led_time_interval = STATUS_LED_BASE_INTERVAL_MS / 8;
	TIM_PWM_SET(0);
}
