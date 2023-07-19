#pragma once
#include "bsp_defs.h"

#define LCD_FRAMEBUFFER0_SIZE 184320
extern uint32_t lcd_framebuffer0[LCD_FRAMEBUFFER0_SIZE];

bsp_result_t graphics_init();
void gfx_draw_fillrect(uint32_t x_pos, uint32_t y_pos, uint32_t width, uint32_t height, uint32_t color);
void gfx_fillscreen(uint32_t color);
void gfx_clearscreen();

void gfx_prepare();










//CopyBuffer((uint32_t *)Images[ImageIndex ++], (uint32_t *)LCD_FRAME_BUFFER, 67, 140, IMAGE_WIDTH, IMAGE_HEIGHT);


//static void CopyBuffer(uint32_t *pSrc, uint32_t *pDst, uint16_t x, uint16_t y, uint16_t xsize, uint16_t ysize)
//{
//
//  uint32_t destination = (uint32_t)pDst + (y * 800 + x) * 4;
//  uint32_t source      = (uint32_t)pSrc;
//
//  /*##-1- Configure the DMA2D Mode, Color Mode and output offset #############*/
//  hdma2d.Init.Mode         = DMA2D_M2M;
//  hdma2d.Init.ColorMode    = DMA2D_ARGB8888;
//  hdma2d.Init.OutputOffset = 800 - xsize;
//
//  /*##-2- DMA2D Callbacks Configuration ######################################*/
//  hdma2d.XferCpltCallback  = NULL;
//
//  /*##-3- Foreground Configuration ###########################################*/
//  hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
//  hdma2d.LayerCfg[1].InputAlpha = 0xFF;
//  hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888;
//  hdma2d.LayerCfg[1].InputOffset = 0;
//
//  hdma2d.Instance          = DMA2D;
//
//  /* DMA2D Initialization */
//  if(HAL_DMA2D_Init(&hdma2d) == HAL_OK)
//  {
//    if(HAL_DMA2D_ConfigLayer(&hdma2d, 1) == HAL_OK)
//    {
//      if (HAL_DMA2D_Start(&hdma2d, source, destination, xsize, ysize) == HAL_OK)
//      {
//        /* Polling For DMA transfer */
//        HAL_DMA2D_PollForTransfer(&hdma2d, 100);
//      }
//    }
//  }
//}
