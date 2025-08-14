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
#include "base.h"

#define SAMAPP_DELAY           EVE_sleep(2000);

static EVE_HalContext s_halContext;
static EVE_HalContext* s_pHalContext;
void SAMAPP_Widget();

/* Header of raw data containing properties of bitmap */
SAMAPP_Bitmap_header_t SAMAPP_Bitmap_RawData_Header =
{
    /* format,width,height,stride,arrayoffset */
    RGB565      ,    40,      40,    40 * 2,    0
};

int main(int argc, char* argv[])
{
    s_pHalContext = &s_halContext;
    Gpu_Init(s_pHalContext);
    LVDS_Config(s_pHalContext, YCBCR, MODE_PICTURE);
	if (eve_loadpatch(s_pHalContext) != 0)
		eve_printf_debug("eve_loadpatch failed\n");
	else
		eve_printf_debug("eve_loadpatch OK\n");

    // read and store calibration setting
#if GET_CALIBRATION == 1
    EVE_Calibrate(s_pHalContext);
    Calibration_Save(s_pHalContext);
#endif

    EVE_Util_clearScreen(s_pHalContext);

    char *info[] =
    {  "EVE Sample Application",
        "This sample demonstrate the using of basic widgets", 
        "",
        ""
    }; 

    while (true) {
        WelcomeScreen(s_pHalContext, info);
        SAMAPP_Widget();

        EVE_Util_clearScreen(s_pHalContext);

        Gpu_Release(s_pHalContext);

        /* Init HW Hal for next loop*/
        Gpu_Init(s_pHalContext);
        LVDS_Config(s_pHalContext, YCBCR, MODE_PICTURE);
		if (eve_loadpatch(s_pHalContext) != 0)
			eve_printf_debug("eve_loadpatch failed\n");
		else
			eve_printf_debug("eve_loadpatch OK\n");
#if GET_CALIBRATION == 1
        Calibration_Restore(s_pHalContext);
#endif
    }

    return 0;
}

/**
* @brief Show logo
*
*/
void SAMAPP_Widget_logo()
{
    Draw_Text(s_pHalContext, "Example for: Showing logo");

    Display_Start(s_pHalContext);

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
*/
void SAMAPP_Widget_clock()
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
    EVE_Util_loadRawFile(s_pHalContext, DDR_BITMAPS_STARTADDR, TEST_DIR "\\SAMAPP_Bitmap_RawData.bin");

    /* Draw clock with blue as background and read as needle color */
    Display_Start(s_pHalContext);
    /* flat effect and default color background */
    xOffset = xDistBtwClocks / 2;
    yOffset = radius + 5;
    EVE_CoCmd_bgColor(s_pHalContext, 0x0000ff);
    EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0x00, 0x00);
    EVE_CoCmd_clock(s_pHalContext, xOffset, yOffset, radius, OPT_FLAT, 30, 100, 5, 10);
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + radius + 20), 31, OPT_CENTER, "Flat effect"); //text info

    /* no seconds needle */
    EVE_CoCmd_bgColor(s_pHalContext, 0x00ff00);
    EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0x00, 0x00);
    EVE_CoCmd_fgColor(s_pHalContext, 0xff0000);
    xOffset += xDistBtwClocks;
    EVE_CoCmd_clock(s_pHalContext, xOffset, yOffset, radius, OPT_NOSECS, 10, 10, 5, 10);
    EVE_CoDl_colorA(s_pHalContext, 255);
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + radius + 20), 31, OPT_CENTER, "No Secs"); //text info

    /* no background color */
    EVE_CoCmd_bgColor(s_pHalContext, 0x00ffff);
    EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0xff, 0x00);
    xOffset += xDistBtwClocks;
    EVE_CoCmd_clock(s_pHalContext, xOffset, yOffset, radius, OPT_NOBACK, 10, 10, 5, 10);
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + radius + 20), 31, OPT_CENTER, "No BG"); //text info

    /* No ticks */
    EVE_CoCmd_bgColor(s_pHalContext, 0xff00ff);
    EVE_CoDl_colorRgb(s_pHalContext, 0x00, 0xff, 0xff);
    xOffset += xDistBtwClocks;
    EVE_CoCmd_clock(s_pHalContext, xOffset, yOffset, radius, OPT_NOTICKS, 10, 10, 5, 10);
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + radius + 20), 31, OPT_CENTER, "No Ticks"); //text info

    /* No hands */
    EVE_CoCmd_bgColor(s_pHalContext, 0x808080);
    EVE_CoDl_colorRgb(s_pHalContext, 0x00, 0xff, 0x00);
    xOffset += xDistBtwClocks;
    EVE_CoCmd_clock(s_pHalContext, xOffset, yOffset, radius, OPT_NOHANDS, 10, 10, 5, 10);
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + radius + 20), 31, OPT_CENTER, "No Hands"); //text info

    /* Bigger clock */
    yOffset += (radius + 50);
    radius = (int16_t) (s_pHalContext->Height - (2 * radius + 5 + 20)); //calculate radius based on remaining height
    radius = (radius - 5 - 50) / 2;
    xOffset = radius + 10;
    yOffset += radius + 5;
    EVE_CoDl_colorRgb(s_pHalContext, 0x00, 0x00, 0xff);
    EVE_CoCmd_clock(s_pHalContext, xOffset, yOffset, radius, 0, 10, 10, 5, 10);

    xOffset += 2 * radius + 20;
    /* Lena clock with no background and no ticks */
    EVE_CoDl_lineWidth(s_pHalContext, 10 * 16);
    EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0xff, 0xff);
    EVE_CoDl_begin(s_pHalContext, RECTS);
    EVE_CoDl_vertex2f_4(s_pHalContext, (xOffset - radius + 10) * 16, (yOffset - radius + 10) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (xOffset + radius - 10) * 16, (yOffset + radius - 10) * 16);
    EVE_CoDl_end(s_pHalContext);
    EVE_CoDl_colorA(s_pHalContext, 0xff);
    EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0xff, 0xff);
    EVE_CoDl_colorMask(s_pHalContext, 0, 0, 0, 1);
    EVE_CoDl_clear(s_pHalContext, 1, 1, 1);
    EVE_CoDl_begin(s_pHalContext, RECTS);
    EVE_CoDl_vertex2f_4(s_pHalContext, (xOffset - radius + 12) * 16, (yOffset - radius + 12) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (xOffset + radius - 12) * 16, (yOffset + radius - 12) * 16);
    EVE_CoDl_end(s_pHalContext);
    EVE_CoDl_colorMask(s_pHalContext, 1, 1, 1, 1);
    EVE_CoDl_blendFunc(s_pHalContext, DST_ALPHA, ONE_MINUS_DST_ALPHA);
    /* Lena bitmap - scale proportionately wrt output resolution */
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_scale(s_pHalContext, 65536 * 2 * radius / SAMAPP_Bitmap_RawData_Header.Width,
        65536 * 2 * radius / SAMAPP_Bitmap_RawData_Header.Height);
    EVE_CoCmd_setMatrix(s_pHalContext);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_bitmapSource(s_pHalContext, DDR_BITMAPS_STARTADDR);
    EVE_CoDl_bitmapLayout(s_pHalContext, SAMAPP_Bitmap_RawData_Header.Format,
            SAMAPP_Bitmap_RawData_Header.Stride, SAMAPP_Bitmap_RawData_Header.Height);
    EVE_CoDl_bitmapSize(s_pHalContext, BILINEAR, BORDER, BORDER, 2 * radius, 2 * radius);
    EVE_CoDl_vertex2f_4(s_pHalContext, (xOffset - radius) * 16, (yOffset - radius) * 16);
    EVE_CoDl_end(s_pHalContext);
    EVE_CoDl_blendFunc_default(s_pHalContext);
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_setMatrix(s_pHalContext);
    EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0xff, 0xff);
    EVE_CoCmd_clock(s_pHalContext, xOffset, yOffset, radius, OPT_NOTICKS | OPT_NOBACK, 10, 10, 5,
        10);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief demonstrate a gauge widget
*/
void SAMAPP_Widget_gauge()
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
    yOffset = cRadius + 5;

    /* Download the bitmap data */
    EVE_Util_loadRawFile(s_pHalContext, RAM_G, TEST_DIR "\\SAMAPP_Bitmap_RawData.bin");

    /* Draw gauge with blue as background and read as needle color */
    Display_Start(s_pHalContext);

    /* flat effect and default color background */
    xOffset = xDistBtwClocks / 2;
    EVE_CoCmd_bgColor(s_pHalContext, 0x0000ff);
    EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0x00, 0x00);
    EVE_CoCmd_gauge(s_pHalContext, xOffset, yOffset, cRadius, OPT_FLAT, 5, 4, 45, 100);
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + cRadius + 20), 31, OPT_CENTER, "Flat effect"); //text info

    /* 3d effect */
    EVE_CoCmd_bgColor(s_pHalContext, 0x00ff00);
    EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0x00, 0x00);
    EVE_CoCmd_fgColor(s_pHalContext, 0xff0000);
    xOffset += xDistBtwClocks;
    EVE_CoCmd_gauge(s_pHalContext, xOffset, yOffset, cRadius, 0, 5, 1, 60, 100);
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + cRadius + 20), 31, OPT_CENTER, "3d effect"); //text info

    /* no background color */
    EVE_CoCmd_bgColor(s_pHalContext, 0x00ffff);
    EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0xff, 0x00);
    xOffset += xDistBtwClocks;
    EVE_CoCmd_gauge(s_pHalContext, xOffset, yOffset, cRadius, OPT_NOBACK, 1, 6, 90, 100);
    EVE_CoDl_colorA(s_pHalContext, 255);
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + cRadius + 20), 31, OPT_CENTER, "No BG"); //text info

    /* No ticks */
    EVE_CoCmd_bgColor(s_pHalContext, 0xff00ff);
    EVE_CoDl_colorRgb(s_pHalContext, 0x00, 0xff, 0xff);
    xOffset += xDistBtwClocks;
    EVE_CoCmd_gauge(s_pHalContext, xOffset, yOffset, cRadius, OPT_NOTICKS, 5, 4, 20, 100);
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + cRadius + 20), 31, OPT_CENTER, "No Ticks"); //text info

    /* No hands */
    EVE_CoCmd_bgColor(s_pHalContext, 0x808080);
    EVE_CoDl_colorRgb(s_pHalContext, 0x00, 0xff, 0x00);
    xOffset += xDistBtwClocks;
    EVE_CoCmd_gauge(s_pHalContext, xOffset, yOffset, cRadius, OPT_NOPOINTER, 5, 4, 55, 100);
    EVE_CoDl_colorA(s_pHalContext, 255);
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + cRadius + 20), 31, OPT_CENTER, "No Hands"); //text info

    /* Bigger gauge */
    yOffset += cRadius + 50;
    cRadius = (int16_t) (s_pHalContext->Height - (2 * cRadius + 5 + 20)); //calculate radius based on remaining height
    cRadius = (cRadius - 5 - 50) / 2;
    xOffset = cRadius + 10;
    yOffset += cRadius + 5;
    EVE_CoCmd_bgColor(s_pHalContext, 0x808000);
    EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0xff, 0xff);
    EVE_CoCmd_gauge(s_pHalContext, xOffset, yOffset, cRadius, OPT_NOBACK, 5, 4, 80, 100);
    EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0x00, 0x00);
    EVE_CoCmd_gauge(s_pHalContext, xOffset, yOffset, cRadius, OPT_NOTICKS | OPT_NOBACK, 5, 4, 30, 100);

    xOffset += 2 * cRadius + 20;
    /* Lena gauge with no background and no ticks */
    EVE_CoDl_pointSize(s_pHalContext, cRadius * 16);
    EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0xff, 0xff);
    EVE_CoDl_begin(s_pHalContext, POINTS);
    EVE_CoDl_vertex2f_4(s_pHalContext, xOffset * 16, yOffset * 16);
    EVE_CoDl_end(s_pHalContext);
    EVE_CoDl_colorA(s_pHalContext, 0xff);
    EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0xff, 0xff);
    EVE_CoDl_colorMask(s_pHalContext, 0, 0, 0, 1);
    EVE_CoDl_clear(s_pHalContext, 1, 1, 1);
    EVE_CoDl_begin(s_pHalContext, POINTS);
    EVE_CoDl_pointSize(s_pHalContext, (cRadius - 2) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, xOffset * 16, yOffset * 16);
    EVE_CoDl_end(s_pHalContext);
    EVE_CoDl_colorMask(s_pHalContext, 1, 1, 1, 1);
    EVE_CoDl_blendFunc(s_pHalContext, DST_ALPHA, ONE_MINUS_DST_ALPHA);
    /* Lena bitmap - scale proportionately wrt output resolution */
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_scale(s_pHalContext, 65536 * 2 * cRadius / SAMAPP_Bitmap_RawData_Header.Width,
        65536 * 2 * cRadius / SAMAPP_Bitmap_RawData_Header.Height);
    EVE_CoCmd_setMatrix(s_pHalContext);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_bitmapSource(s_pHalContext, 0);
    EVE_CoDl_bitmapLayout(s_pHalContext,
        SAMAPP_Bitmap_RawData_Header.Format, SAMAPP_Bitmap_RawData_Header.Stride, SAMAPP_Bitmap_RawData_Header.Height);
    EVE_CoDl_bitmapSize(s_pHalContext, BILINEAR, BORDER, BORDER, 2 * cRadius, 2 * cRadius);
    EVE_CoDl_vertex2f_4(s_pHalContext, (xOffset - cRadius) * 16, (yOffset - cRadius) * 16);
    EVE_CoDl_end(s_pHalContext);
    EVE_CoDl_blendFunc_default(s_pHalContext);
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_setMatrix(s_pHalContext);

    EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0xff, 0xff);
    EVE_CoCmd_gauge(s_pHalContext, xOffset, yOffset, cRadius, OPT_NOTICKS | OPT_NOBACK, 5, 4, 30,
        100);

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief demonstrate gradient widget
*
*/
void SAMAPP_Widget_gradient()
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

    xOffset = 20;
    yOffset = 40;
    hScissor = (int16_t)((s_pHalContext->Height - 3 * yOffset) / 2);
    wScissor = (int16_t)((s_pHalContext->Width - 4 * xOffset) / 3);

    /* Draw gradient  */
    Display_StartColor(s_pHalContext, (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 });
    EVE_CoDl_scissorSize(s_pHalContext, wScissor, hScissor);
    /* Horizontal gradient effect */
    EVE_CoDl_scissorXY(s_pHalContext, xOffset, yOffset); //clip the display
    EVE_CoCmd_gradient(s_pHalContext, xOffset, yOffset, 0x808080, (xOffset + wScissor), yOffset, 0xffff00);

    /* Vertical gradient effect */
    xOffset += wScissor + 20;
    EVE_CoDl_scissorXY(s_pHalContext, xOffset, yOffset); //clip the display
    EVE_CoCmd_gradient(s_pHalContext, xOffset, yOffset, 0xff0000, xOffset, (yOffset + hScissor), 0x00ff00);

    /* diagonal gradient effect */
    xOffset += wScissor + 20;
    EVE_CoDl_scissorXY(s_pHalContext, xOffset, yOffset); //clip the display
    EVE_CoCmd_gradient(s_pHalContext, xOffset, yOffset, 0x800000, (xOffset + wScissor), (yOffset + hScissor), 0xffffff);

    /* Diagonal gradient with text info */
    xOffset = 20;
    yOffset += hScissor + 40;
    EVE_CoDl_scissorSize(s_pHalContext, wScissor, 50);
    EVE_CoDl_scissorXY(s_pHalContext, xOffset, (yOffset + hScissor / 2 - 25)); //clip the display
    EVE_CoCmd_gradient(s_pHalContext, xOffset, (yOffset + hScissor / 2 - 25), 0x606060,
        (xOffset + wScissor), (yOffset + hScissor / 2 + 25), 0x404080);
    EVE_CoCmd_text(s_pHalContext, (xOffset + wScissor / 2), (yOffset + hScissor / 2), 31,
        OPT_CENTER, "Gradient Gradient Gradient"); //text info

    /* Draw horizontal, vertical and diagonal with alpha */
    xOffset += wScissor + 40;
    EVE_CoDl_scissorSize(s_pHalContext, wScissor, hScissor);
    EVE_CoDl_scissorXY(s_pHalContext, xOffset, yOffset); //clip the display
    EVE_CoCmd_gradient(s_pHalContext, xOffset, yOffset, 0x808080, (xOffset + wScissor), yOffset, 0xffff00);
    wScissor -= 100;
    hScissor -= 100;
    EVE_CoDl_scissorSize(s_pHalContext, wScissor, hScissor);
    xOffset += 50;
    yOffset += 50;
    EVE_CoDl_scissorXY(s_pHalContext, xOffset, yOffset); //clip the display
    EVE_CoCmd_gradient(s_pHalContext, xOffset, yOffset, 0x800000, xOffset, (yOffset + hScissor),
        0xffffff);
    wScissor -= 100;
    hScissor -= 100;
    EVE_CoDl_scissorSize(s_pHalContext, wScissor, hScissor);
    xOffset += 50;
    yOffset += 50;
    EVE_CoDl_scissorXY(s_pHalContext, xOffset, yOffset); //clip the display
    EVE_CoCmd_gradient(s_pHalContext, xOffset, yOffset, 0x606060, (xOffset + wScissor),
        (yOffset + hScissor), 0x404080);

    /* Display the text wrt gradient */
    wScissor = (int16_t) ((s_pHalContext->Width - 4 * 20) / 3);
    hScissor = (int16_t) ((s_pHalContext->Height - 3 * 40) / 2);
    xOffset = 20 + wScissor / 2;
    yOffset = 40 + hScissor + 20;
    EVE_CoDl_scissorXY(s_pHalContext, 0, 0); //set to default values
    EVE_CoDl_scissorSize(s_pHalContext, 2048, 2048);
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset, 31, OPT_CENTER, "Horizontal grad"); //text info
    xOffset += wScissor + 10;
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset, 31, OPT_CENTER, "Vertical grad"); //text info
    xOffset += wScissor + 10;
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset, 31, OPT_CENTER, "Diagonal grad"); //text info

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief API to demonstrate CMD_GRADIENTA with transparency
*
*/
void SAMAPP_Widget_gradientA()
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

    xOffset = 20;
    yOffset = 40;
    hScissor = (int16_t)((s_pHalContext->Height - 3 * yOffset) / 2);
    wScissor = (int16_t)((s_pHalContext->Width - 4 * xOffset) / 3);
    /* Draw gradient  */

    Display_StartColor(s_pHalContext, (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 });
    EVE_CoDl_scissorSize(s_pHalContext, wScissor, hScissor);
    /* Horizontal gradient effect */
    EVE_CoDl_scissorXY(s_pHalContext, xOffset, yOffset); //clip the display
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset + hScissor / 4, 31, 0, "GradientA: Horizontal gradient"); //text info
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset + hScissor * 3 / 4, 31, 0, "GradientA: Horizontal gradient"); //text info
    EVE_CoCmd_gradientA(s_pHalContext, xOffset, yOffset, 0x80808080, (xOffset + wScissor), yOffset,
        0xffffff00);
    /* Vertical gradient effect */
    xOffset += wScissor + 20;
    EVE_CoDl_scissorXY(s_pHalContext, xOffset, yOffset); //clip the display
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset + hScissor / 4, 31, 0, "GradientA: Vertical gradient"); //text info
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset + hScissor * 3 / 4, 31, 0, "GradientA: Vertical gradient"); //text info
    EVE_CoCmd_gradientA(s_pHalContext, xOffset, yOffset, 0x80ff0000, xOffset, (yOffset + hScissor),
        0xff00ff00);
    /* diagonal gradient effect */
    xOffset += wScissor + 20;
    EVE_CoDl_scissorXY(s_pHalContext, xOffset, yOffset); //clip the display
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset + hScissor / 4, 31, 0, "GradientA: Diagonal gradient"); //text info
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset + hScissor * 3 / 4, 31, 0, "GradientA: Diagonal gradient"); //text info
    EVE_CoCmd_gradientA(s_pHalContext, xOffset, yOffset, 0x80800000, (xOffset + wScissor),
        (yOffset + hScissor), 0xffffffff);
    /* Diagonal gradient with text info */
    xOffset = 20;
    yOffset += hScissor + 40;
    EVE_CoDl_scissorSize(s_pHalContext, wScissor, 50);
    EVE_CoDl_scissorXY(s_pHalContext, xOffset, (yOffset + hScissor / 2 - 25)); //clip the display
    EVE_CoCmd_text(s_pHalContext, (xOffset + wScissor / 2), (yOffset + hScissor / 2), 31,
        OPT_CENTER, "GradientA GradientA GradientA"); //text info
    EVE_CoCmd_gradientA(s_pHalContext, xOffset, (yOffset + hScissor / 2 - 25), 0x00606060,
        (xOffset + wScissor), (yOffset + hScissor / 2 + 15), 0xff404080);

    /* Draw horizontal, vertical and diagonal with alpha */
    xOffset += wScissor + 40;
    EVE_CoDl_scissorSize(s_pHalContext, wScissor, hScissor);
    EVE_CoDl_scissorXY(s_pHalContext, xOffset, yOffset); //clip the display
    EVE_CoCmd_gradientA(s_pHalContext, xOffset, yOffset, 0x80808080, (xOffset + wScissor), yOffset,
        0xffffff00);
    wScissor -= 100;
    hScissor -= 100;
    EVE_CoDl_scissorSize(s_pHalContext, wScissor, hScissor);
    xOffset += 50;
    yOffset += 50;
    EVE_CoDl_scissorXY(s_pHalContext, xOffset, yOffset); //clip the display
    EVE_CoCmd_gradientA(s_pHalContext, xOffset, yOffset, 0x80800000, xOffset, (yOffset + hScissor),
        0xffffffff);
    wScissor -= 100;
    hScissor -= 100;
    EVE_CoDl_scissorSize(s_pHalContext, wScissor, hScissor);
    xOffset += 50;
    yOffset += 50;
    EVE_CoDl_scissorXY(s_pHalContext, xOffset, yOffset); //clip the display
    EVE_CoCmd_gradientA(s_pHalContext, xOffset, yOffset, 0x80606060, (xOffset + wScissor),
        (yOffset + hScissor), 0xFF404080);

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief API to demonstrate CMD_CGRADIENT
*
*/
void SAMAPP_Widget_cgradient()
{
    Draw_Text(s_pHalContext, "Example for: circular gradient widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of gradient function. The cgradient */
    /* command draws a rectangle containing a circular gradient, smoothly    */
    /* blending between one color in the center and another at the corner or */
    /* edge.   */
    /*************************************************************************/
    Display_StartColor(s_pHalContext, (uint8_t[]) { 0, 0, 255 }, (uint8_t[]) { 255, 255, 255 });
    EVE_CoCmd_cgradient(s_pHalContext, CORNER_ZERO, 100, 100, 500, 500, 0xff0000, 0x0000ff);
    EVE_CoCmd_text(s_pHalContext, 100, 100 + 500 + 50, 31, 0, "CORNER_ZERO");
    EVE_CoCmd_cgradient(s_pHalContext, EDGE_ZERO, 700, 100, 500, 500, 0xff0000, 0x0000ff);
    EVE_CoCmd_text(s_pHalContext, 700, 100 + 500 + 50, 31, 0, "EDGE_ZERO");

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief Key interactive
*
*/
void SAMAPP_Widget_keysInteractive()
{
    Draw_Text(s_pHalContext, "Example for: Key interactive");

    /*************************************************************************/
    /* Below code demonstrates the usage of keys function. keys function     */
    /* draws buttons with characters given as input parameters. Keys support */
    /* Flat and 3D effects, draw at (x,y) coordinates or center of the display*/
    /* , inbuilt or custom fonts can be used for key display                 */
    /*************************************************************************/
    int32_t loopflag = 600;
    int16_t TextFont = 33;
    int16_t ButtonW = 80;
    int16_t ButtonH = 80;
    int16_t yBtnDst = 5;
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
            /* clear all the charaters after 100 are pressed */
            if (CurrTextIdx > 24)
            {
                CurrTextIdx = 0;
            }
        }

        Display_StartColor(s_pHalContext, (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 });
        /* Draw text entered by user */
        /* make sure the array is a string */
        DispText[CurrTextIdx] = CurrChar;
        DispText[CurrTextIdx + 1] = '\0';

        EVE_CoDl_tagMask(s_pHalContext, 0);
        EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 40, TextFont,
            OPT_CENTER, DispText); //text info
        EVE_CoDl_tagMask(s_pHalContext, 1);

        yOffset = ButtonW * 2 + 10;
        /* Construct a simple keyboard - note that the tags associated with the keys are the character values given in the arguments */
        EVE_CoCmd_fgColor(s_pHalContext, 0x404080);
        EVE_CoCmd_gradColor(s_pHalContext, 0x00ff00);
        EVE_CoCmd_keys(s_pHalContext, yBtnDst, yOffset, 10 * ButtonW, ButtonH, TextFont,
            (OPT_CENTER | (uint16_t)CurrTag), "qwertyuiop");
        EVE_CoCmd_gradColor(s_pHalContext, 0x00ffff);
        yOffset += ButtonH + yBtnDst;
        EVE_CoCmd_keys(s_pHalContext, yBtnDst, yOffset, 10 * ButtonW, ButtonH, TextFont,
            (OPT_CENTER | (uint16_t)CurrTag), "asdfghjkl");
        EVE_CoCmd_gradColor(s_pHalContext, 0xffff00);
        yOffset += ButtonH + yBtnDst;
        EVE_CoCmd_keys(s_pHalContext, yBtnDst, yOffset, 10 * ButtonW, ButtonH, TextFont,
            (OPT_CENTER | (uint16_t)CurrTag), "zxcvbnm"); //hilight button z
        yOffset += ButtonH + yBtnDst;
        EVE_CoDl_tag(s_pHalContext, ' ');
        if (' ' == CurrTag)
        {
            EVE_CoCmd_button(s_pHalContext, yBtnDst, yOffset, 10 * ButtonW, ButtonH, TextFont,
                OPT_FLAT, " "); //mandatory to give '\0' at the end to make sure coprocessor understands the string end
        }
        else
        {
            EVE_CoCmd_button(s_pHalContext, yBtnDst, yOffset, 10 * ButtonW, ButtonH, TextFont, 0,
                " "); //mandatory to give '\0' at the end to make sure coprocessor understands the string end
        }
        yOffset = ButtonW * 2 + 10;
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
        EVE_sleep(10);
        PrevTag = CurrTag;
    }
}

/**
* @brief  keys widget
*
*/
void SAMAPP_Widget_keys()
{
    Draw_Text(s_pHalContext, "Example for: Keys widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of keys function. keys function     */
    /* draws buttons with characters given as input parameters. Keys support */
    /* Flat and 3D effects, draw at (x,y) coordinates or center of the display*/
    /* , inbuilt or custom fonts can be used for key display                 */
    /*************************************************************************/
    int16_t TextFont = 33;
    int16_t ButtonW = 80;
    int16_t ButtonH = 80;
    int16_t yBtnDst = 5;
    int16_t yOffset;
    int16_t xOffset;

    Display_StartColor(s_pHalContext, (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 });
    EVE_CoDl_lineWidth(s_pHalContext, 1 * 16);
    /* Draw keys with flat effect */
    yOffset = ButtonW + 10 + yBtnDst;
    EVE_CoCmd_fgColor(s_pHalContext, 0x404080);
    EVE_CoCmd_keys(s_pHalContext, 10, 10, 4 * ButtonW, ButtonH, TextFont, OPT_FLAT, "ABCD");
    EVE_CoCmd_text(s_pHalContext, 10, yOffset, 29, 0, "Flat effect"); //text info
    /* Draw keys with 3d effect */
    EVE_CoCmd_fgColor(s_pHalContext, 0x800000);
    xOffset = 4 * ButtonW + 20;
    EVE_CoCmd_keys(s_pHalContext, xOffset, 10, 4 * ButtonW, ButtonH, TextFont, 0, "ABCD");
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset, 29, 0, "3D effect"); //text info
    /* Draw keys with center option */
    EVE_CoCmd_fgColor(s_pHalContext, 0xffff000);
    xOffset += 4 * ButtonW + 20;
    EVE_CoCmd_keys(s_pHalContext, xOffset, 10, 4 * ButtonW, ButtonH, TextFont,
        OPT_CENTER, "ABCD");
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset, 29, 0, "Option Center"); //text info

    yOffset = ButtonW * 2 + 10;
    /* Construct a simple keyboard - note that the tags associated with the keys are the character values given in the arguments */
    EVE_CoCmd_fgColor(s_pHalContext, 0x404080);
    EVE_CoCmd_gradColor(s_pHalContext, 0x00ff00);
    EVE_CoCmd_keys(s_pHalContext, yBtnDst, yOffset, 10 * ButtonW, ButtonH, TextFont, OPT_CENTER,
        "qwertyuiop");
    EVE_CoCmd_gradColor(s_pHalContext, 0x00ffff);
    yOffset += ButtonH + yBtnDst;
    EVE_CoCmd_keys(s_pHalContext, yBtnDst, yOffset, 10 * ButtonW, ButtonH, TextFont, OPT_CENTER,
        "asdfghjkl");
    EVE_CoCmd_gradColor(s_pHalContext, 0xffff00);
    yOffset += ButtonH + yBtnDst;
    EVE_CoCmd_keys(s_pHalContext, yBtnDst, yOffset, 10 * ButtonW, ButtonH, TextFont,
        (OPT_CENTER | 'z'), "zxcvbnm"); //hilight button z
    yOffset += ButtonH + yBtnDst;
    EVE_CoCmd_button(s_pHalContext, yBtnDst, yOffset, 10 * ButtonW, ButtonH, TextFont, 0, " "); //mandatory to give '\0' at the end to make sure coprocessor understands the string end
    yOffset = ButtonW * 2 + 10;
    EVE_CoCmd_keys(s_pHalContext, 11 * ButtonW, yOffset, 3 * ButtonW, ButtonH, TextFont, 0, "789");
    yOffset += ButtonH + yBtnDst;
    EVE_CoCmd_keys(s_pHalContext, 11 * ButtonW, yOffset, 3 * ButtonW, ButtonH, TextFont, 0, "456");
    yOffset += ButtonH + yBtnDst;
    EVE_CoCmd_keys(s_pHalContext, 11 * ButtonW, yOffset, 3 * ButtonW, ButtonH, TextFont, 0, "123");
    yOffset += ButtonH + yBtnDst;
    EVE_CoDl_colorA(s_pHalContext, 255);
    EVE_CoCmd_keys(s_pHalContext, 11 * ButtonW, yOffset, 3 * ButtonW, ButtonH, TextFont, (0 | '0'),
        "0."); //hilight button 0
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief demonstrate progress bar widget
*
*/
void SAMAPP_Widget_progressbar()
{
    Draw_Text(s_pHalContext, "Example for: Progress bar widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of progress function. Progress func */
    /* draws process bar with fgcolor for the % completion and bgcolor for   */
    /* % remaining. Progress bar supports flat and 3d effets                 */
    /*************************************************************************/
    int16_t xOffset;
    int16_t yOffset;
    int16_t yDist = (int16_t) (s_pHalContext->Width / 12);
    int16_t ySz = (int16_t) (s_pHalContext->Width / 24);

    Display_StartColor(s_pHalContext, (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 });
    xOffset = 120;
    yOffset = 300;
    /* Draw progress bar with flat effect */
    EVE_CoCmd_bgColor(s_pHalContext, 0x404080);
    EVE_CoCmd_progress(s_pHalContext, xOffset, 30, 200, 40, OPT_FLAT, 50, 100); //note that h/2 will be added on both sides of the progress bar
    EVE_CoDl_colorA(s_pHalContext, 255);
    EVE_CoCmd_text(s_pHalContext, xOffset, 80, 31, 0, "Flat effect"); //text info

    /* Draw progress bar with 3d effect */
    EVE_CoDl_colorRgb(s_pHalContext, 0x00, 0xff, 0x00);
    EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
    EVE_CoCmd_progress(s_pHalContext, xOffset + 300, 30, 200, 40, 0, 75, 100);
    EVE_CoCmd_text(s_pHalContext, xOffset + 300, 80, 31, 0, "3D effect"); //text info

    /* Draw progress bar with 3d effect and string on top */
    EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0x00, 0x00);
    EVE_CoCmd_bgColor(s_pHalContext, 0x000080);
    EVE_CoCmd_progress(s_pHalContext, xOffset, 150, 200, 40, 0, 19660, 65535);
    EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0xff, 0xff);
    EVE_CoCmd_text(s_pHalContext, xOffset+50, 150, 31, 0, "30 %"); //text info

    EVE_CoDl_colorRgb(s_pHalContext, 0x00, 0xa0, 0x00);
    EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
    EVE_CoCmd_progress(s_pHalContext, xOffset, yOffset, 150, ySz, 0, 10, 100);

    EVE_CoCmd_bgColor(s_pHalContext, 0x000080);
    yOffset += yDist;
    EVE_CoCmd_progress(s_pHalContext, xOffset, yOffset, 150, ySz, 0, 40, 100);

    EVE_CoCmd_bgColor(s_pHalContext, 0xffff00);
    yOffset += yDist;
    EVE_CoCmd_progress(s_pHalContext, xOffset, yOffset, 150, ySz, 0, 70, 100);

    EVE_CoCmd_bgColor(s_pHalContext, 0x808080);
    yOffset += yDist;
    EVE_CoCmd_progress(s_pHalContext, xOffset, yOffset, 150, ySz, 0, 90, 100);

    EVE_CoCmd_text(s_pHalContext, xOffset + 380, 200, 31, 0, "40 % TopBottom"); //text info
    EVE_CoCmd_progress(s_pHalContext, xOffset + 280, 200, ySz, 150, 0, 40, 100);

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief demonstrate scroll widget
*
*/
void SAMAPP_Widget_scroll()
{
    Draw_Text(s_pHalContext, "Example for: scroll widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of scroll function. Scroll function */
    /* draws scroll bar with fgcolor for inner color and current location and*/
    /* can be given by val parameter */
    /*************************************************************************/
    int16_t xOffset;
    int16_t yOffset;
    int16_t xDist = (int16_t) (s_pHalContext->Width / 12);
    int16_t yDist = (int16_t) (s_pHalContext->Width / 12);
    int16_t wSz;

    Display_StartColor(s_pHalContext, (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 });
    /* Draw scroll bar with flat effect */
    EVE_CoCmd_fgColor(s_pHalContext, 0xffff00);
    EVE_CoCmd_bgColor(s_pHalContext, 0x404080);
    EVE_CoCmd_scrollbar(s_pHalContext, 20, 30, 200, 20, OPT_FLAT, 20, 30, 100); //note that h/2 size will be added on both sides of the progress bar
    EVE_CoDl_colorA(s_pHalContext, 255);
    EVE_CoCmd_text(s_pHalContext, 20, 60, 31, 0, "Flat effect"); //text info

    /* Draw scroll bar with 3d effect */
    EVE_CoCmd_fgColor(s_pHalContext, 0x00ff00);
    EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
    EVE_CoCmd_scrollbar(s_pHalContext, 280, 30, 200, 20, 0, 20, 30, 100);
    EVE_CoCmd_text(s_pHalContext, 280, 60, 31, 0, "3D effect"); //text info

    xOffset = 20;
    yOffset = 220;
    wSz = (int16_t) ((s_pHalContext->Width / 2) - 40);
    /* Draw horizontal scroll bars */
    EVE_CoCmd_fgColor(s_pHalContext, 0x00a000);
    EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
    EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, wSz, 18, 0, 10, 30, 100);

    EVE_CoCmd_bgColor(s_pHalContext, 0x000080);
    yOffset += yDist;
    EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, wSz, 18, 0, 30, 30, 100);

    EVE_CoCmd_bgColor(s_pHalContext, 0xffff00);
    yOffset += yDist;
    EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, wSz, 18, 0, 50, 30, 100);

    EVE_CoCmd_bgColor(s_pHalContext, 0x808080);
    yOffset += yDist;
    EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, wSz, 18, 0, 70, 30, 100);

    xOffset = (int16_t) (s_pHalContext->Width / 2 + 40);
    yOffset = 80;
    wSz = (int16_t) (s_pHalContext->Height - 100);
    /* draw vertical scroll bars */
    EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
    EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, 18, wSz, 0, 10, 30, 100);

    EVE_CoCmd_bgColor(s_pHalContext, 0x000080);
    xOffset += xDist;
    EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, 18, wSz, 0, 30, 30, 100);

    EVE_CoCmd_bgColor(s_pHalContext, 0xffff00);
    xOffset += xDist;
    EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, 18, wSz, 0, 50, 30, 100);

    EVE_CoCmd_bgColor(s_pHalContext, 0x808080);
    xOffset += xDist;
    EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, 18, wSz, 0, 70, 30, 100);

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief demonstrate slider widget
*
*/
void SAMAPP_Widget_slider()
{
    Draw_Text(s_pHalContext, "Example for: Slider widget");
    
    /*************************************************************************/
    /* Below code demonstrates the usage of slider function. Slider function */
    /* draws slider bar with fgcolor for inner color and bgcolor for the knob*/
    /* , contains input parameter for position of the knob                   */
    /*************************************************************************/
    int16_t xOffset;
    int16_t yOffset;
    int16_t xDist = (int16_t) (s_pHalContext->Width / 12);
    int16_t yDist = (int16_t) (s_pHalContext->Width / 12);
    int16_t wSz;

    Display_StartColor(s_pHalContext, (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 });
    /* Draw scroll bar with flat effect */
    EVE_CoCmd_fgColor(s_pHalContext, 0xffff00);
    EVE_CoCmd_bgColor(s_pHalContext, 0x000080);
    EVE_CoCmd_slider(s_pHalContext, 20, 30, 200, 20, OPT_FLAT, 30, 100); //note that h/2 size will be added on both sides of the progress bar
    EVE_CoDl_colorA(s_pHalContext, 255);
    EVE_CoCmd_text(s_pHalContext, 20, 60, 31, 0, "Flat effect"); //text info

    /* Draw scroll bar with 3d effect */
    EVE_CoCmd_fgColor(s_pHalContext, 0x00ff00);
    EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
    EVE_CoCmd_slider(s_pHalContext, 280, 30, 200, 20, 0, 50, 100);
    EVE_CoCmd_text(s_pHalContext, 280, 60, 31, 0, "3D effect"); //text info

    xOffset = 20;
    yOffset = 220;
    wSz = (int16_t) ((s_pHalContext->Width / 2) - 40);
    /* Draw horizontal slider bars */
    EVE_CoCmd_fgColor(s_pHalContext, 0x00a000);
    EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
    EVE_CoDl_colorRgb(s_pHalContext, 41, 1, 5);
    EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, wSz, 20, 0, 10, 100);

    EVE_CoCmd_bgColor(s_pHalContext, 0x000080);
    EVE_CoDl_colorRgb(s_pHalContext, 11, 7, 65);
    yOffset += yDist;
    EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, wSz, 20, 0, 30, 100);

    EVE_CoCmd_bgColor(s_pHalContext, 0xffff00);
    EVE_CoDl_colorRgb(s_pHalContext, 87, 94, 9);
    yOffset += yDist;
    EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, wSz, 20, 0, 50, 100);

    EVE_CoCmd_bgColor(s_pHalContext, 0x808080);
    EVE_CoDl_colorRgb(s_pHalContext, 51, 50, 52);
    yOffset += yDist;
    EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, wSz, 20, 0, 70, 100);

    xOffset = (int16_t) (s_pHalContext->Width / 2) + 40;
    yOffset = 80;
    wSz = (int16_t) (s_pHalContext->Height - 100);
    /* draw vertical slider bars */
    EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
    EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, 20, wSz, 0, 10, 100);

    EVE_CoCmd_bgColor(s_pHalContext, 0x000080);
    xOffset += xDist;
    EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, 20, wSz, 0, 30, 100);

    EVE_CoCmd_bgColor(s_pHalContext, 0xffff00);
    xOffset += xDist;
    EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, 20, wSz, 0, 50, 100);

    EVE_CoCmd_bgColor(s_pHalContext, 0x808080);
    xOffset += xDist;
    EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, 20, wSz, 0, 70, 100);

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief demonstrate dial widget
*
*/
void SAMAPP_Widget_dial()
{
    Draw_Text(s_pHalContext, "Example for: Dial widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of dial function. Dial function     */
    /* draws rounded bar with fgcolor for knob color and colorrgb for pointer*/
    /* , contains input parameter for angle of the pointer                   */
    /*************************************************************************/
    Display_StartColor(s_pHalContext, (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 });
    /* Draw dial with flat effect */
    EVE_CoCmd_fgColor(s_pHalContext, 0xffff00);
    EVE_CoCmd_bgColor(s_pHalContext, 0x000080);
    EVE_CoCmd_dial(s_pHalContext, 150, 150, 100, OPT_FLAT, (uint16_t) (0.2 * 65535)); //20%
    EVE_CoDl_colorA(s_pHalContext, 255);
    EVE_CoCmd_text(s_pHalContext, 70, 250, 31, 0, "Flat effect"); //text info

    /* Draw dial with 3d effect */
    EVE_CoCmd_fgColor(s_pHalContext, 0x00ff00);
    EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
    EVE_CoCmd_dial(s_pHalContext, 400, 150, 100, 0, (uint16_t) (0.45 * 65535)); //45%
    EVE_CoCmd_text(s_pHalContext, 320, 250, 31, 0, "3D effect"); //text info

    /* Draw increasing dials horizontally */
    EVE_CoCmd_fgColor(s_pHalContext, 0x800000);
    EVE_CoDl_colorRgb(s_pHalContext, 41, 1, 5);
    EVE_CoCmd_dial(s_pHalContext, 100, 500, 50, 0, (uint16_t) (0.30 * 65535));
    EVE_CoCmd_text(s_pHalContext, 70, 550, 31, 0, "30 %"); //text info

    EVE_CoCmd_fgColor(s_pHalContext, 0x000080);
    EVE_CoDl_colorRgb(s_pHalContext, 11, 7, 65);
    EVE_CoCmd_dial(s_pHalContext, 300, 500, 100, 0, (uint16_t) (0.45 * 65535));
    EVE_CoCmd_text(s_pHalContext, 270, 600, 31, 0, "45 %"); //text info

    EVE_CoCmd_fgColor(s_pHalContext, 0xffff00);
    EVE_CoDl_colorRgb(s_pHalContext, 87, 94, 9);
    EVE_CoCmd_dial(s_pHalContext, 600, 500, 150, 0, (uint16_t) (0.60 * 65535));
    EVE_CoCmd_text(s_pHalContext, 570, 650, 31, 0, "60 %"); //text info

    EVE_CoCmd_fgColor(s_pHalContext, 0x808080);
    EVE_CoDl_colorRgb(s_pHalContext, 51, 50, 52);
    EVE_CoCmd_dial(s_pHalContext, 1000, 500, 200, 0, (uint16_t) (0.75 * 65535));
    EVE_CoCmd_text(s_pHalContext, 970, 700, 31, 0, "75 %"); //text info
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief demonstrate toggle widget
*
*/
void SAMAPP_Widget_toggle()
{
    Draw_Text(s_pHalContext, "Example for: Toggle widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of toggle function. Toggle function */
    /* draws line with inside knob to choose between left and right. Toggle  */
    /* inside color can be changed by bgcolor and knob color by fgcolor. Left*/
    /* right texts can be written and the color of the text can be changed by*/
    /* colorrgb graphics function                                            */
    /*************************************************************************/
    int16_t xOffset;
    int16_t yOffset;
    int16_t yDist = 100;

    Display_Start(s_pHalContext);
    /* Draw toggle with flat effect */
    EVE_CoCmd_fgColor(s_pHalContext, 0xffff00);
    EVE_CoCmd_bgColor(s_pHalContext, 0x000080);

    EVE_CoCmd_toggle(s_pHalContext, 100, 100, 80, 31, OPT_FLAT, (uint16_t) (0.5 * 65535),
        "no\xffyes"); //circle radius will be extended on both the horizontal sides
    EVE_CoCmd_text(s_pHalContext, 100, 140, 31, 0, "Flat effect"); //text info

    /* Draw toggle bar with 3d effect */
    EVE_CoCmd_fgColor(s_pHalContext, 0x00ff00);
    EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
    EVE_CoCmd_toggle(s_pHalContext, 400, 100, 80, 31, 0, 1 * 65535, "stop\xffrun");
    EVE_CoCmd_text(s_pHalContext, 400, 140, 31, 0, "3D effect"); //text info

    xOffset = 100;
    yOffset = 250;
    /* Draw horizontal toggle bars */
    EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
    EVE_CoCmd_fgColor(s_pHalContext, 0x410105);
    EVE_CoCmd_toggle(s_pHalContext, xOffset, yOffset, 80, 31, 0, 0 * 65535, "-ve\xff+ve");

    EVE_CoCmd_bgColor(s_pHalContext, 0x000080);
    EVE_CoCmd_fgColor(s_pHalContext, 0x0b0721);
    yOffset += yDist;
    EVE_CoCmd_toggle(s_pHalContext, xOffset, yOffset, 80, 31, 0, (uint16_t) (0.25 * 65535), "zero\xffone");

    EVE_CoCmd_bgColor(s_pHalContext, 0xffff00);
    EVE_CoCmd_fgColor(s_pHalContext, 0x575e1b);
    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);
    yOffset += yDist;
    EVE_CoCmd_toggle(s_pHalContext, xOffset, yOffset, 80, 31, 0, (uint16_t) (0.5 * 65535), "exit\xffinit");

    EVE_CoCmd_bgColor(s_pHalContext, 0x808080);
    EVE_CoCmd_fgColor(s_pHalContext, 0x333234);
    EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0xff, 0xff);
    yOffset += yDist;
    EVE_CoCmd_toggle(s_pHalContext, xOffset, yOffset, 80, 31, 0, (uint16_t) (0.75 * 65535), "off\xffon");

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief demonstrate a spinner widget
*
*/
void SAMAPP_Widget_spinner()
{
    Draw_Text(s_pHalContext, "Example for: Spinner widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of spinner function. Spinner func   */
    /* will wait untill stop command is sent from the mcu. Spinner has option*/
    /* for displaying points in circle fashion or in a line fashion.         */
    /*************************************************************************/
    Display_StartColor(s_pHalContext, (uint8_t[]) { 64, 64, 64 }, (uint8_t[]) { 255, 255, 255 });
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 100, 31, OPT_CENTER, "Spinner circle");
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 200, 31, OPT_CENTER, "Please Wait ...");
    EVE_CoCmd_spinner(s_pHalContext, (int16_t) (s_pHalContext->Width / 2),
        (int16_t) (s_pHalContext->Height / 2), 0, 1); //style 0 and scale 0

    /* Wait till coprocessor completes the operation */
    EVE_Cmd_waitFlush(s_pHalContext);

    EVE_sleep(1000);

    /* spinner with style 1 and scale 1 */
    Display_StartColor(s_pHalContext, (uint8_t[]) { 64, 64, 64 }, (uint8_t[]) { 255, 255, 255 });
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 100, 31, OPT_CENTER, "Spinner line");
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 200, 31, OPT_CENTER, "Please Wait ...");
    EVE_CoDl_colorRgb(s_pHalContext, 0x00, 0x00, 0x80);
    EVE_CoCmd_spinner(s_pHalContext, (int16_t) (s_pHalContext->Width / 2),
        (int16_t) (s_pHalContext->Height / 2), 1, 1); //style 1 and scale 1

    /* Wait till coprocessor completes the operation */
    EVE_Cmd_waitFlush(s_pHalContext);
    EVE_sleep(1000);

    Display_StartColor(s_pHalContext, (uint8_t[]) { 64, 64, 64 }, (uint8_t[]) { 255, 255, 255 });
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 100, 31, OPT_CENTER, "Spinner clockhand");
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 200, 31, OPT_CENTER, "Please Wait ...");
    EVE_CoDl_colorRgb(s_pHalContext, 0x80, 0x00, 0x00);
    EVE_CoCmd_spinner(s_pHalContext, (int16_t) (s_pHalContext->Width / 2),
        (int16_t) ((s_pHalContext->Height / 2) + 20), 2, 1); //style 2 scale 1

    /* Wait till coprocessor completes the operation */
    EVE_Cmd_waitFlush(s_pHalContext);
    EVE_sleep(1000);

    Display_StartColor(s_pHalContext, (uint8_t[]) { 64, 64, 64 }, (uint8_t[]) { 255, 255, 255 });
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 100, 31, OPT_CENTER, "Spinner two dots");
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 200, 31, OPT_CENTER, "Please Wait ...");
    EVE_CoDl_colorRgb(s_pHalContext, 0x80, 0x00, 0x00);
    EVE_CoCmd_spinner(s_pHalContext, (int16_t) (s_pHalContext->Width / 2),
        (int16_t) ((s_pHalContext->Height / 2) + 20), 3, 1); //style 3 scale 0

    /* Wait till coprocessor completes the operation */
    EVE_Cmd_waitFlush(s_pHalContext);
    EVE_sleep(1000);

    /* Send the stop command */
    EVE_CoCmd_stop(s_pHalContext);
    /* Update the command buffer pointers - both read and write pointers */
    EVE_Cmd_waitFlush(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief API to demonstrate text widget
*
*/
void SAMAPP_Widget_text()
{
    Draw_Text(s_pHalContext, "Example for: text widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of text function. Text function     */
    /* draws text with either in built or externally configured text. Text   */
    /* color can be changed by colorrgb and text function supports display of*/
    /* texts on left, right, top, bottom and center respectively             */
    /*************************************************************************/
    
    Display_Start(s_pHalContext);
    /* Draw text at 0,0 location */
    EVE_CoDl_colorRgb(s_pHalContext, 0x00, 0x00, 0x80);
    EVE_CoCmd_text(s_pHalContext, 0, 0, 31, 0, "Bridgetek!");
    EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0x00, 0x00);
    EVE_CoCmd_text(s_pHalContext, 0, 40, 29, 0, "Text31 at 0,0"); //text info

    /* text with centerx */
    EVE_CoDl_colorRgb(s_pHalContext, 0x80, 0x00, 0x00);
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 100, 31, OPT_CENTERX, "Bridgetek!");
    EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0x00, 0x00);
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 140, 29, 0, "Text31 centerX"); //text info

    /* text with centery */
    EVE_CoDl_colorRgb(s_pHalContext, 0x41, 0x01, 0x05);
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 5), 200, 31, OPT_CENTERY, "Bridgetek!");
    EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0x00, 0x00);
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 5), 240, 29, 0, "Text31 centerY"); //text info

    /* text with center */
    EVE_CoDl_colorRgb(s_pHalContext, 0x0b, 0x07, 0x21);
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 300, 31, OPT_CENTER, "Bridgetek!");
    EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0x00, 0x00);
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 340, 29, 0, "Text31 center"); //text info

    /* text with rightx */
    EVE_CoDl_colorRgb(s_pHalContext, 0x57, 0x5e, 0x1b);
    EVE_CoCmd_text(s_pHalContext, (int16_t) s_pHalContext->Width, 150, 31, OPT_RIGHTX, "Bridgetek!");
    EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0x00, 0x00);
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width - 150), 190, 29, 0, "Text31 rightX"); //text info
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief API to demonstrate number widget
*
*/
void SAMAPP_Widget_number()
{
    Draw_Text(s_pHalContext, "Example for: number widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of number function. Number function */
    /* draws text with only 32bit decimal number, signed or unsigned can also*/
    /* be specified as input parameter. Options like centerx, centery, center*/
    /* and rightx are supported                                              */
    /*************************************************************************/
    Display_Start(s_pHalContext);
    /* Draw number at 0,0 location */
    EVE_CoDl_colorRgb(s_pHalContext, 0x00, 0x00, 0x80);
    EVE_CoCmd_number(s_pHalContext, 0, 0, 31, 0, 1234);
    EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0x00, 0x00);
    EVE_CoCmd_text(s_pHalContext, 0, 40, 29, 0, "Number31 at 0,0"); //text info

    /* number with centerx */
    EVE_CoDl_colorRgb(s_pHalContext, 0x80, 0x00, 0x00);
    EVE_CoCmd_number(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 100, 31, OPT_CENTERX | OPT_SIGNED, -1234);
    EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0x00, 0x0);
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 140, 29, 0, "Number31 centerX"); //text info

    /* number with centery */
    EVE_CoDl_colorRgb(s_pHalContext, 0x41, 0x01, 0x05);
    EVE_CoCmd_number(s_pHalContext, (int16_t) (s_pHalContext->Width / 5), 200, 31, OPT_CENTERY, 1234);
    EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0x00, 0x00);
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 5), 240, 29, 0, "Number31 centerY"); //text info

    /* number with center */
    EVE_CoDl_colorRgb(s_pHalContext, 0x0b, 0x07, 0x21);
    EVE_CoCmd_number(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 300, 31, OPT_CENTER | OPT_SIGNED, -1234);
    EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0x00, 0x00);
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 340, 29, 0, "Number31 center"); //text info

    /* number with rightx */
    EVE_CoDl_colorRgb(s_pHalContext, 0x57, 0x5e, 0x1b);
    EVE_CoCmd_number(s_pHalContext, (int16_t) s_pHalContext->Width, 150, 31, OPT_RIGHTX, 1234);
    EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0x00, 0x00);
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width - 200), 190, 29, 0, "Number31 rightX"); //text info
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief API to demonstrate button functionality
*
*/
void SAMAPP_Widget_button()
{
    Draw_Text(s_pHalContext, "Example for: button functionality");

    /*************************************************************************/
    /* Below code demonstrates the usage of button function. Buttons can be  */
    /* constructed using flat or 3d effect. Button color can be changed by   */
    /* fgcolor command and text color is set by COLOR_RGB graphics command   */
    /*************************************************************************/
    int16_t xOffset;
    int16_t yOffset;
    int16_t bWidth;
    int16_t bHeight;
    int16_t bDistx;
    int16_t bDisty;

    bWidth = 150;
    bHeight = 60;
    bDistx = bWidth + (int16_t) ((s_pHalContext->Width - 4 * bWidth) / 5);
    bDisty = bHeight + 10;
    xOffset = 50;
    /*  Construct a buttons with "ONE/TWO/THREE" text and default background */
    /* Draw buttons 60x30 resolution at 10x40,10x75,10x110 */
    Display_StartColor(s_pHalContext, (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 });
    /* flat effect and default color background */
    EVE_CoCmd_fgColor(s_pHalContext, 0x0000ff);
    yOffset = (int16_t) (s_pHalContext->Height / 2 - 2 * bDisty);
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 31, OPT_FLAT, "ABC");
    yOffset += bDisty;
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 31, OPT_FLAT, "ABC");
    yOffset += bDisty;
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 31, OPT_FLAT, "ABC");
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + 60), 31, 0, "Flat effect"); //text info

    /* 3D effect */
    xOffset += bDistx;
    yOffset = (int16_t) (s_pHalContext->Height / 2 - 2 * bDisty);
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 31, 0, "ABC");
    yOffset += bDisty;
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 31, 0, "ABC");
    yOffset += bDisty;
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 31, 0, "ABC");
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + 60), 31, 0, "3D Effect"); //text info

    /* 3d effect with background color */
    EVE_CoCmd_fgColor(s_pHalContext, 0xffff00);
    xOffset += bDistx;
    yOffset = (int16_t) (s_pHalContext->Height / 2 - 2 * bDisty);
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 31, 0, "ABC");
    yOffset += bDisty;
    EVE_CoCmd_fgColor(s_pHalContext, 0x00ffff);
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 31, 0, "ABC");
    yOffset += bDisty;
    EVE_CoCmd_fgColor(s_pHalContext, 0xff00ff);
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 31, 0, "ABC");
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + 60), 31, 0, "3D Color"); //text info

    /* 3d effect with gradient color */
    EVE_CoCmd_fgColor(s_pHalContext, 0x101010);
    EVE_CoCmd_gradColor(s_pHalContext, 0xff0000);
    xOffset += bDistx;
    yOffset = (int16_t) (s_pHalContext->Height / 2 - 2 * bDisty);
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 31, 0, "ABC");
    yOffset += bDisty;
    EVE_CoCmd_gradColor(s_pHalContext, 0x00ff00);
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 31, 0, "ABC");
    yOffset += bDisty;
    EVE_CoCmd_gradColor(s_pHalContext, 0x0000ff);
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 31, 0, "ABC");
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + 60), 31, 0, "3D Gradient"); //text info
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief API to demonstrate screen saver widget - playing of bitmap via macro0
*
*/
void SAMAPP_Widget_screensaver()
{
    Draw_Text(s_pHalContext, "Example for: screen saver with an image");

    /*************************************************************************/
    /* Below code demonstrates the usage of screensaver function. Screen     */
    /* saver modifies macro0 with the vertex2f command.                      */
    /* MCU can display any static display list at the background with the    */
    /* changing bitmap                                                       */
    /*************************************************************************/
    /* Download the bitmap data */
    EVE_Util_loadRawFile(s_pHalContext, RAM_G, TEST_DIR "\\SAMAPP_Bitmap_RawData.bin");

    /* Copy the display list */
    Display_StartColor(s_pHalContext, (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 });
    EVE_CoDl_vertexFormat(s_pHalContext, 2);
    /* lena bitmap */
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_scale(s_pHalContext, 3 * 65536, 3 * 65536); //scale the bitmap 3 times on both sides
    EVE_CoCmd_setMatrix(s_pHalContext);
    /* Send command screen saver */
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_bitmapSource(s_pHalContext, 0);
    EVE_CoDl_bitmapLayout(s_pHalContext, SAMAPP_Bitmap_RawData_Header.Format,
            SAMAPP_Bitmap_RawData_Header.Stride, SAMAPP_Bitmap_RawData_Header.Height);
    EVE_CoDl_bitmapSize(s_pHalContext, BILINEAR, BORDER, BORDER, SAMAPP_Bitmap_RawData_Header.Width * 3,
            SAMAPP_Bitmap_RawData_Header.Height * 3);
    EVE_CoDl_macro(s_pHalContext, 0);
    EVE_CoDl_end(s_pHalContext);
    /* Display the text */
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_setMatrix(s_pHalContext);
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), (int16_t) (s_pHalContext->Height / 2), 31, OPT_CENTER, "Screen Saver ...");
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
*/
void SAMAPP_Widget_sketch()
{
    Draw_Text(s_pHalContext, "Example for: sketch widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of sketch function. Sketch function */
    /* draws line for pen movement. Skecth supports L1 and L8 output formats */
    /* Sketch draws till stop command is executed.                           */
    /*************************************************************************/
    int16_t BorderSz = 40;
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
        Display_Start(s_pHalContext);
        EVE_CoDl_scissorSize(s_pHalContext,
            (s_pHalContext->Width - 2 * BorderSz), (s_pHalContext->Height - 2 * BorderSz));
        EVE_CoDl_scissorXY(s_pHalContext, BorderSz, BorderSz);
        EVE_CoDl_clearColorRgb(s_pHalContext, 0xff, 0xff, 0xff);
        EVE_CoDl_clear(s_pHalContext, 1, 1, 1);

        EVE_CoDl_scissorSize(s_pHalContext, 2048, 2048);
        EVE_CoDl_scissorXY(s_pHalContext, 0, 0);
        EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);
        /* L8 bitmap display */
        EVE_CoDl_begin(s_pHalContext, BITMAPS);
        EVE_CoDl_bitmapSource(s_pHalContext, 0);
        EVE_CoDl_bitmapLayout(s_pHalContext,
            L8, (s_pHalContext->Width - 2 * BorderSz), (s_pHalContext->Height - 2 * BorderSz));
        EVE_CoDl_bitmapSize(s_pHalContext,
            BILINEAR, BORDER, BORDER, (s_pHalContext->Width - 2 * BorderSz), (s_pHalContext->Height - 2 * BorderSz));

        EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0x00, 0x00);
        line_width = 3;
        while (line_width >= 0)
        {
            EVE_CoDl_vertex2f_4(s_pHalContext, (BorderSz + line_width) * 16, (BorderSz + line_width) * 16);
            line_width = line_width - 1;
        }
        EVE_CoDl_end(s_pHalContext);
        /* Display the text */
        EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0xff, 0xff);
        EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 20, 31, OPT_CENTER,
            "Sketch L8");
        EVE_CoDl_tag(s_pHalContext, 'K');
        EVE_CoCmd_button(s_pHalContext, (int16_t)(s_pHalContext->Width - 200), 0, 150, 50, 29, 0, "EXIT");
        Display_End(s_pHalContext);
    }

    /* Send the stop command */
    EVE_CoCmd_stop(s_pHalContext);
    /* Update the command buffer pointers - both read and write pointers */
    EVE_Cmd_waitFlush(s_pHalContext);

    EVE_Util_clearScreen(s_pHalContext);
    EVE_sleep(500);

    EVE_CoCmd_memZero(s_pHalContext, RAM_G, MemZeroSz);
    EVE_CoCmd_sketch(s_pHalContext, BorderSz, BorderSz,
        (int16_t)(s_pHalContext->Width - 2 * BorderSz),
        (int16_t)(s_pHalContext->Height - 2 * BorderSz), RAM_G, L1); //sketch in L1 format
    while ((EVE_CoCmd_regRead(s_pHalContext, REG_TOUCH_TAG, &tag)) && (tag != 'S'))
    {
        /* Display the sketch */
        Display_Start(s_pHalContext);
        EVE_CoDl_scissorSize(s_pHalContext,
            (s_pHalContext->Width - 2 * BorderSz), (s_pHalContext->Height - 2 * BorderSz));
        EVE_CoDl_scissorXY(s_pHalContext, BorderSz, BorderSz);
        EVE_CoDl_clearColorRgb(s_pHalContext, 255, 255, 255);
        EVE_CoDl_clear(s_pHalContext, 1, 1, 1);

        /* default the scissor size */
        EVE_CoDl_scissorSize(s_pHalContext, 2048, 2048);
        EVE_CoDl_scissorXY(s_pHalContext, 0, 0);
        EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);

        /* L1 bitmap display */
        EVE_CoDl_begin(s_pHalContext, BITMAPS);
        EVE_CoDl_bitmapSource(s_pHalContext, 0);
        EVE_CoDl_bitmapLayout(s_pHalContext,
            L1, (s_pHalContext->Width - 2 * BorderSz) / 8, (s_pHalContext->Height - 2 * BorderSz));
        EVE_CoDl_bitmapSize(s_pHalContext,
            BILINEAR, BORDER, BORDER, (s_pHalContext->Width - 2 * BorderSz), (s_pHalContext->Height - 2 * BorderSz));

        line_width = 3;
        while (line_width >= 0)
        {
            EVE_CoDl_vertex2f_4(s_pHalContext, (BorderSz + line_width) * 16, (BorderSz + line_width) * 16);
            line_width = line_width - 1;
        }
        EVE_CoDl_end(s_pHalContext);
        /* Display the text */
        EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0xff, 0xff);
        EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 20, 31, OPT_CENTER,
            "Sketch L1");
        EVE_CoDl_tag(s_pHalContext, 'S');
        EVE_CoCmd_button(s_pHalContext, (int16_t)(s_pHalContext->Width - 200), 0, 150, 50, 29, 0, "EXIT");
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
*/
void SAMAPP_Widget_arc()
{
    Draw_Text(s_pHalContext, "Example for: arc widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of arc function. The arc command    */
    /* draws a circular arc. The end caps of the arc are rounded.            */
    /* If the angles specify a complete circle, a disc is drawn.             */
    /*************************************************************************/
    Display_Start(s_pHalContext);
    uint32_t rgb[20] = { 0x00E6194B, 0x003CB44B, 0x00FFFF19, 0x000082C8, 0x00F58230, 0x00911EB4, 0x0046F0F0, 0x00F032E6, 0x00D2F53C, 0x00FABED4,
        0x00008080, 0x00DCBEFF, 0x00AA6E28, 0x00FFFAC8, 0x00800000, 0x00AAFFC3, 0x00808000, 0x00FFD7B4, 0x00000080, 0x00808080 };
    uint16_t i, r0, r1, a0, a1, x, y;
    uint16_t n = 8;
    uint16_t fac = 0xffff / (n - 1);

    for (i = 1; i < 10; i++)
    {
        r0 = 30 * i;
        r1 = r0 + 8;
        a0 = (i * 0x1933) & 0xffff;
        a1 = (a0 + (i * 0x3933)) & 0xffff;
        EVE_CoDl_colorRgb(s_pHalContext, (rgb[i - 1] >> 16) & 0xff, (rgb[i - 1] >> 8) & 0xff, rgb[i - 1] & 0xff);
        EVE_CoCmd_arc(s_pHalContext, 400, 400, r0, r1, a0, a1);
    }

    EVE_CoDl_colorRgb(s_pHalContext, (rgb[9] >> 16) & 0xff, (rgb[9] >> 8) & 0xff, rgb[9] & 0xff);
    EVE_CoCmd_arc(s_pHalContext, 400, 400, 320, 358, 0, 0xffff);

    for (i = 0; i < n * n; i++)
    {
        x = (i % n) * 100 + (400 + 358 + 100);
        y = (i / n) * 100 + 100;
        a0 = (i % n) * fac;
        a1 = (i / n) * fac;
        EVE_CoDl_colorRgb(s_pHalContext, (rgb[(i - 1) % 20] >> 16) & 0xff, (rgb[(i - 1) % 20] >> 8) & 0xff, rgb[(i - 1) % 20] & 0xff);
        EVE_CoCmd_arc(s_pHalContext, x, y, 30, 45, a0, a1);
    }

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

void SAMAPP_Widget_glow()
{
    Draw_Text(s_pHalContext, "Example for: glow widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of glow function. The glow command  */
    /* draws an additive glow effect centered in a rectangle, using the      */
    /* current color.                                                        */
    /*************************************************************************/
    Display_Start(s_pHalContext);
    EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0xc0, 0x80);
    EVE_CoCmd_text(s_pHalContext, s_pHalContext->Width / 2, s_pHalContext->Height / 2, 33, OPT_CENTER, "glow widget");
    EVE_CoDl_colorRgb(s_pHalContext, 0x60, 0x48, 0x30);
    EVE_CoCmd_glow(s_pHalContext, s_pHalContext->Width / 2 - 400, s_pHalContext->Height / 2 - 250, 800, 500);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

void SAMAPP_Widget() {
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


