/**
 * @file Common.h
 * @brief Common functions
 *
 * @author
 *
 * @date 2024
 * 
 * MIT License
 *
 * Copyright (c) [2024] [Bridgetek Pte Ltd (BRTChip)]
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

#ifndef COMMON_H_
#define COMMON_H_

/*********************
 *      INCLUDES
 *********************/
#include "EVE_CoDl.h"
#include "EVE_Util.h"
#include "EVE_LoadFile.h"
#include "EVE_MediaFifo.h"

/**********************
 *  EXTERN VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/
/// Alignment
#define __ALIGN_MASK(x,mask)                (((x)+(mask))&~(mask))
#define ALIGN(x,a)                          __ALIGN_MASK(x, a - 1)

// example for memory map for assets
#define DDR_BITMAPS_STARTADDR (9 << 20)   /**< from 9M(size : 9M) */
#define DDR_BITMAPS_STARTADDR1 (18 << 20) /**< from 18M(size : 9M) */
#define DDR_BITMAPS_STARTADDR2 (27 << 20) /**< from 27M(size : 9M) */

#define SC0_PTR0_STARTADDR (36 << 20) /**< from 36M(size : 9M) */
#define SC0_PTR1_STARTADDR (45 << 20) /**< from 45M	(size: 9M) */
#define SC1_PTR0_STARTADDR (54 << 20) /**< from 54M	(size: 9M) */
#define SC1_PTR1_STARTADDR (63 << 20) /**< from 63M	(size: 9M) */
#define SC2_PTR0_STARTADDR (72 << 20) /**< from 72M	(size: 9M) */
#define SC2_PTR1_STARTADDR (81 << 20) /**< from 81M	(size: 9M) */

#define DDR_FRAMEBUFFER_STARTADDR (72 << 20) /**< from 72M(size : 9M) */

typedef enum
{
    MODE_PICTURE = 1, /**< 1 */
    MODE_DIRECTVIDEO, /**< 2 */
    MODE_VIDEO,       /**< 3 */
    MODE_LVDSRX,      /**< 4 */
    MODE_LVDSRX_SC    /**< 5 */
} Display_mode;

/**********************
 *  GLOBAL PROTOTYPES
 **********************/
void Gpu_Init(EVE_HalContext *phost);
void Gpu_Release(EVE_HalContext *phost);
int32_t Gpu_Hal_Dec2Ascii(char *pSrc, size_t buf_size, int32_t value);

bool EVE_Calibrate(EVE_HalContext *phost);
void Calibration_Restore(EVE_HalContext *phost);
void Calibration_Save(EVE_HalContext *phost);
void Display_Start(EVE_HalContext* phost, uint8_t* bgColor, uint8_t* textColor, uint32_t tag, uint8_t frac);
void Display_End(EVE_HalContext *phost);
void Draw_TextColor(EVE_HalContext* phost, const char* str, uint8_t* bgColor, uint8_t* textColor);
void Draw_Text(EVE_HalContext *phost, const char *str);
void Play_Sound(EVE_HalContext *phost, uint8_t sound, uint8_t vol, uint8_t midi);
uint8_t Show_Diaglog_YesNo(EVE_HalContext *phost, const uint8_t* title, const uint8_t *msg);
void Show_Diaglog_Info(EVE_HalContext *phost, const uint8_t *msg);
uint32_t Flash_Init(EVE_HalContext *phost, const uint8_t *file);
void WelcomeScreen(EVE_HalContext *phost, char *info[]);
void Display_Config(EVE_HalContext *phost, uint16_t format, Display_mode mode);
#if defined(RP2040_PLATFORM)
void strcat_s(char* dest, size_t dest_size, const char* src);
#endif
#endif /* COMMON_H_ */
