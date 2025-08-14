/**
 * @file Audio.c
 * @brief Sample usage of Audio
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

#include "Audio.h"
#include "FileTransfer.h"
#include "FlashHelper.h"

#define pgm_read_byte(x) (*(x))

static EVE_HalContext s_halContext;
static EVE_HalContext* s_pHalContext;
void SAMAPP_Audio();

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
        "This sample demonstrate the using of Audio", 
        "",
        ""
    }; 

    while (true) {
        WelcomeScreen(s_pHalContext, info);

        SAMAPP_Audio();

        EVE_Util_clearScreen(s_pHalContext);

        EVE_Hal_close(s_pHalContext);
        EVE_Hal_release();

        /* Init HW Hal for next loop*/
        Gpu_Init(s_pHalContext);
        LVDS_Config(s_pHalContext, YCBCR, MODE_PICTURE);
#if GET_CALIBRATION == 1
        Calibration_Restore(s_pHalContext);
#endif
    }

    return 0;
}

char SAMAPP_Snd_Array[5 * 58] =
"Slce\0Sqrq\0Sinw\0Saww\0Triw\0Beep\0Alrm\0Warb\0Crsl\0Pp01\0Pp02\0Pp03\0Pp04\0Pp05\0Pp06\0Pp07\0Pp08\0Pp09\0Pp10\0Pp11\0Pp12\0Pp13\0Pp14\0Pp15\0Pp16\0DMF#\0DMF*\0DMF0\0DMF1\0DMF2\0DMF3\0DMF4\0DMF5\0DMF6\0DMF7\0DMF8\0DMF9\0Harp\0Xyph\0Tuba\0Glok\0Orgn\0Trmp\0Pian\0Chim\0MBox\0Bell\0Clck\0Swth\0Cowb\0Noth\0Hiht\0Kick\0Pop\0Clak\0Chak\0Mute\0uMut\0";

char SAMAPP_Snd_TagArray[58] = { 0x63, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
0x23, 0x2a, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x40, 0x41, 0x42, 0x43,
0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x60,
0x61 };

void helperHighLightBtn(int32_t tagvalsnd, const char* pTagArray, int32_t wbutton, int32_t hbutton)
{
    int32_t numbtnrow = 7;
    int32_t numbtncol = 8;

    for (int i = 0; i < numbtnrow; i++)
    {
        for (int j = 0; j < numbtncol; j++)
        {
            EVE_CoDl_tag(s_pHalContext, pgm_read_byte(pTagArray));
            if (tagvalsnd == pgm_read_byte(pTagArray))
            {
                /* red color for highlight effect */
                EVE_CoDl_colorRgb(s_pHalContext, 0x80, 0x00, 0x00);
                EVE_CoDl_colorA(s_pHalContext, 0x55);
                EVE_CoDl_begin(s_pHalContext, RECTS);
                EVE_CoDl_vertex2f_4(s_pHalContext, (j * wbutton + 2) * 16, (hbutton * i + 2) * 16);
                EVE_CoDl_vertex2f_4(s_pHalContext, ((j * wbutton) + wbutton - 2) * 16,
                        ((hbutton * i) + hbutton - 2) * 16);
                EVE_CoDl_end(s_pHalContext);
            }
            pTagArray++;
        }
    }
    EVE_CoDl_colorA(s_pHalContext, 255);
}

void helperDrawBtn(const char *pTagArray, int32_t wbutton, int32_t hbutton, char *StringArray, char *pString)
{
    int32_t numbtnrow = 7;
    int32_t numbtncol = 8;

    for (int i = 0; i < numbtnrow; i++)
    {
        for (int j = 0; j < numbtncol; j++)
        {
            EVE_CoDl_tag(s_pHalContext, pgm_read_byte(pTagArray));
            EVE_CoDl_colorRgb(s_pHalContext, 0x80, 0x80, 0x00);
            EVE_CoDl_begin(s_pHalContext, RECTS);
            EVE_CoDl_vertex2f_4(s_pHalContext, (j * wbutton + 2) * 16, (hbutton * i + 2) * 16);
            EVE_CoDl_vertex2f_4(s_pHalContext, ((j * wbutton) + wbutton - 2) * 16, ((hbutton * i) + hbutton - 2) * 16);
            EVE_CoDl_end(s_pHalContext);

            strcpy_s(StringArray, 8, pString);
            EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);
            EVE_CoCmd_text(s_pHalContext, (int16_t)((wbutton / 2) + j * wbutton),
            (int16_t)((hbutton / 2) + hbutton * i), 29, OPT_CENTER, StringArray);

            pString += (strlen(StringArray) + 1);
            pTagArray++;
        }
    }
}

/**
* @brief Sample app api to demonstrate audio
* @brief ple app api to demonstrate audio
* @brief  mple app api to demonstrate audio
*
*/
void SAMAPP_Audio_builtin()
{
    int32_t LoopFlag = 1000;
    int32_t wbutton;
    int32_t hbutton;
    int32_t tagval;
    int32_t tagvalsnd = -1;
    int32_t numbtnrow;
    int32_t numbtncol;
    int32_t prevtag = -1;
    uint32_t freqtrack = 0;
    uint32_t currfreq = 0;
    uint32_t prevcurrfreq;
    char* pString;
    char* pTagArray;
    char StringArray[8] = { 0 };

    Draw_Text(s_pHalContext, "Example for: Play built-in sound\n\n\nPlease touch on screen");

    /*************************************************************************/
    /* Below code demonstrates the usage of audio function. All the supported*/
    /* sounds and respective pitches are put as part of keys/buttons, by     */
    /* choosing particular key/button the sound is played                    */
    /*************************************************************************/
    numbtnrow = 7; //number of rows to be created - note that mute and unmute are not played in this application
    numbtncol = 8; //number of colomns to be created
    wbutton = (s_pHalContext->Width - 60) / numbtncol;
    hbutton = s_pHalContext->Height / numbtnrow;

    /* set the volume to maximum */
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_SOUND, 0xFF);
    /* set the tracker to track the slider for frequency */
    EVE_CoCmd_track(s_pHalContext, (int16_t) (s_pHalContext->Width - 40), 40, 15,
        (int16_t) (s_pHalContext->Height - 80), 100);
    EVE_Cmd_waitFlush(s_pHalContext);

    while (LoopFlag--)
    {
        EVE_CoCmd_regRead(s_pHalContext, REG_TOUCH_TAG, &tagval);
        EVE_CoCmd_regRead(s_pHalContext, REG_TRACKER, &freqtrack);

        if (100 == (freqtrack & 0xff))
        {
            currfreq = (freqtrack >> 16);
            currfreq = (88 * currfreq) / 65536;
            if (currfreq > 108)
                currfreq = 108;
        }
        if (tagval > 0)
        {
            if (tagval <= 99)
            {
                tagvalsnd = tagval;
            }
            if (0x63 == tagvalsnd)
            {
                tagvalsnd = 0;
            }
            if ((prevtag != tagval) || (prevcurrfreq != currfreq))
            {
                /* Play sound wrt pitch */
                EVE_CoCmd_regWrite(s_pHalContext, REG_SOUND,
                    (int32_t) (((currfreq + 21) << 8) | tagvalsnd));
                EVE_CoCmd_regWrite(s_pHalContext, REG_PLAY, 1);
            }
            if (0 == tagvalsnd)
                tagvalsnd = 99;
        }
        /* start a new display list for construction of screen */
        Display_Start(s_pHalContext);
        /* custom keys for sound input */
        pTagArray = SAMAPP_Snd_TagArray;
        pString = SAMAPP_Snd_Array;
        /* First draw all the rectangles followed by the font */
        helperDrawBtn(pTagArray, wbutton, hbutton, StringArray, pString);

        /* draw the highlight rectangle */
        pTagArray = SAMAPP_Snd_TagArray;
        helperHighLightBtn(tagvalsnd, pTagArray, wbutton, hbutton);

        /* Draw vertical slider bar for frequency control */
        StringArray[0] = '\0';
        strcat_s(StringArray, sizeof(StringArray), "Pt ");
        Gpu_Hal_Dec2Ascii(StringArray, (int32_t) (currfreq + 21));
        EVE_CoDl_tagMask(s_pHalContext, 0);
        EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width - 25), 10, 29, OPT_CENTER,
            StringArray);
        EVE_CoDl_tagMask(s_pHalContext, 1);
        EVE_CoDl_tag(s_pHalContext, 100);
        EVE_CoCmd_slider(s_pHalContext, (int16_t) (s_pHalContext->Width - 40), 40, 15,
            (int16_t) (s_pHalContext->Height - 80), 0, (int16_t) currfreq, 88);

        prevtag = tagval;

        prevcurrfreq = currfreq;
        /* Wait till coprocessor completes the operation */
        Display_End(s_pHalContext);
        EVE_sleep(10);
    }

    EVE_CoCmd_regWrite(s_pHalContext, REG_SOUND, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAY, 1);
}

/**
* @brief API to demonstrate music playback
*
*/
void SAMAPP_Audio_fromEABConvertedRaw()
{
    uint32_t currreadlen = 0;
    const uint8_t* pBuff = NULL;
    const uint8_t* music_playing = 0;
    uint32_t wrptr = 0;
    uint32_t result = 0;

    Draw_Text(s_pHalContext, "Example for: Play music");

    Draw_Text(s_pHalContext, "Now you will hear the music");

    EVE_Util_loadRawFile(s_pHalContext, RAM_G, TEST_DIR "\\Devil_Ride_30_11025hz.raw");
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_START, RAM_G); //Audio playback start address
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_LENGTH, 334707); //Length of raw data buffer in bytes
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_FREQ, 11025); //Frequency
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_FORMAT, LINEAR_SAMPLES); //Current sampling frequency
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_LOOP, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_L_PB, 155);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_R_PB, 155);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_PLAY, 1);

    // wait until the end of audio
    do
    {
        EVE_CoCmd_regRead(s_pHalContext, REG_PLAYBACK_PLAY, &result);
    } while (result != 0);

    //The file is done, mute the audio first.
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_LOOP, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_LENGTH, 0); //Length of raw data buffer in bytes
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_L_PB, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_R_PB, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_PLAY, 1);
}

/**
* @brief play WAV from command buffer
*
*/
void SAMAPP_Audio_playWavFromCMDB()
{
    Draw_Text(s_pHalContext, "Example for: play wav file from command buffer");

    Draw_Text(s_pHalContext, "Now you will hear the music");

    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_L_PB, 155);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_R_PB, 155);
    EVE_CoCmd_playWav(s_pHalContext, 0);
    uint32_t transfered = 0;
    EVE_Util_loadCmdFile(s_pHalContext, TEST_DIR "\\perfect_beauty.wav", &transfered);

    EVE_Cmd_waitFlush(s_pHalContext);
    //The file is done, mute the audio.
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_L_PB, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_R_PB, 0);
}

/**
* @brief play WAV from MediaFifo
*
*/
void SAMAPP_Audio_playWavFromMediaFifo()
{
    Draw_Text(s_pHalContext, "Example for: Play wav file from Media FiFo");
    uint32_t mediafifo;
    uint32_t mediafifolen;
    /* start video playback, load the data into media fifo */
    mediafifo = RAM_G; //the starting address of the media fifo
    mediafifolen = 16 * 1024;
    EVE_MediaFifo_set(s_pHalContext, mediafifo, mediafifolen); //address of the media fifo buffer

    Draw_Text(s_pHalContext, "Now you will hear the music");

    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_L_PB, 155);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_R_PB, 155);
    EVE_CoCmd_playWav(s_pHalContext, OPT_MEDIAFIFO);
    EVE_Util_loadMediaFile(s_pHalContext, TEST_DIR "\\perfect_beauty.wav", NULL);

    EVE_Cmd_waitFlush(s_pHalContext);
    //The file is done, mute the audio.
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_L_PB, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_R_PB, 0);
    EVE_MediaFifo_close(s_pHalContext);
}

/**
* @brief play WAV from SD card
*
*/
void SAMAPP_Audio_playWavFromSD()
{
    const char *file = "perfect_beauty.wav";
    uint32_t result = 0;

    Draw_Text(s_pHalContext, "Example for: play wav file from SD card");

    Draw_Text(s_pHalContext, "Now you will hear the music");

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
    EVE_CoCmd_playWav(s_pHalContext, OPT_FS);

    EVE_Cmd_waitFlush(s_pHalContext);
    //The file is done, mute the audio.
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_L_PB, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_R_PB, 0);
}

/**
* @brief play WAV from Flash
*
*/
void SAMAPP_Audio_playWavFromFlash()
{
    if (!FlashHelper_SwitchFullMode(s_pHalContext))
    {
        eve_printf("SwitchFullMode failed");
        return;
    }
    Draw_Text(s_pHalContext, "Example for: play wav file from Flash");

    Draw_Text(s_pHalContext, "Now you will hear the music");

    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_L_PB, 155);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_R_PB, 155);
    EVE_CoCmd_flashSource(s_pHalContext, 4096);
    EVE_CoCmd_playWav(s_pHalContext, OPT_FLASH);

    EVE_Cmd_waitFlush(s_pHalContext);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_L_PB, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_R_PB, 0);
}

/**
* @brief load WAV from command buffer
*
*/
void SAMAPP_Audio_loadWavFromCMDBwithPauseResume()
{
    uint32_t btnW = 300;
    uint32_t btnH = 120;
    uint32_t btnX = s_pHalContext->Width / 2 - btnW / 2;
    uint32_t btnY = s_pHalContext->Height - btnH - 50;
    static bool isPause = 0;
    const uint8_t btnPauseTag = 1;
    static bool pressed = 0;
    uint8_t txtPause[2][20] = { "PAUSE", "RESUME" };
    uint32_t result = 0;

    Draw_Text(s_pHalContext, "Example for: Load wav file from command buffer");

    EVE_CoCmd_loadWav(s_pHalContext, RAM_G, 0);
    uint32_t transfered = 0;
    EVE_Util_loadCmdFile(s_pHalContext, TEST_DIR "\\perfect_beauty.wav", &transfered);

    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_LOOP, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_L_PB, 155);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_R_PB, 155);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_PLAY, 1);

    // wait until the end of audio
    do
    {
        EVE_CoCmd_regRead(s_pHalContext, REG_TOUCH_TAG, &result);
        //only change the button when a release happened
        if ((result & 0xFF) == btnPauseTag)
        {
            pressed = 1;
        }
        else
        {
            if (pressed)
            {
                pressed = 0;
                isPause = !isPause;
                EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_PAUSE, isPause);
            }
        }

        Display_StartColor(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 0, 0, 0 });
        EVE_CoCmd_text(s_pHalContext, (uint16_t)(s_pHalContext->Width / 2), (uint16_t)(s_pHalContext->Height / 2), 31, OPT_CENTERX | OPT_FILL, "Now you will hear the music");

        /*** Show a button ***/
        EVE_CoDl_tag(s_pHalContext, btnPauseTag);
        EVE_CoCmd_button(s_pHalContext, btnX, btnY, btnW, btnH, 31, 0, txtPause[isPause]);
        /*** Done button ***/
        Display_End(s_pHalContext);

        EVE_CoCmd_regRead(s_pHalContext, REG_PLAYBACK_PLAY, &result);
    } while (result != 0);

    //The file is done, mute the audio.
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_L_PB, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_R_PB, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_LENGTH, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_PLAY, 1);
}

/**
* @brief load WAV from MediaFifo
*
*/
void SAMAPP_Audio_loadWavFromMediaFifo()
{
    Draw_Text(s_pHalContext, "Example for: Load wav file from Media FiFo");
    uint32_t result = 0;
    uint32_t mediafifo;
    uint32_t mediafifolen;
    /* start video playback, load the data into media fifo */
    mediafifo = RAM_G; //the starting address of the media fifo
    mediafifolen = 16 * 1024;
    EVE_MediaFifo_set(s_pHalContext, mediafifo, mediafifolen); //address of the media fifo buffer

    Draw_Text(s_pHalContext, "Now you will hear the music");

    EVE_CoCmd_loadWav(s_pHalContext, RAM_G + mediafifolen, OPT_MEDIAFIFO);
    EVE_Util_loadMediaFile(s_pHalContext, TEST_DIR "\\perfect_beauty.wav", NULL);

    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_LOOP, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_L_PB, 155);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_R_PB, 155);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_PLAY, 1);

    // wait until the end of audio
    do
    {
        EVE_CoCmd_regRead(s_pHalContext, REG_PLAYBACK_PLAY, &result);
    } while (result != 0);

    //The file is done, mute the audio.
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_L_PB, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_R_PB, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_LENGTH, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_PLAY, 1);
    EVE_MediaFifo_close(s_pHalContext);
}

/**
* @brief load WAV from SD card
*
*/
void SAMAPP_Audio_loadWavFromSD()
{
    const char *file = "perfect_beauty.wav";
    uint32_t result = 0;

    Draw_Text(s_pHalContext, "Example for: load wav file from SD card");

    Draw_Text(s_pHalContext, "Now you will hear the music");

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

    EVE_CoCmd_loadWav(s_pHalContext, RAM_G, OPT_FS);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_LOOP, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_L_PB, 155);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_R_PB, 155);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_PLAY, 1);

    // wait until the end of audio
    do
    {
        EVE_CoCmd_regRead(s_pHalContext, REG_PLAYBACK_PLAY, &result);
    } while (result != 0);

    //The file is done, mute the audio.
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_L_PB, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_R_PB, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_LENGTH, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_PLAY, 1);
}

/**
* @brief load WAV from Flash
*
*/
void SAMAPP_Audio_loadWavFromFlash()
{
    uint32_t result = 0;
    if (!FlashHelper_SwitchFullMode(s_pHalContext))
    {
        eve_printf("SwitchFullMode failed");
        return;
    }
    Draw_Text(s_pHalContext, "Example for: Video display from Flash");

    Draw_Text(s_pHalContext, "Now you will hear the music");

    EVE_CoCmd_flashSource(s_pHalContext, 4096);
    EVE_CoCmd_loadWav(s_pHalContext, RAM_G, OPT_FLASH);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_LOOP, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_L_PB, 155);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_R_PB, 155);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_PLAY, 1);

    // wait until the end of audio
    do
    {
        EVE_CoCmd_regRead(s_pHalContext, REG_PLAYBACK_PLAY, &result);
    } while (result != 0);

    //The file is done, mute the audio.
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_L_PB, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_R_PB, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_LENGTH, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_PLAY, 1);
}

void SAMAPP_Audio()
{
    SAMAPP_Audio_builtin();
    SAMAPP_Audio_fromEABConvertedRaw();
    SAMAPP_Audio_playWavFromCMDB();
    SAMAPP_Audio_playWavFromMediaFifo();
    SAMAPP_Audio_playWavFromSD();
    SAMAPP_Audio_playWavFromFlash();
    SAMAPP_Audio_loadWavFromCMDBwithPauseResume();
    SAMAPP_Audio_loadWavFromMediaFifo();
    SAMAPP_Audio_loadWavFromSD();
    SAMAPP_Audio_loadWavFromFlash();
}


