/**
 * @file Animation.c
 * @brief Sample usage of animation 
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
#include "Animation.h"
#include "Common.h"
#include "FileTransfer.h"
#include "FlashHelper.h"

#define SAMAPP_DELAY EVE_sleep(2000)

static EVE_HalContext s_halContext;
static EVE_HalContext* s_pHalContext;
void SAMAPP_Animation();

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

    Flash_Init(s_pHalContext, TEST_DIR "/Flash/flash.bin", "flash.bin");
    EVE_Util_clearScreen(s_pHalContext);

    char *info[] =
    {  "EVE Sample Application",
        "This sample demonstrate the using of animation", 
        "",
        ""
    }; 

    while (TRUE) {
        WelcomeScreen(s_pHalContext, info);

        SAMAPP_Animation();

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

/**
* @brief API to demonstrate CMD_RUNANIM from Flash
*/
void SAMAPP_Animation_flash()
{
    uint32_t waitmask = -1;
    uint32_t play_control = 1024 * 512;
    uint32_t ch = 0;

    if (!FlashHelper_SwitchFullMode(s_pHalContext))
    {
        eve_printf("Flash is not able to switch full mode");
        return;
    }
    Draw_Text(s_pHalContext, "Example for: Run animation from Flash");
    EVE_CoCmd_flashSource(s_pHalContext, 4096);
    EVE_CoCmd_loadAsset(s_pHalContext, RAM_G, OPT_FLASH);

    EVE_CoCmd_memZero(s_pHalContext, play_control, 1);
    EVE_CoCmd_animStart(s_pHalContext, ch, RAM_G, ANIM_ONCE);
    EVE_CoCmd_animXY(s_pHalContext, ch, s_pHalContext->Width / 2, s_pHalContext->Height / 2);
    EVE_CoCmd_runAnim(s_pHalContext, waitmask, play_control);
    EVE_Cmd_waitFlush(s_pHalContext);

    SAMAPP_DELAY;
    EVE_CoCmd_memSet(s_pHalContext, play_control, 1, 1);
}

/**
* @brief API to demonstrate CMD_RUNANIM from Flash
*/
void SAMAPP_Animation_flash_no_reloc()
{
    uint32_t waitmask = -1;
    uint32_t play_control = 1024 * 512;
    uint32_t ch = 0;
    uint32_t anim_addr = 1454080;
    uint32_t anim_len = 123956;

    if (!FlashHelper_SwitchFullMode(s_pHalContext))
    {
        eve_printf("Flash is not able to switch full mode");
        return;
    }
    Draw_Text(s_pHalContext, "Example for: Run animation from Flash which is not relocatable");

    EVE_CoCmd_flashRead_flush(s_pHalContext, 0, anim_addr, anim_len);
    EVE_CoCmd_memZero(s_pHalContext, play_control, 1);
    EVE_CoCmd_animStart(s_pHalContext, ch, 0, ANIM_ONCE);
    EVE_CoCmd_animXY(s_pHalContext, ch, s_pHalContext->Width / 2, s_pHalContext->Height / 2);
    EVE_CoCmd_runAnim(s_pHalContext, waitmask, play_control);
    EVE_Cmd_waitFlush(s_pHalContext);

    SAMAPP_DELAY;
    EVE_CoCmd_memSet(s_pHalContext, play_control, 1, 1);
}

/**
* @brief API to demonstrate CMD_RUNANIM from SD
*/
void SAMAPP_Animation_SD()
{
    uint32_t waitmask = -1;
    uint32_t play_control = 1024 * 512;
    uint32_t ch = 0;
    uint32_t result = 0;

    Draw_Text(s_pHalContext, "Example for: Run animation from SD card");

	result = EVE_CoCmd_sdattach(s_pHalContext, OPT_4BIT | OPT_IS_SD, result);
	eve_printf_debug("SD attach status 0x%x \n", result);
	if (result != 0)
	{
		eve_printf_debug("SD attach failed\n");
		return;
	}

	result = EVE_CoCmd_fssource(s_pHalContext, "abstract.anim.ram_g.reloc", 0);
	eve_printf_debug("file read status 0x%x \n", result);
	if (result != 0)
	{
		eve_printf_debug("SD read failed\n");
		return;
	}

    EVE_CoCmd_loadAsset(s_pHalContext, RAM_G, OPT_FS);

    EVE_CoCmd_memZero(s_pHalContext, play_control, 1);
    EVE_CoCmd_animStart(s_pHalContext, ch, RAM_G, ANIM_ONCE);
    EVE_CoCmd_animXY(s_pHalContext, ch, s_pHalContext->Width / 2, s_pHalContext->Height / 2);
    EVE_CoCmd_runAnim(s_pHalContext, waitmask, play_control);
    EVE_Cmd_waitFlush(s_pHalContext);

    SAMAPP_DELAY;
    EVE_CoCmd_memSet(s_pHalContext, play_control, 1, 1);
}

/**
* @brief API to demonstrate CMD_RUNANIM from CMDB
*/
void SAMAPP_Animation_CMDB()
{
    uint32_t waitmask = -1;
    uint32_t play_control = 1024 * 512;
    uint32_t ch = 0;
    uint32_t transfered = 0;

    Draw_Text(s_pHalContext, "Example for: Run animation from command buffer");

    EVE_CoCmd_loadAsset(s_pHalContext, RAM_G, 0);
    EVE_Util_loadCmdFile(s_pHalContext, TEST_DIR "\\abstract.anim.ram_g.reloc", &transfered);

    EVE_CoCmd_memZero(s_pHalContext, play_control, 1);
    EVE_CoCmd_animStart(s_pHalContext, ch, RAM_G, ANIM_ONCE);
    EVE_CoCmd_animXY(s_pHalContext, ch, s_pHalContext->Width / 2, s_pHalContext->Height / 2);
    EVE_CoCmd_runAnim(s_pHalContext, waitmask, play_control);
    EVE_Cmd_waitFlush(s_pHalContext);

    SAMAPP_DELAY;
    EVE_CoCmd_memSet(s_pHalContext, play_control, 1, 1);
}

/**
* @brief API to demonstrate CMD_RUNANIM from Mediafifo
*/
void SAMAPP_Animation_mediafifo()
{
    uint32_t waitmask = -1;
    uint32_t play_control = 1024 * 512;
    uint32_t ch = 0;
    uint32_t mediafifo = DDR_BITMAPS_STARTADDR;
    uint32_t mediafifolen = 16 * 1024;

    Draw_Text(s_pHalContext, "Example for: Run animation from Mediafifo");

    EVE_MediaFifo_set(s_pHalContext, mediafifo, mediafifolen);
    EVE_CoCmd_loadAsset(s_pHalContext, RAM_G, OPT_MEDIAFIFO);
    EVE_Util_loadMediaFile(s_pHalContext, TEST_DIR "\\abstract.anim.ram_g.reloc", NULL);

    EVE_CoCmd_memZero(s_pHalContext, play_control, 1);
    EVE_CoCmd_animStart(s_pHalContext, ch, RAM_G, ANIM_ONCE);
    EVE_CoCmd_animXY(s_pHalContext, ch, s_pHalContext->Width / 2, s_pHalContext->Height / 2);
    EVE_CoCmd_runAnim(s_pHalContext, waitmask, play_control);
    EVE_Cmd_waitFlush(s_pHalContext);

    SAMAPP_DELAY;
    EVE_CoCmd_memSet(s_pHalContext, play_control, 1, 1);
    EVE_MediaFifo_close(s_pHalContext);
}

/**
* @brief API to demonstrate animation from Flash by AnimDraw
*/
void SAMAPP_Animation_control()
{
    uint8_t txtAnim_ONCE[] = "Playing Animation by ANIM_ONCE";
    uint8_t txtAnim_LOOP[] = "Playing Animation by ANIM_LOOP";
    uint8_t txtAnim_HOLD[] = "Playing Animation by ANIM_HOLD";
    uint8_t txtPress2Stop[] = "Press \"Stop Anim\" again to exit";

    uint32_t prAnimLoop = ANIM_ONCE;
    const uint8_t* txtAnimLabel = txtAnim_ONCE;

    int timeout = 10000;
    int countStop = 0; // countStop = 2 to exit while loop
    uint16_t channel = 30;
    uint32_t tag = 0;
    uint32_t oldTag = 0;
    uint8_t isPlaying = 0;

    /* Switch Flash to FULL Mode */
    if (!FlashHelper_SwitchFullMode(s_pHalContext))
    {
        eve_printf("Flash is not able to switch full mode");
        return;
    }
    Draw_Text(s_pHalContext, "Example for: ANIM_ONCE, ANIM_LOOP, ANIM_HOLD");

    EVE_CoCmd_flashSource(s_pHalContext, 4096);
    EVE_CoCmd_loadAsset(s_pHalContext, RAM_G, OPT_FLASH);

    while (timeout > 0 && countStop < 2) {
        timeout--;
        Display_StartColor(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 });

        EVE_CoCmd_regRead(s_pHalContext, REG_TOUCH_TAG, &tag);

        if (tag != oldTag) {
            oldTag = tag;
        }
        else {
            tag = 0;
        }

        // Show text information
        if (isPlaying) {
            EVE_CoCmd_text(s_pHalContext, 200, 100, 31, 0, txtAnimLabel);
            EVE_CoCmd_animDraw(s_pHalContext, channel);

            EVE_CoDl_tag(s_pHalContext, 1);
            EVE_CoCmd_button(s_pHalContext, 900, 250, 200, 100, 30, 0, "Change Anim");
        }
        else {
            if (countStop == 0) {
                EVE_CoCmd_text(s_pHalContext, 200, 100, 31, 0, "Press \"Start Anim\" to start animation");
            }
            else if (countStop > 0) {
                EVE_CoCmd_text(s_pHalContext, 200, 100, 31, 0, txtPress2Stop);
            }

            EVE_CoDl_tag(s_pHalContext, 1);
            EVE_CoCmd_button(s_pHalContext, 900, 250, 200, 100, 30, 0, "Start Anim");
        }

        EVE_CoDl_tag(s_pHalContext, 2);
        EVE_CoCmd_button(s_pHalContext, 900, 450, 200, 100, 30, 0, "Stop Anim");

        if (tag == 1) {
            if (isPlaying == 1) {
                EVE_CoCmd_animStop(s_pHalContext, channel);
            }

            isPlaying = 1;
            countStop = 0;

            EVE_CoCmd_animStart(s_pHalContext, channel, RAM_G, prAnimLoop);
            EVE_CoCmd_animXY(s_pHalContext, channel, 500, 400);

            // change AnimLoop attribute for the next user button press
            // change text lable for the current press
            // ONCE -> HOLD -> LOOP -> ONCE -> ...
            switch (prAnimLoop) {
            case ANIM_ONCE:
                prAnimLoop = ANIM_HOLD;
                txtAnimLabel = txtAnim_ONCE;
                break;
            case ANIM_HOLD:
                prAnimLoop = ANIM_LOOP;
                txtAnimLabel = txtAnim_HOLD;
                break;
            case ANIM_LOOP:
                prAnimLoop = ANIM_ONCE;
                txtAnimLabel = txtAnim_LOOP;
                break;
            default: break;
            }
        }
        if (tag == 2) {
            isPlaying = 0;
            EVE_CoCmd_animStop(s_pHalContext, channel);
            countStop++;
        }

        Display_End(s_pHalContext);
    }
}

/**
* @brief API to demonstrate 32-bit register REG_ANIM_ACTIVE
*/
void SAMAPP_Animation_animAtive() 
{
    char str[1000];

    if (!FlashHelper_SwitchFullMode(s_pHalContext))
    {
        eve_printf("Flash is not able to switch full mode");
        return;
    }
    Draw_Text(s_pHalContext, "Example for: REG_ANIM_ACTIVE usage");

    uint16_t channel = 1;
    const int16_t xAnim = (uint16_t)(s_pHalContext->Width / 2);
    const int16_t yAnim = (uint16_t)(s_pHalContext->Height/ 2);
    uint32_t reg_anim_active = 0;

    EVE_CoCmd_flashSource(s_pHalContext, 4096);
    EVE_CoCmd_loadAsset(s_pHalContext, RAM_G, OPT_FLASH);

    EVE_CoCmd_animStart(s_pHalContext, channel, RAM_G, ANIM_ONCE);
    EVE_CoCmd_animXY(s_pHalContext, channel, xAnim, yAnim);
    for (int i = 0; i < 100; i++) {
        Display_StartColor(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 });
        EVE_CoCmd_animDraw(s_pHalContext, channel);

        EVE_CoCmd_regRead(s_pHalContext, REG_ANIM_ACTIVE, &reg_anim_active);
        snprintf(str, 1000, "REG_ANIM_ACTIVE: %u\n", reg_anim_active);
        EVE_CoCmd_text(s_pHalContext, xAnim, 50, 30, OPT_CENTER, str);
        Display_End(s_pHalContext);

        if (0 == reg_anim_active) {
            break;
        }
    }

    SAMAPP_DELAY;
}

/**
* @brief API to demonstrate animation from Flash by AnimFrame
*/
void SAMAPP_Animation_animFrame() 
{
    if (!FlashHelper_SwitchFullMode(s_pHalContext))
    {
        eve_printf("Flash is not able to switch full mode");
        return;
    }
    Draw_Text(s_pHalContext, "Example for: ANIMATION from Flash by AnimFrame");

    EVE_CoCmd_flashSource(s_pHalContext, 4096);
    EVE_CoCmd_loadAsset(s_pHalContext, RAM_G, OPT_FLASH);

    for (uint16_t frame = 0; frame < 40; frame++) {
        Display_StartColor(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 });
        EVE_CoCmd_animFrame(s_pHalContext, s_pHalContext->Width / 2, s_pHalContext->Height / 2, RAM_G, frame);
        Display_End(s_pHalContext);
    }
    SAMAPP_DELAY;
}

void SAMAPP_Animation() 
{
    SAMAPP_Animation_flash();
    SAMAPP_Animation_flash_no_reloc();
    SAMAPP_Animation_SD();
    SAMAPP_Animation_CMDB();
    SAMAPP_Animation_mediafifo();
    SAMAPP_Animation_control();
    SAMAPP_Animation_animAtive();
    SAMAPP_Animation_animFrame();
}


