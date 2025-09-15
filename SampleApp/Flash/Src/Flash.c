/**
 * @file Flash.c
 * @brief Sample usage of flash
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

#include "Flash.h"
#include "FileTransfer.h"
#include "FlashHelper.h"

static EVE_HalContext s_halContext;
static EVE_HalContext* s_pHalContext;
void SAMAPP_Flash();

int main(int argc, char* argv[])
{
    s_pHalContext = &s_halContext;
    Gpu_Init(s_pHalContext);
    LVDS_Config(s_pHalContext, YCBCR, MODE_PICTURE);

    // read and store calibration setting
#if GET_CALIBRATION == 1
    EVE_Calibrate(s_pHalContext);
    Calibration_Save(s_pHalContext);
#endif

    //Flash_Init(s_pHalContext, TEST_DIR "/Flash/flash.bin", "flash.bin");
    EVE_Util_clearScreen(s_pHalContext);

    char *info[] =
    {  "EVE Sample Application",
        "This sample demonstrate the using of flash", 
        "",
        ""
    }; 

    while (true) {
        WelcomeScreen(s_pHalContext, info);

        SAMAPP_Flash();

        EVE_Util_clearScreen(s_pHalContext);

        Gpu_Release(s_pHalContext);

        /* Init HW Hal for next loop*/
        Gpu_Init(s_pHalContext);
        LVDS_Config(s_pHalContext, YCBCR, MODE_PICTURE);
#if GET_CALIBRATION == 1
        Calibration_Restore(s_pHalContext);
#endif
    }

    return 0;
}

char* flashMessage[50];
int messageIndex = 0;

void helperAppendMessage(char *s, int num)
{
    flashMessage[messageIndex] = (char*)malloc(num);
    if (!flashMessage[messageIndex]) {
        eve_printf("Malloc failed");
        return;
    }

    memcpy(flashMessage[messageIndex], s, num);
    
    Display_StartColor(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 0, 0, 0 });

    int y = 0;
    int x = 20;
    for (int i = 0; i <= messageIndex; i++) {
        EVE_CoCmd_text(s_pHalContext, x, y, 30, 0, flashMessage[i]);
        y += 50;
    }
    Display_End(s_pHalContext);

    messageIndex++;
    EVE_sleep(500);
}

void helperClearMessage() {
    for (int i = 0; i < messageIndex; i++) {
        free(flashMessage[i]);
    }
    messageIndex = 0;
}


/**
* @brief API to demonstrate flash states
*
*/
void SAMAPP_Flash_state() {
    const char* flash_status[4] = { "INIT", "DETACHED", "BASIC", "FULL" };
    char mes[50];
    int mesLen = 50;
    uint32_t status = 0;

    Draw_Text(s_pHalContext, "Example for: Flash states");

    eve_printf_debug("\nFLASH STATES\n");
    EVE_CoCmd_regRead(s_pHalContext, REG_FLASH_STATUS, &status);
    eve_sprintf(mes, sizeof(mes), "Flash State: %s (%d)\n", flash_status[status], status);
    helperAppendMessage(mes, mesLen);
    EVE_sleep(1000);

    eve_sprintf(mes, sizeof(mes), "CMD_FLASHDETACH");
    helperAppendMessage(mes, mesLen);

    EVE_CoCmd_flashDetach(s_pHalContext);

    EVE_CoCmd_regRead(s_pHalContext, REG_FLASH_STATUS, &status);
    if (status != FLASH_STATUS_DETACHED)
    {
        eve_sprintf(mes, sizeof(mes), "Flash is not able to detach");
        helperAppendMessage(mes, mesLen);
    }
    else
    {
        eve_sprintf(mes, sizeof(mes), "Flash is detached");
        helperAppendMessage(mes, mesLen);
    }
    eve_sprintf(mes, sizeof(mes), "Flash State: %s (%d)\n", flash_status[status], status);
    helperAppendMessage(mes, mesLen);
    EVE_sleep(1000);

    eve_sprintf(mes, sizeof(mes), "CMD_FLASHATTACH");
    helperAppendMessage(mes, mesLen);

    EVE_CoCmd_flashAttach(s_pHalContext);

    EVE_CoCmd_regRead(s_pHalContext, REG_FLASH_STATUS, &status);
    if (status != FLASH_STATUS_BASIC)
    {
        eve_sprintf(mes, sizeof(mes), "Flash is not able to attach");
        helperAppendMessage(mes, mesLen);
    }
    else
    {
        eve_sprintf(mes, sizeof(mes), "Flash is attached");
        helperAppendMessage(mes, mesLen);
    }
    eve_sprintf(mes, sizeof(mes), "Flash State: %s (%d)\n", flash_status[status], status);
    helperAppendMessage(mes, mesLen);
    EVE_sleep(1000);

    eve_sprintf(mes, sizeof(mes), "CMD_FLASHFAST");
    helperAppendMessage(mes, mesLen);

    EVE_CoCmd_flashFast(s_pHalContext, 0);
    EVE_CoCmd_regRead(s_pHalContext, REG_FLASH_STATUS, &status);

    if (status != FLASH_STATUS_FULL)
    {
        eve_sprintf(mes, sizeof(mes), "Flash is not able to get into full mode");
        helperAppendMessage(mes, mesLen);
    }
    else
    {
        eve_sprintf(mes, sizeof(mes), "Flash gets into full mode");
        helperAppendMessage(mes, mesLen);
    }
    eve_sprintf(mes, sizeof(mes), "Flash State: %s (%d)\n", flash_status[status], status);
    helperAppendMessage(mes, mesLen);
    EVE_sleep(1000); 

    eve_sprintf(mes, sizeof(mes), "Check Flash Size");
    helperAppendMessage(mes, mesLen);

    EVE_CoCmd_regRead(s_pHalContext, REG_FLASH_SIZE, &status);
    eve_sprintf(mes, sizeof(mes), "Flash size : %d MBytes", status);
    helperAppendMessage(mes, mesLen);

    helperClearMessage();
    EVE_sleep(2000);
}

/**
* @brief API to demonstrate flash programming
*
*/
void SAMAPP_Flash_program() {
    Draw_Text(s_pHalContext, "Example for: Flash program");

    /* Erase the flash */
    FlashHelper_SwitchState(s_pHalContext, FLASH_STATUS_FULL);
    EVE_Cmd_waitFlush(s_pHalContext);

    // write flash
    Ftf_Write_File_To_Flash_With_Progressbar(s_pHalContext, 
        TEST_DIR "/Flash/flash.bin", "flash.bin", 0);
    
    helperClearMessage();    
    EVE_sleep(2000);
}

uint32_t Flash_Progress_Ui(EVE_HalContext *phost, const Flash_Progress_t *progress)
{
    char s[100];
    uint16_t w = (uint16_t)(phost->Width * 8 / 10);
    uint16_t h = (uint16_t)(phost->Height * 1 / 10);
    uint16_t x = (uint16_t)((phost->Width - w) / 2);
    uint16_t y = (uint16_t)((phost->Height - h) / 2);
    uint16_t val = (uint16_t)(progress->stage * 100 / LAST_STAGE);
    const uint32_t range = 100;

    Display_StartColor(phost, (uint8_t[]) { 64, 64, 64 }, (uint8_t[]) { 255, 255, 255 });
    EVE_CoCmd_text(phost, x, y - 50, 31, 0, progress->message);
    EVE_CoCmd_progress(phost, x, y, w, h, 0, val, range);

    snprintf(s, 100, "%u %%", val * 100 / range);
    EVE_CoDl_colorRgb(phost, 0, 200, 0);
    EVE_CoCmd_text(phost, x + w / 2, y + 10, 31, 0, s);
    EVE_CoDl_colorRgb(phost, 255, 255, 255);

    Display_End(phost);

    return 1;
}

void SAMAPP_Flash_program_from_EVE_SD()
{
#ifdef EVE_FLASH_NAND
    Draw_Text(s_pHalContext, "Example for: Flash program from EVE connected SD card");

    uint32_t result = 0;
	uint32_t addr_from_sd = RAM_G; // DDR address to save the file read from SD card
	uint32_t addr_from_flash = 1 << 20; // DDR address to save the file read back from flash
    const char *file = "flash.bin";
    Flash_Progress_t progress;
    memcpy(progress.fileName, file, 10);

    progress.stage = COPY_FROM_SD;
    snprintf(progress.message, 100, "Reading EVE connected SD card");
    Flash_Progress_Ui(s_pHalContext, &progress);
    result = EVE_CoCmd_sdattach(s_pHalContext, OPT_4BIT | OPT_IS_SD, 0);
    eve_printf_debug("SD attach status 0x%x \n", result);
    if (result != 0)
    {
        eve_printf_debug("SD attach failed\n");
        snprintf(progress.message, 100, "Flash failed - SD card not attached");
        Flash_Progress_Ui(s_pHalContext, &progress);
        EVE_sleep(2000);
        return;
    }
    result = EVE_CoCmd_fssource(s_pHalContext, file, 0);
    eve_printf_debug("file read status 0x%x \n", result);
    if (result != 0)
    {
        eve_printf_debug("SD read failed\n");
        snprintf(progress.message, 100, "Flash failed - can't find file in SD card");
        Flash_Progress_Ui(s_pHalContext, &progress);
        EVE_sleep(2000);
        return;
    }
    progress.fileSize = EVE_CoCmd_fssize(s_pHalContext, file, 0);
    if (progress.fileSize > 0)
    {
		result = EVE_CoCmd_fsread(s_pHalContext, addr_from_sd, file, 0);
        if (result != 0)
        {
            eve_printf_debug("file read failed\n");
            snprintf(progress.message, 100, "Flash failed - can't read file");
            Flash_Progress_Ui(s_pHalContext, &progress);
            EVE_sleep(2000);
            return;
        }
    }
    else
    {
        eve_printf_debug("file size is not large than 0\n");
        snprintf(progress.message, 100, "Flash failed - file size is not correct");
        Flash_Progress_Ui(s_pHalContext, &progress);
        EVE_sleep(2000);
        return;
    }
    EVE_sleep(1000);

    progress.stage = ERASE_FLASH;
    snprintf(progress.message, 100, "Erasing flash");
    Flash_Progress_Ui(s_pHalContext, &progress);
	EVE_sleep(100);
    FlashHelper_Erase(s_pHalContext);
    EVE_sleep(1000);

    progress.stage = COPY_TO_FLASH;
    snprintf(progress.message, 100, "Writing %s to flash", progress.fileName);
    Flash_Progress_Ui(s_pHalContext, &progress);
	FlashHelper_Program(s_pHalContext, 0, addr_from_sd, progress.fileSize);
    EVE_sleep(1000);

	progress.stage = VERIFY;
	snprintf(progress.message, 100, "Verifying");
	Flash_Progress_Ui(s_pHalContext, &progress);
	EVE_sleep(100);
	if (Show_Diaglog_YesNo(s_pHalContext, "Flash programming",
	        "Verify the data in Flash?"))
	{
		uint32_t crc_from_sd = 0;
		uint32_t crc_from_flash = 0;
		EVE_CoCmd_memCrc(s_pHalContext, addr_from_sd, progress.fileSize, &crc_from_sd);
		FlashHelper_Read(s_pHalContext, addr_from_flash, 0, progress.fileSize, NULL);
		EVE_CoCmd_memCrc(s_pHalContext, addr_from_flash, progress.fileSize, &crc_from_flash);
		if (crc_from_sd == crc_from_flash)
		{
			snprintf(progress.message, 100, "Verify success");
			Flash_Progress_Ui(s_pHalContext, &progress);
			EVE_sleep(1000);
		}
		else
		{
			snprintf(progress.message, 100, "Verify failed");
			Flash_Progress_Ui(s_pHalContext, &progress);
			EVE_sleep(2000);
			return;
		}
	}

    progress.stage = LAST_STAGE;
	snprintf(progress.message, 100, "Flash success");
    Flash_Progress_Ui(s_pHalContext, &progress);
    EVE_sleep(2000);
#endif
}

void SAMAPP_Flash() {
    SAMAPP_Flash_state();
    SAMAPP_Flash_program();
	SAMAPP_Flash_program_from_EVE_SD();
}


