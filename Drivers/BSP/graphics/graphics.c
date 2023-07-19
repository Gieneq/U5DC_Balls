#include "graphics.h"
#include "main.h"
#include "ltdc.h"
#include "dsihost.h"
#include "color_pallete.h"
#include "microtimer.h"
#include "gfxmmu.h"
#include "dma2d.h"
#include <math.h>

#define GFXMMU_FB_SIZE_TEST 730848

extern LTDC_HandleTypeDef hltdc;
extern DSI_HandleTypeDef hdsi;
extern DMA2D_HandleTypeDef hdma2d;

static uint32_t SetPanelConfig(void);
static void DMA2D_FillRect(uint32_t color, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
static __IO uint32_t last_ltdc_line_event_us;
static __IO float ltdc_line_event_frequency_hz;
static __IO float ltdc_line_event_interval_ms;


static __IO uint32_t ltdc_backporch_enter_us;
static __IO uint32_t ltdc_frontporch_enter_us;
static __IO float ltdc_rendering_time_ms;
static __IO float ltdc_blanking_period_ms;

static __IO int32_t ltdc_clear_sreen_duriation_us;
static int line_y_pos = 0;

static __IO uint32_t current_ltdc_x;
static __IO uint32_t current_ltdc_y;

//#if defined(__ICCARM__)
//#pragma location =  0x200D0000
//#elif defined ( __GNUC__ )
//__attribute__((section (".RAM1_D")))
//#endif
//ALIGN_32BYTES (uint32_t   lcd_framebuffer0[184320]);

uint32_t lcd_framebuffer0[LCD_FRAMEBUFFER0_SIZE];

//int counter_1 = 0;
//int counter_2 = 0;
//int counter_3 = 0;
//
//void HAL_DSI_TearingEffectCallback(DSI_HandleTypeDef *hdsi) {
//++counter_1;
//}
//
//void HAL_DSI_EndOfRefreshCallback(DSI_HandleTypeDef *hdsi) {
//++counter_2;
//}
//
//
//void HAL_DSI_ErrorCallback(DSI_HandleTypeDef *hdsi) {
//++counter_3;
//}

uint32_t tmp_y = 0;
#define PENDING_BUFFER_NONE -1
#define PENDING_BUFFER_SOME  0
//#define VSYNC_LINE_IDX 0
static uint32_t pend_buffer = PENDING_BUFFER_NONE;

#define LOCATION_FRONTPORCH 12 //(_hltdc)  ()hltdc   //12
#define LOCATION_BACKPORCH  480 //  (_hltdc)   ()        //482
static uint32_t ltdc_line_current_location = LOCATION_FRONTPORCH;


bsp_result_t graphics_init() {
	if(SetPanelConfig() != 0) {
		return BSP_ERROR;
	}
	gfx_clearscreen();
//	gfx_draw_fillrect(0, 70, 400, 80, COLOR_RED);

	/* VSync stuff */
	HAL_LTDC_ProgramLineEvent(&hltdc, LOCATION_FRONTPORCH);
	pend_buffer = PENDING_BUFFER_NONE;
	last_ltdc_line_event_us = microtimer_get_us();

	return BSP_OK;
}



void HAL_LTDC_LineEventCallback(LTDC_HandleTypeDef *hltdc) {
	/* Finding LTDC location */
	current_ltdc_y = hltdc->Instance->CPSR & 0xFFFF;
	current_ltdc_x = (hltdc->Instance->CPSR >> 16) & 0xFFFF;

	if(ltdc_line_current_location == LOCATION_FRONTPORCH) {
		/* Measure blanking time */
		ltdc_frontporch_enter_us = microtimer_get_us();
		ltdc_blanking_period_ms = (ltdc_frontporch_enter_us - ltdc_backporch_enter_us) / 1000.0F;

		/* Measure interval & frequency */
		ltdc_line_event_interval_ms = (microtimer_get_us() - last_ltdc_line_event_us) / 1000.0F;
		ltdc_line_event_frequency_hz = 1000.0F / ltdc_line_event_interval_ms;
		last_ltdc_line_event_us = microtimer_get_us();
		pend_buffer = PENDING_BUFFER_NONE;
		HAL_LTDC_ProgramLineEvent(hltdc, LOCATION_BACKPORCH);
		ltdc_line_current_location = LOCATION_BACKPORCH;
	}

	else if(ltdc_line_current_location == LOCATION_BACKPORCH) {
		/* Measure rendering time */
		ltdc_backporch_enter_us = microtimer_get_us();
		ltdc_rendering_time_ms = (ltdc_backporch_enter_us - ltdc_frontporch_enter_us) / 1000.0F;
		HAL_LTDC_ProgramLineEvent(hltdc, LOCATION_FRONTPORCH);
		ltdc_line_current_location = LOCATION_FRONTPORCH;
	}



	if(pend_buffer == PENDING_BUFFER_SOME) {
//		LTDC_LAYER(hltdc, 0)->CFBAR = ((uint32_t)Buffers[pend_buffer]);
//		__HAL_LTDC_RELOAD_IMMEDIATE_CONFIG(hltdc);
//		front_buffer = pend_buffer;
		pend_buffer = PENDING_BUFFER_NONE;
	}

}



/**
  * @brief  Check for user input.
  * @param  None
  * @retval Input state (1 : active / 0 : Inactive)
  */
static uint32_t SetPanelConfig(void) {
  if(HAL_DSI_Start(&hdsi) != HAL_OK) return 1;

  /* CMD Mode */
  uint8_t InitParam1[3] = {0xFF ,0x83 , 0x79};
  if (HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 3, 0xB9, InitParam1) != HAL_OK) return 1;

  /* SETPOWER */
  uint8_t InitParam3[16] = {0x44,0x1C,0x1C,0x37,0x57,0x90,0xD0,0xE2,0x58,0x80,0x38,0x38,0xF8,0x33,0x34,0x42};
  if (HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 16, 0xB1, InitParam3) != HAL_OK) return 2;

  /* SETDISP */
  uint8_t InitParam4[9] = {0x80,0x14,0x0C,0x30,0x20,0x50,0x11,0x42,0x1D};
  if (HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 9, 0xB2, InitParam4) != HAL_OK) return 3;

  /* Set display cycle timing */
  uint8_t InitParam5[10] = {0x01,0xAA,0x01,0xAF,0x01,0xAF,0x10,0xEA,0x1C,0xEA};
  if (HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 10, 0xB4, InitParam5) != HAL_OK) return 4;

  /* SETVCOM */
  uint8_t InitParam60[4] = {00,00,00,0xC0};
  if (HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 4, 0xC7, InitParam60) != HAL_OK) return 5;

  /* Set Panel Related Registers */
  if (HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xCC, 0x02) != HAL_OK) return 6;

  if(HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xD2, 0x77) != HAL_OK) return 7;

  uint8_t InitParam50[37] = {0x00,0x07,0x00,0x00,0x00,0x08,0x08,0x32,0x10,0x01,0x00,0x01,0x03,0x72,0x03,0x72,0x00,0x08,0x00,0x08,0x33,0x33,0x05,0x05,0x37,0x05,0x05,0x37,0x0A,0x00,0x00,0x00,0x0A,0x00,0x01,0x00,0x0E};
  if (HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 37, 0xD3, InitParam50) != HAL_OK) return 8;

  uint8_t InitParam51[34] = {0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x19,0x19,0x18,0x18,0x18,0x18,0x19,0x19,0x01,0x00,0x03,0x02,0x05,0x04,0x07,0x06,0x23,0x22,0x21,0x20,0x18,0x18,0x18,0x18,0x00,0x00};
  if (HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 34, 0xD5, InitParam51) != HAL_OK) return 9;

  uint8_t InitParam52[35] = {0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x19,0x19,0x18,0x18,0x19,0x19,0x18,0x18,0x06,0x07,0x04,0x05,0x02,0x03,0x00,0x01,0x20,0x21,0x22,0x23,0x18,0x18,0x18,0x18};
  if (HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 35, 0xD6, InitParam52) != HAL_OK) return 10;

  /* SET GAMMA */
  uint8_t InitParam8[42] = {0x00,0x16,0x1B,0x30,0x36,0x3F,0x24,0x40,0x09,0x0D,0x0F,0x18,0x0E,0x11,0x12,0x11,0x14,0x07,0x12,0x13,0x18,0x00,0x17,0x1C,0x30,0x36,0x3F,0x24,0x40,0x09,0x0C,0x0F,0x18,0x0E,0x11,0x14,0x11,0x12,0x07,0x12,0x14,0x18};
  if (HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 42, 0xE0, InitParam8) != HAL_OK) return 11;

  uint8_t InitParam44[3] = {0x2C,0x2C,00};
  if (HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 3, 0xB6, InitParam44) != HAL_OK) return 12;

  if (HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xBD, 0x00) != HAL_OK) return 13;

  uint8_t InitParam14[] = {0x01,0x00,0x07,0x0F,0x16,0x1F,0x27,0x30,0x38,0x40,0x47,0x4E,0x56,0x5D,0x65,0x6D,0x74,0x7D,0x84,0x8A,0x90,0x99,0xA1,0xA9,0xB0,0xB6,0xBD,0xC4,0xCD,0xD4,0xDD,0xE5,0xEC,0xF3,0x36,0x07,0x1C,0xC0,0x1B,0x01,0xF1,0x34,0x00};
  if (HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 42, 0xC1, InitParam14) != HAL_OK) return 14;

  if (HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xBD, 0x01) != HAL_OK) return 15;

  uint8_t InitParam15[] = {0x00,0x08,0x0F,0x16,0x1F,0x28,0x31,0x39,0x41,0x48,0x51,0x59,0x60,0x68,0x70,0x78,0x7F,0x87,0x8D,0x94,0x9C,0xA3,0xAB,0xB3,0xB9,0xC1,0xC8,0xD0,0xD8,0xE0,0xE8,0xEE,0xF5,0x3B,0x1A,0xB6,0xA0,0x07,0x45,0xC5,0x37,0x00};
  if (HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 42, 0xC1, InitParam15) != HAL_OK) return 16;

  if (HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xBD, 0x02) != HAL_OK) return 17;

  uint8_t InitParam20[42] = {0x00,0x09,0x0F,0x18,0x21,0x2A,0x34,0x3C,0x45,0x4C,0x56,0x5E,0x66,0x6E,0x76,0x7E,0x87,0x8E,0x95,0x9D,0xA6,0xAF,0xB7,0xBD,0xC5,0xCE,0xD5,0xDF,0xE7,0xEE,0xF4,0xFA,0xFF,0x0C,0x31,0x83,0x3C,0x5B,0x56,0x1E,0x5A,0xFF};
  if (HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 42, 0xC1, InitParam20) != HAL_OK) return 18;

  if (HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xBD, 0x00) != HAL_OK) return 19;

  /* Exit Sleep Mode*/
  if (HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P0, 0x11, 0x00) != HAL_OK) return 20;

  HAL_Delay(120);

  /* Clear LCD_FRAME_BUFFER */
//  memset((uint32_t *)LCD_FRAME_BUFFER,0x00, 0xBFFFF);

  /* Display On */
  if (HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P0, 0x29, 0x00) != HAL_OK) return 21;

  HAL_Delay(120);

  /* All setting OK */
  return 0;
}

void gfx_draw_fillrect(uint32_t x_pos, uint32_t y_pos, uint32_t width, uint32_t height, uint32_t color) {
//  uint32_t  px_address = 0;
//  uint32_t  i;
//  uint32_t  j;
//
//  /* Get the rectangle start address */
//  uint32_t startaddress = (hltdc.LayerCfg[0].FBStartAdress + (4 * (y_pos * PIXEL_PERLINE + x_pos)));
//
//  /* Fill the rectangle */
//  for (i = 0; i < height; i++) {
//    px_address = startaddress + (3072 * i); //768 * 4
//    for (j = 0; j < width; j++) {
//    	      *(__IO uint32_t *)(px_address) = color;
//      px_address += 4;
//    }
//  }
	DMA2D_FillRect(color, x_pos, y_pos, width, height);
}

static void DMA2D_FillRect(uint32_t color, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
	hdma2d.Init.Mode = DMA2D_R2M;
	hdma2d.Init.ColorMode = DMA2D_OUTPUT_ARGB8888;
	hdma2d.Init.OutputOffset = PIXEL_PERLINE - width;
	HAL_DMA2D_Init(&hdma2d);
	HAL_DMA2D_Start(
		&hdma2d,
		color,
		hltdc.LayerCfg[0].FBStartAdress + 4 * (y * PIXEL_PERLINE + x),
		width,
		height
	);
	HAL_DMA2D_PollForTransfer(&hdma2d, 100);
}

void gfx_fillscreen(uint32_t color) {
	DMA2D_FillRect(color, 0, 0, LCD_WIDTH, LCD_HEIGHT);
}

void gfx_clearscreen() {
	gfx_fillscreen(COLOR_BLACK);
}


//	memset(lcd_framebuffer0, 0, LCD_FRAMEBUFFER0_SIZE * sizeof(uint32_t));
//	memset((void*)hltdc.LayerCfg[0].FBStartAdress, 0x00, GFXMMU_FB_SIZE_TEST * sizeof(uint32_t)/2);

//	gfx_draw_fillrect(10, line_y_pos++, LCD_WIDTH-20, 40, 0xffff00ff);
//	if(line_y_pos > (LCD_HEIGHT - 40)) {
//		line_y_pos = 0;
//		memset((void*)hltdc.LayerCfg[0].FBStartAdress, 0x00, GFXMMU_FB_SIZE_TEST * sizeof(uint32_t)/2);

static float fi = 0;
static float radius = 140;
static float cx = LCD_WIDTH/2;
static float cy = LCD_HEIGHT/2;
static uint32_t size = 50;
static float freq = 0.5F;
static float t = 0;
void gfx_prepare() {
	int32_t ltdc_clear_sreen_start_us = microtimer_get_us();
	/* Clear screen */
	gfx_fillscreen(0xFF000000); // BLACK

	t += REFRESH_INTERVAL_US/1000000.0F;
	uint32_t sq_xy = (uint32_t)(cx + radius * sinf(2.0F*3.1415F*freq*t)) - size/2;
	gfx_draw_fillrect(sq_xy, sq_xy, size, size, 0xff0000ff);


	uint32_t cq_x = (uint32_t)(cx + radius * sinf(2.0F*3.1415F*freq*t)) - size/2;
	uint32_t cq_y = (uint32_t)(cx + radius * cosf(2.0F*3.1415F*freq*t)) - size/2;
	gfx_draw_fillrect(cq_x, cq_y, size, size, 0xffff0000);

	ltdc_clear_sreen_duriation_us = microtimer_get_us() - ltdc_clear_sreen_start_us;
}
