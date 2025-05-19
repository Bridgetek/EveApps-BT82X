/**
 * @file Video.c
 * @brief Sample usage of video display
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
#include "Video.h"

static EVE_HalContext s_halContext;
static EVE_HalContext* s_pHalContext;
void SAMAPP_Video();

int main(int argc, char* argv[])
{
    s_pHalContext = &s_halContext;
    Gpu_Init(s_pHalContext);
    LVDS_Config(s_pHalContext, YCBCR, MODE_VIDEO);

    // read and store calibration setting
#if !defined(BT8XXEMU_PLATFORM) && GET_CALIBRATION == 1
    EVE_Calibrate(s_pHalContext);
    Calibration_Save(s_pHalContext);
#endif

    Flash_Init(s_pHalContext, TEST_DIR "/Flash/flash.bin", "flash.bin");
    EVE_Util_clearScreen(s_pHalContext);

    char *info[] =
    {  "EVE Sample Application",
        "This sample demonstrate the using of video", 
        "",
        ""
    }; 

    while (TRUE) {
        WelcomeScreen(s_pHalContext, info);

        SAMAPP_Video();

        EVE_Util_clearScreen(s_pHalContext);

        Gpu_Release(s_pHalContext);

        /* Init HW Hal for next loop*/
        Gpu_Init(s_pHalContext);
        LVDS_Config(s_pHalContext, YCBCR, MODE_VIDEO);
#if !defined(BT8XXEMU_PLATFORM) && GET_CALIBRATION == 1
        Calibration_Restore(s_pHalContext);
#endif
    }

    return 0;
}

/**
* @brief Test AVI video playback full screen from flash
*
*/
void SAMAPP_Video_fromFlashFullScreen()
{
    if (!FlashHelper_SwitchFullMode(s_pHalContext))
   {
        eve_printf("SwitchFullMode failed");
        return;
    }
    Draw_Text(s_pHalContext, "Example for: Video display from Flash - full screen ");

    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_L_PB, 155);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_R_PB, 155);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAY_CONTROL, 1); // restore default value
    EVE_CoCmd_flashSource(s_pHalContext, 4096);
    EVE_CoCmd_playVideo(s_pHalContext, OPT_FULLSCREEN | OPT_FLASH | OPT_SOUND);

    EVE_Cmd_waitFlush(s_pHalContext);
}

/**
* @brief API to demonstrate Video display frame by frame from Flash
*
*/
void SAMAPP_Video_frameByFrameFromFlash()
{
    const uint32_t videoW = 1024;
    const uint32_t videoH = 600;
    const uint32_t videoSource = 4;
    const uint32_t completionPtr = 0;
    uint32_t ptr_val = 0;

    if (!FlashHelper_SwitchFullMode(s_pHalContext))
    {
        eve_printf("SwitchFullMode failed");
        return;
    }
    Draw_Text(s_pHalContext, "Example for: Video display frame by frame from Flash");

    EVE_CoCmd_flashSource(s_pHalContext, 4096);
    EVE_CoCmd_videoStart(s_pHalContext, OPT_FLASH);
    do
    {
        EVE_CoCmd_videoFrame(s_pHalContext, videoSource, completionPtr);
        Display_StartColor(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 });
        EVE_CoCmd_setBitmap(s_pHalContext, videoSource, RGB565, videoW, videoH);
        EVE_CoDl_begin(s_pHalContext, BITMAPS);
        EVE_CoDl_vertex2f_4(s_pHalContext, 0, 0);
        EVE_CoDl_end(s_pHalContext);
        Display_End(s_pHalContext);
		EVE_sleep(10);
    } while ((EVE_CoCmd_regRead(s_pHalContext, completionPtr, &ptr_val)) && (ptr_val != 0));
}

/**
* @brief Video playback frame by frame with pause/resume button
*
*/
void SAMAPP_Video_frameByFrameFlashPauseResume()
{
    const uint32_t videoW = 1024;
    const uint32_t videoH = 600;
    const uint32_t videoSource = 4;
    const uint32_t completionPtr = 0;
    uint32_t btnW = 200;
    uint32_t btnH = 120;
    uint32_t btnX = videoW / 2 - btnW / 2;
    uint32_t btnY = videoH - btnH - 50;
    static bool isPause = 0;
    const uint32_t btnPauseTag = 1;
    static bool pressed = 0;
    uint8_t txtPause[2][20] = { "PAUSE", "RESUME" };
    uint32_t transfered = 0;
    uint32_t tag = 0;
    uint32_t ptr_val = 0;

    if (!FlashHelper_SwitchFullMode(s_pHalContext))
    {
        eve_printf("SwitchFullMode failed");
        return;
    }
    Draw_Text(s_pHalContext, "Example for: Video playback frame by frame with pause/resume button");

    EVE_CoCmd_flashSource(s_pHalContext, 4096);
    EVE_CoCmd_videoStart(s_pHalContext, OPT_FLASH);
    do
    {
        //only change the button when a release happened
        if ((EVE_CoCmd_regRead(s_pHalContext, REG_TOUCH_TAG, &tag)) && ((tag & 0xFF) == btnPauseTag))
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

        Display_StartColor(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 });
        // video
        EVE_CoCmd_setBitmap(s_pHalContext, videoSource, RGB565, videoW, videoH);
        EVE_CoDl_begin(s_pHalContext, BITMAPS);
        EVE_CoDl_vertex2f_4(s_pHalContext, 0, 0);
        EVE_CoDl_end(s_pHalContext);

        /*** Show a button ***/
        EVE_CoDl_tag(s_pHalContext, btnPauseTag);
        EVE_CoCmd_button(s_pHalContext, btnX, btnY, btnW, btnH, 31, 0, txtPause[isPause]);
        /*** Done button ***/
        Display_End(s_pHalContext);
		EVE_sleep(10);
    } while ((EVE_CoCmd_regRead(s_pHalContext, completionPtr, &ptr_val)) && (ptr_val != 0));
}

/**
* @brief AVI video playback directly from file
*
*/
void SAMAPP_Video_fromFileDirect()
{
#if defined(DISPLAY_RESOLUTION_WUXGA)
    Draw_Text(s_pHalContext, "Example for: Video display direct from file");
    LVDS_Config(s_pHalContext, YCBCR, MODE_DIRECTVIDEO);

    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_L_PB, 155);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_R_PB, 155);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAY_CONTROL, 1); // restore default value
    eve_printf_debug("Video playback starts.\n");
    EVE_CoCmd_playVideo(s_pHalContext, OPT_DIRECT | OPT_SOUND | OPT_YCBCR);
    uint32_t transfered = 0;
    /* the video size must be the same as the screen resolution */
    EVE_Util_loadCmdFile(s_pHalContext, TEST_DIR "\\flowers_1920x1200.avi", &transfered);

    EVE_Cmd_waitFlush(s_pHalContext);
    LVDS_Config(s_pHalContext, YCBCR, MODE_VIDEO);
    EVE_Util_clearScreen(s_pHalContext);
#endif
}

/**
* @brief AVI video playback from SD card
*
*/
void SAMAPP_Video_fromSD()
{
    const char *file = "flowers_1024x600.avi";
    uint32_t result = 0;

    Draw_Text(s_pHalContext, "Example for: Video display from SD card");

    result = EVE_CoCmd_sdattach(s_pHalContext, OPT_4BIT | OPT_IS_SD, result);
    eve_printf_debug("SD attach status 0x%x \n", result);
	if (result != 0)
	{
		eve_printf_debug("SD attach failed\n");
		return;
	}

    result = EVE_CoCmd_fssource(s_pHalContext, file, 0);
    eve_printf_debug("file read status 0x%x \n", result);
	if (result != 0)
	{
		eve_printf_debug("SD read failed\n");
		return;
	}

    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_L_PB, 155);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_R_PB, 155);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAY_CONTROL, 1); // restore default value
    eve_printf_debug("Video playback starts.\n");
    EVE_CoCmd_playVideo(s_pHalContext, OPT_FS | OPT_SOUND | OPT_YCBCR);

    EVE_Cmd_waitFlush(s_pHalContext);
}

/**
* @brief AVI video playback from SD card with background
*
*/
void SAMAPP_Video_fromSDbackground()
{
    const char *file;
    uint32_t result = 0;

    Draw_Text(s_pHalContext, "Example for: Video display from SD card with background");

    result = EVE_CoCmd_sdattach(s_pHalContext, OPT_4BIT | OPT_IS_SD, result);
    eve_printf_debug("SD attach status 0x%x \n", result);
	if (result != 0)
	{
		eve_printf_debug("SD attach failed\n");
		return;
	}

    file = "pic_in_SD.jpg";
    eve_printf_debug("picture name %s \n", file);
    result = EVE_CoCmd_fssource(s_pHalContext, file, 0);
    eve_printf_debug("file read status 0x%lx \n", result);
	if (result != 0)
	{
		eve_printf_debug("SD read failed\n");
		return;
	}

    Display_Start(s_pHalContext);
    EVE_CoCmd_loadImage(s_pHalContext, DDR_BITMAPS_STARTADDR2, OPT_FS | OPT_TRUECOLOR);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_vertex2f_4(s_pHalContext, 0, 0);
    EVE_CoDl_end(s_pHalContext);

    file = "flowers_1024x600.avi";
    eve_printf_debug("video name %s \n", file);
    result = EVE_CoCmd_fssource(s_pHalContext, file, 0);
    eve_printf_debug("file read status 0x%lx \n", result);

    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_L_PB, 155);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_R_PB, 155);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAY_CONTROL, 1); // restore default value
    eve_printf_debug("Video playback starts.\n");
    EVE_CoCmd_playVideo(s_pHalContext, OPT_FS | OPT_SOUND | OPT_YCBCR | OPT_OVERLAY);

    EVE_Cmd_waitFlush(s_pHalContext);
}

/**
* @brief Video playback from SD card frame by frame with pause/resume button
*
*/
void SAMAPP_Video_frameByFrameSDPauseResume()
{
    const uint32_t videoW = 1024;
    const uint32_t videoH = 600;
    const char *file = "flowers_1024x600.avi";
    const uint32_t videoSource = 4;
    const uint32_t completionPtr = 0;
    uint32_t btnW = 200;
    uint32_t btnH = 120;
    uint32_t btnX = videoW / 2 - btnW / 2;
    uint32_t btnY = videoH - btnH - 50;
    static bool isPause = 0;
    const uint32_t btnPauseTag = 1;
    static bool pressed = 0;
    uint8_t txtPause[2][20] = { "PAUSE", "RESUME" };
    uint32_t transfered = 0;
    uint32_t tag = 0;
    uint32_t ptr_val = 0;

    Draw_Text(s_pHalContext, "Example for: Video playback frame by frame with pause/resume button");

    uint32_t result = 0;
    result = EVE_CoCmd_sdattach(s_pHalContext, OPT_4BIT | OPT_IS_SD, result);
    eve_printf_debug("SD attach status 0x%x \n", result);
	if (result != 0)
	{
		eve_printf_debug("SD attach failed\n");
		return;
	}
    result = EVE_CoCmd_fssource(s_pHalContext, file, 0);
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
        if ((EVE_CoCmd_regRead(s_pHalContext, REG_TOUCH_TAG, &tag)) && ((tag & 0xFF) == btnPauseTag))
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

        Display_StartColor(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 });
        // video
        EVE_CoCmd_setBitmap(s_pHalContext, videoSource, RGB565, videoW, videoH);
        EVE_CoDl_begin(s_pHalContext, BITMAPS);
        EVE_CoDl_vertex2f_4(s_pHalContext, 0, 0);
        EVE_CoDl_end(s_pHalContext);

        /*** Show a button ***/
        EVE_CoDl_tag(s_pHalContext, btnPauseTag);
        EVE_CoCmd_button(s_pHalContext, btnX, btnY, btnW, btnH, 31, 0, txtPause[isPause]);
        /*** Done button ***/
        Display_End(s_pHalContext);
		EVE_sleep(10);
	} while ((EVE_CoCmd_regRead(s_pHalContext, completionPtr, &ptr_val)) && (ptr_val != 0));
}

/**
* @brief Test AVI video playback via REG_CMDB_WRITE/REG_CMDB_SPACE
*
*/
void SAMAPP_Video_fromCMDB()
{
    Draw_Text(s_pHalContext, "Example for: Video display via command buffer");

    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_L_PB, 155);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_R_PB, 155);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAY_CONTROL, 1); // restore default value
    EVE_CoCmd_playVideo(s_pHalContext, OPT_SOUND);
    uint32_t transfered = 0;
    EVE_Util_loadCmdFile(s_pHalContext, TEST_DIR "\\flowers_1024x600.avi", &transfered);

    EVE_Cmd_waitFlush(s_pHalContext);
}

/**
* @brief AVI video playback via CMD buffer
*
*/
void SAMAPP_Video_fromCMDBwithLogoBeside()
{
    Draw_Text(s_pHalContext, "Example for: Video display via command buffer with LOGO");

    /* construct the DL and display */
    EVE_Util_loadImageFile(s_pHalContext, DDR_BITMAPS_STARTADDR, TEST_DIR "\\images-logo.png", NULL, OPT_TRUECOLOR);

    Display_StartColor(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 });
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_bitmapHandle(s_pHalContext, 1);
    EVE_CoCmd_setBitmap(s_pHalContext, DDR_BITMAPS_STARTADDR, ARGB8, 236, 72);
    EVE_CoDl_vertex2f_4(s_pHalContext, 100 * 16, 100 * 16);
    EVE_CoDl_end(s_pHalContext);

    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_L_PB, 155);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_R_PB, 155);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAY_CONTROL, 1); // restore default value
    EVE_CoDl_bitmapHandle(s_pHalContext, 0);
    EVE_CoDl_vertexTranslateX(s_pHalContext, 400 * 16);
    EVE_CoDl_vertexTranslateY(s_pHalContext, 100 * 16);
    EVE_CoCmd_playVideo(s_pHalContext, OPT_OVERLAY | OPT_SOUND);
    uint32_t transfered = 0;
    EVE_Util_loadCmdFile(s_pHalContext, TEST_DIR "\\flowers_1024x600.avi", &transfered);

    EVE_Cmd_waitFlush(s_pHalContext);
}

/**
* @brief Test AVI video playback via MediaFiFo and full screen
*
*/
void SAMAPP_Video_fromMediafifoFullscreen()
{
    Draw_Text(s_pHalContext, "Example for: Video display via Media FIFO full screen");

    uint32_t mediafifo;
    uint32_t mediafifolen;
    /* start video playback, load the data into media fifo */
    mediafifo = RAM_G; //the starting address of the media fifo
    mediafifolen = 16 * 1024;
    EVE_MediaFifo_set(s_pHalContext, mediafifo, mediafifolen); //address of the media fifo buffer
    eve_printf_debug("Mediafifo: Start address and length %d %d\n", mediafifo, mediafifolen);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_L_PB, 155);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_R_PB, 155);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAY_CONTROL, 1); // restore default value
    EVE_CoCmd_playVideo(s_pHalContext, OPT_MEDIAFIFO | OPT_SOUND | OPT_FULLSCREEN);
    EVE_Util_loadMediaFile(s_pHalContext, TEST_DIR "\\flowers_1024x600.avi", NULL, 0);

    EVE_Cmd_waitFlush(s_pHalContext);
    EVE_MediaFifo_close(s_pHalContext);
}

/**
* @brief Test AVI video playback via MediaFiFo and full screen
*
*/
void SAMAPP_Video_fromMediafifowithCompletereg()
{
    Draw_Text(s_pHalContext, "Example for: Video display via Media FIFO with OPT_COMPLETEREG");

    uint32_t mediafifo;
    uint32_t mediafifolen;
    /* start video playback, load the data into media fifo */
    mediafifo = RAM_G; //the starting address of the media fifo
    mediafifolen = 16 * 1024;
    EVE_MediaFifo_set(s_pHalContext, mediafifo, mediafifolen); //address of the media fifo buffer
    eve_printf_debug("Mediafifo: Start address and length %d %d\n", mediafifo, mediafifolen);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_L_PB, 155);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_R_PB, 155);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAY_CONTROL, 1); // restore default value
    uint32_t complete = 0;
    EVE_CoCmd_regRead(s_pHalContext, REG_OBJECT_COMPLETE, &complete);
    eve_printf_debug("REG_OBJECT_COMPLETE %ld\n", complete);
    EVE_CoCmd_playVideo(s_pHalContext, OPT_MEDIAFIFO | OPT_SOUND | OPT_COMPLETEREG);
    EVE_Util_loadMediaFile(s_pHalContext, TEST_DIR "\\flowers_1024x600.avi", NULL, OPT_COMPLETEREG);

    EVE_Cmd_waitFlush(s_pHalContext);
    EVE_MediaFifo_close(s_pHalContext);
}

/**
* @brief video playback via frame by frame from Mediafifo
*
*/
void SAMAPP_Video_frameByFrameMediafifo()
{
    uint16_t aviw = 1024;
    uint16_t avih = 600;
    uint16_t videoFrameStatusAddr = RAM_G; //the 4 byte address for the videoframe status
    uint32_t mediafifo;
    uint32_t mediafifolen;
    uint32_t transfered = 0;
    uint32_t ptr_val = 0;

    Draw_Text(s_pHalContext, "Example for: Video display frame by frame from Mediafifo");

    /* start video playback, load the data into media fifo */
    mediafifo = DDR_BITMAPS_STARTADDR; //the starting address of the media fifo, the begining space is for the decoded video frame
    mediafifolen = 400 * 1024;
    EVE_MediaFifo_set(s_pHalContext, mediafifo, mediafifolen); //address of the media fifo buffer
    eve_printf_debug("Mediafifo: Start address and length %d %d\n", mediafifo, mediafifolen);

    EVE_CoCmd_videoStart(s_pHalContext, OPT_MEDIAFIFO); //initialize AVI video decode
    EVE_Util_loadMediaFile(s_pHalContext, TEST_DIR "\\flowers_1024x600.avi", &transfered, 0);
    do
    {
        EVE_CoCmd_videoFrame(s_pHalContext, 4, videoFrameStatusAddr);
        Display_StartColor(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 });
        EVE_CoCmd_setBitmap(s_pHalContext, 4, RGB565, aviw, avih);
        EVE_CoDl_begin(s_pHalContext, BITMAPS);
        EVE_CoDl_vertex2f_4(s_pHalContext, 0, 0);
        EVE_CoDl_end(s_pHalContext);
        Display_End(s_pHalContext);
		EVE_sleep(10);
        if (s_pHalContext->LoadFileRemaining > 0)
            EVE_Util_loadMediaFile(s_pHalContext, NULL, &transfered, 0);
    } while ((EVE_CoCmd_regRead(s_pHalContext, videoFrameStatusAddr, &ptr_val)) && (ptr_val != 0) && (EVE_MediaFifo_space(s_pHalContext) != mediafifolen - 4)); //loop till end of the file
    EVE_Cmd_waitFlush(s_pHalContext);
    EVE_MediaFifo_close(s_pHalContext);
}

void SAMAPP_Video() {
    SAMAPP_Video_fromFlashFullScreen();
    SAMAPP_Video_frameByFrameFromFlash();
    SAMAPP_Video_frameByFrameFlashPauseResume();
    SAMAPP_Video_fromFileDirect();
    SAMAPP_Video_fromSD();
    SAMAPP_Video_fromSDbackground();
    SAMAPP_Video_frameByFrameSDPauseResume();
    SAMAPP_Video_fromCMDB();
    SAMAPP_Video_fromCMDBwithLogoBeside();
    SAMAPP_Video_fromMediafifoFullscreen();
	SAMAPP_Video_fromMediafifowithCompletereg();
    SAMAPP_Video_frameByFrameMediafifo();
}


