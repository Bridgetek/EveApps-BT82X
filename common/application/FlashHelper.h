/**
 * @file FlashHelper.h
 * @brief EVE's connected flash helper functions
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

#ifndef FLASH_HELPER__H
#define FLASH_HELPER__H

#include "Common.h"

typedef enum
{
    FLASH_CMD_SUCCESS = 0, /**< 0 */
    FLASH_CMD_ALIGNED_ERR, /**< 1 */
    FLASH_CMD_ERR /**< 2 */
} Flash_Cmd_Status_t;

void FlashHelper_Erase(EVE_HalContext *phost);
Flash_Cmd_Status_t FlashHelper_Program(EVE_HalContext *phost, uint32_t dest_flash, uint32_t src_ram, uint32_t num);
Flash_Cmd_Status_t FlashHelper_Read(EVE_HalContext *phost, uint32_t dest_ram, uint32_t src_flash, uint32_t num, uint8_t *read_data);
Flash_Cmd_Status_t FlashHelper_Write(EVE_HalContext *phost, uint32_t dest_flash, uint32_t num, const uint8_t *write_data);

#ifdef EVE_FLASH_NAND
uint32_t FlashHelper_GetBadBlockNum(EVE_HalContext *phost);
Flash_Cmd_Status_t FlashHelper_EraseBlocks(EVE_HalContext *phost, uint32_t addr, uint32_t len);
int32_t FlashHelper_GetAddrAvail(EVE_HalContext *phost, uint32_t size, uint32_t addr);
#else

#define FLASH_WRITE_ALIGN_BYTE (256)
#define FLASH_UPDATE_ALIGN_BYTE (4096)
#define FLASH_READ_ALIGN_BYTE (64)

Flash_Cmd_Status_t FlashHelper_Update(EVE_HalContext *phost, uint32_t dest_flash, uint32_t src_ram, uint32_t num);
#endif

uint32_t FlashHelper_GetState(EVE_HalContext *phost);
uint32_t FlashHelper_SwitchState(EVE_HalContext *phost, uint8_t nextState);
uint32_t FlashHelper_SwitchFullMode(EVE_HalContext *phost);

#endif /* FLASH_HELPER__H */
/* end of file */
