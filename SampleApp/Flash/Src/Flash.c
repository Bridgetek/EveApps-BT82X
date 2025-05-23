/**
 * @file Flash.c
 * @brief Sample usage of flash
 *
 * @author Bridgetek
 *
 * @date 2019
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

#include "Common.h"
#include "Flash.h"

static EVE_HalContext s_halContext;
static EVE_HalContext* s_pHalContext;
void SAMAPP_Flash();

int main(int argc, char* argv[])
{
    s_pHalContext = &s_halContext;
    Gpu_Init(s_pHalContext);
    LVDS_Config(s_pHalContext, YCBCR, MODE_PICTURE);

    // read and store calibration setting
#if !defined(BT8XXEMU_PLATFORM) && GET_CALIBRATION == 1
    EVE_Calibrate(s_pHalContext);
    Calibration_Save(s_pHalContext);
#endif

    Flash_Init(s_pHalContext, TEST_DIR "/Flash/flash.bin", "flash.bin");
    EVE_Util_clearScreen(s_pHalContext);

    char *info[] =
    {  "EVE Sample Application",
        "This sample demonstrate the using of flash", 
        "",
        ""
    }; 

    while (TRUE) {
        WelcomeScreen(s_pHalContext, info);

        SAMAPP_Flash();

        EVE_Util_clearScreen(s_pHalContext);

        Gpu_Release(s_pHalContext);

        /* Init HW Hal for next loop*/
        Gpu_Init(s_pHalContext);
        LVDS_Config(s_pHalContext, YCBCR, MODE_PICTURE);
#if !defined(BT8XXEMU_PLATFORM) && GET_CALIBRATION == 1
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
    char mes[50];
    int mesLen = 50;

    Draw_Text(s_pHalContext, "Example for: Flash program");

    /* Erase the flash */
    FlashHelper_SwitchState(s_pHalContext, FLASH_STATUS_FULL);
    EVE_Cmd_waitFlush(s_pHalContext);

    eve_sprintf(mes, sizeof(mes), "Erasing flash");
    helperAppendMessage(mes, mesLen);
    EVE_CoCmd_flashErase(s_pHalContext);
    EVE_Cmd_waitFlush(s_pHalContext);

    eve_sprintf(mes, sizeof(mes), "Flash is erased");
    helperAppendMessage(mes, mesLen);

    eve_sprintf(mes, sizeof(mes), "Programming flash....");
    helperAppendMessage(mes, mesLen);
    EVE_sleep(500);

    // write flash
    Ftf_Write_File_To_Flash_With_Progressbar(s_pHalContext, 
        TEST_DIR "/Flash/flash.bin", "flash.bin", 0);
    
    helperClearMessage();    
    EVE_sleep(2000);
}

void SAMAPP_Flash() {
    SAMAPP_Flash_state();
    SAMAPP_Flash_program();
}


