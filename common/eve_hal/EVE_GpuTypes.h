/**
 * @file EVE_GpuTypes.h
 * @brief Defines EVE hardware values
 *
 * @author Bridgetek
 *
 * @date 2018
 * 
 * MIT License
 *
 * Copyright (c) [2019] [Bridgetek Pte Ltd (BRTChip)]
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#ifndef EVE_GPU_TYPES__H
#define EVE_GPU_TYPES__H

/*********************
 *      INCLUDES
 *********************/
#include "EVE_Config.h"

/***********
** MACROS **
***********/

#define EVE_GPU_NUMCHAR_PERFONT (128)
#define EVE_GPU_FONT_TABLE_SIZE (148)

#define EVE_82X_RESET_ACTIVE 0x000268
#define EVE_82X_RESET_REMOVAL 0x002068

/**********************
 *      TYPEDEFS
 **********************/

/* SPI channel options */
typedef enum EVE_SPI_CHANNELS_T
{
	EVE_SPI_SINGLE_CHANNEL = 0x00, /**< 0x00 */
	EVE_SPI_DUAL_CHANNEL = 0x01, /**< 0x01 */
	EVE_SPI_QUAD_CHANNEL = 0x02, /**< 0x02 */
} EVE_SPI_CHANNELS_T;

/* APIs for Host Commands */
typedef enum EVE_82X_PLL_FREQ_T
{
	EVE_SYSCLK_DEFAULT = 0x00, /**< 0x00 */
	EVE_SYSCLK_84M = 0x07, /**< 0x07 */
	EVE_SYSCLK_72M = 0x06, /**< 0x06 */
	EVE_SYSCLK_60M = 0x05, /**< 0x05 */
	EVE_SYSCLK_48M = 0x04, /**< 0x04 */
	EVE_SYSCLK_36M = 0x03, /**< 0x03 */
	EVE_SYSCLK_24M = 0x02, /**< 0x02 */
} EVE_82X_PLL_FREQ_T;

typedef enum EVE_82X_ROM_AND_ADC_T
{
	EVE_MAIN_ROM = 0x80, /**< 0x08, main graphicas ROM used */
	EVE_RCOSATAN_ROM = 0x40, /**< 0x40, line slope table used for */
	EVE_SAMPLE_ROM = 0x20, /**< 0x20, JA samples */
	EVE_JABOOT_ROM = 0x10, /**< 0x10, JA microcode */
	EVE_J1BOOT_ROM = 0x08, /**< 0x08, J1 microcode */
	EVE_ADC = 0x01, /**< 0x01 */
	EVE_POWER_ON_ROM_AND_ADC = 0x00, /**< 0x00, specify this element to power on all ROMs and ADCs */
} EVE_82X_ROM_AND_ADC_T;

typedef enum EVE_82X_GPIO_DRIVE_STRENGTH_T
{
	EVE_5MA = 0x00, /**< 0x00, default current */
	EVE_10MA = 0x01, /**< 0x01 */
	EVE_15MA = 0x02, /**< 0x02 */
	EVE_20MA = 0x03, /**< 0x03 */
} EVE_82X_GPIO_DRIVE_STRENGTH_T;

typedef enum EVE_82X_GPIO_GROUP_T
{
	EVE_GPIO0 = 0x00, /**< 0x00 */
	EVE_GPIO1 = 0x04, /**< 0x04 */
	EVE_GPIO2 = 0x08, /**< 0x08 */
	EVE_GPIO3 = 0x0C, /**< 0x0C */
	EVE_GPIO4 = 0x10, /**< 0x10 */
	EVE_DISP = 0x20, /**< 0x20 */
	EVE_DE = 0x24, /**< 0x24 */
	EVE_VSYNC_HSYNC = 0x28, /**< 0x28 */
	EVE_PCLK = 0x2C, /**< 0x2C */
	EVE_BACKLIGHT = 0x30, /**< 0x30 */
	EVE_R_G_B = 0x34, /**< 0x34 */
	EVE_AUDIO_L = 0x38, /**< 0x38 */
	EVE_INT_N = 0x3C, /**< 0x3C */
	EVE_TOUCHWAKE = 0x40, /**< 0x40 */
	EVE_SCL = 0x44, /**< 0x44 */
	EVE_SDA = 0x48, /**< 0x48 */
	EVE_SPI_MISO_MOSI_IO2_IO3 = 0x4C, /**< 0x4C */
} EVE_82X_GPIO_GROUP_T;

/** Legacy font table structure \n
 * Font table address in ROM can be found by reading the address from top 256 Bytes in DDR RAM. \n
 * All the values are in bytes
 */
typedef struct EVE_Gpu_Fonts
{
	/* All the values are in bytes */
	/** Width of each character font from 0 to 127 */
	uint8_t FontWidth[EVE_GPU_NUMCHAR_PERFONT];
	/** Bitmap format of font wrt bitmap formats - L1, L4, L8 */
	uint16_t FontBitmapFormat;
	/** font flags */
	uint16_t FontFlags;
	/** Font line stride */
	uint32_t FontLineStride;
	/** Font width in pixels */
	uint32_t FontWidthInPixels;
	/** Font height in pixels */
	uint32_t FontHeightInPixels;
	/** Pointer to font graphics raw data */
	uint32_t PointerToFontGraphicsData;
} EVE_Gpu_Fonts;

#if defined(EVE_SUPPORT_UNICODE)
#define XF_GPTR(xf) ((unsigned int *)&(((int *)xf)[10]))
#define XF_WPTR(xf)((unsigned int *)&(((char *)xf)[40 + 4 * (xf->number_of_characters / 128)]))
#define XF_WIDTH(xf) ((unsigned char *)&(((char *)xf)[40 + 8 * (xf->number_of_characters / 128)]))

/** extended format 1 font table for unicode support */
typedef struct EVE_Gpu_FontsExt
{
	uint32_t signature;
	uint32_t size;
	uint16_t format;
	uint16_t flags;
	uint32_t swizzle;
	uint32_t layout_width; // Font bitmap line stride, in bytes 
	uint32_t layout_height;// Font bitmap height, in pixels 
	uint32_t pixel_width; // Font screen width, in pixels 
	uint32_t pixel_height; // Font screen height, in pixels 
	uint32_t start_of_graphic_data;// Pointer to font graphic data 
	uint32_t number_of_characters; // Total number of characters in font: N (multiple of 128)
} EVE_Gpu_FontsExt;

#define XF2_OPTR(xf2) ((uint32_t *)&(((uint32_t *)xf2)[11]))
/** extended format 2 font table */
typedef struct EVE_Gpu_FontsExt2
{
	uint32_t signature;
	uint32_t size;
	uint16_t format;
	uint16_t flags;
	uint32_t swizzle;
	uint32_t layout_width;
	uint32_t layout_height;
	uint32_t pixel_width;
	uint32_t pixel_height;
	uint16_t xPadding;
	uint16_t leading;
	uint32_t number_of_characters;
	uint16_t midline;
	uint16_t baseline;
}EVE_Gpu_FontsExt2;

typedef struct EVE_FontsExt2_kern
{
	uint32_t code;
	uint32_t distance;
} EVE_FontsExt2_kern;

typedef struct EVE_FontsExt2_chblk
{
	uint32_t glyph;
	uint32_t width;
	struct EVE_FontsExt2_kern kerns[1];
} EVE_FontsExt2_chblk;
#endif

#endif /* #ifndef EVE_GPU_TYPES__H */

/* end of file */