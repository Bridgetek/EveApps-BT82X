/**
 * @file FileTransfer.c
 * @brief File transfer interface from host to flash
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

#if defined(MSVC_PLATFORM)
#define EVE_FLASH_DIR         __FILE__ "\\..\\..\\..\\common\\eve_flash"
#else
#define EVE_FLASH_DIR         "/Test/common/eve_flash"
#endif

#define FTF_PROGESS_READ      1
#define FTF_PROGESS_WRITE     2
#define MSG_SIZE              200
#define FLASH_DELAY_MS        1000

#ifdef EVE_FLASH_NAND
typedef struct Flash_Progress
{
    char message[MSG_SIZE];
    char stage;
    uint32_t fileSize;
} Flash_Progress;

typedef enum
{
    COPY_FROM, /**< 0 */
    GET_FLASH_ADDR, /**< 1 */
    ERASE_FLASH, /**< 2 */
    COPY_TO_FLASH, /**< 3 */
    VERIFY, /**< 4 */

    LAST_STAGE = 5, /**< 5 */
} Flash_stage;

/**
 * @brief Default UI for the progress bar
 *
 * @param phost Pointer to Hal context
 * @param progress Flash_Progress_t struct
 * @return uint32_t 1 on successful, 0 on error
 */
uint32_t Flash_Progress_Ui(EVE_HalContext *phost, const Flash_Progress *progress)
{
    char s[MSG_SIZE];
    uint16_t w = (uint16_t)(phost->Width * 8 / 10);
    uint16_t h = (uint16_t)(phost->Height * 1 / 10);
    uint16_t x = (uint16_t)((phost->Width - w) / 2);
    uint16_t y = (uint16_t)((phost->Height - h) / 2);
    uint16_t val = (uint16_t)(progress->stage * 100 / LAST_STAGE);
    const uint32_t range = 100;

    Display_Start(phost, (uint8_t[]) { 64, 64, 64 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_CoCmd_text(phost, x, y - 50, 31, 0, progress->message);
    EVE_CoCmd_progress(phost, x, y, w, h, 0, val, range);

    snprintf(s, MSG_SIZE, "%u %%", val * 100 / range);
    EVE_CoDl_colorRgb(phost, 0, 200, 0);
    EVE_CoCmd_text(phost, x + w / 2, y + 10, 31, 0, s);
    EVE_CoDl_colorRgb(phost, 255, 255, 255);

    Display_End(phost);
    EVE_sleep(FLASH_DELAY_MS);

    return 1;
}

/**
 * @brief Write file to flash and show default progress bar on LCD
 *
 * @param phost Pointer to Hal context
 * @param file File to transfer, should include path if not from EVE connected SD card
 * @param address Address on flash
 * @param fromEveSD Copy file from EVE connected SD card or not
 * @return uint32_t Number of bytes transfered
 */
uint32_t Write_To_Flash_With_Progressbar(EVE_HalContext *phost, const char *file, uint32_t *address, bool fromEveSD)
{
    Flash_Progress progress;
    uint32_t result = 0;
    uint32_t addr_from = RAM_G; // DDR address to save the file read from SDcard/PC
    uint32_t addr_from_flash = 64 << 20; // DDR address to save the file read back from flash

    progress.stage = COPY_FROM;
    if (fromEveSD)
    {
        snprintf(progress.message, MSG_SIZE, "Reading EVE connected SD card");
        Flash_Progress_Ui(phost, &progress);
        result = EVE_CoCmd_sdattach(phost, OPT_4BIT | OPT_IS_SD, 0);
        eve_printf_debug("SD attach status 0x%x \n", result);
        if (result != 0)
        {
            eve_printf_debug("SD attach failed\n");
            snprintf(progress.message, MSG_SIZE, "Flash failed - SD card not attached");
            Flash_Progress_Ui(phost, &progress);
            EVE_sleep(FLASH_DELAY_MS);
            return 0;
        }
        result = EVE_CoCmd_fssource(phost, file, 0);
        eve_printf_debug("file read status 0x%x \n", result);
        if (result != 0)
        {
            eve_printf_debug("SD read failed\n");
            snprintf(progress.message, MSG_SIZE, "Flash failed - can't find file in SD card");
            Flash_Progress_Ui(phost, &progress);
            EVE_sleep(FLASH_DELAY_MS);
            return 0;
        }
        progress.fileSize = EVE_CoCmd_fssize(phost, file, 0);
        if (progress.fileSize > 0)
        {
            result = EVE_CoCmd_fsread(phost, addr_from, file, 0);
            if (result != 0)
            {
                eve_printf_debug("file read failed\n");
                snprintf(progress.message, MSG_SIZE, "Flash failed - can't read file");
                Flash_Progress_Ui(phost, &progress);
                EVE_sleep(FLASH_DELAY_MS);
                return 0;
            }
        }
        else
        {
            eve_printf_debug("file size is not large than 0\n");
            snprintf(progress.message, MSG_SIZE, "Flash failed - file size is not correct");
            Flash_Progress_Ui(phost, &progress);
            EVE_sleep(FLASH_DELAY_MS);
            return 0;
        }
    }
    else
    {
#if defined(MSVC_PLATFORM)
        snprintf(progress.message, MSG_SIZE, "Reading from PC");
#else
        snprintf(progress.message, MSG_SIZE, "Reading from MCU's SD card");
#endif
        Flash_Progress_Ui(phost, &progress);
        progress.fileSize = EVE_Util_loadRawFile(phost, addr_from, file);
        if (progress.fileSize == 0)
        {
            eve_printf_debug("file size is not large than 0\n");
            snprintf(progress.message, MSG_SIZE, "Flash failed - file can't be found");
            Flash_Progress_Ui(phost, &progress);
            EVE_sleep(FLASH_DELAY_MS);
            return 0;
        }
    }

    EVE_sleep(FLASH_DELAY_MS);
    progress.stage = GET_FLASH_ADDR;
    snprintf(progress.message, MSG_SIZE, "Search for an available flash address");
    Flash_Progress_Ui(phost, &progress);
    *address = FlashHelper_GetAddrAvail(phost, progress.fileSize, *address);

    EVE_sleep(FLASH_DELAY_MS);
    progress.stage = ERASE_FLASH;
    snprintf(progress.message, MSG_SIZE, "Erasing flash");
    Flash_Progress_Ui(phost, &progress);
    FlashHelper_EraseBlocks(phost, *address, progress.fileSize);

    progress.stage = COPY_TO_FLASH;
    snprintf(progress.message, MSG_SIZE, "Writing to flash");
    Flash_Progress_Ui(phost, &progress);
    FlashHelper_Program(phost, *address, addr_from, progress.fileSize);

    progress.stage = VERIFY;
    snprintf(progress.message, MSG_SIZE, "Verifying");
    Flash_Progress_Ui(phost, &progress);
    if (Show_Diaglog_YesNo(phost, "Flash programming",
            "Verify the data in Flash?"))
    {
        uint32_t crc_from_sd = 0;
        uint32_t crc_from_flash = 0;
        EVE_CoCmd_memCrc(phost, addr_from, progress.fileSize, &crc_from_sd);
        FlashHelper_Read(phost, addr_from_flash, *address, progress.fileSize, NULL);
        EVE_CoCmd_memCrc(phost, addr_from_flash, progress.fileSize, &crc_from_flash);
        if (crc_from_sd == crc_from_flash)
        {
            snprintf(progress.message, MSG_SIZE, "Verify success");
            Flash_Progress_Ui(phost, &progress);
        }
        else
        {
            snprintf(progress.message, MSG_SIZE, "Verify failed");
            Flash_Progress_Ui(phost, &progress);
            EVE_sleep(FLASH_DELAY_MS);
            return 0;
        }
    }

    progress.stage = LAST_STAGE;
    snprintf(progress.message, MSG_SIZE, "Flash success");
    Flash_Progress_Ui(phost, &progress);
    EVE_sleep(FLASH_DELAY_MS);
    return progress.fileSize;
}

#else

#define FREAD_BLOCK               (8 * 1024)
#define BLOBSIZE                  4096
#define FILE_BLOB                 (EVE_FLASH_DIR "\\bt82x.blob")

typedef struct Ftf_Progress
{
    char file[200];
    char fileName[200];
    char message[MSG_SIZE];
    uint32_t fileSize;
    uint32_t sent;
    uint32_t bytesPerPercent;
    uint32_t addr;
    uint8_t direction;
} Ftf_Progress;

/**
 * @brief Write blob file to flash
 * When updating a flash image, the following steps are to be expected:
 *   - Reading the first 4096 bytes in flash image into RAM_G and update the blob first
 *       using cmd_flashupdate in Basic mode.
 *   - Send the command cmd_flashfast to enter into fast mode.
 *   - If success, program the rest data of flash image by cmd_flashupdate
 *   - If not success, write a default blob.
 *
 * @param phost Pointer to Hal context
 * @param pbuff data buffer
 * @return uint32_t 1 on successful, 0 on error
 */
uint32_t Ftf_Update_Blob(EVE_HalContext* phost, const char* pbuff) 
{
    eve_printf("Updating blob\n");
    char buf[BLOBSIZE] = { 0 };

    FlashHelper_SwitchState(phost, FLASH_STATUS_BASIC); // basic mode
    EVE_Hal_wrMem(phost, RAM_G, pbuff, BLOBSIZE);
    printf("REMG: 0x%lx\n", EVE_Hal_rd32(phost, RAM_G));
    EVE_Cmd_waitFlush(phost);
    FlashHelper_Update(phost, 0, RAM_G, BLOBSIZE);
    EVE_Cmd_waitFlush(phost);
    FlashHelper_Read(phost, 4096, 0, BLOBSIZE, buf);
    printf("REMG+4096: 0x%lx, buf: 0x%lx\n", EVE_Hal_rd32(phost, 4096), buf[0]);

    int ret = FlashHelper_SwitchFullMode(phost);

    if (ret == 1) {
        eve_printf("Blob updated successful\n");
        return 1;
    }
    else {
        eve_printf("Failed to update Blob\n");
    }

    return 0;
}

/**
 * @brief Write default blob file to flash
 *
 * @param phost Pointer to Hal context
 * @return uint32_t 1 on successful, 0 on error
 */
uint32_t Ftf_Write_Blob_Default(EVE_HalContext *phost)
{
    char pBuff[BLOBSIZE];
    eve_printf("Writing blob default\n");

    EVE_Util_readFile(phost, pBuff, BLOBSIZE, FILE_BLOB);

    return Ftf_Update_Blob(phost, pBuff);
}

/**
 * @brief Write blob file to flash
 *
 * @param phost Pointer to Hal context
 * @param blobfile Blob file address
 * @return uint32_t 1 on successful, 0 on error
 */
uint32_t Ftf_Write_BlobFile(EVE_HalContext* phost, const char* blobfile)
{
    char pBuff[BLOBSIZE];

    eve_printf("Writing blob from file %s\n", blobfile);

    EVE_Util_readFile(phost, pBuff, BLOBSIZE, blobfile);

    int ret = Ftf_Update_Blob(phost, pBuff);

    /// fail to default Blob
    if (!ret) {
        return Ftf_Write_Blob_Default(phost);
    }

    return 1;
}

/**
 * @brief File transfer progress bar initialization
 *
 * @param phost Pointer to Hal context
 * @param file File to transfer
 * @param addr Address on flash
 * @param direction FTF_PROGESS_READ or FTF_PROGESS_WRITE
 * @return Ftf_Progress*
 */
Ftf_Progress* Ftf_Progress_Init(EVE_HalContext* phost, const char* file, uint32_t addr, uint8_t direction)
{
    static Ftf_Progress progress;
    uint32_t range = 0;
    int32_t fileSize = 0;

    progress.sent = 0;
    progress.addr = addr;
    progress.direction = direction;
#pragma warning(push)
#pragma warning(disable : 4996)
    strcpy(progress.file, file);
#pragma warning(pop)

    if (direction == FTF_PROGESS_READ) {
        snprintf(progress.message, MSG_SIZE, "Reading from flash");
    }
    else {
        // update blob from file first
        if (addr == 0) {
            Ftf_Write_BlobFile(phost, file);
        }
        else {
            Ftf_Write_Blob_Default(phost);
        }

        // Jump to real data
        if (addr == 0) {
            progress.sent = progress.addr = (uint32_t)EVE_Util_loadFile(phost, RAM_G, BLOBSIZE, file, progress.sent);
        }

        snprintf(progress.message, MSG_SIZE, "Writing to flash");
        progress.fileSize = phost->LoadFileRemaining;

    }

    /// Destination address in flash memory must be 4096-byte aligned
    progress.bytesPerPercent = ALIGN(progress.fileSize / 100, 4096);

    if (progress.bytesPerPercent < FREAD_BLOCK) {
        progress.bytesPerPercent = FREAD_BLOCK;
    }

    return &progress;
}

/**
 * @brief Write a block data of file to flash
 *
 * @param phost Pointer to Hal context
 * @param progress Ftf_Progress struct
 * @return uint32_t Percent of data transfered, 100 mean file transfer is done
 */
uint32_t Ftf_Progress_Write_Next(EVE_HalContext* phost, Ftf_Progress* progress)
{
    uint32_t bytes;
    uint32_t sent = 0;
    uint32_t ramGSent = 0;
    uint32_t blockSize = 0;

    // Tranfer 1 percent of file
    blockSize = FREAD_BLOCK > progress->bytesPerPercent ? progress->bytesPerPercent : FREAD_BLOCK;

    // Tranfer to ram_g
    while (progress->sent < progress->fileSize && sent < progress->bytesPerPercent) {
        bytes = EVE_Util_loadFile(phost, ramGSent, blockSize, progress->file, NULL);
        ramGSent += bytes;
        sent += bytes;
        progress->sent += bytes;
    }

    // Update flash from ram_g
    ramGSent = (ramGSent + 4095) & (~4095);//to ensure 4KB alignment
    FlashHelper_Update(phost, progress->addr, 0, ramGSent);
    progress->addr += ramGSent;

    return progress->sent * 100 / progress->fileSize; /* Percent */
}


/**
 * @brief Default UI for the progress bar
 * User may construct their own UI for the progress bar with Ftf_Progress_Init and Ftf_Progress_Write_Next
 *
 * @param phost Pointer to Hal context
 * @param progress Ftf_Progress struct
 * @return uint32_t 1 on successful, 0 on error
 */
uint32_t Ftf_Progress_Ui(EVE_HalContext* phost, const Ftf_Progress* progress)
{
    char s[100];
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
    uint16_t opt;
    uint16_t font = 30;
    uint16_t val;
    const uint32_t range = 1000;
    uint64_t sent64 = progress->sent;

    if (progress->fileSize == 0) {
        return 1;
    }
    opt = 0;
    val = (uint16_t)(sent64 * 1000 / progress->fileSize);

    w = (uint16_t)(phost->Width * 8 / 10);
    h = (uint16_t)(phost->Height * 1 / 10);
    x = (uint16_t)((phost->Width - w) / 2);
    y = (uint16_t)((phost->Height - h) / 2);

    Display_Start(phost, (uint8_t[]) { 64, 64, 64 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_CoCmd_text(phost, x, y - 50, font, opt, progress->message);
    EVE_CoCmd_progress(phost, x, y, w, h, opt, val, range);

    snprintf(s, 100, "%u %%", val * 100 / range);
    EVE_CoDl_colorRgb(phost, 0, 200, 0);
    EVE_CoCmd_text(phost, x + w / 2, y + 5, font, opt, s);
    EVE_CoDl_colorRgb(phost, 255, 255, 255);

    Display_End(phost);

    return 1;
}

/**
 * @brief Write file to flash and show default progress bar on LCD
 *
 * @param phost Pointer to Hal context
 * @param file File to transfer
 * @param address Address on flash
 * @param fromEveSD Copy file from EVE connected SD card or not
 * @return uint32_t Number of bytes transfered
 */
uint32_t Write_To_Flash_With_Progressbar(EVE_HalContext *phost, const char *file, uint32_t *address, bool fromEveSD)
{
    //TODO: program flash from EVE connected SD card
    Ftf_Progress* progress = Ftf_Progress_Init(phost, file, *address, FTF_PROGESS_WRITE);

    if (!progress) {
        return -1; /// error
    }

    while (1)
    {
        uint32_t pc = Ftf_Progress_Write_Next(phost, progress);
        Ftf_Progress_Ui(phost, progress);

        if (pc >= 100) {
            break;
        }
    }

    return progress->fileSize;
}
#endif


