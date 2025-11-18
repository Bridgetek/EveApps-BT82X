/**
 * @file FlashHelper.c
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

#include "FlashHelper.h"
#include "Common.h"
#include "PatchDefault.h"

/**
 * @brief Write data to flash. 
 *
 * Constraints:
 * - Destination flash address must be virgin (not used before)
 * - data array must be aligned 256-bit
 *
 * @param phost Pointer to Hal context
 * @param dest Destination on flash
 * @param num Number of byte to write
 * @param data Data buffer to write
 */
void FlashHelper_flashWriteExt(EVE_HalContext *phost, uint32_t dest, uint32_t num, const uint8_t *data)
{
    uint32_t send_data32 = 0;

    EVE_Cmd_startFunc(phost);
    EVE_Cmd_wr32(phost, CMD_FLASHWRITE);
    EVE_Cmd_wr32(phost, dest);
    EVE_Cmd_wr32(phost, num);
    for (uint32_t i = 0; i < num; i = i + 4)
    {
        /* Pack 4 bytes into a 32-bit data each sending package */
        send_data32 = *data++;
        send_data32 |= (*data++) << 8;
        send_data32 |= (*data++) << 16;
        send_data32 |= (*data++) << 24;
        EVE_Cmd_wr32(phost, send_data32);
    }
    EVE_Cmd_endFunc(phost);
}

#ifdef EVE_FLASH_NAND

static bool nand_init = false;
static uint32_t bad_block_tbl[64] = { 0 };
static uint32_t page_sz = 2048;
static uint32_t page_num = 64;
static uint32_t block_num = 1024;

/**
 * @brief Get flash information
 *
 * @param phost Pointer to Hal context
 * @param page_sz Page size
 * @param num_of_page Page number
 * @param num_of_block Block number
 */
static void FlashHelper_GetParamters(EVE_HalContext *phost, uint32_t *page_sz, uint32_t *num_of_page, uint32_t *num_of_block)
{
    EVE_CMD_NFLASHPARAMETERS(phost, RAM_G);
    EVE_Cmd_waitFlush(phost);

    *page_sz = EVE_Hal_rd32(phost, 80);
    *num_of_page = EVE_Hal_rd32(phost, 92);
    *num_of_block = EVE_Hal_rd32(phost, 96);
    eve_printf_debug("page size %ld, num %ld, block num %ld\n", *page_sz, *num_of_page, *num_of_block);
}

/**
 * @brief Get bad block number
 *
 * @param phost Pointer to Hal context
 * @return uint32_t Bad block number
 */
uint32_t FlashHelper_GetBadBlockNum(EVE_HalContext *phost)
{
    uint32_t page_sz = 0;
    uint32_t page_num = 0;
    uint32_t block_num = 0;
    uint32_t bad_block_num = 0;

    FlashHelper_GetParamters(phost, &page_sz, &page_num, &block_num);
    for (uint32_t i = 0; i < block_num; i++)
    {
        uint32_t err = EVE_CMD_NFLASHISBLOCKBAD(phost, i, 0);
        if (err != 0)
        {
            bad_block_num++;
            bad_block_tbl[i / 8] |= 0x1 << i % 8;
        }
    }
    //for (int i = 0; i < 32; i++)
    //{
    //    eve_printf_debug("%lx ", bad_block_tbl[i]);
    //}
    //eve_printf_debug("\n");
    nand_init = true;
    return bad_block_num;
}

/**
 * @brief Erase entire flash
 * @warning This function erases the entire Flash, which means the bad block information will be lost. Please use with caution.
 *
 * @param phost Pointer to Hal context
 */
void FlashHelper_Erase(EVE_HalContext *phost)
{
    if (Show_Diaglog_YesNo(phost, "Flash programming",
            "Continue to erase the whole Flash?"))
    {
        EVE_CoCmd_flashErase(phost);
        EVE_Cmd_waitFlush(phost);
    }
}

/**
 * @brief Erase flash by block number
 *
 * @param phost Pointer to Hal context
 * @param block_no Block number to erase
 */
static Flash_Cmd_Status_t FlashHelper_EraseBlock(EVE_HalContext *phost, uint32_t block_no)
{
    if (nand_init == false)
    {
        FlashHelper_GetBadBlockNum(phost);
    }

    if ((bad_block_tbl[block_no / 8] & (0x1 << (block_no % 8))) != 0)
    {
        eve_printf_debug("Bad block can't be erased.\n");
        return FLASH_CMD_ERR;
    }
    EVE_CMD_NFLASHERASEBLOCK(phost, block_no);
    uint32_t status = 0;
    status = EVE_CMD_NFLASHSTATUS(phost, status);
    if ((status & 0x04) != 0)
    {
        eve_printf_debug("NAND FLASH erase failed.\n");
        return FLASH_CMD_ERR;
    }
    return FLASH_CMD_SUCCESS;
}

/**
 * @brief Erase flash by block number
 *
 * @param phost Pointer to Hal context
 * @param address Flash address
 * @param size Size to erase
 */
Flash_Cmd_Status_t FlashHelper_EraseBlocks(EVE_HalContext *phost, uint32_t addr, uint32_t size)
{
    if (nand_init == false)
    {
        FlashHelper_GetBadBlockNum(phost);
    }

    uint32_t block_sz = page_sz * page_num;
    uint32_t last_chunk = (size % block_sz);
    uint32_t num_of_block = (last_chunk > 0) ? (size / block_sz + 1) : (size / block_sz);
    uint32_t block_no = addr / block_sz;
    Flash_Cmd_Status_t status;

    for (uint32_t i = block_no; i < block_no + num_of_block; i++)
    {
        status = FlashHelper_EraseBlock(phost, i);
        if (status != FLASH_CMD_SUCCESS)
            return FLASH_CMD_ERR;
    }
    return FLASH_CMD_SUCCESS;
}

/**
 * @brief Writes the given data to flash.
 * It assumes that the flash is previously programmed to all-ones.
 *
 * @note
 * dest_flash: destination address in flash memory. Must be page size aligned \n
 * src_ram: source data in main memory. Must be 4-byte aligned \n
 * num: number of bytes to write, should be multiple of page size, otherwise, dummy data will be padded
 *
 * @param phost Pointer to Hal context
 * @param dest_flash Destination on flash
 * @param src_ram Source in RAM_G
 * @param num Numbber of byte to write
 * @return Flash_Cmd_Status_t
 */
Flash_Cmd_Status_t FlashHelper_Program(EVE_HalContext *phost, uint32_t dest_flash, uint32_t src_ram, uint32_t num)
{
    if (nand_init == false)
    {
        FlashHelper_GetBadBlockNum(phost);
    }
    uint32_t last_chunk = (num % page_sz); /* Remaining bytes that do not fill a complete page */
    uint32_t pages = 0;

    if ((dest_flash % page_sz != 0) || ((src_ram % 4) != 0)) /* Check aligned address */
    {
        return FLASH_CMD_ALIGNED_ERR;
    }

    if (last_chunk == 0) /* num is multiple of page size */
    {
        pages = num / page_sz;
    }
    else /* num is not fit in multiple of page size */
    {
        EVE_CoCmd_memSet(phost, src_ram + num, 0xff, page_sz - last_chunk);
        pages = num / page_sz + 1;
    }

    // program page by page
    for (uint32_t i = 0; i < pages; i++)
    {
        EVE_CoCmd_flashProgram(phost, dest_flash + i * page_sz, src_ram + i * page_sz, page_sz);
        EVE_Cmd_waitFlush(phost);
        uint32_t status = 0;
        status = EVE_CMD_NFLASHSTATUS(phost, status);
        if ((status & 0x08) != 0)
        {
            eve_printf_debug("NAND FLASH program failed.\n");
            return FLASH_CMD_ERR;
        }
    }
    return FLASH_CMD_SUCCESS;
}

/**
 * @brief Read data from flash to array
 *
 * @param phost Pointer to Hal context
 * @param dest_ram Destination on RAM_G
 * @param src_flash Source in flash
 * @param num Number of byte to read
 * @param read_data Buffer to get data
 * @return Flash_Cmd_Status_t
 */
Flash_Cmd_Status_t FlashHelper_Read(EVE_HalContext *phost, uint32_t dest_ram, uint32_t src_flash, uint32_t num, uint8_t *read_data)
{
    num = num - (num % 4); /* Only read lesser or equal aligned bytes */

    if ((src_flash % page_sz != 0) || ((dest_ram % 4) != 0)) /* Check aligned address */
    {
        return FLASH_CMD_ALIGNED_ERR;
    }

    EVE_CoCmd_flashRead(phost, dest_ram, src_flash, num);
    EVE_Cmd_waitFlush(phost);

    EVE_Hal_rdMem(phost, read_data, dest_ram, num);
    return FLASH_CMD_SUCCESS;
}

/**
 * @brief Write data to flash page by page.
 * 
 * @note
 * Destination flash address must be virgin (not used before). Otherwise, users have to perform flash erase before using. \n
 * Automatically padding 0xFF to non-aligned num. \n
 *
 * @param phost Pointer to Hal context
 * @param dest_flash Destination on flash.
 * @param num Number of bytes to write.
 * @param write_data Data buffer to write
 * @return Flash_Cmd_Status_t
 */
Flash_Cmd_Status_t FlashHelper_Write(EVE_HalContext *phost, uint32_t dest_flash, uint32_t num, const uint8_t *write_data)
{
    if (nand_init == false)
    {
        FlashHelper_GetBadBlockNum(phost);
    }
    uint32_t last_chunk = (num % page_sz); /* Remaining bytes that do not fill a complete page */
    uint32_t status = 0;

    if (dest_flash % page_sz != 0) /* Check aligned address */
    {
        return FLASH_CMD_ALIGNED_ERR;
    }

    /* Write first aligned chunks of write_data */
    if (num > last_chunk)
    {
        for (uint32_t i = 0; i < num / page_sz; i++)
        {
            FlashHelper_flashWriteExt(phost, dest_flash + i * page_sz, page_sz, write_data + i * page_sz);
            EVE_Cmd_waitFlush(phost);
            status = EVE_CMD_NFLASHSTATUS(phost, status);
            if ((status & 0x08) != 0)
            {
                eve_printf_debug("NAND FLASH write failed.\n");
                return FLASH_CMD_ERR;
            }
        }
    }

    if (last_chunk != 0) /* write_data is not aligned */
    {
        /* Write the rest write_data */
        char *pad = (char *)malloc(page_sz);
        if (!pad)
        {
            eve_printf("Malloc failed");
            return FLASH_CMD_ERR;
        }
        memset(pad, 0xff, page_sz);
        const char * tail_data = write_data + num - last_chunk;
        memcpy(pad, tail_data, last_chunk);

        FlashHelper_flashWriteExt(phost, dest_flash + num - last_chunk, page_sz, pad);
        EVE_Cmd_waitFlush(phost);
        status = EVE_CMD_NFLASHSTATUS(phost, status);
        if ((status & 0x08) != 0)
        {
            eve_printf_debug("NAND FLASH write failed.\n");
            return FLASH_CMD_ERR;
        }
        free(pad);
    }
    return FLASH_CMD_SUCCESS;
}

/**
 * @brief Obtain a flash address with continuous vacant space available for storing data
 * 
 * @note
 * This function does not verify whether the pages are in use; it only searches for continuous blocks without bad ones.
 * 
 * @param phost Pointer to Hal context
 * @param size File size
 * @param dest_flash Given address for flash image
 * @return int32_t Flash address, -1 is not found
 */
int32_t FlashHelper_GetAddrAvail(EVE_HalContext *phost, uint32_t size, uint32_t dest_flash)
{
    if (nand_init == false)
    {
        FlashHelper_GetBadBlockNum(phost);
    }
    uint32_t block_sz = page_sz * page_num;
    uint32_t last_chunk = (size % block_sz);
    uint32_t num_of_block = (last_chunk > 0) ? (size / block_sz + 1) : (size / block_sz);
    uint32_t i, j;

    // check given address can be used or not
    if (dest_flash % block_sz != 0) /* Check aligned address */
    {
        eve_printf_debug("Given flash address is not align to block size\n");
    }
    else
    {
        for (j = dest_flash / block_sz; j < (dest_flash / block_sz + num_of_block); j++)
        {
            if (bad_block_tbl[j / 8] & (0x1 << (j % 8)))
            {
                eve_printf_debug("bad block %d found, given address is not available\n", j);
                break;
            }
        }
        if (j == (dest_flash / block_sz + num_of_block))
        {
            return dest_flash;
        }
    }

    // search for a new address
    for (i = 0; i <= (block_num - num_of_block); i++)
    {
        for (j = i; j < (i + num_of_block); j++)
        {
            if (bad_block_tbl[j / 8] & (0x1 << (j % 8)))
            {
                // bad block found
                i = j; // skip to the block after the bad block
                break;
            }
        }
        if (j == (i + num_of_block))
        {
            eve_printf_debug("located flash address to block %d\n", i);
            return (block_sz * i);
        }
    }

    eve_printf_debug("no flash address available\n");
    return -1;
}

#else

/**
 * @brief Writes the given data to flash.
 * If the data matches the existing contents of flash, nothing is done.
 * Otherwise the flash is erased in 4K units, and the data is written.
 *
 * @note
 * dest_flash: destination address in flash memory. Must be 4096-byte aligned \n
 * src_ram: source data in main memory. Must be 4-byte aligned \n
 * num: number of bytes to write, should be multiple of 4096, otherwise, dummy data will be padded \n
 *
 * @param phost Pointer to Hal context
 * @param dest_flash Destination on flash
 * @param src_ram Source in RAM_G
 * @param num Number of byte to write
 * @return Flash_Cmd_Status_t
 */
Flash_Cmd_Status_t FlashHelper_Update(EVE_HalContext* phost, uint32_t dest_flash, uint32_t src_ram, uint32_t num)
{
    uint32_t last_chunk = (num % 4096); /* must be multiple of 4096. Cut off the extended data */

    if ((dest_flash % FLASH_UPDATE_ALIGN_BYTE != 0) || ((src_ram % 4) != 0)) /* Check aligned address */
    {
        return FLASH_CMD_ALIGNED_ERR;
    }

    if (num < FLASH_UPDATE_ALIGN_BYTE)
    {
        EVE_CoCmd_flashUpdate(phost, dest_flash, src_ram, FLASH_UPDATE_ALIGN_BYTE);
        EVE_Cmd_waitFlush(phost);
    }
    else if (last_chunk == 0) /* num is multiple of 4k */
    {
        EVE_CoCmd_flashUpdate(phost, dest_flash, src_ram, num);
        EVE_Cmd_waitFlush(phost);
    }
    else /* num is not fit in multiple of 4k */
    {
        EVE_CoCmd_flashUpdate(phost, dest_flash, src_ram, num - last_chunk);
        EVE_Cmd_waitFlush(phost);

        /* 4k is quite big for allocating new stack/heap data. So reuse the pointer and write dummy data to flash */
        EVE_CoCmd_flashUpdate(phost, dest_flash + num - last_chunk, src_ram + num - last_chunk, FLASH_UPDATE_ALIGN_BYTE);
        EVE_Cmd_waitFlush(phost);
    }
    return FLASH_CMD_SUCCESS;
}

/**
 * @brief Writes the given data to flash.
 * It assumes that the flash is previously programmed to all-ones.
 *
 * @note
 * dest_flash: destination address in flash memory. Must be 4096-byte aligned \n
 * src_ram: source data in main memory. Must be 4-byte aligned \n
 * num: number of bytes to write, should be multiple of 4096, otherwise, dummy data will be padded
 *
 * @param phost Pointer to Hal context
 * @param dest_flash Destination on flash
 * @param src_ram Source in RAM_G
 * @param num Numbber of byte to write
 * @return Flash_Cmd_Status_t
 */
Flash_Cmd_Status_t FlashHelper_Program(EVE_HalContext *phost, uint32_t dest_flash, uint32_t src_ram, uint32_t num)
{
    uint32_t last_chunk = (num % 4096); /* must be multiple of 4096. Cut off the extended data */

    if ((dest_flash % FLASH_UPDATE_ALIGN_BYTE != 0) || ((src_ram % 4) != 0)) /* Check aligned address */
    {
        return FLASH_CMD_ALIGNED_ERR;
    }

    if (num < FLASH_UPDATE_ALIGN_BYTE)
    {
        EVE_CoCmd_flashProgram(phost, dest_flash, src_ram, FLASH_UPDATE_ALIGN_BYTE);
        EVE_Cmd_waitFlush(phost);
    }
    else if (last_chunk == 0) /* num is multiple of 4k */
    {
        EVE_CoCmd_flashProgram(phost, dest_flash, src_ram, num);
        EVE_Cmd_waitFlush(phost);
    }
    else /* num is not fit in multiple of 4k */
    {
        EVE_CoCmd_memSet(phost, src_ram + num, 0xff, 4096 - last_chunk);
        EVE_CoCmd_flashProgram(phost, dest_flash, src_ram, 4096 - last_chunk + num);
        EVE_Cmd_waitFlush(phost);
    }
    return FLASH_CMD_SUCCESS;
}

/**
 * @brief Read data from flash to array
 *
 * @param phost Pointer to Hal context
 * @param dest_ram Destination on RAM_G
 * @param src_flash Source in flash
 * @param num Number of byte to read
 * @param read_data Buffer to get data
 * @return Flash_Cmd_Status_t
 */
Flash_Cmd_Status_t FlashHelper_Read(EVE_HalContext* phost, uint32_t dest_ram, uint32_t src_flash, uint32_t num, uint8_t *read_data)
{
    num = num - (num % 4); /* Only read lesser or equal aligned bytes */

    if ((src_flash % FLASH_READ_ALIGN_BYTE != 0) || ((dest_ram % 4) != 0)) /* Check aligned address */
    {
        return FLASH_CMD_ALIGNED_ERR;
    }

    EVE_CoCmd_flashRead(phost, dest_ram, src_flash, num);
    EVE_Cmd_waitFlush(phost);

    EVE_Hal_rdMem(phost, read_data, dest_ram, num);
    return FLASH_CMD_SUCCESS;
}

/**
 * @brief Erase entire flash
 * @warning This function erases the entire Flash, which means the bad block information will be lost. Please use with caution.
 *
 * @param phost Pointer to Hal context
 */
void FlashHelper_Erase(EVE_HalContext* phost)
{
    EVE_CoCmd_flashErase(phost);
    EVE_Cmd_waitFlush(phost);
}

/**
 * @brief Write data to flash, and align byte if needed.
 * 
 * @note
 * Destination flash address must be virgin (not used before). Otherwise, users have to perform flash erase before using. \n
 * Destination address must be 256-byte aligned. \n
 * Automatically padding 0xFF to non-aligned num. \n
 *
 * @param phost Pointer to Hal context
 * @param dest_flash Destination on flash. Must be 256-byte aligned
 * @param num Number of bytes to write. Must be multiple of 256
 * @param write_data Data buffer to write
 * @return Flash_Cmd_Status_t
 */
Flash_Cmd_Status_t FlashHelper_Write(EVE_HalContext* phost, uint32_t dest_flash, uint32_t num, const uint8_t* write_data)
{
    uint8_t padding_arr[FLASH_WRITE_ALIGN_BYTE]; /* write_data must be 256-byte aligned */
    uint32_t aligned_length = num % FLASH_WRITE_ALIGN_BYTE;

    if (dest_flash % FLASH_WRITE_ALIGN_BYTE != 0) /* Check aligned address */
    {
        return FLASH_CMD_ALIGNED_ERR;
    }

    if (aligned_length == 0) /* write_data is already aligned */
    {
        FlashHelper_flashWriteExt(phost, dest_flash, num, write_data);
        EVE_Cmd_waitFlush(phost);
    }
    else
    {
        /* Write first aligned chunks of write_data */
        if (num - aligned_length > 0)
        {
            FlashHelper_flashWriteExt(phost, dest_flash, num - aligned_length, write_data);
            EVE_Cmd_waitFlush(phost);
        }
        /* Write the rest write_data */
        memset(padding_arr, 0xff, sizeof(padding_arr));
        write_data = write_data + num - aligned_length;
        memcpy(padding_arr, write_data, aligned_length);

        FlashHelper_flashWriteExt(phost, dest_flash + num - aligned_length, FLASH_WRITE_ALIGN_BYTE, padding_arr);
        EVE_Cmd_waitFlush(phost);
    }
    return FLASH_CMD_SUCCESS;
}
#endif

/**
 * @brief Flash state/status
 *
 * @param phost Pointer to Hal context
 * @return uint32_t
 */
uint32_t FlashHelper_GetState(EVE_HalContext *phost)
{
    return EVE_Hal_rd32(phost, REG_FLASH_STATUS);
}

/**
 * @brief Switch to other flash state
 * 
 * Error code:
 * - 0x0    command succeeds
 * - 0xffff command fails (invalid transition state)
 * - 0xe001 flash is not attached
 * - 0xe002 no header detected in sector 0 - is flash blank?
 * - 0xe003 sector 0 data failed integrity check
 * - 0xe004 device/blob mismatch - was correct blob loaded?
 * - 0xe005 failed full-speed test - check board wiring
 *
 * @param phost Pointer to Hal context
 * @param nextState New state to switch to
 * @return uint32_t
 */
uint32_t FlashHelper_SwitchState(EVE_HalContext* phost, uint8_t nextState)
{
    uint32_t ret = 0;
    //uint8_t read_data[CMD_FIFO_SIZE]; Debug only
    uint32_t curr_flash_state = EVE_Hal_rd32(phost, REG_FLASH_STATUS);
    uint32_t ret_addr = 0;
    if (curr_flash_state == nextState) {
        return ret;
    }

    //Only handle if nextState is diff
    if (FLASH_STATUS_DETACHED == nextState)
    {
        EVE_CoCmd_flashDetach(phost);
        EVE_Cmd_waitFlush(phost);
    }
    else if (FLASH_STATUS_BASIC == nextState)
    {
        if (FLASH_STATUS_FULL == curr_flash_state)
        {
            do
            {
                EVE_CoCmd_flashDetach(phost);
                EVE_Cmd_waitFlush(phost);
            } while (FLASH_STATUS_DETACHED != EVE_Hal_rd32(phost, REG_FLASH_STATUS));
        }
        EVE_CoCmd_flashAttach(phost);
        EVE_Cmd_waitFlush(phost);
    }
    else if (FLASH_STATUS_FULL == nextState)
    {
        if (FLASH_STATUS_BASIC != curr_flash_state)
        {
            do
            {
                EVE_CoCmd_flashAttach(phost);
                EVE_Cmd_waitFlush(phost);
            } while (FLASH_STATUS_BASIC != EVE_Hal_rd32(phost, REG_FLASH_STATUS));
        }
        EVE_CoCmd_flashFast(phost, 0);
        EVE_Cmd_waitFlush(phost);

        /* Read the return code in CMD_BUFFER */
        ret_addr = (EVE_Cmd_wp(phost) - 4) & EVE_CMD_FIFO_MASK;
        ret_addr = (ret_addr + 3) & EVE_CMD_FIFO_ALIGNMENT_MASK; // 4 byte alignment

        ret = EVE_Hal_rd32(phost, RAM_CMD + ret_addr);
    }
    else
    {
        ret = 0xffff;
    }

    return ret;
}

/**
 * @brief Switch flash into full mode
 *
 * @param phost Pointer to Hal context
 * @return uint32_t
 */
uint32_t FlashHelper_SwitchFullMode(EVE_HalContext* phost)
{
    uint32_t val;
    /* Try detaching and attaching the flash followed by fast mdoe */
    EVE_CoCmd_flashDetach(phost);
    EVE_Cmd_waitFlush(phost);
    val = EVE_Hal_rd32(phost, REG_FLASH_STATUS);

    if (FLASH_STATUS_DETACHED != val)
    {
        eve_printf("Error, Flash is not able to detatch %d\n", val);
        return 0;
    }

    EVE_CoCmd_flashAttach(phost);
    EVE_Cmd_waitFlush(phost);
    val = EVE_Hal_rd32(phost, REG_FLASH_STATUS);

    if (FLASH_STATUS_BASIC != val)
    {
        eve_printf("Error, Flash is not able to attach %d\n", val);
        return 0;
    }

    EVE_CoCmd_flashFast(phost, 0);
    EVE_Cmd_waitFlush(phost);
    val = EVE_Hal_rd32(phost, REG_FLASH_STATUS);

    if (FLASH_STATUS_FULL != val)
    {
        eve_printf("Error, Flash is not able to get into full mode, status: %d\n", val);
        return 0;
    }
    return 1;
}

/* Nothing beyond this */
