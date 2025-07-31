/**
 * @file EVE_Hal.h
 * @brief Main file to include the EVE HAL in your applications
 *
 * @author Bridgetek
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

#ifndef EVE_HAL__H
#define EVE_HAL__H

/*************
** INCLUDES **
*************/
#include "EVE_HalImpl.h"

/**********************
** GLOBAL PROTOTYPES **
**********************/

/** @name INIT */
///@{
/** Initialize HAL platform */
EVE_HalPlatform *EVE_Hal_initialize();
/** Release HAL platform */
void EVE_Hal_release();
/** Get the default configuration parameters */
void EVE_Hal_defaults(EVE_HalParameters *parameters);
/** Opens a new HAL context using the specified parameters */
bool EVE_Hal_open(EVE_HalContext *phost, const EVE_HalParameters *parameters);
/** Close a HAL context */
void EVE_Hal_close(EVE_HalContext *phost);
/** Idle. Call regularly to update frequently changing internal state.
This is also called while waiting for cmd, in addition to the user idle callback */
void EVE_Hal_idle(EVE_HalContext *phost);
///@}

/** @name TRANSFER HELPERS */
///@{
uint32_t EVE_Hal_rd32(EVE_HalContext *phost, uint32_t addr);
void EVE_Hal_rdMem(EVE_HalContext *phost, uint8_t *result, uint32_t addr, uint32_t size);

void EVE_Hal_wr32(EVE_HalContext *phost, uint32_t addr, uint32_t v);
void EVE_Hal_wrMem(EVE_HalContext *phost, uint32_t addr, const uint8_t *buffer, uint32_t size);
void EVE_Hal_wrString(EVE_HalContext *phost, uint32_t addr, const char *str, uint32_t index, uint32_t size, uint32_t padMask);
///@}

/** @name SPI command */
///@{
void EVE_Hal_SPICmd_pwr_state(EVE_HalContext *phost, EVE_PWR_STATE_T state);
void EVE_Hal_SPICmd_setpllsp1(EVE_HalContext *phost, EVE_CMD_SYS_PLL_FREQ val);
void EVE_Hal_SPICmd_sysclkdiv(EVE_HalContext *phost, EVE_CMD_SYS_CLK_DIV val);
void EVE_Hal_SPICmd_setbootcfg(EVE_HalContext *phost, EVE_CMD_BOOT_CFG val);
void EVE_Hal_SPICmd_bootcfgen(EVE_HalContext *phost, EVE_CMD_BOOT_CFG_EN val);
void EVE_Hal_SPICmd_setddrtype(EVE_HalContext *phost, EVE_CMD_DDR_TYPE val);
///@}

#endif /* #ifndef EVE_HAL__H */

/* end of file */
