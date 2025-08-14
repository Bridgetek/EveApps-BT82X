/**
 * @file Bitmap.c
 * @brief Sample usage of bitmap
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

#include "Bitmap.h"
#include "FileTransfer.h"
#include "FlashHelper.h"

// angle and circle
#define MAX_ANGLE 360
#define MAX_CIRCLE_UNIT 65536

#define SAMAPP_DELAY     EVE_sleep(3000)

static EVE_HalContext s_halContext;
static EVE_HalContext* s_pHalContext;
void SAMAPP_Bitmap();

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
        "This sample demonstrate the using of bitmap", 
        "",
        ""
    }; 

    while (true) {
        WelcomeScreen(s_pHalContext, info);

        SAMAPP_Bitmap();

        EVE_Util_clearScreen(s_pHalContext);

        Gpu_Release(s_pHalContext);

        /* Init HW Hal for next loop*/
        Gpu_Init(s_pHalContext);
#if GET_CALIBRATION == 1
        //Calibration_Restore(s_pHalContext);
#endif

        LVDS_Config(s_pHalContext, YCBCR, MODE_PICTURE);
    }

    return 0;
}

/**
 * @brief API to demonstrate CMD_GETIMAGE
 *
 */
void SAMAPP_Bitmap_getImage()
{
    uint32_t source;
    uint32_t fmt;
    uint32_t w;
    uint32_t h;
    uint32_t palette;
    int32_t ImgW = 800, ImgH = 600;

    Draw_Text(s_pHalContext, "Example for: CMD_GETIMAGE");

    EVE_Util_loadImageFile(s_pHalContext, DDR_BITMAPS_STARTADDR, TEST_DIR "\\CR_003_dithering.png", NULL, 0);

    // Start drawing bitmap
    Display_Start(s_pHalContext);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoCmd_setBitmap(s_pHalContext, DDR_BITMAPS_STARTADDR, RGB565, ImgW, ImgH);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width - ImgW) / 2 * 16, (s_pHalContext->Height - ImgH) / 2 * 16);
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);

    EVE_sleep(500);

    // Now getImage properties
    EVE_CoCmd_getImage(s_pHalContext, &source, &fmt, &w, &h, &palette);

    eve_printf_debug("Loaded image: \nsource: %u, format: %u, width: %u, height: %u, palette: %u\n",
        source, fmt,
        w, h, palette);

    SAMAPP_DELAY;
}

/**
 * @brief API to demonstrate CMD_LOADIMAGE w/o OPT_DITHER
 * For PNG decoder, the dithering is enabled
 *
 */
void SAMAPP_Bitmap_dithering()
{
    uint16_t w = 800;
    uint16_t h = 480;
    uint16_t otp = OPT_NODL;

    EVE_Util_clearScreen(s_pHalContext);

    for (int i = 0; i < 2; i++)
    {
        if (i == 0)
        {
            Draw_Text(s_pHalContext, "Example for PNG dithering: Dithering disable");
        }
        else
        {
            Draw_Text(s_pHalContext, "Example for PNG dithering: Dithering enable");

            /// Now enable dithering support
            otp |= OPT_DITHER;
        }

        //EVE_Util_loadImageFile(s_pHalContext, address, TEST_DIR "\\loadimage-dither-testcase.png", 0);
        EVE_Util_loadImageFile(s_pHalContext, DDR_BITMAPS_STARTADDR, TEST_DIR "\\loadimage-dither-testcase.png", NULL, otp);

        // Start drawing bitmap
        Display_Start(s_pHalContext);
        EVE_CoCmd_setBitmap(s_pHalContext, DDR_BITMAPS_STARTADDR, RGB565, w, h);
        EVE_CoDl_begin(s_pHalContext, BITMAPS);
        EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width - w) / 2 * 16, (s_pHalContext->Height - h) / 2 * 16);
        EVE_CoDl_end(s_pHalContext);
        Display_End(s_pHalContext);

        SAMAPP_DELAY;
    }
}

/**
 * @brief Draw ASTC image on screen
 *
 * @param str Image label
 * @param x Image X
 * @param y Image Y
 * @param w Image W
 * @param h Image H
 * @param margin Image margin
 * @param source Image source
 * @param numcell Cell number
 */
void helperDrawASTC(const char *title, uint32_t source, uint16_t fmt, uint16_t x, uint16_t y,
    uint16_t w, uint16_t h, uint16_t margin, uint16_t numcell)
{
    int m1 = 30;

    if (title != 0)
    {
        EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);
        EVE_CoCmd_text(s_pHalContext, x, y, 16, 0, title);
        EVE_CoDl_colorRgb(s_pHalContext, 0xFF, 0xFF, 0xFF);
    }
    EVE_CoCmd_setBitmap(s_pHalContext, source, fmt, w, h);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    for (int i = 0; i < numcell; i++)
    {
        EVE_CoDl_cell(s_pHalContext, i);
        EVE_CoDl_vertex2f_4(s_pHalContext, (x + m1 + margin * i) * 16, y * 16);
    }
    EVE_CoDl_end(s_pHalContext);
}

/**
 * @brief API to demonstrate ASTC layout
 *
 */
void SAMAPP_Bitmap_ASTCLayoutRAMG()
{
    const char *files = TEST_DIR "\\numbers_astc12x10.raw";
    int16_t x = 0;
    int16_t y = 0;

    EVE_Util_loadRawFile(s_pHalContext, RAM_G, files);

    Draw_Text(s_pHalContext, "Example for: ASTC bitmap on RAM_G");

    Display_Start(s_pHalContext);
    x = 20;
    y = 20;
    helperDrawASTC("1x1", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 12, 10, 20, 2);
    y += 30;
    helperDrawASTC("1x2", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 12, 20, 20, 2);
    y += 30;
    helperDrawASTC("1x3", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 12, 30, 20, 2);
    y += 40;
    helperDrawASTC("1x4", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 12, 40, 20, 2);
    y += 50;
    helperDrawASTC("1x5", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 12, 50, 20, 2);

    x += 90;
    y = 20;
    helperDrawASTC("2x1", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 24, 10, 40, 2);
    y += 30;
    helperDrawASTC("2x2", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 24, 20, 40, 2);
    y += 30;
    helperDrawASTC("2x3", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 24, 30, 40, 2);
    y += 40;
    helperDrawASTC("2x4", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 24, 40, 40, 2);
    y += 50;
    helperDrawASTC("2x5", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 24, 50, 40, 2);

    x += 110;
    y = 20;
    helperDrawASTC("3x1", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 36, 10, 50, 2);
    y += 30;
    helperDrawASTC("3x2", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 36, 20, 50, 2);
    y += 30;
    helperDrawASTC("3x3", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 36, 30, 50, 2);
    y += 40;
    helperDrawASTC("3x4", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 36, 40, 50, 2);
    y += 50;
    helperDrawASTC("3x5", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 36, 50, 50, 2);

    x += 130;
    y = 20;
    helperDrawASTC("4x1", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 48, 10, 60, 2);
    y += 30;
    helperDrawASTC("4x2", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 48, 20, 60, 2);
    helperDrawASTC("4x3", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y + 30, 48, 30, 60, 2);
    y += 70;
    helperDrawASTC("4x4", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 48, 40, 60, 2);
    y += 50;
    helperDrawASTC("4x5", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 48, 50, 60, 2);

    x += 150;
    y = 20;
    helperDrawASTC("5x1", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 60, 10, 70, 2);
    y += 30;
    helperDrawASTC("5x2", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 60, 20, 70, 2);
    y += 30;
    helperDrawASTC("5x3", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 60, 30, 70, 2);
    y += 40;
    helperDrawASTC("5x4", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 60, 40, 70, 2);
    y += 50;
    helperDrawASTC("5x5", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 60, 50, 70, 2);

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief API to demonstrate multicell ASTC bitmap on RAMG
 *
 */
void SAMAPP_Bitmap_ASTCMultiCellRAMG()
{
    const char *astc_file = TEST_DIR "\\fu_168x960_COMPRESSED_RGBA_ASTC_4x4_KHR.raw";
    int16_t x = 0;
    int16_t y = 50;
    uint32_t astc_addr = DDR_BITMAPS_STARTADDR;

    Draw_Text(s_pHalContext, "Example for: Multicell ASTC bitmap on RAM_G");

    EVE_Util_loadRawFile(s_pHalContext, astc_addr, astc_file);

    Display_Start(s_pHalContext);
    EVE_CoDl_colorRgb(s_pHalContext, 0x33, 0x33, 0x33);
    EVE_CoCmd_text(s_pHalContext, 20, y, 30, OPT_FILL,
        "Image of 168x96 (10 cells, 1008 blocks / cell):\n");

    y += 50;
    EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0xff, 0xff);
    helperDrawASTC(0, astc_addr, COMPRESSED_RGBA_ASTC_4x4_KHR, x, y, 168, 96, 170, 10);

    y += 180;
    EVE_CoDl_colorRgb(s_pHalContext, 0x33, 0x33, 0x33);
    EVE_CoCmd_text(s_pHalContext, 20, y, 30, OPT_FILL,
        "Image of 168x60 (16 cells, 630 blocks / cell):\n");
    y += 50;
    EVE_CoDl_colorRgb(s_pHalContext, 255, 255, 255);
    helperDrawASTC(0, astc_addr, COMPRESSED_RGBA_ASTC_4x4_KHR, x, y, 168, 60, 170, 10);

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}


/**
 * @brief API to demonstrate image rotate by CMD_ROTATEAROUND
 *
 */
void SAMAPP_Bitmap_rotate()
{
    uint16_t image_w = 256;
    uint16_t image_h = 256;

    Draw_Text(s_pHalContext, "Example for: Bitmap rotate");
    EVE_Util_loadImageFile(s_pHalContext, DDR_BITMAPS_STARTADDR, TEST_DIR "\\mandrill256.jpg", NULL, OPT_RGB565);

    for (uint16_t i = 0; i <= 360; i++)
    {
        Display_Start(s_pHalContext);
        EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);
        EVE_CoCmd_text(s_pHalContext, 10, 0, 31, 0, "CMD_ROTATEAROUND"); // text info
        EVE_CoDl_colorRgb(s_pHalContext, 255, 255, 255);

        EVE_CoDl_bitmapHandle(s_pHalContext, 0);
        EVE_CoCmd_setBitmap(s_pHalContext, DDR_BITMAPS_STARTADDR, RGB565, image_w, image_h);
        EVE_CoDl_bitmapSize(s_pHalContext, BILINEAR, BORDER, BORDER, image_w, image_h);

        EVE_CoDl_begin(s_pHalContext, BITMAPS);
        EVE_CoCmd_loadIdentity(s_pHalContext);
        EVE_CoCmd_rotateAround(s_pHalContext, image_w / 2, image_h / 2, i * 65536 / 360, 65536 * 1);
        EVE_CoCmd_setMatrix(s_pHalContext);
        EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width / 2 - image_w / 2) * 16, (s_pHalContext->Height / 2 - image_h / 2) * 16);
        EVE_CoDl_end(s_pHalContext);

        EVE_CoCmd_loadIdentity(s_pHalContext);
        EVE_CoCmd_setMatrix(s_pHalContext);

        Display_End(s_pHalContext);
        EVE_sleep(10);
    }
    SAMAPP_DELAY;
}

/**
 * @brief Rotate image by CMD_ROTATEAROUND
 *
 * @param address Address of image
 * @param format Image format
 * @param x Image X
 * @param y Image Y
 * @param w Image W
 * @param h Image H
 * @param rotation_angle Rotate angle
 */
void helperRotateAroundOne(uint32_t address, uint16_t format, uint16_t x, uint16_t y,
    uint16_t w, uint16_t h, uint16_t rotation_angle)
{
    int16_t lw;
    int16_t lh;
    const uint16_t TRANSLATE_XY = 100;

    EVE_CoDl_saveContext(s_pHalContext);
    EVE_CoCmd_setBitmap(s_pHalContext, address, format, w, h);

    lw = w + 2 * TRANSLATE_XY;
    lh = h + 2 * TRANSLATE_XY;
    EVE_CoDl_bitmapSize(s_pHalContext, NEAREST, BORDER, BORDER, lw, lh);

    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_translate(s_pHalContext, TRANSLATE_XY * MAX_CIRCLE_UNIT, TRANSLATE_XY * MAX_CIRCLE_UNIT);
    EVE_CoCmd_rotateAround(s_pHalContext, w / 2, h / 2, rotation_angle * MAX_CIRCLE_UNIT / MAX_ANGLE, MAX_CIRCLE_UNIT);
    EVE_CoCmd_setMatrix(s_pHalContext);

    EVE_CoDl_vertex2f_4(s_pHalContext, x * 16, y * 16);
    EVE_CoDl_end(s_pHalContext);

    EVE_CoDl_restoreContext(s_pHalContext);
}

/**
 * @brief Rotate image by CMD_ROTATE and CMD_TRANSLATE
 *
 * @param address Address of image
 * @param format Image format
 * @param x Image X
 * @param y Image Y
 * @param w Image W
 * @param h Image H
 * @param rotation_angle Rotate angle
 */
void helperRotateAndTranslateOne(uint32_t address, uint16_t format, uint16_t x, uint16_t y,
    uint16_t w, uint16_t h, uint16_t rotation_angle)
{
    int16_t lw;
    int16_t lh;
    const uint16_t TRANSLATE_XY = 100;

    EVE_CoDl_saveContext(s_pHalContext);
    EVE_CoCmd_setBitmap(s_pHalContext, address, format, w, h);

    lw = w * 2;
    lh = h * 2;
    EVE_CoDl_bitmapSize(s_pHalContext, NEAREST, BORDER, BORDER, lw, lh);

    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_translate(s_pHalContext, (w / 2 + TRANSLATE_XY) * MAX_CIRCLE_UNIT, (w / 2 + TRANSLATE_XY) * MAX_CIRCLE_UNIT);
    EVE_CoCmd_rotate(s_pHalContext, rotation_angle * MAX_CIRCLE_UNIT / MAX_ANGLE);
    EVE_CoCmd_translate(s_pHalContext, -((int32_t)(w / 2)) * MAX_CIRCLE_UNIT, -((int32_t)(h / 2)) * MAX_CIRCLE_UNIT);
    EVE_CoCmd_setMatrix(s_pHalContext);

    EVE_CoDl_vertex2f_4(s_pHalContext, x * 16, y * 16);
    EVE_CoDl_end(s_pHalContext);

    EVE_CoDl_restoreContext(s_pHalContext);
}

/**
 * @brief rotate and translate
 *
 */
void SAMAPP_Bitmap_rotateAndTranslate()
{
    int count = 0;
    const uint16_t TRANSLATE_XY = 100;

    // tiles
    uint16_t tile_w = 256;
    uint16_t tile_h = 256;
    uint16_t tile1_x = 100;
    uint16_t tile1_y = 100;
    uint16_t tile2_x = (uint16_t)(s_pHalContext->Width - tile1_x - tile_w - 800);
    uint16_t tile2_y = tile1_y;

    Draw_Text(s_pHalContext, "Example for: Bitmap rotate and translate");
    EVE_Util_loadImageFile(s_pHalContext, DDR_BITMAPS_STARTADDR, TEST_DIR "\\mandrill256.jpg", NULL, OPT_RGB565);

    while (count++ < 30 * 10)
    {
        /*Display List start*/
        Display_Start(s_pHalContext);
        static uint16_t rotation_angle = 50;
        rotation_angle = (rotation_angle + 1) % MAX_ANGLE;

        helperRotateAroundOne(DDR_BITMAPS_STARTADDR, RGB565, tile1_x, tile1_y, tile_w, tile_h, rotation_angle);
        helperRotateAndTranslateOne(DDR_BITMAPS_STARTADDR, RGB565, tile2_x, tile2_y, tile_w, tile_h, rotation_angle);

        EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);
        EVE_CoCmd_text(s_pHalContext, tile1_x + TRANSLATE_XY / 2, tile1_y, 31, 0, "Rotate by RotateAround");
        EVE_CoCmd_text(s_pHalContext, tile2_x + TRANSLATE_XY / 2, tile2_y, 31, 0, "Rotate by Rotate and Translate");
        EVE_CoDl_colorRgb(s_pHalContext, 255, 255, 255);

        Display_End(s_pHalContext);
        EVE_sleep(10);
    }
    SAMAPP_DELAY;
}

/**
 * @brief Load image from SD card
 *
 */
void SAMAPP_Bitmap_loadImagefromSD()
{
    uint32_t result = 0;
    const char *file;
    uint32_t ptr = 0;
    uint32_t w;
    uint32_t h;

    Draw_Text(s_pHalContext, "Example for: Load image from SD card");

    file = "pic_in_SD.jpg";
    eve_printf_debug("picture name %s \n", file);
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

    Display_Start(s_pHalContext);
    EVE_CoCmd_loadImage(s_pHalContext, DDR_BITMAPS_STARTADDR2, OPT_FS | OPT_TRUECOLOR);

    // Now get image properties
    EVE_CoCmd_getProps(s_pHalContext, &ptr, &w, &h);
    eve_printf_debug("Loaded image: \nptr: %u, width: %u, height: %u \n", ptr, w, h);

    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width - w) / 2 * 16, (s_pHalContext->Height - h) / 2 * 16);
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief Load image from flash
 *
 */
void SAMAPP_Bitmap_loadImagefromFlash()
{
    if (!FlashHelper_SwitchFullMode(s_pHalContext))
    {
        eve_printf("SwitchFullMode failed");
        return;
    }
    Draw_Text(s_pHalContext, "Example for: Load image from Flash");

    Display_Start(s_pHalContext);
    EVE_CoCmd_flashSource(s_pHalContext, 4096);
    EVE_CoCmd_loadImage(s_pHalContext, DDR_BITMAPS_STARTADDR, OPT_FLASH | OPT_RGB565);

    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width - 256) / 2 * 16, (s_pHalContext->Height - 256) / 2 * 16);
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}


/**
* @brief Load image in YCBCR
*
*/
void SAMAPP_Bitmap_loadImage()
{
    int32_t ImgW = 256, ImgH = 256;
    const uint8_t *image = TEST_DIR "\\mandrill256.jpg";
    Draw_Text(s_pHalContext, "Example for: Load image and display as YCBCR image");

    Display_Start(s_pHalContext);
    EVE_Util_loadImageFile(s_pHalContext, DDR_BITMAPS_STARTADDR, image, NULL, OPT_YCBCR);

    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width - ImgW) / 2 * 16, (s_pHalContext->Height - ImgH) / 2 * 16);
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);

    SAMAPP_DELAY;
}

/**
 * @brief Load image in L8
 *
 */
void SAMAPP_Bitmap_loadImageMono()
{
    int16_t ImgW = 256, ImgH = 256;
    const char *file = TEST_DIR "\\mandrill256.jpg";
    Draw_Text(s_pHalContext, "Example for: Load image and display as monochromic image");

    Display_Start(s_pHalContext);
    EVE_Util_loadImageFile(s_pHalContext, DDR_BITMAPS_STARTADDR, file, NULL, OPT_MONO);

    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_blendFunc(s_pHalContext, SRC_ALPHA, ZERO);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width - ImgW) / 2 * 16, (s_pHalContext->Height - ImgH) / 2 * 16);
    EVE_CoDl_blendFunc_default(s_pHalContext);
    EVE_CoDl_end(s_pHalContext);

    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 255);
    EVE_CoCmd_text(s_pHalContext, s_pHalContext->Width / 2, 100, 31, OPT_CENTER, "Display bitmap by jpg decode L8");

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief Load image in full color
 *
 */
void SAMAPP_Bitmap_loadImageFullColor()
{
    int16_t ImgW = 256, ImgH = 256;
    const char *file = TEST_DIR "\\mandrill256.jpg";
    Draw_Text(s_pHalContext, "Example for: Load image full color ");

    Display_Start(s_pHalContext);
    /* decode the jpeg data */
    EVE_Util_loadImageFile(s_pHalContext, DDR_BITMAPS_STARTADDR, file, NULL, OPT_TRUECOLOR);

    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_vertex2f_4(s_pHalContext, ((s_pHalContext->Width - ImgW) / 2 * 16), ((s_pHalContext->Height - ImgH) / 2 * 16));
    EVE_CoDl_end(s_pHalContext);

    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 255);
    EVE_CoCmd_text(s_pHalContext, s_pHalContext->Width / 2, 100, 31, OPT_CENTER, "Display bitmap by jpg decode");

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief Load DXT1 compressed image. The BRIDGETEK DXT1 conversion utility outputs 2 seperate files: L1, RGB565.
 * The 2 files should be combined to create the final image.  The bitmap size can be reduced up to 4 folds of the original size.
 *
 */
void SAMAPP_Bitmap_DXT1()
{
    uint16_t imgWidth = 320;
    uint16_t imgHeight = 240;
    uint16_t colorLayer_width = imgWidth / 4;
    uint16_t colorLayer_height = imgHeight / 4;
    uint16_t colorLayer_stride = colorLayer_width * 2;
    uint16_t gradientLayer_width = imgWidth;
    uint16_t gradientLayer_height = imgHeight;
    uint16_t gradientLayer_stride = gradientLayer_width / 8;
    uint16_t szPerFile = colorLayer_stride * colorLayer_height * 2;
    uint8_t colorHandle = 1;
    uint8_t gradientHandle = 2;

    Draw_Text(s_pHalContext, "Example for: Load DXT1_L1_RGB565 compressed image");

    EVE_Util_loadRawFile(s_pHalContext, DDR_BITMAPS_STARTADDR, TEST_DIR "\\bird_320x240_RGB565.raw");
    EVE_Util_loadRawFile(s_pHalContext, DDR_BITMAPS_STARTADDR + szPerFile, TEST_DIR "\\bird_320x240_L1.raw");

    Display_Start(s_pHalContext);

    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_setMatrix(s_pHalContext);

    EVE_CoDl_saveContext(s_pHalContext);

    // Color handle
    EVE_CoDl_bitmapHandle(s_pHalContext, colorHandle);
    EVE_CoCmd_setBitmap(s_pHalContext, DDR_BITMAPS_STARTADDR, RGB565, colorLayer_width, colorLayer_height);
    EVE_CoDl_bitmapSize(s_pHalContext, NEAREST, BORDER, BORDER, imgWidth, imgHeight);

    // gradient handle
    EVE_CoDl_bitmapHandle(s_pHalContext, gradientHandle);
    EVE_CoCmd_setBitmap(s_pHalContext, DDR_BITMAPS_STARTADDR + szPerFile, L1, imgWidth, imgHeight);

    // start drawing bitmaps
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_blendFunc(s_pHalContext, ONE, ZERO);
    EVE_CoDl_colorA(s_pHalContext, 0x55);
    EVE_CoDl_bitmapHandle(s_pHalContext, gradientHandle);
    EVE_CoDl_cell(s_pHalContext, 0);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width / 2 - imgWidth / 2) * 16, (s_pHalContext->Height / 2 - imgHeight / 2) * 16);
    EVE_CoDl_blendFunc(s_pHalContext, ONE, ONE);
    EVE_CoDl_colorA(s_pHalContext, 0xAA);
    EVE_CoDl_cell(s_pHalContext, 1);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width / 2 - imgWidth / 2) * 16, (s_pHalContext->Height / 2 - imgHeight / 2) * 16);

    EVE_CoDl_colorMask(s_pHalContext, 1, 1, 1, 0);
    EVE_CoCmd_scale(s_pHalContext, 4UL * 65536UL, 4UL * 65536UL); // Color passes, scaled up 4x, nearest
    EVE_CoCmd_setMatrix(s_pHalContext);
    EVE_CoDl_blendFunc(s_pHalContext, DST_ALPHA, ZERO);
    EVE_CoDl_bitmapHandle(s_pHalContext, colorHandle);
    EVE_CoDl_cell(s_pHalContext, 1);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width / 2 - imgWidth / 2) * 16, (s_pHalContext->Height / 2 - imgHeight / 2) * 16); // Color layer 1 (RGB*L2)
    EVE_CoDl_blendFunc(s_pHalContext, ONE_MINUS_DST_ALPHA, ONE);
    EVE_CoDl_cell(s_pHalContext, 0);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width / 2 - imgWidth / 2) * 16, (s_pHalContext->Height / 2 - imgHeight / 2) * 16); // Color layer 0 (RGB*(1-L2) + DST)
    EVE_CoDl_end(s_pHalContext);
    EVE_CoDl_restoreContext(s_pHalContext);

    // reset the transformation matrix because its not part of the context, RESTORE_CONTEXT() command will not revert the command.
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_setMatrix(s_pHalContext);

    EVE_CoDl_colorRgb(s_pHalContext, 255, 0, 0);
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 100, 31, OPT_CENTER,
        "DXT1_L1_RGB565: 37.5KB.");
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 180, 31, OPT_CENTER,
        "RGB565: 150KB.");

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief Load DXT1L2 compressed image. The BRIDGETEK DXT1L2 conversion utility outputs 2 seperate files: RGB565,L2
 * The 2 files should be combined to create the final image.  The bitmap size can be reduced up to 4 folds of the original size.
 *
 */
void SAMAPP_Bitmap_DXT1L2()
{
    uint16_t imgWidth = 320;
    uint16_t imgHeight = 240;
    uint16_t colorLayer_width = imgWidth / 4;
    uint16_t colorLayer_height = imgHeight / 4;
    uint16_t colorLayer_stride = colorLayer_width * 2;
    uint16_t cFileSize = colorLayer_stride * colorLayer_height * 2;
    uint8_t colorHandle = 1;
    uint8_t gradientHandle = 2;

    Draw_Text(s_pHalContext, "Example for: Load DXT1_L2_RGB565 compressed image");

    EVE_Util_loadRawFile(s_pHalContext, DDR_BITMAPS_STARTADDR, TEST_DIR "\\bird_320x240_RGB565.raw");
    EVE_Util_loadRawFile(s_pHalContext, DDR_BITMAPS_STARTADDR + cFileSize, TEST_DIR "\\bird_320x240_L2.raw");

    Display_Start(s_pHalContext);

    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_setMatrix(s_pHalContext);

    EVE_CoDl_saveContext(s_pHalContext);

    // Color handle
    EVE_CoDl_bitmapHandle(s_pHalContext, colorHandle);
    EVE_CoCmd_setBitmap(s_pHalContext, DDR_BITMAPS_STARTADDR, RGB565, colorLayer_width, colorLayer_height);
    EVE_CoDl_bitmapSize(s_pHalContext, NEAREST, BORDER, BORDER, imgWidth, imgHeight);

    // L2 handle
    EVE_CoDl_bitmapHandle(s_pHalContext, gradientHandle);
    EVE_CoCmd_setBitmap(s_pHalContext, DDR_BITMAPS_STARTADDR + cFileSize, L2, imgWidth, imgHeight);

    // start drawing bitmaps
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_blendFunc(s_pHalContext, ONE, ZERO);
    EVE_CoDl_colorA(s_pHalContext, 0xFF);
    EVE_CoDl_bitmapHandle(s_pHalContext, gradientHandle);
    EVE_CoDl_cell(s_pHalContext, 0);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width / 2 - imgWidth / 2) * 16, (s_pHalContext->Height / 2 - imgHeight / 2) * 16);

    EVE_CoDl_colorMask(s_pHalContext, 1, 1, 1, 0);
    EVE_CoCmd_scale(s_pHalContext, 4UL * 65536UL, 4UL * 65536UL); // Color passes, scaled up 4x, nearest
    EVE_CoCmd_setMatrix(s_pHalContext);
    EVE_CoDl_blendFunc(s_pHalContext, DST_ALPHA, ZERO);
    EVE_CoDl_bitmapHandle(s_pHalContext, colorHandle);
    EVE_CoDl_cell(s_pHalContext, 1);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width / 2 - imgWidth / 2) * 16, (s_pHalContext->Height / 2 - imgHeight / 2) * 16); // Color layer 1 (RGB*L2)
    EVE_CoDl_blendFunc(s_pHalContext, ONE_MINUS_DST_ALPHA, ONE);
    EVE_CoDl_cell(s_pHalContext, 0);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width / 2 - imgWidth / 2) * 16, (s_pHalContext->Height / 2 - imgHeight / 2) * 16); // Color layer 0 (RGB*(1-L2) + DST)
    EVE_CoDl_end(s_pHalContext);
    EVE_CoDl_restoreContext(s_pHalContext);

    // reset the transformation matrix because its not part of the context, RESTORE_CONTEXT() command will not revert the command.
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_setMatrix(s_pHalContext);

    EVE_CoDl_colorRgb(s_pHalContext, 255, 0, 0);
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 100, 31, OPT_CENTER,
        "DXT1_L2_RGB565: 37.5KB.");
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 180, 31, OPT_CENTER,
        "RGB565: 150KB.");

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief draw image in YCBCR
 *
 * For format YCBCR,  the image width restriction:
 * - 2 pixel aligned
 * the image height restriction:
 * - 2 pixel aligned
 */
void SAMAPP_Bitmap_YCBCR()
{
    int16_t ImgW = 256, ImgH = 256;
    Draw_Text(s_pHalContext, "Example for: YCBCR format");

    EVE_Util_loadRawFile(s_pHalContext, DDR_BITMAPS_STARTADDR, TEST_DIR "\\mandrill256_256x256_YCbCr.raw");

    Display_Start(s_pHalContext);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoCmd_setBitmap(s_pHalContext, DDR_BITMAPS_STARTADDR, YCBCR, ImgW, ImgH);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width - ImgW) / 2 * 16, (s_pHalContext->Height - ImgH) / 2 * 16);
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);

    SAMAPP_DELAY;
}

/**
 * @brief draw image in LA1
 *
 * For format LA1,  the image width restriction:
 * - 8 pixel aligned
 */
void SAMAPP_Bitmap_LA1()
{
    int16_t ImgW = 256, ImgH = 256;
    Draw_Text(s_pHalContext, "Example for: LA1 format");

    EVE_Util_loadRawFile(s_pHalContext, DDR_BITMAPS_STARTADDR, TEST_DIR "\\mandrill256_256x256_LA1.raw");

    Display_Start(s_pHalContext);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoCmd_setBitmap(s_pHalContext, DDR_BITMAPS_STARTADDR, LA1, ImgW, ImgH);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width - ImgW) / 2 * 16, (s_pHalContext->Height - ImgH) / 2 * 16);
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);

    SAMAPP_DELAY;
}

/**
 * @brief draw image in LA2
 *
 * For format LA2,  the image width restriction:
 * - 4 pixel aligned
 */
void SAMAPP_Bitmap_LA2()
{
    int16_t ImgW = 256, ImgH = 256;
    Draw_Text(s_pHalContext, "Example for: LA2 format");

    EVE_Util_loadRawFile(s_pHalContext, DDR_BITMAPS_STARTADDR, TEST_DIR "\\mandrill256_256x256_LA2.raw");

    Display_Start(s_pHalContext);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoCmd_setBitmap(s_pHalContext, DDR_BITMAPS_STARTADDR, LA2, ImgW, ImgH);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width - ImgW) / 2 * 16, (s_pHalContext->Height - ImgH) / 2 * 16);
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);

    SAMAPP_DELAY;
}

/**
 * @brief draw image in LA4
 *
 * For format LA4,  the image width restriction:
 * - 2 pixel aligned
 */
void SAMAPP_Bitmap_LA4()
{
    int16_t ImgW = 256, ImgH = 256;
    Draw_Text(s_pHalContext, "Example for: LA4 format");

    EVE_Util_loadRawFile(s_pHalContext, DDR_BITMAPS_STARTADDR, TEST_DIR "\\mandrill256_256x256_LA4.raw");

    Display_Start(s_pHalContext);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoCmd_setBitmap(s_pHalContext, DDR_BITMAPS_STARTADDR, LA4, ImgW, ImgH);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width - ImgW) / 2 * 16, (s_pHalContext->Height - ImgH) / 2 * 16);
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);

    SAMAPP_DELAY;
}

/**
 * @brief draw image in LA8
 *
 */
void SAMAPP_Bitmap_LA8()
{
    int16_t ImgW = 256, ImgH = 256;
    Draw_Text(s_pHalContext, "Example for: LA8 format");

    EVE_Util_loadRawFile(s_pHalContext, DDR_BITMAPS_STARTADDR, TEST_DIR "\\mandrill256_256x256_LA8.raw");

    Display_Start(s_pHalContext);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoCmd_setBitmap(s_pHalContext, DDR_BITMAPS_STARTADDR, LA8, ImgW, ImgH);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width - ImgW) / 2 * 16, (s_pHalContext->Height - ImgH) / 2 * 16);
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);

    SAMAPP_DELAY;
}

/**
 * @brief draw image in RGB6
 * 
 * For format RGB6,  the image width restriction:
 * - 8 pixel aligned
 */
void SAMAPP_Bitmap_RGB6()
{
    int16_t ImgW = 256, ImgH = 256;
    Draw_Text(s_pHalContext, "Example for: RGB6 format");

    EVE_Util_loadRawFile(s_pHalContext, DDR_BITMAPS_STARTADDR, TEST_DIR "\\mandrill256_256x256_RGB6.raw");

    Display_Start(s_pHalContext);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoCmd_setBitmap(s_pHalContext, DDR_BITMAPS_STARTADDR, RGB6, ImgW, ImgH);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width - ImgW) / 2 * 16, (s_pHalContext->Height - ImgH) / 2 * 16);
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);

    SAMAPP_DELAY;
}

/**
 * @brief draw image in RGB8
 *
 * For format RGB8,  the image width restriction:
 * - 2 pixel aligned
 */
void SAMAPP_Bitmap_RGB8()
{
    int16_t ImgW = 256, ImgH = 256;
    Draw_Text(s_pHalContext, "Example for: RGB8 format");

    EVE_Util_loadRawFile(s_pHalContext, DDR_BITMAPS_STARTADDR, TEST_DIR "\\mandrill256_256x256_RGB8.raw");

    Display_Start(s_pHalContext);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoCmd_setBitmap(s_pHalContext, DDR_BITMAPS_STARTADDR, RGB8, ImgW, ImgH);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width - ImgW) / 2 * 16, (s_pHalContext->Height - ImgH) / 2 * 16);
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);

    SAMAPP_DELAY;
}

/**
 * @brief draw image in ARGB6
 * 
 * For format ARGB6,  the image width restriction:
 * - 2 pixel aligned
 */
void SAMAPP_Bitmap_ARGB6()
{
    int16_t ImgW = 180, ImgH = 180;
    Draw_Text(s_pHalContext, "Example for: ARGB6 format");

    EVE_Util_loadRawFile(s_pHalContext, DDR_BITMAPS_STARTADDR, TEST_DIR "\\case_180x180_ARGB6.raw");

    Display_Start(s_pHalContext);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoCmd_setBitmap(s_pHalContext, DDR_BITMAPS_STARTADDR, ARGB6, ImgW, ImgH);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width - ImgW) / 2 * 16, (s_pHalContext->Height - ImgH) / 2 * 16);
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);

    SAMAPP_DELAY;
}

/**
 * @brief draw image in ARGB8
 *
 */
void SAMAPP_Bitmap_ARGB8()
{
    int16_t ImgW = 180, ImgH = 180;
    Draw_Text(s_pHalContext, "Example for: ARGB8 format");

    EVE_Util_loadRawFile(s_pHalContext, DDR_BITMAPS_STARTADDR, TEST_DIR "\\case_180x180_ARGB8.raw");

    Display_Start(s_pHalContext);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoCmd_setBitmap(s_pHalContext, DDR_BITMAPS_STARTADDR, ARGB8, ImgW, ImgH);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width - ImgW) / 2 * 16, (s_pHalContext->Height - ImgH) / 2 * 16);
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);

    SAMAPP_DELAY;
}

/**
 * @brief draw image in palettedARGB8
 *
 */
void SAMAPP_Bitmap_PALETTEDARGB8()
{
    int16_t ImgW = 181, ImgH = 185;
    Draw_Text(s_pHalContext, "Example for: PalettedARGB8 format");

    EVE_Util_loadRawFile(s_pHalContext, DDR_BITMAPS_STARTADDR, TEST_DIR "\\case_181x185_PALETTEDARGB8_lut.raw");
    EVE_Util_loadRawFile(s_pHalContext, DDR_BITMAPS_STARTADDR + 1024, TEST_DIR "\\case_181x185_PALETTEDARGB8_index.raw");

    Display_Start(s_pHalContext);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoCmd_setBitmap(s_pHalContext, DDR_BITMAPS_STARTADDR + 1024, PALETTEDARGB8, ImgW, ImgH);
    EVE_CoDl_paletteSource(s_pHalContext, DDR_BITMAPS_STARTADDR);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width / 2 - ImgW / 2) * 16, (s_pHalContext->Height / 2 - ImgH / 2) * 16);
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);

    SAMAPP_DELAY;
}

void SAMAPP_Bitmap() 
{
    SAMAPP_Bitmap_getImage();
    SAMAPP_Bitmap_dithering();
    SAMAPP_Bitmap_ASTCLayoutRAMG();
    SAMAPP_Bitmap_ASTCMultiCellRAMG();
    SAMAPP_Bitmap_rotate();
    SAMAPP_Bitmap_rotateAndTranslate();
    SAMAPP_Bitmap_loadImagefromSD();
    SAMAPP_Bitmap_loadImagefromFlash();
    SAMAPP_Bitmap_loadImage();
    SAMAPP_Bitmap_loadImageMono();
    SAMAPP_Bitmap_loadImageFullColor();
    SAMAPP_Bitmap_DXT1();
    SAMAPP_Bitmap_DXT1L2();
    SAMAPP_Bitmap_YCBCR();
    SAMAPP_Bitmap_LA1();
    SAMAPP_Bitmap_LA2();
    SAMAPP_Bitmap_LA4();
    SAMAPP_Bitmap_LA8();
    SAMAPP_Bitmap_RGB6();
    SAMAPP_Bitmap_RGB8();
    SAMAPP_Bitmap_ARGB6();
    SAMAPP_Bitmap_ARGB8();
    SAMAPP_Bitmap_PALETTEDARGB8();
}


