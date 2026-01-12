/**
 * @file Extension.c
 * @brief Sample usage of extension
 *
 * @author Bridgetek
 *
 * @date 2025
 * 
 * MIT License
 *
 * Copyright (c) [2025] [Bridgetek Pte Ltd (BRTChip)]
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

#include "Extension.h"
#include "FileTransfer.h"
#include "FlashHelper.h"
#include "Patch.h"

#define SAMAPP_DELAY_MS            2000
#define SAMAPP_DELAY               EVE_sleep(SAMAPP_DELAY_MS)
#define EXT_REGION_SCISSOR_W       240
#define EXT_REGION_SCISSOR_H       320
#define EXT_REGION_SCISSOR_X       150
#define EXT_REGION_SCISSOR_Y       0
#define EXT_REGION_ENDREGION1_X    0
#define EXT_REGION_ENDREGION1_Y    90
#define EXT_REGION_ENDREGION2_X    0
#define EXT_REGION_ENDREGION2_Y    50
#define EXT_REGION_ENDREGION3_X    0
#define EXT_REGION_ENDREGION3_Y    10
#define EXT_REGION_ENDREGION1_W    0
#define EXT_REGION_ENDREGION1_H    80
#define EXT_REGION_ENDREGION2_W    0
#define EXT_REGION_ENDREGION2_H    170
#define EXT_REGION_ENDREGION3_W    0
#define EXT_REGION_ENDREGION3_H    300
#define EXT_LINE_X1                0
#define EXT_LINE_Y1                0
#define EXT_LINE_X2                512
#define EXT_LINE_Y2                320
#define EXT_TOUCHOFFSET_WIN_NUM    2
#define EXT_TOUCHOFFSET_WIN0_X     100
#define EXT_TOUCHOFFSET_WIN0_Y     100
#define EXT_TOUCHOFFSET_WIN1_X     800
#define EXT_TOUCHOFFSET_WIN1_Y     400
#define EXT_TOUCHOFFSET_WIN_W      512
#define EXT_TOUCHOFFSET_WIN_H      512
#define EXT_TOUCHOFFSET_BTN_NUM    4
#define EXT_TOUCHOFFSET_BTN_W      220
#define EXT_TOUCHOFFSET_BTN_H      220
#define EXT_TOUCHOFFSET_BTN_FONT   32
#define EXT_TOUCHOFFSET_BTN_X_OFF  24
#define EXT_TOUCHOFFSET_BTN_Y_OFF  24
#define EXT_TOUCHOFFSET_BTN_X_STEP 244
#define EXT_TOUCHOFFSET_BTN_Y_STEP 244
#define EXT_TOUCHOFFSET_FRAMES     1000
#define EXT_TOUCHOFFSET_SLEEP_MS   6

static EVE_HalContext s_halContext;
static EVE_HalContext* s_pHalContext;

static void SAMAPP_Extension();

int main(int argc, char* argv[])
{
    s_pHalContext = &s_halContext;
    Gpu_Init(s_pHalContext);
    Display_Config(s_pHalContext, YCBCR, MODE_PICTURE);
#if 0 // This is for showcase purposes only - the patch has already been loaded in Common.c
    if (eve_loadpatch(s_pHalContext) != 0)
    {
        eve_printf_debug("eve_loadpatch failed\n");
        return 1;
    }
    else
    {
        eve_printf_debug("eve_loadpatch OK\n");
    }
#endif

    // read and store calibration setting
#if GET_CALIBRATION == 1
    EVE_Calibrate(s_pHalContext);
    Calibration_Save(s_pHalContext);
#endif

    EVE_Util_clearScreen(s_pHalContext);

    char *info[] =
    {  "EVE Sample Application",
       "This sample demonstrates the use of Extension",
       "",
       ""
    };
    WelcomeScreen(s_pHalContext, info);

    SAMAPP_Extension();

    EVE_Util_clearScreen(s_pHalContext);
    Gpu_Release(s_pHalContext);
    return 0;
}

static void SAMAPP_Extension_Region()
{
    Draw_Text(s_pHalContext, "Example for: EVE_CMD_REGION and EVE_CMD_ENDREGION from base package");

    Display_Start(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 0 }, 0, 4);
    EVE_CoDl_scissorSize(s_pHalContext, EXT_REGION_SCISSOR_W, EXT_REGION_SCISSOR_H);
    EVE_CoDl_scissorXY(s_pHalContext, EXT_REGION_SCISSOR_X, EXT_REGION_SCISSOR_Y);

    EVE_CMD_REGION(s_pHalContext);
    EVE_CoDl_colorRgb(s_pHalContext, 255, 0, 0); // red
    EVE_CoDl_clearColorRgb(s_pHalContext, 255, 165, 0); // orange
    EVE_CoDl_clear(s_pHalContext, 1, 1, 1);
    EVE_CMD_REGION(s_pHalContext);
    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 255); // blue
    EVE_CoDl_clearColorRgb(s_pHalContext, 80, 80, 200); // medium blue
    EVE_CoDl_clear(s_pHalContext, 1, 1, 1);
    EVE_CMD_REGION(s_pHalContext);
    EVE_CoDl_clearColorRgb(s_pHalContext, 250, 80, 200); // bright pink
    EVE_CoDl_clear(s_pHalContext, 1, 1, 1);
    EVE_CMD_ENDREGION(s_pHalContext, EXT_REGION_ENDREGION1_X, EXT_REGION_ENDREGION1_Y, EXT_REGION_ENDREGION1_W, EXT_REGION_ENDREGION1_H);
    EVE_CMD_ENDREGION(s_pHalContext, EXT_REGION_ENDREGION2_X, EXT_REGION_ENDREGION2_Y, EXT_REGION_ENDREGION2_W, EXT_REGION_ENDREGION2_H);
    EVE_CMD_ENDREGION(s_pHalContext, EXT_REGION_ENDREGION3_X, EXT_REGION_ENDREGION3_Y, EXT_REGION_ENDREGION3_W, EXT_REGION_ENDREGION3_H);

    EVE_CoDl_begin(s_pHalContext, LINES);
    EVE_CoDl_vertex2f_4(s_pHalContext, 16 * EXT_LINE_X1, 16 * EXT_LINE_Y1);
    EVE_CoDl_vertex2f_4(s_pHalContext, 16 * EXT_LINE_X2, 16 * EXT_LINE_Y2);
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}


/**
 * @brief Demonstrates the use of EVE_CMD_TOUCHOFFSET and EVE_CMD_ENDTOUCHOFFSET.
 *
 * Draws two subwindows with interactive buttons, using touch offset commands.
 * Handles button highlighting and frame rendering.
 *
 * @param None
 * @return None
 */
static void SAMAPP_Extension_Touchoffset()
{
    Draw_Text(s_pHalContext, "Example for: EVE_CMD_TOUCHOFFSET and EVE_CMD_ENDTOUCHOFFSET from base package");
    uint32_t counter = EXT_TOUCHOFFSET_FRAMES, frame = 0;
    uint8_t i, j;
    uint16_t win[EXT_TOUCHOFFSET_WIN_NUM][2] = { { EXT_TOUCHOFFSET_WIN0_X, EXT_TOUCHOFFSET_WIN0_Y }, { EXT_TOUCHOFFSET_WIN1_X, EXT_TOUCHOFFSET_WIN1_Y } }; // subwindow location
    uint32_t render_addr[EXT_TOUCHOFFSET_WIN_NUM] = { DDR_BITMAPS_STARTADDR, DDR_BITMAPS_STARTADDR1 }; // rendertarget address
    uint16_t x, y; // button location
    uint32_t tag = 0xFF;
    uint32_t prev_tag[2] = { 0 };
    char buttonChList[2][4][2] = { { "a", "b", "c", "d"}, { "e", "f", "g", "h"} }; // button label text
    bool highlighted[2][4] = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }; // by default, no highlight

    while(frame < counter)
    {
        for (i = 0; i < EXT_TOUCHOFFSET_WIN_NUM; i++)
        {
            EVE_CMD_TOUCHOFFSET(s_pHalContext, win[i][0], win[i][1]);
            EVE_CoCmd_renderTarget(s_pHalContext, render_addr[i], RGB6, EXT_TOUCHOFFSET_WIN_W, EXT_TOUCHOFFSET_WIN_H);
            Display_Start(s_pHalContext, (uint8_t[]) { 100, 0, 30 }, (uint8_t[]) { 255, 255, 255 }, 0xFF, 4);
            for (j = 0; j < EXT_TOUCHOFFSET_BTN_NUM; j++)
            {
                x = EXT_TOUCHOFFSET_BTN_X_OFF + (j % 2) * EXT_TOUCHOFFSET_BTN_X_STEP;
                y = EXT_TOUCHOFFSET_BTN_Y_OFF + (j / 2) * EXT_TOUCHOFFSET_BTN_Y_STEP;
                EVE_CoDl_tag(s_pHalContext, 'a' + i * EXT_TOUCHOFFSET_BTN_NUM + j);
                if (highlighted[i][j])
                    EVE_CoCmd_fgColor(s_pHalContext, 0xd06038);
                else
                    EVE_CoCmd_fgColor(s_pHalContext, 0x3870);
                EVE_CoCmd_button(s_pHalContext, x, y, EXT_TOUCHOFFSET_BTN_W, EXT_TOUCHOFFSET_BTN_H, EXT_TOUCHOFFSET_BTN_FONT, OPT_FLAT, buttonChList[i][j]);
            }
            EVE_CoDl_tag(s_pHalContext, 0);
            Display_End(s_pHalContext);
            EVE_CoCmd_graphicsFinish(s_pHalContext);
            EVE_CMD_ENDTOUCHOFFSET(s_pHalContext);
            EVE_Cmd_waitFlush(s_pHalContext);

            EVE_sleep(EXT_TOUCHOFFSET_SLEEP_MS);
            tag = EVE_Hal_rd32(s_pHalContext, REG_TOUCH_TAG);
            if (prev_tag[i] == 0)
            {
                if ((tag >= (uint32_t)('a' + i * EXT_TOUCHOFFSET_BTN_NUM)) && (tag < (uint32_t)('a' + (i + 1) * EXT_TOUCHOFFSET_BTN_NUM)))
                {
                    highlighted[i][tag - 'a' - i * EXT_TOUCHOFFSET_BTN_NUM] ^= 1;
                }
            }
            prev_tag[i] = tag;
        }

        if (frame % EXT_TOUCHOFFSET_WIN_NUM == 0)
            EVE_CoCmd_renderTarget(s_pHalContext, SC0_PTR0_STARTADDR, YCBCR, s_pHalContext->Width, s_pHalContext->Height);
        else
            EVE_CoCmd_renderTarget(s_pHalContext, SC0_PTR1_STARTADDR, YCBCR, s_pHalContext->Width, s_pHalContext->Height);
        Display_Start(s_pHalContext, (uint8_t[]) { 0, 60, 0 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
        EVE_CoCmd_number(s_pHalContext, 0, 0, 30, 0, frame);
        EVE_CoDl_vertexFormat(s_pHalContext, 0);
        EVE_CoDl_begin(s_pHalContext, BITMAPS);
        for (i = 0; i < EXT_TOUCHOFFSET_WIN_NUM; i++)
        {
            EVE_CoCmd_setBitmap(s_pHalContext, render_addr[i], RGB6, EXT_TOUCHOFFSET_WIN_W, EXT_TOUCHOFFSET_WIN_W);
            EVE_CoDl_vertex2f(s_pHalContext, win[i][0], win[i][1]);
        }
        EVE_CoDl_end(s_pHalContext);
        EVE_CoCmd_sync(s_pHalContext);
        if (frame % EXT_TOUCHOFFSET_WIN_NUM == 0)
            EVE_Hal_wr32(s_pHalContext, REG_SO_SOURCE, SC0_PTR0_STARTADDR);
        else
            EVE_Hal_wr32(s_pHalContext, REG_SO_SOURCE, SC0_PTR1_STARTADDR);
        Display_End(s_pHalContext);

        frame++;
    }
    Display_Config(s_pHalContext, YCBCR, MODE_PICTURE);
}

/**
 * @brief Main entry to run all extension demos
 *
 * @param None
 * @return None
 */
static void SAMAPP_Extension()
{
    SAMAPP_Extension_Region();
    SAMAPP_Extension_Touchoffset();
}


