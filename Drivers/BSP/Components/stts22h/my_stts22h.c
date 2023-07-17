#include "my_stts22h.h"
#include <stdint.h>
#include "i2c.h"
#include "microtimer.h"
#include <stdbool.h>
#include <string.h>

#define STTS22H_I2C_TIMEOUT 1000
#define STTS22H_READ_TEMP_INTERVAL_US 500000UL

#define INITIAL_TEMPERATURE -40.0F

//#define STTS22H_BASE_ADDRESS        0x37U
#define STTS22H_ADDRESS_WRITE       0x7E //(STTS22H_BASE_ADDRESS << 1)
#define STTS22H_ADDRESS_READ        0x7F //((STTS22H_BASE_ADDRESS << 1) | 0x1)
#define STTS22H_WHOAMI              0x01U
#define STTS22H_WHOAMI_VALUE        0xA0U
#define STTS22H_CTRL                0x04U
#define STTS22H_STATUS              0x05U
#define STTS22H_TEMP_L              0x06U


#define STATUS_IS_BUSY(_status) (_status & (1<<0) ? true : false)
#define START_ONE_SHOT(_ctrl) _ctrl |= (1<<0)

static uint32_t last_time;
static float last_read_temperature;


static bool my_stts22h_try_starting_oneshot() {
	uint8_t ctrl = (1<<6) | (1<<3) | (1<<0);
	HAL_StatusTypeDef ret = HAL_OK;
	ret = HAL_I2C_Mem_Write(&hi2c3, STTS22H_ADDRESS_WRITE, STTS22H_CTRL, I2C_MEMADD_SIZE_8BIT, &ctrl, 1, STTS22H_I2C_TIMEOUT);
	if(ret != HAL_OK) {
		return false;
	}

	return true;
}

static bool my_stts22h_refresh_temperature() {
	uint8_t temperature_registers[2] = {0x0, 0x0};

	HAL_StatusTypeDef ret = HAL_OK;
	ret = HAL_I2C_Mem_Read(&hi2c3, STTS22H_ADDRESS_READ, STTS22H_TEMP_L, I2C_MEMADD_SIZE_8BIT, temperature_registers, 2, STTS22H_I2C_TIMEOUT);
	if(ret != HAL_OK) {
		return false;
	}

	uint32_t temp_l = temperature_registers[0];
	uint32_t temp_h = temperature_registers[1];
	uint32_t temp_v = temp_h<<8 | temp_l;

	if(temp_v < 32768) {
		last_read_temperature = temp_v / 100.0F;
	} else {
		last_read_temperature = (temp_v - (uint32_t)(32768)*2U) / 100.0F;
	}

	return true;
}

/**
 * Check value of factory whoami register. In both cases
 * when value is not correct or communication fail
 * returns false.
 */
static bool my_stts22h_check_id() {
	uint8_t whoami_value = 0;

	HAL_StatusTypeDef ret = HAL_OK;
	ret = HAL_I2C_Mem_Read(&hi2c3, STTS22H_ADDRESS_READ, STTS22H_WHOAMI, I2C_MEMADD_SIZE_8BIT, &whoami_value, 1, STTS22H_I2C_TIMEOUT);
	if(ret != HAL_OK) {
		return false;
	}

	if(whoami_value != STTS22H_WHOAMI_VALUE) {
		return false;
	}

	return true;
}

/*
 * In both cases: sensor is busy, bus is busy and
 * communication failed return true
 */
static bool my_stts22h_is_busy() {
	uint8_t status = 0;
	HAL_StatusTypeDef ret = HAL_OK;
	ret = HAL_I2C_Mem_Read(&hi2c3, STTS22H_ADDRESS_READ, STTS22H_WHOAMI, I2C_MEMADD_SIZE_8BIT, &status, 1, STTS22H_I2C_TIMEOUT);
	if(ret != HAL_OK) {
		return true;
	}

	if(status) {
		return false;
	}

	return true;
}




bsp_result_t my_stts22h_init() {
	if(my_stts22h_check_id() == false) {
		return BSP_ERROR;
	}

	last_time = microtimer_get_us();
	last_read_temperature = INITIAL_TEMPERATURE;

	return BSP_OK;
}

static int temp_updates_counter = 0;
static uint32_t eval_time = 0;

void my_stts2h_update() {
	uint32_t current_time = microtimer_get_us();

	if(current_time - last_time > STTS22H_READ_TEMP_INTERVAL_US) {
		/* Interval passed, try reading till success */
		uint32_t some_time = microtimer_get_us();

		my_stts22h_try_starting_oneshot();

		while(my_stts22h_is_busy() != false) {
			;
		}

		my_stts22h_refresh_temperature();

		temp_updates_counter++;

		// remove time only if succedded
		last_time += STTS22H_READ_TEMP_INTERVAL_US;

		eval_time = microtimer_get_us() - some_time;
	}

}

float my_stts2h_get_temperature() {
	return last_read_temperature;
}








