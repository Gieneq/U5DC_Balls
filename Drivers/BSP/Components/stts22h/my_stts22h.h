#pragma once
#include "bsp_defs.h"
#include "i2c_bus.h"


//todo pass pointer to common I2C bu
bsp_result_t my_stts22h_init();
void my_stts2h_update();
float my_stts2h_get_temperature();
