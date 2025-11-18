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
#include "PatchDefault.h"

#define FLASH_DELAY_MS 1000
#define FLASH_TITLE_FONT 31
#define FLASH_TITLE_Y_INC 60
#define FLASH_MSG_NUM 50
#define FLASH_MSG_LEN 50
#define FLASH_MSG_OFFSET_X 20

static EVE_HalContext s_halContext;
static EVE_HalContext* s_pHalContext;
static void SAMAPP_Flash();

int main(int argc, char* argv[])
{
    s_pHalContext = &s_halContext;
    Gpu_Init(s_pHalContext);
    Display_Config(s_pHalContext, YCBCR, MODE_PICTURE);

    // read and store calibration setting
#if GET_CALIBRATION == 1
    EVE_Calibrate(s_pHalContext);
    Calibration_Save(s_pHalContext);
#endif

    EVE_Util_clearScreen(s_pHalContext);

    char *info[] =
    {  "EVE Sample Application",
        "This sample demonstrates the use of flash", 
        "",
        ""
    };
    WelcomeScreen(s_pHalContext, info);

    SAMAPP_Flash();

    EVE_Util_clearScreen(s_pHalContext);
    Gpu_Release(s_pHalContext);
    return 0;
}

static char *flashMessage[FLASH_MSG_NUM];
static int messageIndex = 0;

// helper to append messages
static void helperAppendMessage(char *s, int num)
{
    flashMessage[messageIndex] = (char*)malloc(num);
    if (!flashMessage[messageIndex]) {
        eve_printf("Malloc failed");
        return;
    }

    memcpy(flashMessage[messageIndex], s, num);
    
    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 0, 0, 0 }, 0, 4);

    int y = 0;
    int x = FLASH_MSG_OFFSET_X;
    for (int i = 0; i <= messageIndex; i++)
    {
        EVE_CoCmd_text(s_pHalContext, x, y, FLASH_TITLE_FONT, 0, flashMessage[i]);
        y += FLASH_TITLE_Y_INC;
    }
    Display_End(s_pHalContext);

    messageIndex++;
    EVE_sleep(FLASH_DELAY_MS);
}

// helper to clear message
static void helperClearMessage()
{
    for (int i = 0; i < messageIndex; i++)
    {
        free(flashMessage[i]);
    }
    messageIndex = 0;
}

// helper to get flash state
static void helperGetFlashState(char *msg, int len, uint32_t state)
{
    uint32_t status = 0;
    const char *flash_status[4] = { "INIT", "DETACHED", "BASIC", "FULL" };

    EVE_CoCmd_regRead(s_pHalContext, REG_FLASH_STATUS, &status);
    if (state < 4)
    {
        if (status != state)
        {
            eve_sprintf(msg, len, "Flash is not able to change to %s\n", flash_status[state]);
            helperAppendMessage(msg, len);
        }
        else
        {
            eve_sprintf(msg, len, "Flash is changed to %s\n", flash_status[state]);
            helperAppendMessage(msg, len);
        }
    }
    eve_sprintf(msg, len, "Flash State: %s (%d)\n", flash_status[status], status);
    helperAppendMessage(msg, len);
    EVE_sleep(FLASH_DELAY_MS);
}
    /**
 * @brief API to demonstrate flash states
 *
 * @param None
 * @return None
 */
static void SAMAPP_Flash_state()
{
    char msg[FLASH_MSG_LEN];
    int msgLen = FLASH_MSG_LEN;
    uint32_t status = 0;

    Draw_Text(s_pHalContext, "Example for: Flash states");

    eve_printf_debug("\nFLASH STATES\n");
    helperGetFlashState(msg, msgLen, 4);

    eve_sprintf(msg, msgLen, "CMD_FLASHDETACH");
    helperAppendMessage(msg, msgLen);
    EVE_CoCmd_flashDetach(s_pHalContext);

    helperGetFlashState(msg, msgLen, FLASH_STATUS_DETACHED);

    eve_sprintf(msg, msgLen, "CMD_FLASHATTACH");
    helperAppendMessage(msg, msgLen);
    EVE_CoCmd_flashAttach(s_pHalContext);

    helperGetFlashState(msg, msgLen, FLASH_STATUS_BASIC);

    eve_sprintf(msg, msgLen, "CMD_FLASHFAST");
    helperAppendMessage(msg, msgLen);
    EVE_CoCmd_flashFast(s_pHalContext, 0);

    helperGetFlashState(msg, msgLen, FLASH_STATUS_FULL);

    eve_sprintf(msg, msgLen, "Check Flash Size");
    helperAppendMessage(msg, msgLen);
    EVE_CoCmd_regRead(s_pHalContext, REG_FLASH_SIZE, &status);
    eve_sprintf(msg, msgLen, "Flash size : %d MBytes", status);
    helperAppendMessage(msg, msgLen);

    helperClearMessage();
    EVE_sleep(FLASH_DELAY_MS);
}

/**
 * @brief API to demonstrate flash programming
 *
 * @param None
 * @return None
 */
static void SAMAPP_Flash_program()
{
    Draw_Text(s_pHalContext, "Example for: Flash program");

    // write flash
    uint32_t addr_flash = 0;
    Write_To_Flash_With_Progressbar(s_pHalContext, TEST_DIR "/Flash/flash.bin", &addr_flash, false);
    eve_printf_debug("flash image programed at 0x%lx\n", addr_flash);
}

/**
 * @brief API to demonstrate flash programming from EVE connected SD card
 *
 * @param None
 * @return None
 */
void SAMAPP_Flash_program_from_EVE_SD()
{
    Draw_Text(s_pHalContext, "Example for: Flash program from EVE connected SD card");

    uint32_t addr_flash = 0;
    Write_To_Flash_With_Progressbar(s_pHalContext, "flash.bin", &addr_flash, true);
    eve_printf_debug("flash image programed at 0x%lx\n", addr_flash);
}

/**
 * @brief Main entry to run all flash demos
 *
 * @param None
 * @return None
 */
static void SAMAPP_Flash() {
    SAMAPP_Flash_state();
    SAMAPP_Flash_program();
    SAMAPP_Flash_program_from_EVE_SD();
}


