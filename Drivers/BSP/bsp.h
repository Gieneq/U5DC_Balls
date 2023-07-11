#pragma once
#include "bsp_defs.h"
#include "microtimer.h"
#include "status_led.h"
#include "i2c_bus.h"

bsp_result_t bsp_init();
void bsp_update();
