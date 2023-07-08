#include "bsp.h"

bsp_result_t bsp_init() {
	bsp_result_t ret = BSP_OK;

	ret = status_led_init();
	if(ret != BSP_OK) {
		return ret;
	}

	return BSP_OK;
}
