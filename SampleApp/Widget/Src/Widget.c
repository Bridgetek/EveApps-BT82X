/**
 * @file Widget.c
 * @brief Sample usage of some widgets
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

#include "Widget.h"
#include "FileTransfer.h"
#include "FlashHelper.h"

#define SAMAPP_DELAY_MS         2000
#define SAMAPP_DELAY            EVE_sleep(SAMAPP_DELAY_MS)
#define WIDGET_LOAD_ADDR        RAM_G
#define WIDGET_TITLE_FONT_LARGE 33
#define WIDGET_TITLE_FONT       31
#define WIDGET_TITLE_FONT_SMALL 29
#define WIDGET_TITLE_Y_INC      25
#define WIDGET_LOOP             600
#define WIDGET_DELAY_MS         10
#define WIDGET_X_INC1           10
#define WIDGET_X_INC2           20
#define WIDGET_Y_INC1           5
#define WIDGET_Y_INC2           40
#define WIDGET_LINE_W           10
#define WIDGET_COLOR_GREEN      0xBFD1C4
#define WIDGET_COLOR_RED        0xB95756
#define WIDGET_COLOR_PINK       0xED78C1
#define WIDGET_COLOR_GREY       0x919191
#define WIDGET_COLOR_ORANGE     0xFEC37D
#define WIDGET_COLOR_BLUE       0x2E43B5
#define WIDGET_COLOR_PURPLE     0x8C2AED
#define WIDGET_COLOR_WHITE      0xFFFFFF
#define WIDGET_COLOR_YELLOW     0xFFFD55
#define WIDGET_GRAD_W           100
#define WIDGET_CG_X             100
#define WIDGET_CG_Y             100
#define WIDGET_BTN_W            80
#define WIDGET_BTN_H            80
#define WIDGET_BTN_Y_INC        5
#define WIDGET_ARC_COLOR_NUM    10
#define WIDGET_ARC_R            30
#define WIDGET_ARC_W            8
#define WIDGET_GLOW_W           400
#define WIDGET_GLOW_H           250

static EVE_HalContext s_halContext;
static EVE_HalContext* s_pHalContext;
static void SAMAPP_Widget();

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
        "This sample demonstrates the use of basic widgets", 
        "",
        ""
    };
    WelcomeScreen(s_pHalContext, info);

    SAMAPP_Widget();

    EVE_Util_clearScreen(s_pHalContext);
    Gpu_Release(s_pHalContext);
    return 0;
}

/**
 * @brief Show logo
 *
 * @param None
 * @return None
 */
static void SAMAPP_Widget_logo()
{
    Draw_Text(s_pHalContext, "Example for: Showing logo");

    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);

    EVE_CoCmd_logo(s_pHalContext);

    EVE_CoDl_display(s_pHalContext);
    EVE_Cmd_waitLogo(s_pHalContext);

    eve_printf_debug("Logo animation has finished playing.\n");

    SAMAPP_DELAY;
    EVE_Util_clearScreen(s_pHalContext);
}

/**
 * @brief demonstrate clock widget
 *
 * @param None
 * @return None
 */
static void SAMAPP_Widget_clock()
{
    Draw_Text(s_pHalContext, "Example for: clock widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of clock function. Clocks can be    */
    /* constructed using flat or 3d effect. Clock background and foreground  */
    /* colors can be set by gbcolor and colorrgb. Clock can be constructed   */
    /* with multiple options such as no background, no needles, no pointer.  */
    /*************************************************************************/
    int16_t xOffset;
    int16_t yOffset;
    int16_t radius;
    int16_t xDistBtwClocks;

    xDistBtwClocks = (int16_t) (s_pHalContext->Width / 5);
    radius = xDistBtwClocks / 2 - (int16_t) (s_pHalContext->Width / 64);

    /* Download the bitmap data for lena faced clock */
    EVE_Util_loadRawFile(s_pHalContext, WIDGET_LOAD_ADDR, TEST_DIR WIDGET_BITMAP);

    /* Draw clock with blue as background and rad as needle color */
    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    /* flat effect and default color background */
    xOffset = xDistBtwClocks / 2;
    yOffset = radius + WIDGET_Y_INC1;
    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_BLUE);
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_RED);
    EVE_CoCmd_clock(s_pHalContext, xOffset, yOffset, radius, OPT_FLAT, 10, 10, 5, 10); // 10:10:05
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + radius + WIDGET_TITLE_Y_INC), WIDGET_TITLE_FONT, OPT_CENTER, "Flat effect"); //text info

    /* no seconds needle */
    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_GREEN);
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_PURPLE);
    xOffset += xDistBtwClocks;
    EVE_CoCmd_clock(s_pHalContext, xOffset, yOffset, radius, OPT_NOSECS, 10, 10, 5, 10); // 10:10:05
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + radius + WIDGET_TITLE_Y_INC), WIDGET_TITLE_FONT, OPT_CENTER, "No Secs"); //text info

    /* no background color */
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_ORANGE);
    xOffset += xDistBtwClocks;
    EVE_CoCmd_clock(s_pHalContext, xOffset, yOffset, radius, OPT_NOBACK, 10, 10, 5, 10); // 10:10:05
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + radius + WIDGET_TITLE_Y_INC), WIDGET_TITLE_FONT, OPT_CENTER, "No BG"); //text info

    /* No ticks */
    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_PINK);
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_BLUE);
    xOffset += xDistBtwClocks;
    EVE_CoCmd_clock(s_pHalContext, xOffset, yOffset, radius, OPT_NOTICKS, 10, 10, 5, 10); // 10:10:05
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + radius + WIDGET_TITLE_Y_INC), WIDGET_TITLE_FONT, OPT_CENTER, "No Ticks"); //text info

    /* No hands */
    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_GREY);
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_GREEN);
    xOffset += xDistBtwClocks;
    EVE_CoCmd_clock(s_pHalContext, xOffset, yOffset, radius, OPT_NOHANDS, 10, 10, 5, 10); // 10:10:05
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + radius + WIDGET_TITLE_Y_INC), WIDGET_TITLE_FONT, OPT_CENTER, "No Hands"); //text info
    yOffset += radius + 2 * WIDGET_TITLE_Y_INC;

    /* Bigger clock */
    radius = (int16_t)(s_pHalContext->Height - (WIDGET_Y_INC1 + 2 * radius + 2 * WIDGET_TITLE_Y_INC)); //calculate radius based on remaining height
    radius = (radius - 2 * WIDGET_Y_INC1) / 2;
    xOffset = radius + WIDGET_X_INC1;
    yOffset += radius + WIDGET_Y_INC1;
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_BLUE);
    EVE_CoCmd_clock(s_pHalContext, xOffset, yOffset, radius, 0, 10, 10, 5, 10); // 10:10:05

    xOffset += 2 * radius + WIDGET_X_INC1;
    /* Lena clock with no background and no ticks */
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_WHITE);
    EVE_CoDl_colorMask(s_pHalContext, 0, 0, 0, 1);
    EVE_CoDl_begin(s_pHalContext, RECTS);
    EVE_CoDl_lineWidth(s_pHalContext, WIDGET_LINE_W * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (xOffset - radius + WIDGET_LINE_W) * 16, (yOffset - radius + WIDGET_LINE_W) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (xOffset + radius - WIDGET_LINE_W) * 16, (yOffset + radius - WIDGET_LINE_W) * 16);
    EVE_CoDl_end(s_pHalContext);
    EVE_CoDl_colorMask(s_pHalContext, 1, 1, 1, 1);
    EVE_CoDl_blendFunc(s_pHalContext, DST_ALPHA, ONE_MINUS_DST_ALPHA);
    /* Lena bitmap - scale proportionately wrt output resolution */
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_scale(s_pHalContext, 65536 * 2 * radius / WIDGET_BITMAP_W, 65536 * 2 * radius / WIDGET_BITMAP_H);
    EVE_CoCmd_setMatrix(s_pHalContext);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_bitmapSource(s_pHalContext, WIDGET_LOAD_ADDR);
    EVE_CoDl_bitmapLayout(s_pHalContext, WIDGET_BITMAP_FORMAT, 3 * WIDGET_BITMAP_W, WIDGET_BITMAP_H);
    EVE_CoDl_bitmapSize(s_pHalContext, BILINEAR, BORDER, BORDER, 2 * radius, 2 * radius);
    EVE_CoDl_vertex2f_4(s_pHalContext, (xOffset - radius) * 16, (yOffset - radius) * 16);
    EVE_CoDl_end(s_pHalContext);
    // reset to default value
    EVE_CoDl_blendFunc_default(s_pHalContext);
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_setMatrix(s_pHalContext);
    EVE_CoCmd_clock(s_pHalContext, xOffset, yOffset, radius, OPT_NOTICKS | OPT_NOBACK, 10, 10, 5, 10); // 10:10:05
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief demonstrate a gauge widget
 *
 * @param None
 * @return None
 */
static void SAMAPP_Widget_gauge()
{
    Draw_Text(s_pHalContext, "Example for: gauge widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of gauge function. Gauge can be     */
    /* constructed using flat or 3d effect. Gauge background and foreground  */
    /* colors can be set by gbcolor and colorrgb. Gauge can be constructed   */
    /* with multiple options such as no background, no minors/majors and     */
    /* no pointer.                                                           */
    /*************************************************************************/
    int16_t xOffset;
    int16_t yOffset;
    int16_t cRadius;
    int16_t xDistBtwClocks;

    xDistBtwClocks = (int16_t) (s_pHalContext->Width / 5);
    cRadius = xDistBtwClocks / 2 - (int16_t) (s_pHalContext->Width / 64);

    /* Download the bitmap data */
    EVE_Util_loadRawFile(s_pHalContext, WIDGET_LOAD_ADDR, TEST_DIR WIDGET_BITMAP);

    /* Draw gauge with blue as background and read as needle color */
    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    /* flat effect and default color background */
    xOffset = xDistBtwClocks / 2;
    yOffset = cRadius + WIDGET_Y_INC1;
    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_BLUE);
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_RED);
    EVE_CoCmd_gauge(s_pHalContext, xOffset, yOffset, cRadius, OPT_FLAT, 5, 4, 45, 100);
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + cRadius + WIDGET_TITLE_Y_INC), WIDGET_TITLE_FONT, OPT_CENTER, "Flat effect"); //text info

    /* 3d effect */
    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_GREEN);
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_PURPLE);
    xOffset += xDistBtwClocks;
    EVE_CoCmd_gauge(s_pHalContext, xOffset, yOffset, cRadius, 0, 5, 1, 60, 100);
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + cRadius + WIDGET_TITLE_Y_INC), WIDGET_TITLE_FONT, OPT_CENTER, "3d effect"); //text info

    /* no background color */
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_ORANGE);
    xOffset += xDistBtwClocks;
    EVE_CoCmd_gauge(s_pHalContext, xOffset, yOffset, cRadius, OPT_NOBACK, 1, 6, 90, 100);
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + cRadius + WIDGET_TITLE_Y_INC), WIDGET_TITLE_FONT, OPT_CENTER, "No BG"); //text info

    /* No ticks */
    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_PINK);
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_BLUE);
    xOffset += xDistBtwClocks;
    EVE_CoCmd_gauge(s_pHalContext, xOffset, yOffset, cRadius, OPT_NOTICKS, 5, 4, 20, 100);
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + cRadius + WIDGET_TITLE_Y_INC), WIDGET_TITLE_FONT, OPT_CENTER, "No Ticks"); //text info

    /* No hands */
    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_GREY);
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_GREEN);
    xOffset += xDistBtwClocks;
    EVE_CoCmd_gauge(s_pHalContext, xOffset, yOffset, cRadius, OPT_NOPOINTER, 5, 4, 55, 100);
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + cRadius + WIDGET_TITLE_Y_INC), WIDGET_TITLE_FONT, OPT_CENTER, "No Hands"); //text info
    yOffset += cRadius + 2 * WIDGET_TITLE_Y_INC;

    /* Bigger gauge */
    cRadius = (int16_t)(s_pHalContext->Height - (WIDGET_Y_INC1 + 2 * cRadius + 2 * WIDGET_TITLE_Y_INC)); //calculate radius based on remaining height
    cRadius = (cRadius - 2 * WIDGET_Y_INC1) / 2;
    xOffset = cRadius + WIDGET_X_INC1;
    yOffset += cRadius + WIDGET_Y_INC1;
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_BLUE);
    EVE_CoCmd_gauge(s_pHalContext, xOffset, yOffset, cRadius, OPT_NOTICKS | OPT_NOBACK, 5, 4, 30, 100);

    xOffset += 2 * cRadius + WIDGET_X_INC1;
    /* Lena gauge with no background and no ticks */
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_WHITE);
    EVE_CoDl_colorMask(s_pHalContext, 0, 0, 0, 1);
    EVE_CoDl_begin(s_pHalContext, POINTS);
    EVE_CoDl_pointSize(s_pHalContext, cRadius * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, xOffset * 16, yOffset * 16);
    EVE_CoDl_end(s_pHalContext);
    EVE_CoDl_colorMask(s_pHalContext, 1, 1, 1, 1);
    EVE_CoDl_blendFunc(s_pHalContext, DST_ALPHA, ONE_MINUS_DST_ALPHA);
    /* Lena bitmap - scale proportionately wrt output resolution */
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_scale(s_pHalContext, 65536 * 2 * cRadius / WIDGET_BITMAP_W, 65536 * 2 * cRadius / WIDGET_BITMAP_H);
    EVE_CoCmd_setMatrix(s_pHalContext);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_bitmapSource(s_pHalContext, WIDGET_LOAD_ADDR);
    EVE_CoDl_bitmapLayout(s_pHalContext, WIDGET_BITMAP_FORMAT, 3 * WIDGET_BITMAP_W, WIDGET_BITMAP_H);
    EVE_CoDl_bitmapSize(s_pHalContext, BILINEAR, BORDER, BORDER, 2 * cRadius, 2 * cRadius);
    EVE_CoDl_vertex2f_4(s_pHalContext, (xOffset - cRadius) * 16, (yOffset - cRadius) * 16);
    EVE_CoDl_end(s_pHalContext);
    // reset to default value
    EVE_CoDl_blendFunc_default(s_pHalContext);
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_setMatrix(s_pHalContext);
    EVE_CoCmd_gauge(s_pHalContext, xOffset, yOffset, cRadius, OPT_NOTICKS | OPT_NOBACK, 5, 4, 30,
        100);

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief demonstrate gradient widget
 *
 * @param None
 * @return None
 */
static void SAMAPP_Widget_gradient()
{
    Draw_Text(s_pHalContext, "Example for: gradient widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of gradient function. Gradient func */
    /* can be used to construct three effects - horizontal, vertical and     */
    /* diagonal effects.                                                      */
    /*************************************************************************/
    int16_t wScissor;
    int16_t hScissor;
    int16_t xOffset;
    int16_t yOffset;

    xOffset = WIDGET_X_INC2;
    yOffset = WIDGET_Y_INC2;
    hScissor = (int16_t)((s_pHalContext->Height - 3 * yOffset) / 2);
    wScissor = (int16_t)((s_pHalContext->Width - 4 * xOffset) / 3);

    /* Draw gradient  */
    Display_Start(s_pHalContext, (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_CoDl_scissorSize(s_pHalContext, wScissor, hScissor);
    /* Horizontal gradient effect */
    EVE_CoDl_scissorXY(s_pHalContext, xOffset, yOffset); //clip the display
    EVE_CoCmd_gradient(s_pHalContext, xOffset, yOffset, WIDGET_COLOR_GREY, (xOffset + wScissor), yOffset, WIDGET_COLOR_YELLOW);

    /* Vertical gradient effect */
    xOffset += wScissor + WIDGET_X_INC2;
    EVE_CoDl_scissorXY(s_pHalContext, xOffset, yOffset); //clip the display
    EVE_CoCmd_gradient(s_pHalContext, xOffset, yOffset, WIDGET_COLOR_RED, xOffset, (yOffset + hScissor), WIDGET_COLOR_GREEN);

    /* diagonal gradient effect */
    xOffset += wScissor + WIDGET_X_INC2;
    EVE_CoDl_scissorXY(s_pHalContext, xOffset, yOffset); //clip the display
    EVE_CoCmd_gradient(s_pHalContext, xOffset, yOffset, WIDGET_COLOR_PINK, (xOffset + wScissor), (yOffset + hScissor), WIDGET_COLOR_WHITE);

    EVE_CoDl_scissorXY(s_pHalContext, 0, 0); //set to default values
    EVE_CoDl_scissorSize(s_pHalContext, 2048, 2048);
    // text info
    xOffset = WIDGET_X_INC2;
    yOffset = WIDGET_Y_INC2 + hScissor + WIDGET_TITLE_Y_INC;
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset, WIDGET_TITLE_FONT, OPT_CENTERY, "Horizontal grad");
    xOffset += wScissor + WIDGET_X_INC2;
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset, WIDGET_TITLE_FONT, OPT_CENTERY, "Vertical grad");
    xOffset += wScissor + WIDGET_X_INC2;
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset, WIDGET_TITLE_FONT, OPT_CENTERY, "Diagonal grad");

    /* Diagonal gradient with text info */
    xOffset = WIDGET_X_INC2;
    yOffset += WIDGET_Y_INC2;
    EVE_CoDl_scissorSize(s_pHalContext, wScissor, WIDGET_Y_INC2);
    EVE_CoDl_scissorXY(s_pHalContext, xOffset, (yOffset + hScissor / 2 - WIDGET_Y_INC2 / 2)); //clip the display
    EVE_CoCmd_gradient(s_pHalContext, xOffset, (yOffset + hScissor / 2 - WIDGET_Y_INC2 / 2), WIDGET_COLOR_GREY,
        (xOffset + wScissor), (yOffset + hScissor / 2 + WIDGET_Y_INC2 / 2), WIDGET_COLOR_BLUE);
    EVE_CoCmd_text(s_pHalContext, (xOffset + wScissor / 2), (yOffset + hScissor / 2), WIDGET_TITLE_FONT,
        OPT_CENTER, "Gradient Gradient Gradient"); //text info

    /* Draw horizontal, vertical and diagonal */
    xOffset += wScissor + WIDGET_X_INC2;
    EVE_CoDl_scissorSize(s_pHalContext, wScissor, hScissor);
    EVE_CoDl_scissorXY(s_pHalContext, xOffset, yOffset); //clip the display
    EVE_CoCmd_gradient(s_pHalContext, xOffset, yOffset, WIDGET_COLOR_GREY, (xOffset + wScissor), yOffset, WIDGET_COLOR_YELLOW);
    wScissor -= WIDGET_GRAD_W;
    hScissor -= WIDGET_GRAD_W;
    EVE_CoDl_scissorSize(s_pHalContext, wScissor, hScissor);
    xOffset += WIDGET_GRAD_W / 2;
    yOffset += WIDGET_GRAD_W / 2;
    EVE_CoDl_scissorXY(s_pHalContext, xOffset, yOffset); //clip the display
    EVE_CoCmd_gradient(s_pHalContext, xOffset, yOffset, WIDGET_COLOR_PINK, xOffset, (yOffset + hScissor), WIDGET_COLOR_WHITE);
    wScissor -= WIDGET_GRAD_W;
    hScissor -= WIDGET_GRAD_W;
    EVE_CoDl_scissorSize(s_pHalContext, wScissor, hScissor);
    xOffset += WIDGET_GRAD_W / 2;
    yOffset += WIDGET_GRAD_W / 2;
    EVE_CoDl_scissorXY(s_pHalContext, xOffset, yOffset); //clip the display
    EVE_CoCmd_gradient(s_pHalContext, xOffset, yOffset, WIDGET_COLOR_GREY, (xOffset + wScissor), (yOffset + hScissor), WIDGET_COLOR_PURPLE);

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief API to demonstrate CMD_GRADIENTA with transparency
 *
 * @param None
 * @return None
 */
static void SAMAPP_Widget_gradientA()
{
    Draw_Text(s_pHalContext, "Example for: gradient widget with transparency");

    /*************************************************************************/
    /* Below code demonstrates the usage of gradient function. Gradient func */
    /* can be used to construct three effects - horizontal, vertical and     */
    /* diagonal effects.                                                      */
    /*************************************************************************/
    /*GradientA */
    int16_t wScissor;
    int16_t hScissor;
    int16_t xOffset;
    int16_t yOffset;

    xOffset = WIDGET_X_INC2;
    yOffset = WIDGET_Y_INC2;
    hScissor = (int16_t)((s_pHalContext->Height - 3 * yOffset) / 2);
    wScissor = (int16_t)((s_pHalContext->Width - 4 * xOffset) / 3);
    /* Draw gradient  */

    Display_Start(s_pHalContext, (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_CoDl_scissorSize(s_pHalContext, wScissor, hScissor);
    /* Horizontal gradient effect */
    EVE_CoDl_scissorXY(s_pHalContext, xOffset, yOffset); //clip the display
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset + hScissor / 4, WIDGET_TITLE_FONT, 0, "GradientA: Horizontal gradient"); //text info
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset + hScissor * 3 / 4, WIDGET_TITLE_FONT, 0, "GradientA: Horizontal gradient"); //text info
    EVE_CoCmd_gradientA(s_pHalContext, xOffset, yOffset, 0x80000000 | WIDGET_COLOR_GREY, (xOffset + wScissor), yOffset,
        0xFF000000 | WIDGET_COLOR_YELLOW);
    /* Vertical gradient effect */
    xOffset += wScissor + WIDGET_X_INC2;
    EVE_CoDl_scissorXY(s_pHalContext, xOffset, yOffset); //clip the display
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset + hScissor / 4, WIDGET_TITLE_FONT, 0, "GradientA: Vertical gradient"); //text info
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset + hScissor * 3 / 4, WIDGET_TITLE_FONT, 0, "GradientA: Vertical gradient"); //text info
    EVE_CoCmd_gradientA(s_pHalContext, xOffset, yOffset, 0x80000000 | WIDGET_COLOR_RED, xOffset, (yOffset + hScissor),
        0xFF000000 | WIDGET_COLOR_GREEN);
    /* diagonal gradient effect */
    xOffset += wScissor + WIDGET_X_INC2;
    EVE_CoDl_scissorXY(s_pHalContext, xOffset, yOffset); //clip the display
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset + hScissor / 4, WIDGET_TITLE_FONT, 0, "GradientA: Diagonal gradient"); //text info
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset + hScissor * 3 / 4, WIDGET_TITLE_FONT, 0, "GradientA: Diagonal gradient"); //text info
    EVE_CoCmd_gradientA(s_pHalContext, xOffset, yOffset, 0x80000000 | WIDGET_COLOR_PINK, (xOffset + wScissor),
        (yOffset + hScissor), 0xFF000000 | WIDGET_COLOR_WHITE);
    /* Diagonal gradient with text info */
    xOffset = WIDGET_X_INC2;
    yOffset += hScissor + WIDGET_Y_INC2;
    EVE_CoDl_scissorSize(s_pHalContext, wScissor, WIDGET_Y_INC2);
    EVE_CoDl_scissorXY(s_pHalContext, xOffset, (yOffset + hScissor / 2 - WIDGET_Y_INC2 / 2)); //clip the display
    EVE_CoCmd_text(s_pHalContext, (xOffset + wScissor / 2), (yOffset + hScissor / 2), WIDGET_TITLE_FONT,
        OPT_CENTER, "GradientA GradientA GradientA"); //text info
    EVE_CoCmd_gradientA(s_pHalContext, xOffset, (yOffset + hScissor / 2 - WIDGET_Y_INC2 / 2), 0x00000000 | WIDGET_COLOR_GREY,
        (xOffset + wScissor), (yOffset + hScissor / 2 + WIDGET_Y_INC2 / 2), 0xFF000000 | WIDGET_COLOR_BLUE);

    /* Draw horizontal, vertical and diagonal with alpha */
    xOffset += wScissor + WIDGET_X_INC2;
    EVE_CoDl_scissorSize(s_pHalContext, wScissor, hScissor);
    EVE_CoDl_scissorXY(s_pHalContext, xOffset, yOffset); //clip the display
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + hScissor / 4), WIDGET_TITLE_FONT, 0, "GradientA GradientA"); //text info
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + hScissor * 3 / 4), WIDGET_TITLE_FONT, 0, "GradientA GradientA"); //text info
    EVE_CoCmd_gradientA(s_pHalContext, xOffset, yOffset, 0x80000000 | WIDGET_COLOR_GREY, (xOffset + wScissor), yOffset,
        0xFF000000 | WIDGET_COLOR_YELLOW);
    wScissor -= WIDGET_GRAD_W;
    hScissor -= WIDGET_GRAD_W;
    EVE_CoDl_scissorSize(s_pHalContext, wScissor, hScissor);
    xOffset += WIDGET_GRAD_W / 2;
    yOffset += WIDGET_GRAD_W / 2;
    EVE_CoDl_scissorXY(s_pHalContext, xOffset, yOffset); //clip the display
    EVE_CoCmd_gradientA(s_pHalContext, xOffset, yOffset, 0x80000000 | WIDGET_COLOR_PINK, xOffset, (yOffset + hScissor),
        0xFF000000 | WIDGET_COLOR_WHITE);
    wScissor -= WIDGET_GRAD_W;
    hScissor -= WIDGET_GRAD_W;
    EVE_CoDl_scissorSize(s_pHalContext, wScissor, hScissor);
    xOffset += WIDGET_GRAD_W / 2;
    yOffset += WIDGET_GRAD_W / 2;
    EVE_CoDl_scissorXY(s_pHalContext, xOffset, yOffset); //clip the display
    EVE_CoCmd_gradientA(s_pHalContext, xOffset, yOffset, 0x80000000 | WIDGET_COLOR_GREY, (xOffset + wScissor),
        (yOffset + hScissor), 0xFF000000 | WIDGET_COLOR_PURPLE);

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief API to demonstrate CMD_CGRADIENT
 *
 * @param None
 * @return None
 */
static void SAMAPP_Widget_cgradient()
{
    Draw_Text(s_pHalContext, "Example for: circular gradient widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of cgradient function. The cgradient */
    /* command draws a rectangle containing a circular gradient, smoothly     */
    /* blending between one color in the center and another at the corner or  */
    /* edge.   */
    /*************************************************************************/
    Display_Start(s_pHalContext, (uint8_t[]) { 77, 77, 77 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_CoCmd_cgradient(s_pHalContext, CORNER_ZERO, WIDGET_CG_X, WIDGET_CG_Y, s_pHalContext->Width / 6, s_pHalContext->Width / 6,
        WIDGET_COLOR_RED, WIDGET_COLOR_BLUE);
    EVE_CoCmd_text(s_pHalContext, WIDGET_CG_X, WIDGET_CG_X + s_pHalContext->Width / 6 + WIDGET_TITLE_Y_INC, WIDGET_TITLE_FONT, 0, "CORNER_ZERO");
    EVE_CoCmd_cgradient(s_pHalContext, EDGE_ZERO, s_pHalContext->Width / 2, WIDGET_CG_Y, s_pHalContext->Width / 6, s_pHalContext->Width / 6,
        WIDGET_COLOR_RED, WIDGET_COLOR_BLUE);
    EVE_CoCmd_text(s_pHalContext, s_pHalContext->Width / 2, WIDGET_CG_X + s_pHalContext->Width / 6 + WIDGET_TITLE_Y_INC, WIDGET_TITLE_FONT, 0, "EDGE_ZERO");

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief Key interactive
 *
 * @param None
 * @return None
 */
static void SAMAPP_Widget_keysInteractive()
{
    Draw_Text(s_pHalContext, "Example for: Key interactive");

    /*************************************************************************/
    /* Below code demonstrates the usage of keys function. keys function     */
    /* draws buttons with characters given as input parameters. Keys support */
    /* Flat and 3D effects, draw at (x,y) coordinates or center of the display*/
    /* , inbuilt or custom fonts can be used for key display                 */
    /*************************************************************************/
    int32_t loopflag = WIDGET_LOOP;
    int16_t TextFont = WIDGET_TITLE_FONT_LARGE;
    int16_t ButtonW = WIDGET_BTN_W;
    int16_t ButtonH = WIDGET_BTN_H;
    int16_t yBtnDst = WIDGET_BTN_Y_INC;
    int16_t yOffset;
#define SAMAPP_COPRO_WIDGET_KEYS_INTERACTIVE_TEXTSIZE (512)
    char DispText[SAMAPP_COPRO_WIDGET_KEYS_INTERACTIVE_TEXTSIZE];
    char CurrChar = '|';
    uint32_t CurrTag = 0;
    uint32_t PrevTag = 0;
    uint8_t Pendown = 1;
    int32_t CurrTextIdx = 0;

    while (loopflag--)
    {
        /* Check the user input and then add the characters into array */
        EVE_CoCmd_regRead(s_pHalContext, REG_TOUCH_TAG, &CurrTag);

        CurrChar = (char)CurrTag;
        if (0 == CurrTag)
        {
            CurrChar = '|';
        }

        /* check whether pwndown has happened */
        if (!CurrTag && PrevTag && 1 == Pendown && 0 != PrevTag)
        {
            CurrTextIdx++;
            /* clear all the charaters */
            if (CurrTextIdx > 24)
            {
                CurrTextIdx = 0;
            }
        }

        Display_Start(s_pHalContext, (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
        /* Draw text entered by user */
        /* make sure the array is a string */
        DispText[CurrTextIdx] = CurrChar;
        DispText[CurrTextIdx + 1] = '\0';

        EVE_CoDl_tagMask(s_pHalContext, 0);
        yOffset = WIDGET_Y_INC2 + WIDGET_TITLE_Y_INC;
        EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), WIDGET_Y_INC2 + WIDGET_TITLE_Y_INC, TextFont,
            OPT_CENTER, DispText); //text info
        EVE_CoDl_tagMask(s_pHalContext, 1);

        yOffset += WIDGET_Y_INC2;
        /* Construct a simple keyboard - note that the tags associated with the keys are the character values given in the arguments */
        EVE_CoCmd_fgColor(s_pHalContext, WIDGET_COLOR_GREEN);
        EVE_CoCmd_gradColor(s_pHalContext, WIDGET_COLOR_RED);
        EVE_CoCmd_keys(s_pHalContext, 0, yOffset, 10 * ButtonW, ButtonH, TextFont,
            (OPT_CENTER | (uint16_t)CurrTag), "qwertyuiop");
        EVE_CoCmd_gradColor(s_pHalContext, WIDGET_COLOR_YELLOW);
        yOffset += ButtonH + yBtnDst;
        EVE_CoCmd_keys(s_pHalContext, 0, yOffset, 10 * ButtonW, ButtonH, TextFont,
            (OPT_CENTER | (uint16_t)CurrTag), "asdfghjkl");
        EVE_CoCmd_gradColor(s_pHalContext, WIDGET_COLOR_BLUE);
        yOffset += ButtonH + yBtnDst;
        EVE_CoCmd_keys(s_pHalContext, 0, yOffset, 10 * ButtonW, ButtonH, TextFont,
            (OPT_CENTER | (uint16_t)CurrTag), "zxcvbnm");
        yOffset += ButtonH + yBtnDst;
        EVE_CoDl_tag(s_pHalContext, ' ');
        if (' ' == CurrTag)
        {
            EVE_CoCmd_button(s_pHalContext, 0, yOffset, 10 * ButtonW, ButtonH, TextFont,
                OPT_FLAT, " "); //mandatory to give '\0' at the end to make sure coprocessor understands the string end
        }
        else
        {
            EVE_CoCmd_button(s_pHalContext, 0, yOffset, 10 * ButtonW, ButtonH, TextFont, 0,
                " "); //mandatory to give '\0' at the end to make sure coprocessor understands the string end
        }
        yOffset = ButtonW + WIDGET_TITLE_Y_INC;
        EVE_CoCmd_keys(s_pHalContext, 11 * ButtonW, yOffset, 3 * ButtonW, ButtonH, TextFont,
            (0 | (uint16_t)CurrTag), "789");
        yOffset += ButtonH + yBtnDst;
        EVE_CoCmd_keys(s_pHalContext, 11 * ButtonW, yOffset, 3 * ButtonW, ButtonH, TextFont,
            (0 | (uint16_t)CurrTag), "456");
        yOffset += ButtonH + yBtnDst;
        EVE_CoCmd_keys(s_pHalContext, 11 * ButtonW, yOffset, 3 * ButtonW, ButtonH, TextFont,
            (0 | (uint16_t)CurrTag), "123");
        yOffset += ButtonH + yBtnDst;
        EVE_CoDl_colorA(s_pHalContext, 255);
        EVE_CoCmd_keys(s_pHalContext, 11 * ButtonW, yOffset, 3 * ButtonW, ButtonH, TextFont,
            (0 | (uint16_t)CurrTag), "0."); //hilight button 0
        Display_End(s_pHalContext);
        EVE_sleep(WIDGET_DELAY_MS);
        PrevTag = CurrTag;
    }
}

/**
 * @brief keys widget
 *
 * @param None
 * @return None
 */
static void SAMAPP_Widget_keys()
{
    Draw_Text(s_pHalContext, "Example for: Keys widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of keys function. keys function     */
    /* draws buttons with characters given as input parameters. Keys support */
    /* Flat and 3D effects, draw at (x,y) coordinates or center of the display*/
    /* , inbuilt or custom fonts can be used for key display                 */
    /*************************************************************************/
    int16_t TextFont = WIDGET_TITLE_FONT_LARGE;
    int16_t ButtonW = WIDGET_BTN_W;
    int16_t ButtonH = WIDGET_BTN_H;
    int16_t yBtnDst = WIDGET_BTN_Y_INC;
    int16_t yOffset;
    int16_t xOffset;

    Display_Start(s_pHalContext, (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_CoDl_lineWidth(s_pHalContext, 1 * 16);
    /* Draw keys with flat effect */
    xOffset = WIDGET_X_INC1;
    yOffset = WIDGET_Y_INC1 + ButtonH + WIDGET_TITLE_Y_INC;
    EVE_CoCmd_fgColor(s_pHalContext, WIDGET_COLOR_RED);
    EVE_CoCmd_keys(s_pHalContext, xOffset, WIDGET_Y_INC1, 4 * ButtonW, ButtonH, TextFont, OPT_FLAT, "ABCD");
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset, WIDGET_TITLE_FONT, OPT_CENTERY, "Flat effect"); //text info
    /* Draw keys with 3d effect */
    EVE_CoCmd_fgColor(s_pHalContext, WIDGET_COLOR_YELLOW);
    xOffset += 4 * ButtonW + WIDGET_X_INC2;
    EVE_CoCmd_keys(s_pHalContext, xOffset, WIDGET_Y_INC1, 4 * ButtonW, ButtonH, TextFont, 0, "ABCD");
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset, WIDGET_TITLE_FONT, OPT_CENTERY, "3D effect"); //text info
    /* Draw keys with center option */
    EVE_CoCmd_fgColor(s_pHalContext, WIDGET_COLOR_BLUE);
    xOffset += 4 * ButtonW + WIDGET_X_INC2;
    EVE_CoCmd_keys(s_pHalContext, xOffset, WIDGET_Y_INC1, 4 * ButtonW, ButtonH, TextFont, OPT_CENTER, "ABCD");
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset, WIDGET_TITLE_FONT, OPT_CENTERY, "Option Center"); //text info

    yOffset += WIDGET_Y_INC2;
    /* Construct a simple keyboard - note that the tags associated with the keys are the character values given in the arguments */
    EVE_CoCmd_fgColor(s_pHalContext, WIDGET_COLOR_GREEN);
    EVE_CoCmd_gradColor(s_pHalContext, WIDGET_COLOR_RED);
    EVE_CoCmd_keys(s_pHalContext, 0, yOffset, 10 * ButtonW, ButtonH, TextFont, OPT_CENTER,
        "qwertyuiop");
    EVE_CoCmd_gradColor(s_pHalContext, WIDGET_COLOR_YELLOW);
    yOffset += ButtonH + yBtnDst;
    EVE_CoCmd_keys(s_pHalContext, 0, yOffset, 10 * ButtonW, ButtonH, TextFont, OPT_CENTER,
        "asdfghjkl");
    EVE_CoCmd_gradColor(s_pHalContext, WIDGET_COLOR_BLUE);
    yOffset += ButtonH + yBtnDst;
    EVE_CoCmd_keys(s_pHalContext, 0, yOffset, 10 * ButtonW, ButtonH, TextFont,
        (OPT_CENTER | 'z'), "zxcvbnm"); //highlight button z
    yOffset += ButtonH + yBtnDst;
    EVE_CoCmd_button(s_pHalContext, yBtnDst, yOffset, 10 * ButtonW, ButtonH, TextFont, 0, " "); //mandatory to give '\0' at the end to make sure coprocessor understands the string end
    yOffset = WIDGET_Y_INC1 + ButtonH + WIDGET_TITLE_Y_INC + WIDGET_Y_INC2;
    EVE_CoCmd_keys(s_pHalContext, 11 * ButtonW, yOffset, 3 * ButtonW, ButtonH, TextFont, 0, "789");
    yOffset += ButtonH + yBtnDst;
    EVE_CoCmd_keys(s_pHalContext, 11 * ButtonW, yOffset, 3 * ButtonW, ButtonH, TextFont, 0, "456");
    yOffset += ButtonH + yBtnDst;
    EVE_CoCmd_keys(s_pHalContext, 11 * ButtonW, yOffset, 3 * ButtonW, ButtonH, TextFont, 0, "123");
    yOffset += ButtonH + yBtnDst;
    EVE_CoDl_colorA(s_pHalContext, 255);
    EVE_CoCmd_keys(s_pHalContext, 11 * ButtonW, yOffset, 3 * ButtonW, ButtonH, TextFont, (0 | '0'),
        "0."); //highlight button 0
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief demonstrate progress bar widget
 *
 * @param None
 * @return None
 */
static void SAMAPP_Widget_progressbar()
{
    Draw_Text(s_pHalContext, "Example for: Progress bar widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of progress function. Progress func */
    /* draws process bar with fgcolor for the % completion and bgcolor for   */
    /* % remaining. Progress bar supports flat and 3d effets                 */
    /*************************************************************************/
    int16_t xOffset = WIDGET_X_INC2;
    int16_t yOffset = WIDGET_Y_INC2;
    int16_t yDist = (int16_t)(s_pHalContext->Height / 10);
    int16_t h = (int16_t)(s_pHalContext->Height / 24);
    int16_t w = (int16_t)(s_pHalContext->Width / 8);

    Display_Start(s_pHalContext, (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    /* Draw progress bar with flat effect */
    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_YELLOW);
    EVE_CoCmd_progress(s_pHalContext, xOffset, yOffset, w, h, OPT_FLAT, 50, 100); //note that h/2 will be added on both sides of the progress bar
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset + h + WIDGET_TITLE_Y_INC, WIDGET_TITLE_FONT, 0, "Flat effect"); //text info

    /* Draw progress bar with 3d effect */
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_GREEN);
    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_GREY);
    EVE_CoCmd_progress(s_pHalContext, xOffset + 2 * w, yOffset, w, h, 0, 75, 100);
    EVE_CoCmd_text(s_pHalContext, xOffset + 2 * w, yOffset + h + WIDGET_TITLE_Y_INC, WIDGET_TITLE_FONT, 0, "3D effect"); //text info

    /* Draw progress bar with 3d effect and string on top */
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_RED);
    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_BLUE);
    yOffset += 2 * yDist;
    EVE_CoCmd_progress(s_pHalContext, xOffset, yOffset, w, h, 0, 19660, 65535);
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_WHITE);
    EVE_CoCmd_text(s_pHalContext, xOffset + w / 2, yOffset + h / 2, WIDGET_TITLE_FONT, OPT_CENTER, "30 %"); //text info

    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_BLUE);
    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_GREY);
    yOffset += yDist;
    EVE_CoCmd_progress(s_pHalContext, xOffset, yOffset, w, h, 0, 10, 100);

    EVE_CoCmd_progress(s_pHalContext, xOffset + 2 * w, yOffset, h, w, 0, 40, 100);
    EVE_CoCmd_text(s_pHalContext, xOffset + 2 * w + h, yOffset, WIDGET_TITLE_FONT, 0, "40 % TopBottom"); //text info

    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_ORANGE);
    yOffset += yDist;
    EVE_CoCmd_progress(s_pHalContext, xOffset, yOffset, w, h, 0, 40, 100);

    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_GREEN);
    yOffset += yDist;
    EVE_CoCmd_progress(s_pHalContext, xOffset, yOffset, w, h, 0, 70, 100);

    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_YELLOW);
    yOffset += yDist;
    EVE_CoCmd_progress(s_pHalContext, xOffset, yOffset, w, h, 0, 90, 100);

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief demonstrate scroll widget
 *
 * @param None
 * @return None
 */
static void SAMAPP_Widget_scroll()
{
    Draw_Text(s_pHalContext, "Example for: scroll widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of scroll function. Scroll function */
    /* draws scroll bar with fgcolor for inner color and current location and*/
    /* can be given by val parameter */
    /*************************************************************************/
    int16_t xOffset = WIDGET_X_INC2;
    int16_t yOffset = WIDGET_Y_INC2;
    int16_t xDist = (int16_t)(s_pHalContext->Width / 12);
    int16_t yDist = (int16_t)(s_pHalContext->Height / 10);
    int16_t w = (int16_t)(s_pHalContext->Width / 3);
    int16_t h = (int16_t)(s_pHalContext->Height / 50);

    Display_Start(s_pHalContext, (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    /* Draw scroll bar with flat effect */
    EVE_CoCmd_fgColor(s_pHalContext, WIDGET_COLOR_YELLOW);
    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_BLUE);
    EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, w / 2, h, OPT_FLAT, 20, 30, 100); //note that h/2 size will be added on both sides of the progress bar
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset + h + WIDGET_TITLE_Y_INC, WIDGET_TITLE_FONT, 0, "Flat effect"); //text info

    /* Draw scroll bar with 3d effect */
    EVE_CoCmd_fgColor(s_pHalContext, WIDGET_COLOR_GREEN);
    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_GREY);
    xOffset += w / 2 + WIDGET_X_INC2;
    EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, w / 2, h, 0, 20, 30, 100);
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset + h + WIDGET_TITLE_Y_INC, WIDGET_TITLE_FONT, 0, "3D effect"); //text info

    /* Draw horizontal scroll bars */
    EVE_CoCmd_fgColor(s_pHalContext, WIDGET_COLOR_BLUE);
    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_GREY);
    xOffset = WIDGET_X_INC2;
    yOffset += 2 * yDist;
    EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, w, h, 0, 10, 20, 100);

    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_ORANGE);
    yOffset += yDist;
    EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, w, h, 0, 30, 20, 100);

    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_GREEN);
    yOffset += yDist;
    EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, w, h, 0, 50, 20, 100);

    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_YELLOW);
    yOffset += yDist;
    EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, w, h, 0, 70, 20, 100);

    xOffset = s_pHalContext->Width / 2;
    yOffset = WIDGET_Y_INC2;
    /* draw vertical scroll bars */
    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_GREY);
    EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, h, w, 0, 10, 10, 100);

    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_ORANGE);
    xOffset += xDist;
    EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, h, w, 0, 30, 20, 100);

    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_GREEN);
    xOffset += xDist;
    EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, h, w, 0, 50, 30, 100);

    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_YELLOW);
    xOffset += xDist;
    EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, h, w, 0, 70, 30, 100);

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief demonstrate slider widget
 *
 * @param None
 * @return None
 */
static void SAMAPP_Widget_slider()
{
    Draw_Text(s_pHalContext, "Example for: Slider widget");
    
    /*************************************************************************/
    /* Below code demonstrates the usage of slider function. Slider function */
    /* draws slider bar with fgcolor for inner color and bgcolor for the knob*/
    /* , contains input parameter for position of the knob                   */
    /*************************************************************************/
    int16_t xOffset = WIDGET_X_INC2;
    int16_t yOffset = WIDGET_Y_INC2;
    int16_t xDist = (int16_t)(s_pHalContext->Width / 12);
    int16_t yDist = (int16_t)(s_pHalContext->Height / 10);
    int16_t w = (int16_t)(s_pHalContext->Width / 3);
    int16_t h = (int16_t)(s_pHalContext->Height / 50);

    Display_Start(s_pHalContext, (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    /* Draw scroll bar with flat effect */
    EVE_CoCmd_fgColor(s_pHalContext, WIDGET_COLOR_RED);
    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_YELLOW);
    EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, w / 2, h, OPT_FLAT, 30, 100); //note that h/2 size will be added on both sides of the progress bar
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset + h + WIDGET_TITLE_Y_INC, WIDGET_TITLE_FONT, 0, "Flat effect"); //text info

    /* Draw scroll bar with 3d effect */
    EVE_CoCmd_fgColor(s_pHalContext, WIDGET_COLOR_GREEN);
    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_GREY);
    xOffset += w / 2 + WIDGET_X_INC2;
    EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, w / 2, h, 0, 50, 100);
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset + h + WIDGET_TITLE_Y_INC, WIDGET_TITLE_FONT, 0, "3D effect"); //text info

    /* Draw horizontal slider bars */
    EVE_CoCmd_fgColor(s_pHalContext, WIDGET_COLOR_ORANGE);
    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_GREY);
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_WHITE);
    xOffset = WIDGET_X_INC2;
    yOffset += 2 * yDist;
    EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, w, h, 0, 10, 100);

    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_BLUE);
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_RED);
    yOffset += yDist;
    EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, w, h, 0, 30, 100);

    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_GREEN);
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_GREY);
    yOffset += yDist;
    EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, w, h, 0, 50, 100);

    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_YELLOW);
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_WHITE);
    yOffset += yDist;
    EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, w, h, 0, 70, 100);

    xOffset = s_pHalContext->Width / 2;
    yOffset = WIDGET_Y_INC2;
    /* draw vertical slider bars */
    EVE_CoCmd_fgColor(s_pHalContext, WIDGET_COLOR_BLUE);
    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_GREY);
    EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, h, w, 0, 10, 100);

    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_ORANGE);
    xOffset += xDist;
    EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, h, w, 0, 30, 100);

    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_GREEN);
    xOffset += xDist;
    EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, h, w, 0, 50, 100);

    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_YELLOW);
    xOffset += xDist;
    EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, h, w, 0, 70, 100);

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief demonstrate dial widget
 *
 * @param None
 * @return None
 */
static void SAMAPP_Widget_dial()
{
    Draw_Text(s_pHalContext, "Example for: Dial widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of dial function. Dial function     */
    /* draws rounded bar with fgcolor for knob color and colorrgb for pointer*/
    /* , contains input parameter for angle of the pointer                   */
    /*************************************************************************/
    int16_t r = (int16_t)(s_pHalContext->Width / 15);
    int16_t xOffset = WIDGET_X_INC2 + r;
    int16_t yOffset = (int16_t)(s_pHalContext->Height / 4);

    Display_Start(s_pHalContext, (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    /* Draw dial with flat effect */
    EVE_CoCmd_fgColor(s_pHalContext, WIDGET_COLOR_YELLOW);
    EVE_CoCmd_dial(s_pHalContext, xOffset, yOffset, r, OPT_FLAT, (uint16_t) (0.2 * 65535)); //20%
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset + r + WIDGET_TITLE_Y_INC, WIDGET_TITLE_FONT, OPT_CENTER, "Flat effect"); //text info

    /* Draw dial with 3d effect */
    EVE_CoCmd_fgColor(s_pHalContext, WIDGET_COLOR_GREEN);
    xOffset += 3 * r;
    EVE_CoCmd_dial(s_pHalContext, xOffset, yOffset, r, 0, (uint16_t)(0.45 * 65535)); //45%
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset + r + WIDGET_TITLE_Y_INC, WIDGET_TITLE_FONT, OPT_CENTER, "3D effect"); //text info

    /* Draw increasing dials horizontally */
    EVE_CoCmd_fgColor(s_pHalContext, WIDGET_COLOR_RED);
    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);
    xOffset = WIDGET_X_INC2 + r;
    yOffset = (int16_t)(s_pHalContext->Height * 3 / 4);
    r = r / 2;
    EVE_CoCmd_dial(s_pHalContext, xOffset, yOffset, r, 0, (uint16_t)(0.30 * 65535));
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset + r + WIDGET_TITLE_Y_INC, WIDGET_TITLE_FONT, OPT_CENTER, "30 %"); //text info

    EVE_CoCmd_fgColor(s_pHalContext, WIDGET_COLOR_BLUE);
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_GREY);
    xOffset += 3 * r;
    r = r * 3 / 2;
    EVE_CoCmd_dial(s_pHalContext, xOffset, yOffset, r, 0, (uint16_t) (0.45 * 65535));
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset + r + WIDGET_TITLE_Y_INC, WIDGET_TITLE_FONT, OPT_CENTER, "45 %"); //text info

    EVE_CoCmd_fgColor(s_pHalContext, WIDGET_COLOR_ORANGE);
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_BLUE);
    xOffset += 3 * r;
    r = r * 3 / 2;
    EVE_CoCmd_dial(s_pHalContext, xOffset, yOffset, r, 0, (uint16_t) (0.60 * 65535));
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset + r + WIDGET_TITLE_Y_INC, WIDGET_TITLE_FONT, OPT_CENTER, "60 %"); //text info

    EVE_CoCmd_fgColor(s_pHalContext, WIDGET_COLOR_GREY);
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_RED);
    xOffset += 3 * r;
    r = r * 3 / 2;
    EVE_CoCmd_dial(s_pHalContext, xOffset, yOffset, r, 0, (uint16_t) (0.75 * 65535));
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset + r + WIDGET_TITLE_Y_INC, WIDGET_TITLE_FONT, OPT_CENTER, "75 %"); //text info
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief demonstrate toggle widget
 *
 * @param None
 * @return None
 */
static void SAMAPP_Widget_toggle()
{
    Draw_Text(s_pHalContext, "Example for: Toggle widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of toggle function. Toggle function */
    /* draws line with inside knob to choose between left and right. Toggle  */
    /* inside color can be changed by bgcolor and knob color by fgcolor. Left*/
    /* right texts can be written and the color of the text can be changed by*/
    /* colorrgb graphics function                                            */
    /*************************************************************************/
    int16_t w = (int16_t)(s_pHalContext->Width / 15);
    int16_t xOffset = WIDGET_X_INC2 + w;
    int16_t yOffset = WIDGET_Y_INC2;
    int16_t yDist = (int16_t)(s_pHalContext->Height / 10);

    Display_Start(s_pHalContext, (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    /* Draw toggle with flat effect */
    EVE_CoCmd_fgColor(s_pHalContext, WIDGET_COLOR_YELLOW);
    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_BLUE);

    EVE_CoCmd_toggle(s_pHalContext, xOffset, yOffset, w, WIDGET_TITLE_FONT, OPT_FLAT, (uint16_t)(0.5 * 65535),
        "no\xffyes"); //circle radius will be extended on both the horizontal sides
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset + yDist, WIDGET_TITLE_FONT, 0, "Flat effect"); //text info

    /* Draw toggle bar with 3d effect */
    EVE_CoCmd_fgColor(s_pHalContext, WIDGET_COLOR_ORANGE);
    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_GREEN);
    xOffset += 4 * w;
    EVE_CoCmd_toggle(s_pHalContext, xOffset, yOffset, w, WIDGET_TITLE_FONT, 0, 1 * 65535, "stop\xffrun");
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset + yDist, WIDGET_TITLE_FONT, 0, "3D effect"); //text info

    /* Draw horizontal toggle bars */
    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_RED);
    EVE_CoCmd_fgColor(s_pHalContext, WIDGET_COLOR_PURPLE);
    xOffset = WIDGET_X_INC2 + w;
    yOffset += 2 * yDist;
    EVE_CoCmd_toggle(s_pHalContext, xOffset, yOffset, w, WIDGET_TITLE_FONT, 0, 0 * 65535, "-ve\xff+ve");

    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_BLUE);
    EVE_CoCmd_fgColor(s_pHalContext, WIDGET_COLOR_GREEN);
    yOffset += yDist;
    EVE_CoCmd_toggle(s_pHalContext, xOffset, yOffset, w, WIDGET_TITLE_FONT, 0, (uint16_t)(0.25 * 65535), "zero\xffone");

    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_YELLOW);
    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);
    yOffset += yDist;
    EVE_CoCmd_toggle(s_pHalContext, xOffset, yOffset, w, WIDGET_TITLE_FONT, 0, (uint16_t)(0.5 * 65535), "exit\xffinit");

    EVE_CoCmd_bgColor(s_pHalContext, WIDGET_COLOR_GREY);
    EVE_CoCmd_fgColor(s_pHalContext, WIDGET_COLOR_WHITE);
    yOffset += yDist;
    EVE_CoCmd_toggle(s_pHalContext, xOffset, yOffset, w, WIDGET_TITLE_FONT, 0, (uint16_t)(0.75 * 65535), "off\xffon");

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief demonstrate a spinner widget
 *
 * @param None
 * @return None
 */
static void SAMAPP_Widget_spinner()
{
    Draw_Text(s_pHalContext, "Example for: Spinner widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of spinner function. Spinner func   */
    /* will wait untill stop command is sent from the mcu. Spinner has option*/
    /* for displaying points in circle fashion or in a line fashion.         */
    /*************************************************************************/
    Display_Start(s_pHalContext, (uint8_t[]) { 64, 64, 64 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), WIDGET_TITLE_Y_INC, WIDGET_TITLE_FONT, OPT_CENTER, "Spinner circle");
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 4 * WIDGET_TITLE_Y_INC, WIDGET_TITLE_FONT, OPT_CENTER, "Please Wait ...");
    EVE_CoCmd_spinner(s_pHalContext, (int16_t) (s_pHalContext->Width / 2),
        (int16_t) (s_pHalContext->Height / 2), 0, 1); //style 0 and scale 0

    /* Wait till coprocessor completes the operation */
    EVE_Cmd_waitFlush(s_pHalContext);
    SAMAPP_DELAY;

    /* spinner with style 1 and scale 1 */
    Display_Start(s_pHalContext, (uint8_t[]) { 64, 64, 64 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), WIDGET_TITLE_Y_INC, WIDGET_TITLE_FONT, OPT_CENTER, "Spinner line");
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 4 * WIDGET_TITLE_Y_INC, WIDGET_TITLE_FONT, OPT_CENTER, "Please Wait ...");
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_BLUE);
    EVE_CoCmd_spinner(s_pHalContext, (int16_t) (s_pHalContext->Width / 2),
        (int16_t) (s_pHalContext->Height / 2), 1, 1); //style 1 and scale 1

    /* Wait till coprocessor completes the operation */
    EVE_Cmd_waitFlush(s_pHalContext);
    SAMAPP_DELAY;

    Display_Start(s_pHalContext, (uint8_t[]) { 64, 64, 64 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), WIDGET_TITLE_Y_INC, WIDGET_TITLE_FONT, OPT_CENTER, "Spinner clockhand");
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 4 * WIDGET_TITLE_Y_INC, WIDGET_TITLE_FONT, OPT_CENTER, "Please Wait ...");
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_RED);
    EVE_CoCmd_spinner(s_pHalContext, (int16_t) (s_pHalContext->Width / 2),
        (int16_t) (s_pHalContext->Height / 2), 2, 1); //style 2 scale 1

    /* Wait till coprocessor completes the operation */
    EVE_Cmd_waitFlush(s_pHalContext);
    SAMAPP_DELAY;

    Display_Start(s_pHalContext, (uint8_t[]) { 64, 64, 64 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), WIDGET_TITLE_Y_INC, WIDGET_TITLE_FONT, OPT_CENTER, "Spinner two dots");
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 4 * WIDGET_TITLE_Y_INC, WIDGET_TITLE_FONT, OPT_CENTER, "Please Wait ...");
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_GREEN);
    EVE_CoCmd_spinner(s_pHalContext, (int16_t) (s_pHalContext->Width / 2),
        (int16_t) (s_pHalContext->Height / 2), 3, 1); //style 3 scale 0

    /* Wait till coprocessor completes the operation */
    EVE_Cmd_waitFlush(s_pHalContext);
    SAMAPP_DELAY;

    /* Send the stop command */
    EVE_CoCmd_stop(s_pHalContext);
    /* Update the command buffer pointers - both read and write pointers */
    EVE_Cmd_waitFlush(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief API to demonstrate text widget
 *
 * @param None
 * @return None
 */
static void SAMAPP_Widget_text()
{
    Draw_Text(s_pHalContext, "Example for: text widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of text function. Text function     */
    /* draws text with either in built or externally configured text. Text   */
    /* color can be changed by colorrgb and text function supports display of*/
    /* texts on left, right, top, bottom and center respectively             */
    /*************************************************************************/
    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 0x00, 0x00, 0x00 }, 0, 4);
    /* Draw text at 0,0 location */
    EVE_CoCmd_text(s_pHalContext, 0, 0, WIDGET_TITLE_FONT, 0, "Bridgetek!");
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_RED);
    EVE_CoCmd_text(s_pHalContext, 0, WIDGET_TITLE_Y_INC, WIDGET_TITLE_FONT_SMALL, 0, "Text31 at 0,0"); //text info

    /* text with centerx */
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_BLUE);
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), (int16_t)(s_pHalContext->Height / 8), WIDGET_TITLE_FONT, OPT_CENTERX, "Bridgetek!");
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_RED);
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), (int16_t)(s_pHalContext->Height / 8 + WIDGET_TITLE_Y_INC), WIDGET_TITLE_FONT_SMALL, 0, "Text31 centerX"); //text info

    /* text with centery */
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_GREEN);
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 5), (int16_t)(s_pHalContext->Height * 2 / 8), WIDGET_TITLE_FONT, OPT_CENTERY, "Bridgetek!");
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_RED);
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 5), (int16_t)(s_pHalContext->Height * 2 / 8 + WIDGET_TITLE_Y_INC), WIDGET_TITLE_FONT_SMALL, 0, "Text31 centerY"); //text info

    /* text with center */
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_ORANGE);
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), (int16_t)(s_pHalContext->Height * 3 / 8), WIDGET_TITLE_FONT, OPT_CENTER, "Bridgetek!");
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_RED);
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), (int16_t)(s_pHalContext->Height * 3 / 8 + WIDGET_TITLE_Y_INC), WIDGET_TITLE_FONT_SMALL, 0, "Text31 center"); //text info

    /* text with rightx */
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_PURPLE);
    EVE_CoCmd_text(s_pHalContext, (int16_t)s_pHalContext->Width, (int16_t)(s_pHalContext->Height * 4 / 8), WIDGET_TITLE_FONT, OPT_RIGHTX, "Bridgetek!");
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_RED);
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width - 150), (int16_t)(s_pHalContext->Height * 4 / 8 + WIDGET_TITLE_Y_INC), WIDGET_TITLE_FONT_SMALL, 0, "Text31 rightX"); //text info
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief API to demonstrate number widget
 *
 * @param None
 * @return None
 */
static void SAMAPP_Widget_number()
{
    Draw_Text(s_pHalContext, "Example for: number widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of number function. Number function */
    /* draws text with only 32bit decimal number, signed or unsigned can also*/
    /* be specified as input parameter. Options like centerx, centery, center*/
    /* and rightx are supported                                              */
    /*************************************************************************/
    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 0x00, 0x00, 0x00 }, 0, 4);
    /* Draw number at 0,0 location */
    EVE_CoCmd_number(s_pHalContext, 0, 0, WIDGET_TITLE_FONT, 0, 1234);
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_RED);
    EVE_CoCmd_text(s_pHalContext, 0, WIDGET_TITLE_Y_INC, WIDGET_TITLE_FONT_SMALL, 0, "Number31 at 0,0"); //text info

    /* number with centerx */
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_BLUE);
    EVE_CoCmd_number(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), (int16_t)(s_pHalContext->Height / 8), WIDGET_TITLE_FONT, OPT_CENTERX | OPT_SIGNED, -1234);
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_RED);
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), (int16_t)(s_pHalContext->Height / 8 + WIDGET_TITLE_Y_INC), WIDGET_TITLE_FONT_SMALL, 0, "Number31 centerX"); //text info

    /* number with centery */
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_GREEN);
    EVE_CoCmd_number(s_pHalContext, (int16_t)(s_pHalContext->Width / 5), (int16_t)(s_pHalContext->Height * 2 / 8), WIDGET_TITLE_FONT, OPT_CENTERY, 1234);
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_RED);
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 5), (int16_t)(s_pHalContext->Height * 2 / 8 + WIDGET_TITLE_Y_INC), WIDGET_TITLE_FONT_SMALL, 0, "Number31 centerY"); //text info

    /* number with center */
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_ORANGE);
    EVE_CoCmd_number(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), (int16_t)(s_pHalContext->Height * 3 / 8), WIDGET_TITLE_FONT, OPT_CENTER | OPT_SIGNED, -1234);
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_RED);
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), (int16_t)(s_pHalContext->Height * 3 / 8 + WIDGET_TITLE_Y_INC), WIDGET_TITLE_FONT_SMALL, 0, "Number31 center"); //text info

    /* number with rightx */
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_PURPLE);
    EVE_CoCmd_number(s_pHalContext, (int16_t)s_pHalContext->Width, (int16_t)(s_pHalContext->Height * 4 / 8), WIDGET_TITLE_FONT, OPT_RIGHTX, 1234);
    EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_RED);
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width - 200), (int16_t)(s_pHalContext->Height * 4 / 8 + WIDGET_TITLE_Y_INC), WIDGET_TITLE_FONT_SMALL, 0, "Number31 rightX"); //text info
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief API to demonstrate button functionality
 *
 * @param None
 * @return None
 */
static void SAMAPP_Widget_button()
{
    Draw_Text(s_pHalContext, "Example for: button functionality");

    /*************************************************************************/
    /* Below code demonstrates the usage of button function. Buttons can be  */
    /* constructed using flat or 3d effect. Button color can be changed by   */
    /* fgcolor command and text color is set by COLOR_RGB graphics command   */
    /*************************************************************************/
    int16_t xOffset = WIDGET_X_INC2;
    int16_t yOffset = WIDGET_Y_INC2;
    int16_t xDist = (int16_t)(s_pHalContext->Width / 6);
    int16_t yDist = (int16_t)(s_pHalContext->Height / 8);
    int16_t w = WIDGET_BTN_W;
    int16_t h = WIDGET_BTN_W;

    /*  Construct a buttons with "ONE/TWO/THREE" text and default background */
    Display_Start(s_pHalContext, (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    /* flat effect and default color background */
    EVE_CoCmd_fgColor(s_pHalContext, WIDGET_COLOR_BLUE);
    yOffset = (int16_t) (s_pHalContext->Height / 2 - 2 * yDist);
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, w, h, WIDGET_TITLE_FONT, OPT_FLAT, "ABC");
    yOffset += yDist;
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, w, h, WIDGET_TITLE_FONT, OPT_FLAT, "ABC");
    yOffset += yDist;
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, w, h, WIDGET_TITLE_FONT, OPT_FLAT, "ABC");
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + h + WIDGET_TITLE_Y_INC), WIDGET_TITLE_FONT, OPT_CENTERY, "Flat effect"); //text info

    /* 3D effect */
    xOffset += xDist;
    yOffset = (int16_t)(s_pHalContext->Height / 2 - 2 * yDist);
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, w, h, WIDGET_TITLE_FONT, 0, "ABC");
    yOffset += yDist;
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, w, h, WIDGET_TITLE_FONT, 0, "ABC");
    yOffset += yDist;
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, w, h, WIDGET_TITLE_FONT, 0, "ABC");
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + h + WIDGET_TITLE_Y_INC), WIDGET_TITLE_FONT, OPT_CENTERY, "3D Effect"); //text info

    /* 3d effect with background color */
    EVE_CoCmd_fgColor(s_pHalContext, WIDGET_COLOR_YELLOW);
    xOffset += xDist;
    yOffset = (int16_t)(s_pHalContext->Height / 2 - 2 * yDist);
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, w, h, WIDGET_TITLE_FONT, 0, "ABC");
    EVE_CoCmd_fgColor(s_pHalContext, WIDGET_COLOR_GREEN);
    yOffset += yDist;
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, w, h, WIDGET_TITLE_FONT, 0, "ABC");
    EVE_CoCmd_fgColor(s_pHalContext, WIDGET_COLOR_PINK);
    yOffset += yDist;
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, w, h, WIDGET_TITLE_FONT, 0, "ABC");
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + h + WIDGET_TITLE_Y_INC), WIDGET_TITLE_FONT, OPT_CENTERY, "3D Color"); //text info

    /* 3d effect with gradient color */
    EVE_CoCmd_fgColor(s_pHalContext, WIDGET_COLOR_GREY);
    EVE_CoCmd_gradColor(s_pHalContext, WIDGET_COLOR_RED);
    xOffset += xDist;
    yOffset = (int16_t)(s_pHalContext->Height / 2 - 2 * yDist);
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, w, h, WIDGET_TITLE_FONT, 0, "ABC");
    yOffset += yDist;
    EVE_CoCmd_gradColor(s_pHalContext, WIDGET_COLOR_YELLOW);
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, w, h, WIDGET_TITLE_FONT, 0, "ABC");
    yOffset += yDist;
    EVE_CoCmd_gradColor(s_pHalContext, WIDGET_COLOR_BLUE);
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, w, h, WIDGET_TITLE_FONT, 0, "ABC");
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + h + WIDGET_TITLE_Y_INC), WIDGET_TITLE_FONT, OPT_CENTERY, "3D Gradient"); //text info
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief API to demonstrate screen saver widget - playing of bitmap via macro0
 *
 * @param None
 * @return None
 */
static void SAMAPP_Widget_screensaver()
{
    Draw_Text(s_pHalContext, "Example for: screen saver with an image");

    /*************************************************************************/
    /* Below code demonstrates the usage of screensaver function. Screen     */
    /* saver modifies macro0 with the vertex2f command.                      */
    /* MCU can display any static display list at the background with the    */
    /* changing bitmap                                                       */
    /*************************************************************************/
    /* Download the bitmap data */
    EVE_Util_loadRawFile(s_pHalContext, WIDGET_LOAD_ADDR, TEST_DIR WIDGET_BITMAP);

    /* Copy the display list */
    Display_Start(s_pHalContext, (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 }, 0, 2);
    /* lena bitmap */
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_scale(s_pHalContext, 3 * 65536, 3 * 65536); //scale the bitmap 3 times on both sides
    EVE_CoCmd_setMatrix(s_pHalContext);
    /* Send command screen saver */
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_bitmapSource(s_pHalContext, WIDGET_LOAD_ADDR);
    EVE_CoDl_bitmapLayout(s_pHalContext, WIDGET_BITMAP_FORMAT, 3 * WIDGET_BITMAP_W, WIDGET_BITMAP_H);
    EVE_CoDl_bitmapSize(s_pHalContext, BILINEAR, BORDER, BORDER, WIDGET_BITMAP_W * 3, WIDGET_BITMAP_H * 3);
    EVE_CoDl_macro(s_pHalContext, 0);
    EVE_CoDl_end(s_pHalContext);
    /* Display the text */
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_setMatrix(s_pHalContext);
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), (int16_t) (s_pHalContext->Height / 2), WIDGET_TITLE_FONT, OPT_CENTER, "Screen Saver ...");
    EVE_CoDl_display(s_pHalContext);
    EVE_CoCmd_screenSaver(s_pHalContext); //screen saver command will continuously update the macro0 with vertex2f command
    EVE_Cmd_waitFlush(s_pHalContext);
    EVE_sleep(5000);

    /* Send the stop command */
    EVE_CoCmd_stop(s_pHalContext);
    /* Update the command buffer pointers - both read and write pointers */
    EVE_Cmd_waitFlush(s_pHalContext);
    EVE_CoDl_vertexFormat(s_pHalContext, 4);
}

/**
 * @brief API to demonstrate sketch widget
 *
 * @param None
 * @return None
 */
static void SAMAPP_Widget_sketch()
{
    Draw_Text(s_pHalContext, "Example for: sketch widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of sketch function. Sketch function */
    /* draws line for pen movement. Skecth supports L1 and L8 output formats */
    /* Sketch draws till stop command is executed.                           */
    /*************************************************************************/
    int16_t BorderSz = WIDGET_BTN_W;
    uint32_t MemZeroSz;
    int8_t line_width;
    uint32_t tag = 0;
    /* Send command sketch */

    MemZeroSz = 1L * (s_pHalContext->Width - 2 * BorderSz) * (s_pHalContext->Height - 2 * BorderSz);
    /* Sketch L8 format */
    /* Send command sketch */
    EVE_CoCmd_memZero(s_pHalContext, RAM_G, MemZeroSz);
    EVE_CoCmd_sketch(s_pHalContext, BorderSz, BorderSz,
        (int16_t)(s_pHalContext->Width - 2 * BorderSz),
        (int16_t)(s_pHalContext->Height - 2 * BorderSz), 0, L8); //sketch in L8 format
    /* Display the sketch */
    while ((EVE_CoCmd_regRead(s_pHalContext, REG_TOUCH_TAG, &tag)) && (tag != 'K'))
    {
        Display_Start(s_pHalContext, (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
        EVE_CoDl_scissorSize(s_pHalContext,
            (s_pHalContext->Width - 2 * BorderSz), (s_pHalContext->Height - 2 * BorderSz));
        EVE_CoDl_scissorXY(s_pHalContext, BorderSz, BorderSz);
        EVE_CoDl_clearColorRgb_ex(s_pHalContext, WIDGET_COLOR_WHITE);
        EVE_CoDl_clear(s_pHalContext, 1, 1, 1);

        /* default the scissor size */
        EVE_CoDl_scissorSize(s_pHalContext, 2048, 2048);
        EVE_CoDl_scissorXY(s_pHalContext, 0, 0);
        /* L8 bitmap display */
        EVE_CoDl_begin(s_pHalContext, BITMAPS);
        EVE_CoDl_bitmapSource(s_pHalContext, RAM_G);
        EVE_CoDl_bitmapLayout(s_pHalContext,
            L8, (s_pHalContext->Width - 2 * BorderSz), (s_pHalContext->Height - 2 * BorderSz));
        EVE_CoDl_bitmapSize(s_pHalContext,
            BILINEAR, BORDER, BORDER, (s_pHalContext->Width - 2 * BorderSz), (s_pHalContext->Height - 2 * BorderSz));

        EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_RED);
        line_width = 3;
        while (line_width >= 0)
        {
            EVE_CoDl_vertex2f_4(s_pHalContext, (BorderSz + line_width) * 16, (BorderSz + line_width) * 16);
            line_width = line_width - 1;
        }
        EVE_CoDl_end(s_pHalContext);
        /* Display the text */
        EVE_CoDl_colorRgb_ex(s_pHalContext, WIDGET_COLOR_WHITE);
        EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), WIDGET_TITLE_Y_INC, WIDGET_TITLE_FONT, OPT_CENTER,
            "Sketch L8");
        EVE_CoDl_tag(s_pHalContext, 'K');
        EVE_CoCmd_button(s_pHalContext, (int16_t)(s_pHalContext->Width - WIDGET_BTN_W), 0, WIDGET_BTN_W, WIDGET_BTN_H, WIDGET_TITLE_FONT_SMALL, 0, "EXIT");
        Display_End(s_pHalContext);
    }

    /* Send the stop command */
    EVE_CoCmd_stop(s_pHalContext);
    /* Update the command buffer pointers - both read and write pointers */
    EVE_Cmd_waitFlush(s_pHalContext);

    EVE_Util_clearScreen(s_pHalContext);
    SAMAPP_DELAY;

    EVE_CoCmd_memZero(s_pHalContext, RAM_G, MemZeroSz);
    EVE_CoCmd_sketch(s_pHalContext, BorderSz, BorderSz,
        (int16_t)(s_pHalContext->Width - 2 * BorderSz),
        (int16_t)(s_pHalContext->Height - 2 * BorderSz), RAM_G, L1); //sketch in L1 format
    while ((EVE_CoCmd_regRead(s_pHalContext, REG_TOUCH_TAG, &tag)) && (tag != 'S'))
    {
        /* Display the sketch */
        Display_Start(s_pHalContext, (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
        EVE_CoDl_scissorSize(s_pHalContext,
            (s_pHalContext->Width - 2 * BorderSz), (s_pHalContext->Height - 2 * BorderSz));
        EVE_CoDl_scissorXY(s_pHalContext, BorderSz, BorderSz);
        EVE_CoDl_clearColorRgb_ex(s_pHalContext, WIDGET_COLOR_WHITE);
        EVE_CoDl_clear(s_pHalContext, 1, 1, 1);

        /* default the scissor size */
        EVE_CoDl_scissorSize(s_pHalContext, 2048, 2048);
        EVE_CoDl_scissorXY(s_pHalContext, 0, 0);

        /* L1 bitmap display */
        EVE_CoDl_begin(s_pHalContext, BITMAPS);
        EVE_CoDl_bitmapSource(s_pHalContext, RAM_G);
        EVE_CoDl_bitmapLayout(s_pHalContext,
            L1, (s_pHalContext->Width - 2 * BorderSz) / 8, (s_pHalContext->Height - 2 * BorderSz));
        EVE_CoDl_bitmapSize(s_pHalContext,
            BILINEAR, BORDER, BORDER, (s_pHalContext->Width - 2 * BorderSz), (s_pHalContext->Height - 2 * BorderSz));

        EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);
        line_width = 3;
        while (line_width >= 0)
        {
            EVE_CoDl_vertex2f_4(s_pHalContext, (BorderSz + line_width) * 16, (BorderSz + line_width) * 16);
            line_width = line_width - 1;
        }
        EVE_CoDl_end(s_pHalContext);
        /* Display the text */
        EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0xff, 0xff);
        EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), WIDGET_TITLE_Y_INC, WIDGET_TITLE_FONT, OPT_CENTER,
            "Sketch L1");
        EVE_CoDl_tag(s_pHalContext, 'S');
        EVE_CoCmd_button(s_pHalContext, (int16_t)(s_pHalContext->Width - WIDGET_BTN_W), 0, WIDGET_BTN_W, WIDGET_BTN_H, WIDGET_TITLE_FONT_SMALL, 0, "EXIT");
        Display_End(s_pHalContext);
    }

    /* Send the stop command */
    EVE_CoCmd_stop(s_pHalContext);
    /* Update the command buffer pointers - both read and write pointers */
    EVE_Cmd_waitFlush(s_pHalContext);
}

/**
 * @brief API to demonstrate sketch widget
 *
 * @param None
 * @return None
 */
static void SAMAPP_Widget_arc()
{
    Draw_Text(s_pHalContext, "Example for: arc widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of arc function. The arc command    */
    /* draws a circular arc. The end caps of the arc are rounded.            */
    /* If the angles specify a complete circle, a disc is drawn.             */
    /*************************************************************************/
    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    uint32_t rgb[WIDGET_ARC_COLOR_NUM] = { 0x00E6194B, 0x003CB44B, 0x00FFFF19, 0x000082C8, 0x00F58230, 0x00911EB4, 0x0046F0F0, 0x00F032E6, 0x00D2F53C, 0x00FABED4 };
    uint16_t i, r0, r1, a0, a1;

    for (i = 1; i < WIDGET_ARC_COLOR_NUM; i++)
    {
        r0 = WIDGET_ARC_R * i;
        r1 = r0 + WIDGET_ARC_W;
        a0 = (i * 0x1933) & 0xffff;
        a1 = (a0 + (i * 0x3933)) & 0xffff;
        EVE_CoDl_colorRgb_ex(s_pHalContext, rgb[i - 1]);
        EVE_CoCmd_arc(s_pHalContext, s_pHalContext->Width / 2, s_pHalContext->Height / 2, r0, r1, a0, a1);
    }

    EVE_CoDl_colorRgb_ex(s_pHalContext, rgb[WIDGET_ARC_COLOR_NUM - 1]);
    EVE_CoCmd_arc(s_pHalContext, s_pHalContext->Width / 2, s_pHalContext->Height / 2,
        WIDGET_ARC_COLOR_NUM * WIDGET_ARC_R, WIDGET_ARC_COLOR_NUM * WIDGET_ARC_R + 5 * WIDGET_ARC_W, 0, 0xffff);

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief API to demonstrate glow widget
 *
 * @param None
 * @return None
 */
static void SAMAPP_Widget_glow()
{
    Draw_Text(s_pHalContext, "Example for: glow widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of glow function. The glow command  */
    /* draws an additive glow effect centered in a rectangle, using the      */
    /* current color.                                                        */
    /*************************************************************************/
    Display_Start(s_pHalContext, (uint8_t[]) { 0xDD, 0XDD, 0xDD }, (uint8_t[]) { 0xFF, 0xC0, 0x80 }, 0, 4);
    EVE_CoCmd_text(s_pHalContext, s_pHalContext->Width / 2, s_pHalContext->Height / 2, WIDGET_TITLE_FONT_LARGE, OPT_CENTER, "glow widget");
    EVE_CoDl_colorRgb(s_pHalContext, 0x60, 48, 30);
    EVE_CoCmd_glow(s_pHalContext, s_pHalContext->Width / 2 - WIDGET_GLOW_W / 2, s_pHalContext->Height / 2 - WIDGET_GLOW_H / 2, WIDGET_GLOW_W, WIDGET_GLOW_H);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief Main entry to run all widget demos
 *
 * @param None
 * @return None
 */
static void SAMAPP_Widget()
{
    SAMAPP_Widget_logo();
    SAMAPP_Widget_clock();
    SAMAPP_Widget_gauge();
    SAMAPP_Widget_gradient();
    SAMAPP_Widget_gradientA();
    SAMAPP_Widget_cgradient();
    SAMAPP_Widget_keysInteractive();
    SAMAPP_Widget_keys();
    SAMAPP_Widget_progressbar();
    SAMAPP_Widget_scroll();
    SAMAPP_Widget_slider();
    SAMAPP_Widget_dial();
    SAMAPP_Widget_toggle();
    SAMAPP_Widget_spinner();
    SAMAPP_Widget_text();
    SAMAPP_Widget_number();
    SAMAPP_Widget_button();
    SAMAPP_Widget_screensaver();
    SAMAPP_Widget_sketch();
    SAMAPP_Widget_arc();
    SAMAPP_Widget_glow();
}


