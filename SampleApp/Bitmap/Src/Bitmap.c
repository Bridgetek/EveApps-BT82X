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

#define MAX_ANGLE                   360
#define MAX_CIRCLE_UNIT             65536
#define SAMAPP_DELAY_MS             3000
#define SAMAPP_DELAY                EVE_sleep(SAMAPP_DELAY_MS)
#define BITMAP_DELAY_MS             10
#define BITMAP_TITLE_FONT           31
#define BITMAP_LOAD_ADDR            RAM_G
#define BITMAP_ASTC_CELL_X          20
#define BITMAP_ASTC_CELL_Y          20
#define BITMAP_ASTC_CELL_X_INC      20
#define BITMAP_ASTC_CELL_Y_INC      10
#define BITMAP_ASTC_CELL_MARGIN     10
#define BITMAP_ASTC_CELL_CELL       2
#define BITMAP_ASTC_TITLE_W         30
#define BITMAP_ASTC_CELL_TITLE_FONT 16
#define BITMAP_ASTC_FU_X            20
#define BITMAP_ASTC_FU_Y            50
#define BITMAP_ASTC_FU_MARGIN       5
#define BITMAP_ASTC_FU_CELL1        10
#define BITMAP_ASTC_FU_CELL2        16
#define BITMAP_MANDRILL_ROTATE_XY   100

static EVE_HalContext s_halContext;
static EVE_HalContext* s_pHalContext;
static void SAMAPP_Bitmap();
static uint32_t addr_flash = 0;

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

    addr_flash = Flash_Init(s_pHalContext, FLASH_FILE);
    EVE_Util_clearScreen(s_pHalContext);

    char *info[] =
    {  "EVE Sample Application",
       "This sample demonstrates the use of bitmap",
       "",
       ""
    };
    WelcomeScreen(s_pHalContext, info);

    SAMAPP_Bitmap();

    EVE_Util_clearScreen(s_pHalContext);
    Gpu_Release(s_pHalContext);
    return 0;
}

// Helper to draw a bitmap centered on the screen
static void helperDrawBitmapCentered(EVE_HalContext* phost, uint32_t addr, uint32_t format, uint16_t w, uint16_t h) 
{
    Display_Start(phost, (uint8_t[]){255, 255, 255}, (uint8_t[]){255, 255, 255}, 0, 4);
    EVE_CoCmd_setBitmap(phost, addr, format, w, h);
    EVE_CoDl_begin(phost, BITMAPS);
    EVE_CoDl_vertex2f_4(phost, (phost->Width - w) / 2 * 16, (phost->Height - h) / 2 * 16);
    EVE_CoDl_end(phost);
    Display_End(phost);
}

/**
 * @brief API to demonstrate CMD_GETIMAGE
 *
 * Loads and displays an image, then retrieves its properties using CMD_GETIMAGE.
 *
 * @param None
 * @return None
 */
static void SAMAPP_Bitmap_getImage()
{
    uint32_t source, fmt, w, h, palette;

    Draw_Text(s_pHalContext, "Example for: CMD_GETIMAGE");

    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_Util_loadImageFile(s_pHalContext, BITMAP_LOAD_ADDR, TEST_DIR BITMAP_SUNFLOWER, NULL, 0);

    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width - BITMAP_SUNFLOWER_W) / 2 * 16, (s_pHalContext->Height - BITMAP_SUNFLOWER_H) / 2 * 16);
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);

    // Now getImage properties
    EVE_CoCmd_getImage(s_pHalContext, &source, &fmt, &w, &h, &palette);
    eve_printf_debug("Loaded image: source: %u, format: %u, width: %u, height: %u, palette: %u\n",
        source, fmt, w, h, palette);

    SAMAPP_DELAY;
}

/**
 * @brief API to demonstrate CMD_LOADIMAGE w/o OPT_DITHER
 *
 * For PNG decoder, the dithering is enabled/disabled and the result is displayed.
 *
 * @param None
 * @return None
 */
static void SAMAPP_Bitmap_dithering()
{
    uint16_t otp = 0;

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

        EVE_Util_loadImageFile(s_pHalContext, BITMAP_LOAD_ADDR, TEST_DIR BITMAP_DITHER, NULL, otp);
        helperDrawBitmapCentered(s_pHalContext, BITMAP_LOAD_ADDR, RGB565, BITMAP_DITHER_W, BITMAP_DITHER_H);

        SAMAPP_DELAY;
    }
}

/**
 * @brief Draw ASTC image on screen
 *
 * @param title Image label
 * @param source Image source address
 * @param fmt Bitmap format
 * @param x Image X position
 * @param y Image Y position
 * @param w Image width
 * @param h Image height
 * @param margin Margin between cells
 * @param numcell Number of cells to draw
 * @return None
 */
static void helperDrawASTC(const char *title, uint32_t source, uint16_t fmt, uint16_t x, uint16_t y,
    uint16_t w, uint16_t h, uint16_t margin, uint16_t numcell)
{
    int t_w = BITMAP_ASTC_TITLE_W;

    if (title != 0)
    {
        EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);
        EVE_CoCmd_text(s_pHalContext, x, y, BITMAP_ASTC_CELL_TITLE_FONT, 0, title);
        EVE_CoDl_colorRgb(s_pHalContext, 255, 255, 255);
    }
    EVE_CoCmd_setBitmap(s_pHalContext, source, fmt, w, h);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    for (int i = 0; i < numcell; i++)
    {
        EVE_CoDl_cell(s_pHalContext, i);
        EVE_CoDl_vertex2f_4(s_pHalContext, (x + t_w + (margin + w) * i) * 16, y * 16);
    }
    EVE_CoDl_end(s_pHalContext);
}

/**
 * @brief API to demonstrate ASTC layout
 *
 * Loads and displays various ASTC bitmap layouts from RAM_G.
 *
 * @param None
 * @return None
 */
static void SAMAPP_Bitmap_ASTCLayoutRAMG()
{
    int16_t x = BITMAP_ASTC_CELL_X;
    int16_t y = BITMAP_ASTC_CELL_Y;

    Draw_Text(s_pHalContext, "Example for: ASTC bitmap on RAM_G");

    EVE_Util_loadRawFile(s_pHalContext, BITMAP_LOAD_ADDR, TEST_DIR BITMAP_ASTC_CELL);

    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    helperDrawASTC("1x1", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, BITMAP_ASTC_CELL_W, BITMAP_ASTC_CELL_H, BITMAP_ASTC_CELL_MARGIN, BITMAP_ASTC_CELL_CELL);
    y += 3 * BITMAP_ASTC_CELL_Y_INC;
    helperDrawASTC("1x2", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, BITMAP_ASTC_CELL_W, 2 * BITMAP_ASTC_CELL_H, BITMAP_ASTC_CELL_MARGIN, BITMAP_ASTC_CELL_CELL);
    y += 3 * BITMAP_ASTC_CELL_Y_INC;
    helperDrawASTC("1x3", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, BITMAP_ASTC_CELL_W, 3 * BITMAP_ASTC_CELL_H, BITMAP_ASTC_CELL_MARGIN, BITMAP_ASTC_CELL_CELL);
    y += 4 * BITMAP_ASTC_CELL_Y_INC;
    helperDrawASTC("1x4", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, BITMAP_ASTC_CELL_W, 4 * BITMAP_ASTC_CELL_H, BITMAP_ASTC_CELL_MARGIN, BITMAP_ASTC_CELL_CELL);
    y += 5 * BITMAP_ASTC_CELL_Y_INC;
    helperDrawASTC("1x5", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, BITMAP_ASTC_CELL_W, 5 * BITMAP_ASTC_CELL_H, BITMAP_ASTC_CELL_MARGIN, BITMAP_ASTC_CELL_CELL);

    x += 4 * BITMAP_ASTC_CELL_X_INC;
    y = BITMAP_ASTC_CELL_Y;
    helperDrawASTC("2x1", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 2 * BITMAP_ASTC_CELL_W, BITMAP_ASTC_CELL_H, BITMAP_ASTC_CELL_MARGIN, BITMAP_ASTC_CELL_CELL);
    y += 3 * BITMAP_ASTC_CELL_Y_INC;
    helperDrawASTC("2x2", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 2 * BITMAP_ASTC_CELL_W, 2 * BITMAP_ASTC_CELL_H, BITMAP_ASTC_CELL_MARGIN, BITMAP_ASTC_CELL_CELL);
    y += 3 * BITMAP_ASTC_CELL_Y_INC;
    helperDrawASTC("2x3", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 2 * BITMAP_ASTC_CELL_W, 3 * BITMAP_ASTC_CELL_H, BITMAP_ASTC_CELL_MARGIN, BITMAP_ASTC_CELL_CELL);
    y += 4 * BITMAP_ASTC_CELL_Y_INC;
    helperDrawASTC("2x4", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 2 * BITMAP_ASTC_CELL_W, 4 * BITMAP_ASTC_CELL_H, BITMAP_ASTC_CELL_MARGIN, BITMAP_ASTC_CELL_CELL);
    y += 5 * BITMAP_ASTC_CELL_Y_INC;
    helperDrawASTC("2x5", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 2 * BITMAP_ASTC_CELL_W, 5 * BITMAP_ASTC_CELL_H, BITMAP_ASTC_CELL_MARGIN, BITMAP_ASTC_CELL_CELL);

    x += 6 * BITMAP_ASTC_CELL_X_INC;
    y = BITMAP_ASTC_CELL_Y;
    helperDrawASTC("3x1", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 3 * BITMAP_ASTC_CELL_W, BITMAP_ASTC_CELL_H, BITMAP_ASTC_CELL_MARGIN, BITMAP_ASTC_CELL_CELL);
    y += 3 * BITMAP_ASTC_CELL_Y_INC;
    helperDrawASTC("3x2", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 3 * BITMAP_ASTC_CELL_W, 2 * BITMAP_ASTC_CELL_H, BITMAP_ASTC_CELL_MARGIN, BITMAP_ASTC_CELL_CELL);
    y += 3 * BITMAP_ASTC_CELL_Y_INC;
    helperDrawASTC("3x3", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 3 * BITMAP_ASTC_CELL_W, 3 * BITMAP_ASTC_CELL_H, BITMAP_ASTC_CELL_MARGIN, BITMAP_ASTC_CELL_CELL);
    y += 4 * BITMAP_ASTC_CELL_Y_INC;
    helperDrawASTC("3x4", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 3 * BITMAP_ASTC_CELL_W, 4 * BITMAP_ASTC_CELL_H, BITMAP_ASTC_CELL_MARGIN, BITMAP_ASTC_CELL_CELL);
    y += 5 * BITMAP_ASTC_CELL_Y_INC;
    helperDrawASTC("3x5", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 3 * BITMAP_ASTC_CELL_W, 5 * BITMAP_ASTC_CELL_H, BITMAP_ASTC_CELL_MARGIN, BITMAP_ASTC_CELL_CELL);

    x += 8 * BITMAP_ASTC_CELL_X_INC;
    y = BITMAP_ASTC_CELL_Y;
    helperDrawASTC("4x1", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 4 * BITMAP_ASTC_CELL_W, BITMAP_ASTC_CELL_H, BITMAP_ASTC_CELL_MARGIN, BITMAP_ASTC_CELL_CELL);
    y += 3 * BITMAP_ASTC_CELL_Y_INC;
    helperDrawASTC("4x2", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 4 * BITMAP_ASTC_CELL_W, 2 * BITMAP_ASTC_CELL_H, BITMAP_ASTC_CELL_MARGIN, BITMAP_ASTC_CELL_CELL);
    y += 3 * BITMAP_ASTC_CELL_Y_INC;
    helperDrawASTC("4x3", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 4 * BITMAP_ASTC_CELL_W, 3 * BITMAP_ASTC_CELL_H, BITMAP_ASTC_CELL_MARGIN, BITMAP_ASTC_CELL_CELL);
    y += 4 * BITMAP_ASTC_CELL_Y_INC;
    helperDrawASTC("4x4", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 4 * BITMAP_ASTC_CELL_W, 4 * BITMAP_ASTC_CELL_H, BITMAP_ASTC_CELL_MARGIN, BITMAP_ASTC_CELL_CELL);
    y += 5 * BITMAP_ASTC_CELL_Y_INC;
    helperDrawASTC("4x5", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 4 * BITMAP_ASTC_CELL_W, 5 * BITMAP_ASTC_CELL_H, BITMAP_ASTC_CELL_MARGIN, BITMAP_ASTC_CELL_CELL);

    x += 10 * BITMAP_ASTC_CELL_X_INC;
    y = BITMAP_ASTC_CELL_Y;
    helperDrawASTC("5x1", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 5 * BITMAP_ASTC_CELL_W, BITMAP_ASTC_CELL_H, BITMAP_ASTC_CELL_MARGIN, BITMAP_ASTC_CELL_CELL);
    y += 3 * BITMAP_ASTC_CELL_Y_INC;
    helperDrawASTC("5x2", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 5 * BITMAP_ASTC_CELL_W, 2 * BITMAP_ASTC_CELL_H, BITMAP_ASTC_CELL_MARGIN, BITMAP_ASTC_CELL_CELL);
    y += 3 * BITMAP_ASTC_CELL_Y_INC;
    helperDrawASTC("5x3", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 5 * BITMAP_ASTC_CELL_W, 3 * BITMAP_ASTC_CELL_H, BITMAP_ASTC_CELL_MARGIN, BITMAP_ASTC_CELL_CELL);
    y += 4 * BITMAP_ASTC_CELL_Y_INC;
    helperDrawASTC("5x4", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 5 * BITMAP_ASTC_CELL_W, 4 * BITMAP_ASTC_CELL_H, BITMAP_ASTC_CELL_MARGIN, BITMAP_ASTC_CELL_CELL);
    y += 5 * BITMAP_ASTC_CELL_Y_INC;
    helperDrawASTC("5x5", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 5 * BITMAP_ASTC_CELL_W, 5 * BITMAP_ASTC_CELL_H, BITMAP_ASTC_CELL_MARGIN, BITMAP_ASTC_CELL_CELL);

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief API to demonstrate multicell ASTC bitmap on RAMG
 *
 * Loads and displays multicell ASTC bitmaps from RAM_G.
 *
 * @param None
 * @return None
 */
static void SAMAPP_Bitmap_ASTCMultiCellRAMG()
{
    int16_t x = BITMAP_ASTC_FU_X;
    int16_t y = BITMAP_ASTC_FU_Y;

    Draw_Text(s_pHalContext, "Example for: Multicell ASTC bitmap on RAM_G");

    EVE_Util_loadRawFile(s_pHalContext, BITMAP_LOAD_ADDR, TEST_DIR BITMAP_ASTC_FU);

    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 0x33, 0x33, 0x33 }, 0, 4);
    EVE_CoCmd_text(s_pHalContext, BITMAP_ASTC_FU_X, y, BITMAP_TITLE_FONT, OPT_FILL,
        "Image of 168x96 (10 cells, 1008 blocks / cell):\n");

    y += BITMAP_ASTC_FU_Y;
    EVE_CoDl_colorRgb(s_pHalContext, 255, 255, 255);
    helperDrawASTC(0, BITMAP_LOAD_ADDR, COMPRESSED_RGBA_ASTC_4x4_KHR, x, y, BITMAP_ASTC_FU_W, BITMAP_ASTC_FU_H / BITMAP_ASTC_FU_CELL1, BITMAP_ASTC_FU_MARGIN, BITMAP_ASTC_FU_CELL1);

    y += 4 * BITMAP_ASTC_FU_Y;
    EVE_CoDl_colorRgb(s_pHalContext, 0x33, 0x33, 0x33);
    EVE_CoCmd_text(s_pHalContext, BITMAP_ASTC_FU_X, y, BITMAP_TITLE_FONT, OPT_FILL,
        "Image of 168x60 (16 cells, 630 blocks / cell):\n");
    y += BITMAP_ASTC_FU_Y;
    EVE_CoDl_colorRgb(s_pHalContext, 255, 255, 255);
    helperDrawASTC(0, BITMAP_LOAD_ADDR, COMPRESSED_RGBA_ASTC_4x4_KHR, x, y, BITMAP_ASTC_FU_W, BITMAP_ASTC_FU_H / BITMAP_ASTC_FU_CELL2, BITMAP_ASTC_FU_MARGIN, BITMAP_ASTC_FU_CELL1);

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}


/**
 * @brief API to demonstrate image rotate by CMD_ROTATEAROUND
 *
 * @param None
 * @return None
 */
static void SAMAPP_Bitmap_rotate()
{
    Draw_Text(s_pHalContext, "Example for: Bitmap rotate");
    EVE_Util_loadImageFile(s_pHalContext, BITMAP_LOAD_ADDR, TEST_DIR BITMAP_MANDRILL, NULL, OPT_RGB565);

    for (uint16_t i = 0; i <= MAX_ANGLE; i++)
    {
        Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 0, 0, 0 }, 0, 4);
        EVE_CoCmd_text(s_pHalContext, s_pHalContext->Width / 2, 0, BITMAP_TITLE_FONT, OPT_CENTERX, "CMD_ROTATEAROUND"); // text info
        EVE_CoDl_colorRgb(s_pHalContext, 255, 255, 255);

        EVE_CoDl_bitmapHandle(s_pHalContext, 0);
        EVE_CoCmd_setBitmap(s_pHalContext, BITMAP_LOAD_ADDR, RGB565, BITMAP_MANDRILL_W, BITMAP_MANDRILL_H);

        EVE_CoDl_begin(s_pHalContext, BITMAPS);
        EVE_CoCmd_loadIdentity(s_pHalContext);
        EVE_CoCmd_rotateAround(s_pHalContext, BITMAP_MANDRILL_W / 2, BITMAP_MANDRILL_H / 2, i * MAX_CIRCLE_UNIT / MAX_ANGLE, MAX_CIRCLE_UNIT);
        EVE_CoCmd_setMatrix(s_pHalContext);
        EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width / 2 - BITMAP_MANDRILL_W / 2) * 16, (s_pHalContext->Height / 2 - BITMAP_MANDRILL_H / 2) * 16);
        EVE_CoDl_end(s_pHalContext);

        EVE_CoCmd_loadIdentity(s_pHalContext);
        EVE_CoCmd_setMatrix(s_pHalContext);

        Display_End(s_pHalContext);
        EVE_sleep(BITMAP_DELAY_MS);
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
static void helperRotateAroundOne(uint32_t address, uint16_t format, uint16_t x, uint16_t y,
    uint16_t w, uint16_t h, uint16_t rotation_angle)
{
    int16_t lw;
    int16_t lh;
    const uint16_t TRANSLATE_XY = BITMAP_MANDRILL_ROTATE_XY;

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
static void helperRotateAndTranslateOne(uint32_t address, uint16_t format, uint16_t x, uint16_t y,
    uint16_t w, uint16_t h, uint16_t rotation_angle)
{
    int16_t lw;
    int16_t lh;
    const uint16_t TRANSLATE_XY = BITMAP_MANDRILL_ROTATE_XY;

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
 * @param None
 * @return None
 */
static void SAMAPP_Bitmap_rotateAndTranslate()
{
    int count = MAX_ANGLE;
    const uint16_t TRANSLATE_XY = BITMAP_MANDRILL_ROTATE_XY;
    static uint16_t rotation_angle = 0;
    uint16_t tile1_x = BITMAP_MANDRILL_ROTATE_XY;
    uint16_t tile1_y = BITMAP_MANDRILL_ROTATE_XY;
    uint16_t tile2_x = (uint16_t)(s_pHalContext->Width / 2);
    uint16_t tile2_y = tile1_y;

    Draw_Text(s_pHalContext, "Example for: Bitmap rotate and translate");
    EVE_Util_loadImageFile(s_pHalContext, BITMAP_LOAD_ADDR, TEST_DIR BITMAP_MANDRILL, NULL, OPT_RGB565);

    while (count--)
    {
        /*Display List start*/
        Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 0, 0, 0 }, 0, 4);
        rotation_angle = (rotation_angle + 1) % MAX_ANGLE;

        EVE_CoCmd_text(s_pHalContext, tile1_x + TRANSLATE_XY / 2, tile1_y, BITMAP_TITLE_FONT, 0, "Rotate by RotateAround");
        EVE_CoCmd_text(s_pHalContext, tile2_x + TRANSLATE_XY / 2, tile2_y, BITMAP_TITLE_FONT, 0, "Rotate by Rotate and Translate");
        EVE_CoDl_colorRgb(s_pHalContext, 255, 255, 255);

        helperRotateAroundOne(BITMAP_LOAD_ADDR, RGB565, tile1_x, tile1_y, BITMAP_MANDRILL_W, BITMAP_MANDRILL_H, rotation_angle);
        helperRotateAndTranslateOne(BITMAP_LOAD_ADDR, RGB565, tile2_x, tile2_y, BITMAP_MANDRILL_W, BITMAP_MANDRILL_H, rotation_angle);

        Display_End(s_pHalContext);
        EVE_sleep(10);
    }
    SAMAPP_DELAY;
}

/**
 * @brief Load image from SD card
 *
 * @param None
 * @return None
 */
static void SAMAPP_Bitmap_loadImagefromSD()
{
    uint32_t result = 0;
    uint32_t ptr = 0;
    uint32_t w;
    uint32_t h;

    Draw_Text(s_pHalContext, "Example for: Load image from SD card");

    result = EVE_CoCmd_sdattach(s_pHalContext, OPT_4BIT | OPT_IS_SD, result);
    eve_printf_debug("SD attach status 0x%x \n", result);
    if (result != 0)
    {
        eve_printf_debug("SD attach failed\n");
        return;
    }

    result = EVE_CoCmd_fssource(s_pHalContext, BITMAP_SD, 0);
    eve_printf_debug("file read status 0x%x \n", result);
    if (result != 0)
    {
        eve_printf_debug("SD read failed\n");
        return;
    }

    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_CoCmd_loadImage(s_pHalContext, BITMAP_LOAD_ADDR, OPT_FS | OPT_TRUECOLOR);

    // Now get image properties
    EVE_CoCmd_getProps(s_pHalContext, &ptr, &w, &h);
    eve_printf_debug("Loaded image: ptr: %u, width: %u, height: %u \n", ptr, w, h);

    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width - w) / 2 * 16, (s_pHalContext->Height - h) / 2 * 16);
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief Load image from flash
 *
 * @param None
 * @return None
 */
static void SAMAPP_Bitmap_loadImagefromFlash()
{
    if (!FlashHelper_SwitchFullMode(s_pHalContext))
    {
        eve_printf("SwitchFullMode failed");
        return;
    }
    Draw_Text(s_pHalContext, "Example for: Load image from Flash");

    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_CoCmd_flashSource(s_pHalContext, addr_flash + BITMAP_FLASH_ADDR);
    EVE_CoCmd_loadImage(s_pHalContext, BITMAP_LOAD_ADDR, OPT_FLASH | OPT_RGB565);

    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width - BITMAP_MANDRILL_W) / 2 * 16, (s_pHalContext->Height - BITMAP_MANDRILL_H) / 2 * 16);
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}


/**
* @brief Load image in YCBCR
 *
 * @param None
 * @return None
*/
static void SAMAPP_Bitmap_loadImage()
{
    Draw_Text(s_pHalContext, "Example for: Load image and display as YCBCR image");

    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_Util_loadImageFile(s_pHalContext, BITMAP_LOAD_ADDR, TEST_DIR BITMAP_MANDRILL, NULL, OPT_YCBCR);

    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width - BITMAP_MANDRILL_W) / 2 * 16, (s_pHalContext->Height - BITMAP_MANDRILL_H) / 2 * 16);
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);

    SAMAPP_DELAY;
}

/**
 * @brief Load image in L8
 *
 * @param None
 * @return None
 */
static void SAMAPP_Bitmap_loadImageMono()
{
    Draw_Text(s_pHalContext, "Example for: Load image and display as monochromic image");

    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_Util_loadImageFile(s_pHalContext, BITMAP_LOAD_ADDR, TEST_DIR BITMAP_MANDRILL, NULL, OPT_MONO);

    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_blendFunc(s_pHalContext, SRC_ALPHA, ZERO);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width - BITMAP_MANDRILL_W) / 2 * 16, (s_pHalContext->Height - BITMAP_MANDRILL_H) / 2 * 16);
    EVE_CoDl_blendFunc_default(s_pHalContext);
    EVE_CoDl_end(s_pHalContext);

    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);
    EVE_CoCmd_text(s_pHalContext, s_pHalContext->Width / 2, 0, BITMAP_TITLE_FONT, OPT_CENTERX, "Display bitmap by jpg decode L8");

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief Load image in full color
 *
 * @param None
 * @return None
 */
static void SAMAPP_Bitmap_loadImageFullColor()
{
    Draw_Text(s_pHalContext, "Example for: Load image full color ");

    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    /* decode the jpeg data */
    EVE_Util_loadImageFile(s_pHalContext, BITMAP_LOAD_ADDR, TEST_DIR BITMAP_MANDRILL, NULL, OPT_TRUECOLOR);

    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_vertex2f_4(s_pHalContext, ((s_pHalContext->Width - BITMAP_MANDRILL_W) / 2 * 16), ((s_pHalContext->Height - BITMAP_MANDRILL_H) / 2 * 16));
    EVE_CoDl_end(s_pHalContext);

    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);
    EVE_CoCmd_text(s_pHalContext, s_pHalContext->Width / 2, 0, BITMAP_TITLE_FONT, OPT_CENTERX, "Display bitmap by jpg decode");

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
 *
 * @param None
 * @return None
 */
static void SAMAPP_Bitmap_YCBCR()
{
    Draw_Text(s_pHalContext, "Example for: YCBCR format");

    EVE_Util_loadRawFile(s_pHalContext, BITMAP_LOAD_ADDR, TEST_DIR BITMAP_MANDRILL_YCBCR);

    helperDrawBitmapCentered(s_pHalContext, BITMAP_LOAD_ADDR, YCBCR, BITMAP_MANDRILL_W, BITMAP_MANDRILL_H);

    SAMAPP_DELAY;
}

/**
 * @brief draw image in LA1
 *
 * For format LA1,  the image width restriction:
 * - 8 pixel aligned
 *
 * @param None
 * @return None
 */
static void SAMAPP_Bitmap_LA1()
{
    Draw_Text(s_pHalContext, "Example for: LA1 format");

    EVE_Util_loadRawFile(s_pHalContext, BITMAP_LOAD_ADDR, TEST_DIR BITMAP_MANDRILL_LA1);

    helperDrawBitmapCentered(s_pHalContext, BITMAP_LOAD_ADDR, LA1, BITMAP_MANDRILL_W, BITMAP_MANDRILL_H);

    SAMAPP_DELAY;
}

/**
 * @brief draw image in LA2
 *
 * For format LA2,  the image width restriction:
 * - 4 pixel aligned
 *
 * @param None
 * @return None
 */
static void SAMAPP_Bitmap_LA2()
{
    Draw_Text(s_pHalContext, "Example for: LA2 format");

    EVE_Util_loadRawFile(s_pHalContext, BITMAP_LOAD_ADDR, TEST_DIR BITMAP_MANDRILL_LA2);

    helperDrawBitmapCentered(s_pHalContext, BITMAP_LOAD_ADDR, LA2, BITMAP_MANDRILL_W, BITMAP_MANDRILL_H);

    SAMAPP_DELAY;
}

/**
 * @brief draw image in LA4
 *
 * For format LA4,  the image width restriction:
 * - 2 pixel aligned
 *
 * @param None
 * @return None
 */
static void SAMAPP_Bitmap_LA4()
{
    Draw_Text(s_pHalContext, "Example for: LA4 format");

    EVE_Util_loadRawFile(s_pHalContext, BITMAP_LOAD_ADDR, TEST_DIR BITMAP_MANDRILL_LA4);

    helperDrawBitmapCentered(s_pHalContext, BITMAP_LOAD_ADDR, LA4, BITMAP_MANDRILL_W, BITMAP_MANDRILL_H);

    SAMAPP_DELAY;
}

/**
 * @brief draw image in LA8
  *
 * @param None
 * @return None
 */
static void SAMAPP_Bitmap_LA8()
{
    Draw_Text(s_pHalContext, "Example for: LA8 format");

    EVE_Util_loadRawFile(s_pHalContext, BITMAP_LOAD_ADDR, TEST_DIR BITMAP_MANDRILL_LA8);

    helperDrawBitmapCentered(s_pHalContext, BITMAP_LOAD_ADDR, LA8, BITMAP_MANDRILL_W, BITMAP_MANDRILL_H);

    SAMAPP_DELAY;
}

/**
 * @brief draw image in RGB6
 * 
 * For format RGB6,  the image width restriction:
 * - 8 pixel aligned
 *
 * @param None
 * @return None
 */
static void SAMAPP_Bitmap_RGB6()
{
    Draw_Text(s_pHalContext, "Example for: RGB6 format");

    EVE_Util_loadRawFile(s_pHalContext, BITMAP_LOAD_ADDR, TEST_DIR BITMAP_MANDRILL_RGB6);

    helperDrawBitmapCentered(s_pHalContext, BITMAP_LOAD_ADDR, RGB6, BITMAP_MANDRILL_W, BITMAP_MANDRILL_H);

    SAMAPP_DELAY;
}

/**
 * @brief draw image in RGB8
 *
 * For format RGB8,  the image width restriction:
 * - 2 pixel aligned
 *
 * @param None
 * @return None
 */
static void SAMAPP_Bitmap_RGB8()
{
    Draw_Text(s_pHalContext, "Example for: RGB8 format");

    EVE_Util_loadRawFile(s_pHalContext, BITMAP_LOAD_ADDR, TEST_DIR BITMAP_MANDRILL_RGB8);

    helperDrawBitmapCentered(s_pHalContext, BITMAP_LOAD_ADDR, RGB8, BITMAP_MANDRILL_W, BITMAP_MANDRILL_H);

    SAMAPP_DELAY;
}

/**
 * @brief draw image in ARGB6
 * 
 * For format ARGB6,  the image width restriction:
 * - 2 pixel aligned
 *
 * @param None
 * @return None
 */
static void SAMAPP_Bitmap_ARGB6()
{
    Draw_Text(s_pHalContext, "Example for: ARGB6 format");

    EVE_Util_loadRawFile(s_pHalContext, BITMAP_LOAD_ADDR, TEST_DIR BITMAP_CASE_ARGB6);

    helperDrawBitmapCentered(s_pHalContext, BITMAP_LOAD_ADDR, ARGB6, BITMAP_CASE_W, BITMAP_CASE_H);

    SAMAPP_DELAY;
}

/**
 * @brief draw image in ARGB8
 *
 * @param None
 * @return None
 */
static void SAMAPP_Bitmap_ARGB8()
{
    Draw_Text(s_pHalContext, "Example for: ARGB8 format");

    EVE_Util_loadRawFile(s_pHalContext, BITMAP_LOAD_ADDR, TEST_DIR BITMAP_CASE_ARGB8);

    helperDrawBitmapCentered(s_pHalContext, BITMAP_LOAD_ADDR, ARGB8, BITMAP_CASE_W, BITMAP_CASE_H);

    SAMAPP_DELAY;
}

/**
 * @brief draw image in palettedARGB8
 *
 * @param None
 * @return None
 */
static void SAMAPP_Bitmap_PALETTEDARGB8()
{
    Draw_Text(s_pHalContext, "Example for: PalettedARGB8 format");

    EVE_Util_loadRawFile(s_pHalContext, BITMAP_LOAD_ADDR, TEST_DIR BITMAP_CASE_P_ARGB8_LUT);
    EVE_Util_loadRawFile(s_pHalContext, BITMAP_LOAD_ADDR + BITMAP_CASE_P_LUT_SZ, TEST_DIR BITMAP_CASE_P_ARGB8_INDEX);

    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoCmd_setBitmap(s_pHalContext, BITMAP_LOAD_ADDR + BITMAP_CASE_P_LUT_SZ, PALETTEDARGB8, BITMAP_CASE_P_W, BITMAP_CASE_P_H);
    EVE_CoDl_paletteSource(s_pHalContext, BITMAP_LOAD_ADDR);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width / 2 - BITMAP_CASE_P_W / 2) * 16, (s_pHalContext->Height / 2 - BITMAP_CASE_P_H / 2) * 16);
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);

    SAMAPP_DELAY;
}

/**
 * @brief Main entry to run all bitmap demos
 *
 * @param None
 * @return None
 */
static void SAMAPP_Bitmap()
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


