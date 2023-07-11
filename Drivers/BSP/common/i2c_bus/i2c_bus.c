#include "i2c_bus.h"
#include "i2c.h"


//#define VL53L5CX_ADDRESS_WRITE     (0x52<<1) | 0x0
//#define VL53L5CX_ADDRESS_READ      (0x52<<1) | 0x1

extern I2C_HandleTypeDef hi2c3;

//ideas: lock(aquire) buf by some device

void I2C3_EV_IRQHandler(void) {

}

void I2C3_ER_IRQHandler(void) {

}


bsp_result_t i2c_bus_init() {

	MX_I2C3_Init();

	HAL_I2C_MspInit(&hi2c3);

	return BSP_OK;
}
