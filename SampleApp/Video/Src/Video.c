/**
 * @file Video.c
 * @brief Sample usage of video display
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

#include "Video.h"
#include "FileTransfer.h"
#include "FlashHelper.h"

#define VIDEO_DELAY_MS        10
#define VIDEO_TITLE_FONT      31
#define VIDEO_VOL             155
#define VIDEO_LOAD_ADDR       RAM_G
#define VIDEO_BTN_W           400
#define VIDEO_BTN_H           120
#define VIDEO_BTN_TAG         1
#define VIDEO_BTN_Y_INC       50
#define VIDEO_MEDIAFIFO_ADDR  DDR_BITMAPS_STARTADDR
#define VIDEO_MEDIAFIFO_SZ    500 * 1024
#define VIDEO_LOGO_X          100
#define VIDEO_LOGO_Y          100
#define VIDEO_VIDEO_X         400

static EVE_HalContext s_halContext;
static EVE_HalContext* s_pHalContext;
static void SAMAPP_Video();
static uint32_t addr_flash = 0;

int main(int argc, char* argv[])
{
    s_pHalContext = &s_halContext;
    Gpu_Init(s_pHalContext);
    Display_Config(s_pHalContext, YCBCR, MODE_VIDEO);

    // read and store calibration setting
#if GET_CALIBRATION == 1
    EVE_Calibrate(s_pHalContext);
    Calibration_Save(s_pHalContext);
#endif

    addr_flash = Flash_Init(s_pHalContext, FLASH_FILE);
    EVE_Util_clearScreen(s_pHalContext);

    char *info[] =
    {  "EVE Sample Application",
        "This sample demonstrates the use of video", 
        "",
        ""
    };
    WelcomeScreen(s_pHalContext, info);

    SAMAPP_Video();

    EVE_Util_clearScreen(s_pHalContext);
    Gpu_Release(s_pHalContext);
    return 0;
}

// Helper to set playback volume
static void helperSetVolume(EVE_HalContext *phost, uint8_t left, uint8_t right)
{
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_L_PB, left);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_R_PB, right);
}

/**
 * @brief Test AVI video playback full screen from flash
 * 
 * @note When using CMD_PLAYVIDEO/EVE_CoCmd_playVideo for video playback, SwapChain1(SC1) with double buffers is used by default. \n
 *  For configuration details, refer to 'Display_Config()' in Common.c.
 *
 * @param None
 * @return None
 */
static void SAMAPP_Video_fromFlashFullScreen()
{
    if (!FlashHelper_SwitchFullMode(s_pHalContext))
   {
        eve_printf("SwitchFullMode failed");
        return;
    }
    Draw_Text(s_pHalContext, "Example for: Video display from Flash - full screen");

    helperSetVolume(s_pHalContext, VIDEO_VOL, VIDEO_VOL);
    EVE_CoCmd_flashSource(s_pHalContext, addr_flash + VIDEO_FLASH_ADDR_FLOWER);
    EVE_CoCmd_playVideo(s_pHalContext, OPT_FULLSCREEN | OPT_FLASH | OPT_SOUND);

    EVE_Cmd_waitFlush(s_pHalContext);
}

/**
 * @brief API to demonstrate Video display frame by frame from Flash
 *
 * @param None
 * @return None
 */
static void SAMAPP_Video_frameByFrameFromFlash()
{
    const uint32_t completionPtr = RAM_G;
    const uint32_t videoSource = RAM_G + 4;

    if (!FlashHelper_SwitchFullMode(s_pHalContext))
    {
        eve_printf("SwitchFullMode failed");
        return;
    }
    Draw_Text(s_pHalContext, "Example for: Video display frame by frame from Flash");

    EVE_CoCmd_flashSource(s_pHalContext, addr_flash + VIDEO_FLASH_ADDR_FLOWER);
    EVE_CoCmd_videoStart(s_pHalContext, OPT_FLASH);
    do
    {
        EVE_CoCmd_videoFrame(s_pHalContext, videoSource, completionPtr);
        Display_Start(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
        EVE_CoCmd_setBitmap(s_pHalContext, videoSource, RGB565, VIDEO_FLOWERS_1024_W, VIDEO_FLOWERS_1024_H);
        EVE_CoDl_begin(s_pHalContext, BITMAPS);
        EVE_CoDl_vertex2f_4(s_pHalContext, 0, 0);
        EVE_CoDl_end(s_pHalContext);
        Display_End(s_pHalContext);
        EVE_sleep(VIDEO_DELAY_MS);
    } while (EVE_Hal_rd32(s_pHalContext, completionPtr) != 0);
}

/**
 * @brief Video playback frame by frame with pause/resume button
 *
 * @param None
 * @return None
 */
static void SAMAPP_Video_frameByFrameFlashPauseResume()
{
    const uint32_t completionPtr = RAM_G;
    const uint32_t videoSource = RAM_G + 4;
    uint32_t btnW = VIDEO_BTN_W;
    uint32_t btnH = VIDEO_BTN_H;
    uint32_t btnX = VIDEO_FLOWERS_1024_W / 2 - btnW / 2;
    uint32_t btnY = VIDEO_FLOWERS_1024_H - btnH - VIDEO_BTN_Y_INC;
    static bool isPause = 0;
    const uint32_t btnPauseTag = VIDEO_BTN_TAG;
    static bool pressed = 0;
    uint8_t txtPause[2][20] = { "PAUSE", "RESUME" };
    uint32_t tag = 0;

    if (!FlashHelper_SwitchFullMode(s_pHalContext))
    {
        eve_printf("SwitchFullMode failed");
        return;
    }
    Draw_Text(s_pHalContext, "Example for: Video playback frame by frame with pause/resume button");

    EVE_CoCmd_flashSource(s_pHalContext, addr_flash + VIDEO_FLASH_ADDR_FLOWER);
    EVE_CoCmd_videoStart(s_pHalContext, OPT_FLASH);
    do
    {
        //only change the button when a release happened
        if ((EVE_CoCmd_regRead(s_pHalContext, REG_TOUCH_TAG, &tag)) && ((tag & 0xFFFFFF) == btnPauseTag))
        {
            pressed = 1;
        }
        else
        {
            if (pressed)
            {
                pressed = 0;
                isPause = !isPause;
            }
        }

        if (!isPause)
        {
            EVE_CoCmd_videoFrame(s_pHalContext, videoSource, completionPtr);
        }

        Display_Start(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
        // video
        EVE_CoCmd_setBitmap(s_pHalContext, videoSource, RGB565, VIDEO_FLOWERS_1024_W, VIDEO_FLOWERS_1024_H);
        EVE_CoDl_begin(s_pHalContext, BITMAPS);
        EVE_CoDl_vertex2f_4(s_pHalContext, 0, 0);
        EVE_CoDl_end(s_pHalContext);

        /*** Show a button ***/
        EVE_CoDl_tag(s_pHalContext, btnPauseTag);
        EVE_CoCmd_button(s_pHalContext, btnX, btnY, btnW, btnH, VIDEO_TITLE_FONT, 0, txtPause[isPause]);
        /*** Done button ***/
        Display_End(s_pHalContext);
        EVE_sleep(VIDEO_DELAY_MS);
    } while (EVE_Hal_rd32(s_pHalContext, completionPtr) != 0);
}

/**
 * @brief API used to demonstrate looping frame-by-frame Flash video playback with a stop button.
 *
 * @param None
 * @return None
 */
static void SAMAPP_Video_frameByFrameFlashLoop()
{
    const uint32_t completionPtr = RAM_G;
    const uint32_t videoSource = RAM_G + 4;
    uint32_t btnW = VIDEO_BTN_W;
    uint32_t btnH = VIDEO_BTN_H;
    uint32_t btnX = VIDEO_FLOWERS_1024_W / 2 - btnW / 2;
    uint32_t btnY = VIDEO_FLOWERS_1024_H + VIDEO_BTN_Y_INC;
    bool stop = false;
    const uint32_t btnStopTag = VIDEO_BTN_TAG;
    uint32_t tag = 0;

    if (!FlashHelper_SwitchFullMode(s_pHalContext))
    {
        eve_printf("SwitchFullMode failed");
        return;
    }
    Draw_Text(s_pHalContext, "Example for: Looping frame by frame Flash video playback with a stop button");

    while (!stop)
    {
        EVE_CoCmd_flashSource(s_pHalContext, addr_flash + VIDEO_FLASH_ADDR_FLOWER);
        EVE_CoCmd_videoStart(s_pHalContext, OPT_FLASH);
        do
        {
            if ((EVE_CoCmd_regRead(s_pHalContext, REG_TOUCH_TAG, &tag)) && ((tag & 0xFFFFFF) == btnStopTag))
            {
                stop = true;
                break;
            }

            EVE_CoCmd_videoFrame(s_pHalContext, videoSource, completionPtr);
            Display_Start(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
            EVE_CoCmd_setBitmap(s_pHalContext, videoSource, RGB565, VIDEO_FLOWERS_1024_W, VIDEO_FLOWERS_1024_H);
            EVE_CoDl_begin(s_pHalContext, BITMAPS);
            EVE_CoDl_vertex2f_4(s_pHalContext, 0, 0);
            EVE_CoDl_end(s_pHalContext);

            /*** Show a button ***/
            EVE_CoDl_tag(s_pHalContext, btnStopTag);
            EVE_CoCmd_button(s_pHalContext, btnX, btnY, btnW, btnH, VIDEO_TITLE_FONT, 0, "STOP VIDEO");
            Display_End(s_pHalContext);
            EVE_sleep(VIDEO_DELAY_MS);
        } while (EVE_Hal_rd32(s_pHalContext, completionPtr) != 0);
    }
}

/**
 * @brief AVI video playback directly from file
 *
 * @param None
 * @return None
 */
static void SAMAPP_Video_fromFileDirect()
{
#if defined(DISPLAY_RESOLUTION_WUXGA)
    Draw_Text(s_pHalContext, "Example for: Video display direct from file");
    Display_Config(s_pHalContext, YCBCR, MODE_DIRECTVIDEO);

    helperSetVolume(s_pHalContext, VIDEO_VOL, VIDEO_VOL);
    eve_printf_debug("Video playback starts.\n");
    EVE_CoCmd_playVideo(s_pHalContext, OPT_DIRECT | OPT_SOUND | OPT_YCBCR);
    /* the video size must be the same as the screen resolution */
    EVE_Util_loadCmdFile(s_pHalContext, TEST_DIR VIDEO_FLOWERS_1920, NULL);

    EVE_Cmd_waitFlush(s_pHalContext);
    Display_Config(s_pHalContext, YCBCR, MODE_VIDEO);
    EVE_Util_clearScreen(s_pHalContext);
#endif
}

/**
 * @brief AVI video playback from SD card
 *
 * @param None
 * @return None
 */
static void SAMAPP_Video_fromSD()
{
    uint32_t result = 0;

    Draw_Text(s_pHalContext, "Example for: Video display from SD card");

    result = EVE_CoCmd_sdattach(s_pHalContext, OPT_4BIT | OPT_IS_SD, result);
    eve_printf_debug("SD attach status 0x%x \n", result);
    if (result != 0)
    {
        eve_printf_debug("SD attach failed\n");
        return;
    }

    result = EVE_CoCmd_fssource(s_pHalContext, VIDEO_AVI, 0);
    eve_printf_debug("file read status 0x%x \n", result);
    if (result != 0)
    {
        eve_printf_debug("SD read failed\n");
        return;
    }

    helperSetVolume(s_pHalContext, VIDEO_VOL, VIDEO_VOL);
    eve_printf_debug("Video playback starts.\n");
    EVE_CoCmd_playVideo(s_pHalContext, OPT_FS | OPT_SOUND | OPT_YCBCR);

    EVE_Cmd_waitFlush(s_pHalContext);
}

/**
 * @brief AVI video playback from SD card with pause/resume button
 *
 * @param None
 * @return None
 */
static void SAMAPP_Video_fromSDPauseResume()
{
    uint32_t btnW = VIDEO_BTN_W;
    uint32_t btnH = VIDEO_BTN_H;
    uint32_t btnX = VIDEO_FLOWERS_1024_W / 2 - btnW / 2;
    uint32_t btnY = VIDEO_FLOWERS_1024_H + VIDEO_BTN_Y_INC;
    static bool isPause = 0;
    static bool pressed = 0;
    uint32_t result = 0;

    Draw_Text(s_pHalContext, "Example for: Video display from SD card with pause/resume button");

    result = EVE_CoCmd_sdattach(s_pHalContext, OPT_4BIT | OPT_IS_SD, result);
    eve_printf_debug("SD attach status 0x%x \n", result);
    if (result != 0)
    {
        eve_printf_debug("SD attach failed\n");
        return;
    }

    result = EVE_CoCmd_fssource(s_pHalContext, VIDEO_AVI, 0);
    eve_printf_debug("file read status 0x%x \n", result);
    if (result != 0)
    {
        eve_printf_debug("SD read failed\n");
        return;
    }

    /*** Show a button ***/
    Display_Start(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_CoCmd_button(s_pHalContext, btnX, btnY, btnW, btnH, VIDEO_TITLE_FONT, 0, "Pause/Resume");

    helperSetVolume(s_pHalContext, VIDEO_VOL, VIDEO_VOL);
    eve_printf_debug("Video playback starts.\n");
    EVE_CoCmd_playVideo(s_pHalContext, OPT_FS | OPT_SOUND | OPT_YCBCR | OPT_OVERLAY | OPT_NODL);
    while (EVE_Cmd_space(s_pHalContext) != EVE_CMD_FIFO_SPACE)
    {
        uint32_t touch = EVE_Hal_rd32(s_pHalContext, REG_TOUCH_SCREEN_XY);
        uint32_t touch_x = touch >> 16;
        uint32_t touch_y = touch & 0xFFFF;
        //only change the button when a release happened
        if ((touch_x > btnX) && (touch_x < (btnX + btnW)) && (touch_y > btnY) && (touch_y < (btnY + btnH)))
        {
            pressed = 1;
        }
        else
        {
            if (pressed)
            {
                pressed = 0;
                isPause = !isPause;
                EVE_Hal_wr32(s_pHalContext, REG_PLAY_CONTROL, isPause ? 0 : 1);
            }
        }
    }

    EVE_Cmd_waitFlush(s_pHalContext);
    EVE_Hal_wr32(s_pHalContext, REG_PLAY_CONTROL, 1); // restore default value
}

/**
 * @brief AVI video playback from SD card with background
 *
 * @param None
 * @return None
 */
static void SAMAPP_Video_fromSDbackground()
{
    uint32_t result = 0;

    Draw_Text(s_pHalContext, "Example for: Video display from SD card with background");

    result = EVE_CoCmd_sdattach(s_pHalContext, OPT_4BIT | OPT_IS_SD, result);
    eve_printf_debug("SD attach status 0x%x \n", result);
    if (result != 0)
    {
        eve_printf_debug("SD attach failed\n");
        return;
    }

    result = EVE_CoCmd_fssource(s_pHalContext, VIDEO_JPG, 0);
    eve_printf_debug("file read status 0x%lx \n", result);
    if (result != 0)
    {
        eve_printf_debug("SD read failed\n");
        return;
    }

    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_CoCmd_loadImage(s_pHalContext, VIDEO_LOAD_ADDR, OPT_FS | OPT_TRUECOLOR);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_vertex2f_4(s_pHalContext, 0, 0);

    result = EVE_CoCmd_fssource(s_pHalContext, VIDEO_AVI, 0);
    eve_printf_debug("file read status 0x%lx \n", result);
    if (result != 0)
    {
        eve_printf_debug("SD read failed\n");
        return;
    }

    helperSetVolume(s_pHalContext, VIDEO_VOL, VIDEO_VOL);
    eve_printf_debug("Video playback starts.\n");
    EVE_CoCmd_playVideo(s_pHalContext, OPT_FS | OPT_SOUND | OPT_YCBCR | OPT_OVERLAY);

    EVE_Cmd_waitFlush(s_pHalContext);
}

/**
 * @brief Video playback from SD card frame by frame with pause/resume button
 *
 * @param None
 * @return None
 */
static void SAMAPP_Video_frameByFrameSDPauseResume()
{
    const uint32_t completionPtr = RAM_G;
    const uint32_t videoSource = RAM_G + 4;
    uint32_t btnX = VIDEO_FLOWERS_1024_W / 2 - VIDEO_BTN_W / 2;
    uint32_t btnY = VIDEO_FLOWERS_1024_H - VIDEO_BTN_H - VIDEO_BTN_Y_INC;
    static bool isPause = 0;
    const uint32_t btnPauseTag = VIDEO_BTN_TAG;
    static bool pressed = 0;
    uint8_t txtPause[2][20] = { "PAUSE", "RESUME" };
    uint32_t tag = 0;

    Draw_Text(s_pHalContext, "Example for: Video playback from SD card, frame by frame with pause/resume button");

    uint32_t result = 0;
    result = EVE_CoCmd_sdattach(s_pHalContext, OPT_4BIT | OPT_IS_SD, result);
    eve_printf_debug("SD attach status 0x%x \n", result);
    if (result != 0)
    {
        eve_printf_debug("SD attach failed\n");
        return;
    }
    result = EVE_CoCmd_fssource(s_pHalContext, VIDEO_AVI, 0);
    eve_printf_debug("file read status 0x%x \n", result);
    if (result != 0)
    {
        eve_printf_debug("SD read failed\n");
        return;
    }

    EVE_CoCmd_videoStart(s_pHalContext, OPT_FS);
    do
    {
        //only change the button when a release happened
        if ((EVE_CoCmd_regRead(s_pHalContext, REG_TOUCH_TAG, &tag)) && ((tag & 0xFFFFFF) == btnPauseTag))
        {
            pressed = 1;
        }
        else
        {
            if (pressed)
            {
                pressed = 0;
                isPause = !isPause;
            }
        }

        if (!isPause)
        {
            EVE_CoCmd_videoFrame(s_pHalContext, videoSource, completionPtr);
        }

        Display_Start(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
        // video
        EVE_CoCmd_setBitmap(s_pHalContext, videoSource, RGB565, VIDEO_FLOWERS_1024_W, VIDEO_FLOWERS_1024_H);
        EVE_CoDl_begin(s_pHalContext, BITMAPS);
        EVE_CoDl_vertex2f_4(s_pHalContext, 0, 0);
        EVE_CoDl_end(s_pHalContext);

        /*** Show a button ***/
        EVE_CoDl_tag(s_pHalContext, btnPauseTag);
        EVE_CoCmd_button(s_pHalContext, btnX, btnY, VIDEO_BTN_W, VIDEO_BTN_H, VIDEO_TITLE_FONT, 0, txtPause[isPause]);
        /*** Done button ***/
        Display_End(s_pHalContext);
        EVE_sleep(VIDEO_DELAY_MS);
    } while (EVE_Hal_rd32(s_pHalContext, completionPtr) != 0);
}

/**
 * @brief Video playback from an SD card, frame by frame, with double-buffer swapping
 *
 * @param None
 * @return None
 */
static void SAMAPP_Video_frameByFrameSDBufferSwap()
{
    const uint32_t completionPtr = RAM_G;
    const uint32_t videoSource1 = DDR_BITMAPS_STARTADDR1;
    const uint32_t videoSource2 = DDR_BITMAPS_STARTADDR2;
    uint32_t frame = 0;

    Draw_Text(s_pHalContext, "Example for: Video playback from SD card, frame by frame, with double-buffer swapping");

    uint32_t result = 0;
    result = EVE_CoCmd_sdattach(s_pHalContext, OPT_4BIT | OPT_IS_SD, result);
    eve_printf_debug("SD attach status 0x%x \n", result);
    if (result != 0)
    {
        eve_printf_debug("SD attach failed\n");
        return;
    }
    result = EVE_CoCmd_fssource(s_pHalContext, VIDEO_AVI, 0);
    eve_printf_debug("file read status 0x%x \n", result);
    if (result != 0)
    {
        eve_printf_debug("SD read failed\n");
        return;
    }

    EVE_CoCmd_videoStart(s_pHalContext, OPT_FS);
    do
    {
        Display_Start(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
        if (frame % 2 == 0)
        {
            EVE_CoCmd_videoFrame(s_pHalContext, videoSource1, completionPtr);
            EVE_CoDl_bitmapHandle(s_pHalContext, 1);
            EVE_CoCmd_setBitmap(s_pHalContext, videoSource1, RGB565, VIDEO_FLOWERS_1024_W, VIDEO_FLOWERS_1024_H);
        }
        else
        {
            EVE_CoCmd_videoFrame(s_pHalContext, videoSource2, completionPtr);
            EVE_CoDl_bitmapHandle(s_pHalContext, 2);
            EVE_CoCmd_setBitmap(s_pHalContext, videoSource2, RGB565, VIDEO_FLOWERS_1024_W, VIDEO_FLOWERS_1024_H);
        }
        EVE_CoDl_begin(s_pHalContext, BITMAPS);
        EVE_CoDl_vertex2f_4(s_pHalContext, 0, 0);
        EVE_CoDl_end(s_pHalContext);
        Display_End(s_pHalContext);
        EVE_sleep(VIDEO_DELAY_MS);
        frame++;
    } while (EVE_Hal_rd32(s_pHalContext, completionPtr) != 0);
}

/**
 * @brief Test AVI video playback via REG_CMDB_WRITE/REG_CMDB_SPACE
 *
 * @param None
 * @return None
 */
static void SAMAPP_Video_fromCMDB()
{
    Draw_Text(s_pHalContext, "Example for: Video display via command buffer");

    helperSetVolume(s_pHalContext, VIDEO_VOL, VIDEO_VOL);
    EVE_CoCmd_playVideo(s_pHalContext, OPT_SOUND);
    EVE_Util_loadCmdFile(s_pHalContext, TEST_DIR VIDEO_FLOWERS_1024, NULL);

    EVE_Cmd_waitFlush(s_pHalContext);
}

/**
 * @brief AVI video playback via CMD buffer
 *
 * @param None
 * @return None
 */
static void SAMAPP_Video_fromCMDBwithLogoBeside()
{
    Draw_Text(s_pHalContext, "Example for: Video display via command buffer with LOGO");

    /* construct the DL and display */
    EVE_Util_loadImageFile(s_pHalContext, VIDEO_LOAD_ADDR, TEST_DIR VIDEO_PNG_LOGO, NULL, OPT_TRUECOLOR);

    Display_Start(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_bitmapHandle(s_pHalContext, 1);
    EVE_CoCmd_setBitmap(s_pHalContext, VIDEO_LOAD_ADDR, ARGB8, VIDEO_PNG_LOGO_W, VIDEO_PNG_LOGO_H);
    EVE_CoDl_vertex2f_4(s_pHalContext, VIDEO_LOGO_X * 16, VIDEO_LOGO_Y * 16);
    EVE_CoDl_end(s_pHalContext);

    helperSetVolume(s_pHalContext, VIDEO_VOL, VIDEO_VOL);
    EVE_CoDl_bitmapHandle(s_pHalContext, 0);
    EVE_CoDl_vertexTranslateX(s_pHalContext, VIDEO_VIDEO_X * 16);
    EVE_CoDl_vertexTranslateY(s_pHalContext, VIDEO_LOGO_Y * 16);
    EVE_CoCmd_playVideo(s_pHalContext, OPT_OVERLAY | OPT_SOUND);
    EVE_Util_loadCmdFile(s_pHalContext, TEST_DIR VIDEO_FLOWERS_1024, NULL);

    EVE_Cmd_waitFlush(s_pHalContext);
}

/**
 * @brief Test AVI video playback via MediaFiFo and full screen
 *
 * @param None
 * @return None
 */
static void SAMAPP_Video_fromMediafifoFullscreen()
{
    Draw_Text(s_pHalContext, "Example for: Video display via Media FIFO full screen");

    /* start video playback, load the data into media fifo */
    EVE_MediaFifo_set(s_pHalContext, VIDEO_MEDIAFIFO_ADDR, VIDEO_MEDIAFIFO_SZ); //address of the media fifo buffer
    helperSetVolume(s_pHalContext, VIDEO_VOL, VIDEO_VOL);
    EVE_CoCmd_playVideo(s_pHalContext, OPT_MEDIAFIFO | OPT_SOUND | OPT_FULLSCREEN);
    EVE_Util_loadMediaFile(s_pHalContext, TEST_DIR VIDEO_FLOWERS_1024, NULL);

    EVE_Cmd_waitFlush(s_pHalContext);
    EVE_MediaFifo_close(s_pHalContext);
}

/**
 * @brief Test AVI video playback via MediaFiFo and OPT_COMPLETEREG
 *
 * @param None
 * @return None
 */
static void SAMAPP_Video_fromMediafifowithCompletereg()
{
    Draw_Text(s_pHalContext, "Example for: Video display via Media FIFO with OPT_COMPLETEREG");

    /* start video playback, load the data into media fifo */
    EVE_MediaFifo_set(s_pHalContext, VIDEO_MEDIAFIFO_ADDR, VIDEO_MEDIAFIFO_SZ);
    helperSetVolume(s_pHalContext, VIDEO_VOL, VIDEO_VOL);
    uint32_t complete = 0;
    EVE_CoCmd_regRead(s_pHalContext, REG_OBJECT_COMPLETE, &complete);
    eve_printf_debug("REG_OBJECT_COMPLETE %ld\n", complete);
    EVE_CoCmd_playVideo(s_pHalContext, OPT_MEDIAFIFO | OPT_SOUND | OPT_COMPLETEREG);
    EVE_Util_loadMediaFile(s_pHalContext, TEST_DIR VIDEO_FLOWERS_1024, NULL);
    EVE_Hal_wr32(s_pHalContext, REG_OBJECT_COMPLETE, 1);
    eve_printf_debug("write 1 to REG_OBJECT_COMPLETE\n");

    EVE_Cmd_waitFlush(s_pHalContext);
    EVE_MediaFifo_close(s_pHalContext);
}

/**
 * @brief Test AVI video playback via MediaFiFo and OPT_COMPLETEREG with fail condition
 *
 * @param None
 * @return None
 */
static void SAMAPP_Video_fromMediafifowithCompleteregFailcase()
{
    Draw_Text(s_pHalContext, "Example for: Video display via Media FIFO with OPT_COMPLETEREG failcase");

    helperSetVolume(s_pHalContext, VIDEO_VOL, VIDEO_VOL);
    uint32_t complete = 0;
    EVE_CoCmd_regRead(s_pHalContext, REG_OBJECT_COMPLETE, &complete);
    eve_printf_debug("REG_OBJECT_COMPLETE %ld\n", complete);

    EVE_Util_loadRawFile(s_pHalContext, VIDEO_MEDIAFIFO_ADDR, TEST_DIR VIDEO_FLOWERS_1024);
    EVE_CoCmd_mediaFifo(s_pHalContext, VIDEO_MEDIAFIFO_ADDR, VIDEO_MEDIAFIFO_SZ); // set a mediafifo larger than data
    EVE_CoCmd_regWrite(s_pHalContext, REG_MEDIAFIFO_READ, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_MEDIAFIFO_WRITE, 1024 * 400); // only pass part of video data to mediafifo

    EVE_CoCmd_playVideo(s_pHalContext, OPT_MEDIAFIFO | OPT_SOUND | OPT_COMPLETEREG);
    while (EVE_Hal_rd32(s_pHalContext, REG_MEDIAFIFO_READ) < 1024 * 400)
        ;

    EVE_Hal_wr32(s_pHalContext, REG_OBJECT_COMPLETE, 1);
    eve_printf_debug("write 1 to REG_OBJECT_COMPLETE\n");
    EVE_Cmd_waitFlush(s_pHalContext);
    if (s_pHalContext->CmdFault)
    {
        EVE_Util_coprocessorFaultRecover(s_pHalContext);
    }

    Draw_Text(s_pHalContext, "Quit video play with partial data passed.");
    EVE_Hal_wr32(s_pHalContext, REG_OBJECT_COMPLETE, 0);
}

/**
 * @brief video playback via frame by frame from Mediafifo
 *
 * @param None
 * @return None
 */
static void SAMAPP_Video_frameByFrameMediafifo()
{
    const uint32_t completionPtr = RAM_G;
    const uint32_t videoSource = RAM_G + 4;
    uint32_t transfered = 0;

    Draw_Text(s_pHalContext, "Example for: Video display frame by frame from Mediafifo");

    /* start video playback, load the data into media fifo */
    EVE_MediaFifo_set(s_pHalContext, VIDEO_MEDIAFIFO_ADDR, VIDEO_MEDIAFIFO_SZ); //address of the media fifo buffer

    EVE_CoCmd_videoStart(s_pHalContext, OPT_MEDIAFIFO); //initialize AVI video decode
    EVE_Util_loadMediaFile(s_pHalContext, TEST_DIR VIDEO_FLOWERS_1024, &transfered);
    do
    {
        EVE_CoCmd_videoFrame(s_pHalContext, videoSource, completionPtr);
        Display_Start(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
        EVE_CoCmd_setBitmap(s_pHalContext, 4, RGB565, VIDEO_FLOWERS_1024_W, VIDEO_FLOWERS_1024_H);
        EVE_CoDl_begin(s_pHalContext, BITMAPS);
        EVE_CoDl_vertex2f_4(s_pHalContext, 0, 0);
        EVE_CoDl_end(s_pHalContext);
        Display_End(s_pHalContext);
        EVE_sleep(VIDEO_DELAY_MS);
        if (s_pHalContext->LoadFileRemaining > 0)
            EVE_Util_loadMediaFile(s_pHalContext, NULL, &transfered);
    } while ((EVE_Hal_rd32(s_pHalContext, completionPtr) != 0) && (EVE_MediaFifo_space(s_pHalContext) != VIDEO_MEDIAFIFO_SZ - 4)); //loop till end of the file
    EVE_Cmd_waitFlush(s_pHalContext);
    EVE_MediaFifo_close(s_pHalContext);
}

/**
 * @brief API demonstrating looping video playback from Mediafifo with stop button.
 *
 * @param None
 * @return None
 */
static void SAMAPP_Video_fromMediafifoLoop()
{
    uint32_t btnW = VIDEO_BTN_W;
    uint32_t btnH = VIDEO_BTN_H;
    uint32_t btnX = VIDEO_FLOWERS_1024_W / 2 - btnW / 2;
    uint32_t btnY = VIDEO_FLOWERS_1024_H + VIDEO_BTN_Y_INC;
    bool stop = false;

    Draw_Text(s_pHalContext, "Example for: Looping Mediafifo video playback with stop button");

    helperSetVolume(s_pHalContext, VIDEO_VOL, VIDEO_VOL);
    EVE_Util_loadRawFile(s_pHalContext, VIDEO_MEDIAFIFO_ADDR, TEST_DIR VIDEO_FLOWERS_1024);
    EVE_CoCmd_mediaFifo(s_pHalContext, VIDEO_MEDIAFIFO_ADDR, 5000 * 1024); // set a mediafifo larger than data
    while (!stop)
    {
        Display_Start(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
        EVE_CoCmd_button(s_pHalContext, btnX, btnY, btnW, btnH, VIDEO_TITLE_FONT, 0, "STOP VIDEO");

        EVE_CoCmd_regWrite(s_pHalContext, REG_MEDIAFIFO_READ, 0);
        EVE_CoCmd_regWrite(s_pHalContext, REG_MEDIAFIFO_WRITE, 5000 * 1024);
        EVE_CoCmd_playVideo(s_pHalContext, OPT_MEDIAFIFO | OPT_SOUND | OPT_OVERLAY);

        while (EVE_Cmd_space(s_pHalContext) != EVE_CMD_FIFO_SPACE)
        {
            uint32_t touch = EVE_Hal_rd32(s_pHalContext, REG_TOUCH_SCREEN_XY);
            uint32_t touch_x = touch >> 16;
            uint32_t touch_y = touch & 0xFFFF;
            if ((touch_x > btnX) && (touch_x < (btnX + btnW)) && (touch_y > btnY) && (touch_y < (btnY + btnH)))
            {
                stop = true;
                EVE_Hal_wr32(s_pHalContext, REG_PLAY_CONTROL, -1);
                break;
            }
        }
        EVE_Cmd_waitFlush(s_pHalContext);
        if (stop == true)
            EVE_Hal_wr32(s_pHalContext, REG_PLAY_CONTROL, 1); // restore default value
    }
}

/**
 * @brief Main entry to run all video demos
 *
 * @param None
 * @return None
 */
static void SAMAPP_Video()
{
    SAMAPP_Video_fromFlashFullScreen();
    SAMAPP_Video_frameByFrameFromFlash();
    SAMAPP_Video_frameByFrameFlashPauseResume();
    SAMAPP_Video_frameByFrameFlashLoop();
    SAMAPP_Video_fromFileDirect();
    SAMAPP_Video_fromSD();
    SAMAPP_Video_fromSDPauseResume();
    SAMAPP_Video_fromSDbackground();
    SAMAPP_Video_frameByFrameSDPauseResume();
    SAMAPP_Video_frameByFrameSDBufferSwap();
    SAMAPP_Video_fromCMDB();
    SAMAPP_Video_fromCMDBwithLogoBeside();
    SAMAPP_Video_fromMediafifoFullscreen();
    SAMAPP_Video_fromMediafifowithCompletereg();
    SAMAPP_Video_fromMediafifowithCompleteregFailcase();
    SAMAPP_Video_frameByFrameMediafifo();
    SAMAPP_Video_fromMediafifoLoop();
}