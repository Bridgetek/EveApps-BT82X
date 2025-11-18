/**
 * @file Primitives.c
 * @brief Sample usage of primitives drawing
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

#include "Primitives.h"
#include "FileTransfer.h"
#include "FlashHelper.h"
#include "Maths.h"

#define SAMAPP_DELAY_MS          3000
#define SAMAPP_DELAY             EVE_sleep(SAMAPP_DELAY_MS)
#define PRIM_POINT_SIZE          50
#define PRIM_LINE_W              5
#define PRIM_LINE_H              50
#define PRIM_RECT_W              50
#define PRIM_RECT_H              100
#define PRIM_BITMAP_LOAD         RAM_G
#define PRIM_BITMAP_LOC          -50
#define PRIM_BARGRAPH_ARRAY_SIZE 256
#define PRIM_BARGRAPH_W          256
#define PRIM_BARGRAPH_H          256
#define PRIM_STRIP_X1            16
#define PRIM_STRIP_Y1            16
#define PRIM_STRIP_X2            300
#define PRIM_SCISSOR_X           100
#define PRIM_SCISSOR_Y           200
#define PRIM_SCISSOR_W           400
#define PRIM_SCISSOR_H           400
#define PRIM_CUBE_SIZE           200
#define PRIM_LOOP                100
#define PRIM_BALL_SIZE           100
#define PRIM_BALL_EDGE           50
#define PRIM_BALL_SPEED_X        10
#define PRIM_BALL_SPEED_Y        8
#define PRIM_BALL_SHADOW_X       20
#define PRIM_BALL_SHADOW_Y       10
#define PRIM_BALL_POINT_NUM      6
#define PRIM_BALL_LINE_NUM       8
#define PRIM_BALL_SOUND          0x50
#define PRIM_BALL_VOL            100
#define PRIM_DELAY_MS            10
#define PRIM_TEXT_X              200
#define PRIM_TEXT_Y              200
#define PRIM_TEXT_INC            20
#define PRIM_TITLE_FONT          30
#define PRIM_CAL_X               20
#define PRIM_CAL_Y               20
#define PRIM_MAP_LINE_W1         20
#define PRIM_MAP_LINE_W2         15
#define PRIM_MAP_LINE_X1         -10
#define PRIM_MAP_LINE_X2         -1
#define PRIM_MAP_LINE_X3         (s_pHalContext->Width / 2)
#define PRIM_MAP_LINE_Y1         -1
#define PRIM_MAP_LINE_Y2         (s_pHalContext->Height / 2)
#define PRIM_TITLE_X             240
#define PRIM_TITLE_Y             90
#define PRIM_REGION_X            50
#define PRIM_REGION_W            500
#define PRIM_REGION_H            500
#define PRIM_REGION_TEXT_W       5
#define PRIM_REGION_SCISSOR_W    50

static EVE_HalContext s_halContext;
static EVE_HalContext* s_pHalContext;
static void SAMAPP_Primitives();

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
        "This sample demonstrates the use of primitives graphics", 
        "",
        ""
    };
    WelcomeScreen(s_pHalContext, info);

    SAMAPP_Primitives();

    EVE_Util_clearScreen(s_pHalContext);
    Gpu_Release(s_pHalContext);
    return 0;
}

/**
 * @brief helper function linear interpolation
 *
 * @param t deltaTime
 * @param a transform position
 * @param b target position
 * @return float
 */
static float helperLerp(float t, float a, float b)
{
    return (1 - t) * a + t * b;
}

/**
 * @brief helper function Smooth linear interpolation
 *
 * @param t deltaTime
 * @param a transform position
 * @param b target position
 * @return float
 */
static float helperSmoothLerp(float t, float a, float b)
{
    float lt = 3 * t * t - 2 * t * t * t;
    return helperLerp(lt, a, b);
}

/**
 * @brief display few points at various offsets with various colors
 *
 * @param None
 * @return None
 */
static void SAMAPP_Primitives_points()
{
    Draw_Text(s_pHalContext, "Example for: Points");

    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    /* Construct DL of points */
    EVE_CoDl_begin(s_pHalContext, POINTS);
    EVE_CoDl_colorRgb(s_pHalContext, 128, 0, 0);
    EVE_CoDl_pointSize(s_pHalContext, (PRIM_POINT_SIZE / 2) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width / 5) * 16, (s_pHalContext->Height / 2) * 16);

    EVE_CoDl_colorRgb(s_pHalContext, 0, 128, 0);
    EVE_CoDl_pointSize(s_pHalContext, PRIM_POINT_SIZE * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width * 2 / 5) * 16, (s_pHalContext->Height / 2) * 16);

    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 128);
    EVE_CoDl_pointSize(s_pHalContext, (PRIM_POINT_SIZE * 2) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width * 3 / 5) * 16, (s_pHalContext->Height / 2) * 16);

    EVE_CoDl_colorRgb(s_pHalContext, 128, 128, 0);
    EVE_CoDl_pointSize(s_pHalContext, (PRIM_POINT_SIZE * 3) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width * 4 / 5) * 16, (s_pHalContext->Height / 2) * 16);
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief Display Lines on screen
 *
 * @param None
 * @return None
 */
static void SAMAPP_Primitives_lines()
{
    Draw_Text(s_pHalContext, "Example for: Lines");

    int16_t LineHeight = PRIM_LINE_H;
    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_CoDl_begin(s_pHalContext, LINES);
    EVE_CoDl_colorRgb(s_pHalContext, 128, 0, 0);
    EVE_CoDl_lineWidth(s_pHalContext, PRIM_LINE_W * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width / 4) * 16, ((s_pHalContext->Height - LineHeight) / 2) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width / 4) * 16, ((s_pHalContext->Height + LineHeight) / 2) * 16);

    LineHeight = PRIM_LINE_H * 4;
    EVE_CoDl_colorRgb(s_pHalContext, 0, 128, 0);
    EVE_CoDl_lineWidth(s_pHalContext, (PRIM_LINE_W * 2) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width * 2 / 4) * 16, ((s_pHalContext->Height - LineHeight) / 2) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width * 2 / 4) * 16, ((s_pHalContext->Height + LineHeight) / 2) * 16);

    LineHeight = PRIM_LINE_H * 10;
    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 128);
    EVE_CoDl_lineWidth(s_pHalContext, (PRIM_LINE_W * 4) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width * 3 / 4) * 16, ((s_pHalContext->Height - LineHeight) / 2) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width * 3 / 4) * 16, ((s_pHalContext->Height + LineHeight) / 2) * 16);
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief Display rectangles on screen
 *
 * @param None
 * @return None
 */
static void SAMAPP_Primitives_rectangles()
{
    Draw_Text(s_pHalContext, "Example for: Rectangles");

    int16_t RectWidth;
    int16_t RectHeight;

    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_CoDl_begin(s_pHalContext, RECTS);
    RectWidth = PRIM_RECT_W;
    RectHeight = PRIM_RECT_H;
    EVE_CoDl_colorRgb(s_pHalContext, 128, 0, 0);
    EVE_CoDl_lineWidth(s_pHalContext, PRIM_LINE_W * 16); //LINE_WIDTH is used for corner curvature
    EVE_CoDl_vertex2f_4(s_pHalContext, ((s_pHalContext->Width / 4) - (RectWidth / 2)) * 16,
            ((s_pHalContext->Height - RectHeight) / 2) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, ((s_pHalContext->Width / 4) + (RectWidth / 2)) * 16,
            ((s_pHalContext->Height + RectHeight) / 2) * 16);

    RectWidth = PRIM_RECT_W * 2;
    RectHeight = PRIM_RECT_H * 2;
    EVE_CoDl_colorRgb(s_pHalContext, 0, 128, 0);
    EVE_CoDl_lineWidth(s_pHalContext, (PRIM_LINE_W * 2) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, ((s_pHalContext->Width * 2 / 4) - (RectWidth / 2)) * 16,
            ((s_pHalContext->Height - RectHeight) / 2) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, ((s_pHalContext->Width * 2 / 4) + (RectWidth / 2)) * 16,
            ((s_pHalContext->Height + RectHeight) / 2) * 16);

    RectWidth = PRIM_RECT_W * 4;
    RectHeight = PRIM_RECT_H * 4;
    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 128);
    EVE_CoDl_lineWidth(s_pHalContext, (PRIM_LINE_W * 8) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, ((s_pHalContext->Width * 3 / 4) - (RectWidth / 2)) * 16,
            ((s_pHalContext->Height - RectHeight) / 2) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, ((s_pHalContext->Width * 3 / 4) + (RectWidth / 2)) * 16,
            ((s_pHalContext->Height + RectHeight) / 2) * 16);
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief Display image at different locations with various colors, -ve offsets, alpha blend
 *
 * @param None
 * @return None
 */
static void SAMAPP_Primitives_bitmap()
{
    Draw_Text(s_pHalContext, "Example for: Display image");

    uint16_t x, y;

    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_Util_loadImageFile(s_pHalContext, PRIM_BITMAP_LOAD, TEST_DIR PRIM_BITMAP, NULL, OPT_RGB565);
    EVE_CoDl_begin(s_pHalContext, BITMAPS); // start drawing bitmaps
    x = ((s_pHalContext->Width / 4) - (PRIM_BITMAP_W / 2));
    y = ((s_pHalContext->Height / 2) - (PRIM_BITMAP_H / 2));
    EVE_CoDl_vertex2f_4(s_pHalContext, x * 16, y * 16);

    EVE_CoDl_colorRgb(s_pHalContext, 255, 64, 64); // red
    x = ((s_pHalContext->Width * 2 / 4) - (PRIM_BITMAP_W / 2));
    y = ((s_pHalContext->Height / 2) - (PRIM_BITMAP_H / 2));
    EVE_CoDl_vertex2f_4(s_pHalContext, x * 16, y * 16);

    EVE_CoDl_colorRgb(s_pHalContext, 64, 180, 64); // green
    x += (PRIM_BITMAP_W / 2);
    y += (PRIM_BITMAP_H / 2);
    EVE_CoDl_vertex2f_4(s_pHalContext, x * 16, y * 16);

    EVE_CoDl_colorRgb(s_pHalContext, 255, 255, 64); // transparent yellow
    EVE_CoDl_colorA(s_pHalContext, 150);
    x += (PRIM_BITMAP_W / 2);
    y += (PRIM_BITMAP_H / 2);
    EVE_CoDl_vertex2f_4(s_pHalContext, x * 16, y * 16);

    EVE_CoDl_colorRgb(s_pHalContext, 255, 255, 255);
    EVE_CoDl_colorA(s_pHalContext, 255);
    EVE_CoDl_vertex2f_4(s_pHalContext, PRIM_BITMAP_LOC * 16, PRIM_BITMAP_LOC * 16); //for -ve coordinates use vertex2f instruction
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief Display a Bar Graph
 *
 * @param None
 * @return None
 */
static void SAMAPP_Primitives_barGraph()
{
    /* Write the data into RAM_G */
    uint8_t Y_Array[PRIM_BARGRAPH_ARRAY_SIZE];
    uint32_t numchunks = 0;
    int32_t String_size;
    uint32_t hoffset = 0;
    uint32_t voffset = 0;
    uint32_t widthaligh;

    Draw_Text(s_pHalContext, "Example for: Display a Bar Graph");

    hoffset = s_pHalContext->Width / 4;
    voffset = (s_pHalContext->Height - PRIM_BARGRAPH_H) / 2; //centre of the screen

    // Draw 1/4 of screen width, and zoom to 2 times larger
    widthaligh = ALIGN_TWO_POWER_N(s_pHalContext->Width / 2, PRIM_BARGRAPH_ARRAY_SIZE);
    numchunks = widthaligh / PRIM_BARGRAPH_ARRAY_SIZE;
    String_size = PRIM_BARGRAPH_ARRAY_SIZE;
    for (uint32_t j = 0; j < numchunks; j++)
    {
        for (int i = 0; i < PRIM_BARGRAPH_ARRAY_SIZE; i++)
        {
            Y_Array[i] = random(128) + 64; //within range
        }
        EVE_Hal_wrMem(s_pHalContext, PRIM_BITMAP_LOAD + j * PRIM_BARGRAPH_ARRAY_SIZE, &Y_Array[0],
            String_size);
    }

    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 128, 0, 0 }, 0, 4);
    EVE_CoDl_bitmapSource(s_pHalContext, PRIM_BITMAP_LOAD);
    EVE_CoDl_bitmapLayout(s_pHalContext, BARGRAPH, PRIM_BARGRAPH_W, 1);
    EVE_CoDl_bitmapSize(s_pHalContext, NEAREST, BORDER, BORDER, PRIM_BARGRAPH_W, PRIM_BARGRAPH_H);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    /* Display text 8x8 at hoffset, voffset location */
    for (uint32_t i = 0; i < numchunks; i++)
    {
        EVE_CoDl_cell(s_pHalContext, i);
        EVE_CoDl_vertex2f_4(s_pHalContext, hoffset * 16, voffset * 16);
        hoffset += PRIM_BARGRAPH_ARRAY_SIZE;
    }

    Display_End(s_pHalContext);
    SAMAPP_DELAY;

    /* drawing of sine wave with rising amplitude */
    String_size = PRIM_BARGRAPH_ARRAY_SIZE;
    for (uint32_t j = 0; j < numchunks; j++)
    {
        for (int i = 0; i < PRIM_BARGRAPH_ARRAY_SIZE; i++)
        {
            int32_t tmpval;
            int16_t tmpidx;
            tmpidx = (int16_t)(i + j * PRIM_BARGRAPH_ARRAY_SIZE);
            tmpval = 128 + ((tmpidx / 4) * Math_Qsin(65536 * tmpidx / 48) / 65536); //within range

            Y_Array[i] = tmpval & 0xff;
        }
        EVE_Hal_wrMem(s_pHalContext, PRIM_BITMAP_LOAD + j * PRIM_BARGRAPH_ARRAY_SIZE, Y_Array, String_size);
    }

    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_CoDl_bitmapSource(s_pHalContext, PRIM_BITMAP_LOAD);
    EVE_CoDl_bitmapLayout(s_pHalContext, BARGRAPH, PRIM_BARGRAPH_W, 1);
    EVE_CoDl_bitmapSize(s_pHalContext, NEAREST, BORDER, BORDER, PRIM_BARGRAPH_W, PRIM_BARGRAPH_H);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);

    /* Display bargraph at hoffset, voffset location */
    voffset = (s_pHalContext->Height - PRIM_BARGRAPH_H) / 2; //centre of the screen
    hoffset = s_pHalContext->Width / 4;
    for (uint32_t i = 0; i < numchunks; i++)
    {
        EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0, 0);
        EVE_CoDl_cell(s_pHalContext, i);
        EVE_CoDl_vertex2f_4(s_pHalContext, hoffset * 16, voffset * 16);
        EVE_CoDl_colorRgb(s_pHalContext, 255, 255, 255);
        EVE_CoDl_vertex2f_4(s_pHalContext, hoffset * 16, (voffset + 4) * 16);
        hoffset += PRIM_BARGRAPH_ARRAY_SIZE;
    }
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief Display Linestrips
 *
 * @param None
 * @return None
 */
static void SAMAPP_Primitives_lineStrips()
{
    Draw_Text(s_pHalContext, "Example for: Linestrips");

    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 168, 64 }, 0, 4);
    EVE_CoDl_begin(s_pHalContext, LINE_STRIP);
    EVE_CoDl_vertex2f_4(s_pHalContext, PRIM_STRIP_X1 * 16, PRIM_STRIP_Y1 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width * 2 / 3) * 16, (s_pHalContext->Height * 2 / 3) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width - PRIM_STRIP_X2) * 16, (s_pHalContext->Height - PRIM_STRIP_Y1) * 16);
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief Display Edgestrips
 *
 * @param None
 * @return None
 */
static void SAMAPP_Primitives_edgeStrips()
{
    Draw_Text(s_pHalContext, "Example for: Edgestrips");

    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 168, 64 }, 0, 4);
    EVE_CoDl_begin(s_pHalContext, EDGE_STRIP_R);
    EVE_CoDl_vertex2f_4(s_pHalContext, PRIM_STRIP_X1 * 16, PRIM_STRIP_Y1 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width * 2 / 3) * 16, (s_pHalContext->Height * 2 / 3) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width - PRIM_STRIP_X2) * 16, (s_pHalContext->Height - PRIM_STRIP_Y1) * 16);
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief Scissor sample
 *
 * @param None
 * @return None
 */
static void SAMAPP_Primitives_scissor()
{
    Draw_Text(s_pHalContext, "Example for: Scissor");

    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_CoDl_scissorXY(s_pHalContext, PRIM_SCISSOR_X, PRIM_SCISSOR_Y); // Scissor rectangle top left
    EVE_CoDl_scissorSize(s_pHalContext, PRIM_SCISSOR_W, PRIM_SCISSOR_H); // Scissor rectangle is 400 x 400 pixels
    EVE_CoDl_clearColorRgb(s_pHalContext, 255, 255, 0); // Clear to yellow
    EVE_CoDl_clear(s_pHalContext, 1, 1, 1);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief Stencil sample
 *
 * @param None
 * @return None
 */
static void SAMAPP_Primitives_stencil()
{
    Draw_Text(s_pHalContext, "Example for: Stencil");

    int16_t PointSize = PRIM_POINT_SIZE * 4;
    int16_t DispBtwPoints = PRIM_POINT_SIZE * 5;

    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_CoDl_stencilOp(s_pHalContext, INCR, INCR);
    EVE_CoDl_pointSize(s_pHalContext, PointSize * 16);
    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 128);
    EVE_CoDl_begin(s_pHalContext, POINTS);
    EVE_CoDl_vertex2f_4(s_pHalContext, ((s_pHalContext->Width - DispBtwPoints) / 2) * 16, (s_pHalContext->Height / 2) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, ((s_pHalContext->Width + DispBtwPoints) / 2) * 16, (s_pHalContext->Height / 2) * 16);
    EVE_CoDl_stencilFunc(s_pHalContext, EQUAL, 2, 255);
    EVE_CoDl_colorRgb(s_pHalContext, 128, 0, 0);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width / 2) * 16, (s_pHalContext->Height / 2) * 16);
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief Display Polygons
 *
 * @param None
 * @return None
 */
static void SAMAPP_Primitives_polygon()
{
    Draw_Text(s_pHalContext, "Example for: Polygons");

    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 0, 0 }, 0, 4);
    EVE_CoDl_stencilOp(s_pHalContext, INCR, INCR);
    EVE_CoDl_colorMask(s_pHalContext, 0, 0, 0, 0); //mask all the colors
    EVE_CoDl_begin(s_pHalContext, EDGE_STRIP_L);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width / 2) * 16, (s_pHalContext->Height / 4) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width * 4 / 5) * 16, (s_pHalContext->Height * 4 / 5) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width / 4) * 16, (s_pHalContext->Height / 2) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width / 2) * 16, (s_pHalContext->Height / 4) * 16);
    EVE_CoDl_end(s_pHalContext);
    EVE_CoDl_colorMask(s_pHalContext, 1, 1, 1, 1); //enable all the colors
    EVE_CoDl_stencilFunc(s_pHalContext, EQUAL, 1, 255);
    EVE_CoDl_begin(s_pHalContext, EDGE_STRIP_L);
    EVE_CoDl_vertex2f_4(s_pHalContext, s_pHalContext->Width * 16, 0);
    EVE_CoDl_vertex2f_4(s_pHalContext, s_pHalContext->Width * 16, s_pHalContext->Height * 16);
    EVE_CoDl_end(s_pHalContext);

    /* Draw lines at the borders to make sure anti aliazing is also done */
    EVE_CoDl_stencilFunc(s_pHalContext, ALWAYS, 0, 255);
    EVE_CoDl_lineWidth(s_pHalContext, 2 * 16);
    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);
    EVE_CoDl_begin(s_pHalContext, LINES);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width / 2) * 16, (s_pHalContext->Height / 4) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width * 4 / 5) * 16, (s_pHalContext->Height * 4 / 5) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width * 4 / 5) * 16, (s_pHalContext->Height * 4 / 5) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width / 4) * 16, (s_pHalContext->Height / 2) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width / 4) * 16, (s_pHalContext->Height / 2) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width / 2) * 16, (s_pHalContext->Height / 4) * 16);
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief Display a cube
 *
 * @param None
 * @return None
 */
static void SAMAPP_Primitives_cube()
{
    Draw_Text(s_pHalContext, "Example for: cube");

    uint32_t points[6 * 5][2], x, y, i, z;
    int16_t xoffset, yoffset, CubeEdgeSz;

    // color vertices
    uint8_t colors[6][3] = { 255,0,0,
        255,128,0,
        255,255,0,
        0,255,0,
        0,0,255,
        128,0,255
    };

    // Cube dimention is of 200*200*200
    CubeEdgeSz = PRIM_CUBE_SIZE;
    xoffset = (s_pHalContext->Width / 2 - CubeEdgeSz); 
    yoffset = ((s_pHalContext->Height - CubeEdgeSz) / 2);
    x = (xoffset + (CubeEdgeSz / 2)); //     xoff+w/2
    y = (yoffset - (CubeEdgeSz / 2)); //     yoff-h/2

    //yz plane (back)
    points[0][0] = x;
    points[0][1] = y;
    points[1][0] = x;
    points[1][1] = y + CubeEdgeSz;
    points[2][0] = x + CubeEdgeSz;
    points[2][1] = y + CubeEdgeSz;
    points[3][0] = x + CubeEdgeSz;
    points[3][1] = y;
    points[4][0] = x;
    points[4][1] = y;

    //yz plane (left)
    points[5][0] = xoffset;
    points[5][1] = yoffset;
    points[6][0] = x;
    points[6][1] = y;
    points[7][0] = x;
    points[7][1] = y + CubeEdgeSz;
    points[8][0] = xoffset;
    points[8][1] = yoffset + CubeEdgeSz;
    points[9][0] = xoffset;
    points[9][1] = yoffset;

    //xz plane(top)
    points[10][0] = xoffset;
    points[10][1] = yoffset;
    points[11][0] = xoffset + CubeEdgeSz;
    points[11][1] = yoffset;
    points[12][0] = x + CubeEdgeSz;
    points[12][1] = y;
    points[13][0] = x;
    points[13][1] = y;
    points[14][0] = xoffset;
    points[14][1] = yoffset;

    //xz plane(bottom)
    points[15][0] = xoffset;
    points[15][1] = yoffset + CubeEdgeSz;
    points[16][0] = xoffset + CubeEdgeSz;
    points[16][1] = yoffset + CubeEdgeSz;
    points[17][0] = x + CubeEdgeSz;
    points[17][1] = y + CubeEdgeSz;
    points[18][0] = x;
    points[18][1] = y + CubeEdgeSz;
    points[19][0] = xoffset;
    points[19][1] = yoffset + CubeEdgeSz;

    //yz plane (right)
    points[20][0] = xoffset + CubeEdgeSz;
    points[20][1] = yoffset + CubeEdgeSz;
    points[21][0] = x + CubeEdgeSz;
    points[21][1] = y + CubeEdgeSz;
    points[22][0] = x + CubeEdgeSz;
    points[22][1] = y;
    points[23][0] = xoffset + CubeEdgeSz;
    points[23][1] = yoffset;
    points[24][0] = xoffset + CubeEdgeSz;
    points[24][1] = yoffset + CubeEdgeSz;

    // xy plane(front)
    points[25][0] = xoffset;
    points[25][1] = yoffset;
    points[26][0] = xoffset + CubeEdgeSz;
    points[26][1] = yoffset;
    points[27][0] = xoffset + CubeEdgeSz;
    points[27][1] = yoffset + CubeEdgeSz;
    points[28][0] = xoffset;
    points[28][1] = yoffset + CubeEdgeSz;
    points[29][0] = xoffset;
    points[29][1] = yoffset;

    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);

    // Draw a cube
    EVE_CoDl_stencilOp(s_pHalContext, INCR, INCR);
    EVE_CoDl_colorA(s_pHalContext, 192);
    for (z = 0; z < 6; z++)
    {
        EVE_CoDl_clear(s_pHalContext, 0, 1, 1);//clear stencil buffer
        EVE_CoDl_colorMask(s_pHalContext, 0, 0, 0, 0);//mask all the colors and draw one surface
        EVE_CoDl_stencilFunc(s_pHalContext, ALWAYS, 0, 255);//stencil function to increment all the values
        EVE_CoDl_begin(s_pHalContext, EDGE_STRIP_L);
        for (i = 0; i < 5; i++)
        {
            EVE_CoDl_vertex2f_4(s_pHalContext, points[z * 5 + i][0] * 16, points[z * 5 + i][1] * 16);
        }
        EVE_CoDl_end(s_pHalContext);
        /* set the color and draw a strip */
        EVE_CoDl_colorMask(s_pHalContext, 1, 1, 1, 1);
        EVE_CoDl_stencilFunc(s_pHalContext, EQUAL, 1, 255);
        EVE_CoDl_colorRgb(s_pHalContext, colors[z][0], colors[z][1], colors[z][2]);
        EVE_CoDl_begin(s_pHalContext, RECTS);
        EVE_CoDl_vertex2f_4(s_pHalContext, xoffset * 16, 0);
        EVE_CoDl_vertex2f_4(s_pHalContext, (xoffset + CubeEdgeSz * 2) * 16, (yoffset + CubeEdgeSz * 2) * 16);
        EVE_CoDl_end(s_pHalContext);
    }
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief draw points followed by lines to create 3d ball kind of effect
 *
 * @param None
 * @return None
 */
static void SAMAPP_Primitives_ballStencil()
{
    Draw_Text(s_pHalContext, "Example for: Display a ball");

    int32_t xball = (s_pHalContext->Width / 2);
    int32_t yball = (s_pHalContext->Height / 2);
    int32_t rball = PRIM_BALL_SIZE;
    int32_t asize;
    int32_t aradius;
    int32_t gridsize = PRIM_BALL_SIZE / 2;
    int32_t asmooth;
    int32_t loopflag = PRIM_LOOP;
    int32_t displ = PRIM_BALL_EDGE;
    int32_t dispr = (s_pHalContext->Width - displ);
    int32_t dispa = PRIM_BALL_EDGE;
    int32_t dispb = (s_pHalContext->Height - dispa);
    int32_t xflag = 1;
    int32_t yflag = 1;

    dispr -= ((dispr - displ) % gridsize);
    dispb -= ((dispb - dispa) % gridsize);
    /* write the play sound */
    EVE_CoCmd_regWrite(s_pHalContext, REG_SOUND, PRIM_BALL_SOUND);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_SOUND, PRIM_BALL_VOL);
    while (loopflag-- )
    {
        if (((xball + rball + PRIM_BALL_SPEED_X) >= dispr) || ((xball - rball - PRIM_BALL_SPEED_X) <= displ))
        {
            xflag ^= 1;
            EVE_CoCmd_regWrite(s_pHalContext, REG_PLAY, 1);
        }
        if (((yball + rball + PRIM_BALL_SPEED_Y) >= dispb) || ((yball - rball - PRIM_BALL_SPEED_Y) <= dispa))
        {
            yflag ^= 1;
            EVE_CoCmd_regWrite(s_pHalContext, REG_PLAY, 1);
        }
        if (xflag)
        {
            xball += PRIM_BALL_SPEED_X;
        }
        else
        {
            xball -= PRIM_BALL_SPEED_X;
        }
        if (yflag)
        {
            yball += PRIM_BALL_SPEED_Y;
        }
        else
        {
            yball -= PRIM_BALL_SPEED_Y;
        }

        Display_Start(s_pHalContext, (uint8_t[]) { 128, 128, 0 }, (uint8_t[]) { 0, 0, 0 }, 0, 4);
        /* draw grid */
        EVE_CoDl_lineWidth(s_pHalContext, 2 * 16);
        EVE_CoDl_begin(s_pHalContext, LINES);
        for (int i = 0; i <= ((dispr - displ) / gridsize); i++)
        {
            EVE_CoDl_vertex2f_4(s_pHalContext, (displ + i * gridsize) * 16, dispa * 16);
            EVE_CoDl_vertex2f_4(s_pHalContext, (displ + i * gridsize) * 16, dispb * 16);
        }
        for (int i = 0; i <= ((dispb - dispa) / gridsize); i++)
        {
            EVE_CoDl_vertex2f_4(s_pHalContext, displ * 16, (dispa + i * gridsize) * 16);
            EVE_CoDl_vertex2f_4(s_pHalContext, dispr * 16, (dispa + i * gridsize) * 16);
        }
        EVE_CoDl_end(s_pHalContext);

        EVE_CoDl_colorMask(s_pHalContext, 0, 0, 0, 0); //mask all the colors
        EVE_CoDl_pointSize(s_pHalContext, rball * 16);
        EVE_CoDl_begin(s_pHalContext, POINTS);
        EVE_CoDl_vertex2f_4(s_pHalContext, xball * 16, yball * 16);
        EVE_CoDl_stencilOp(s_pHalContext, INCR, ZERO);
        EVE_CoDl_stencilFunc(s_pHalContext, GEQUAL, 1, 255);
        /* one side points */

        for (int i = 1; i <= PRIM_BALL_POINT_NUM; i++)
        {
            asize = i * rball * 2 / (PRIM_BALL_POINT_NUM + 1);
            asmooth = (int16_t) helperSmoothLerp(((float) (1.0 * asize / (2 * rball))), 0,
                (float) (2 * rball));

            if (asmooth > rball)
            {
                //change the offset to -ve
                int32_t tempsmooth;
                tempsmooth = asmooth - rball;
                aradius = (rball * rball + tempsmooth * tempsmooth) / (2 * tempsmooth);
                EVE_CoDl_pointSize(s_pHalContext, aradius * 16);
                EVE_CoDl_vertex2f_4(s_pHalContext, (xball - aradius + tempsmooth) * 16, yball * 16);
            }
            else
            {
                int32_t tempsmooth;
                tempsmooth = rball - asmooth;
                aradius = (rball * rball + tempsmooth * tempsmooth) / (2 * tempsmooth);
                EVE_CoDl_pointSize(s_pHalContext, aradius * 16);
                EVE_CoDl_vertex2f_4(s_pHalContext, (xball + aradius - tempsmooth) * 16, yball * 16);
            }
        }

        EVE_CoDl_end(s_pHalContext);
        EVE_CoDl_begin(s_pHalContext, LINES);
        /* draw lines - line should be at least radius diameter */
        for (int i = 1; i <= PRIM_BALL_LINE_NUM; i++)
        {
            asize = (i * rball * 2 / PRIM_BALL_LINE_NUM);
            asmooth = (int16_t) helperSmoothLerp(((float) (1.0 * asize / (2 * rball))), 0,
                (float) (2 * rball));
            EVE_CoDl_lineWidth(s_pHalContext, asmooth * 16);
            EVE_CoDl_vertex2f_4(s_pHalContext, (xball - rball) * 16, (yball - rball) * 16);
            EVE_CoDl_vertex2f_4(s_pHalContext, (xball + rball) * 16, (yball - rball) * 16);
        }
        EVE_CoDl_end(s_pHalContext);

        EVE_CoDl_colorMask(s_pHalContext, 1, 1, 1, 1); // enable all the colors
        EVE_CoDl_stencilFunc(s_pHalContext, ALWAYS, 1, 255);
        EVE_CoDl_stencilOp(s_pHalContext, KEEP, KEEP);
        EVE_CoDl_colorRgb(s_pHalContext, 255, 255, 255);
        EVE_CoDl_pointSize(s_pHalContext, rball * 16);
        EVE_CoDl_begin(s_pHalContext, POINTS);

        EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0); //shadow
        EVE_CoDl_colorA(s_pHalContext, 160);
        EVE_CoDl_vertex2f_4(s_pHalContext, (xball + PRIM_BALL_SHADOW_X) * 16, (yball + PRIM_BALL_SHADOW_Y) * 16);

        EVE_CoDl_colorRgb(s_pHalContext, 255, 255, 255);
        EVE_CoDl_colorA(s_pHalContext, 255);
        EVE_CoDl_vertex2f_4(s_pHalContext, xball * 16, yball * 16);

        EVE_CoDl_colorRgb(s_pHalContext, 255, 0, 0);
        EVE_CoDl_stencilFunc(s_pHalContext, GEQUAL, 1, 1);
        EVE_CoDl_stencilOp(s_pHalContext, KEEP, KEEP);
        EVE_CoDl_vertex2f_4(s_pHalContext, xball * 16, yball * 16);
        EVE_CoDl_end(s_pHalContext);
        Display_End(s_pHalContext);

        EVE_sleep(10);
    }
    SAMAPP_DELAY;
}

/**
 * @brief Additive blending of points
 *
 * @param None
 * @return None
 */
static void SAMAPP_Primitives_additiveBlendPoints()
{
    int32_t hoffset;
    int32_t hoffsetdiff;
    int32_t voffset;
    int32_t flagloop = PRIM_LOOP / 10;
    int32_t hdiff;
    int32_t vdiff;
    int32_t PointSz;

    Draw_Text(s_pHalContext, "Example for: Additive blending of points");

#define MSVC_PI (3.141592653)
    PointSz = s_pHalContext->Width / 100;
    hoffsetdiff = s_pHalContext->Width / 160;
    while (flagloop--)
    {
        Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 20, 91, 20 }, 0, 4);
        EVE_CoDl_blendFunc(s_pHalContext, SRC_ALPHA, DST_ALPHA); //input is source alpha and destination is whole color
        EVE_CoDl_pointSize(s_pHalContext, PointSz * 16);
        EVE_CoDl_begin(s_pHalContext, POINTS);

        /* First 100 random values */
        for (int i = 0; i < 100; i++)
        {
            hoffset = random(s_pHalContext->Width);
            voffset = random(s_pHalContext->Height);
            EVE_CoDl_vertex2f_4(s_pHalContext, hoffset * 16, voffset * 16);
        }

        /* next 800 are sine values of two cycles */
        for (int i = 0; i < 160; i++)
        {
            /* i is x offset, y is sinwave */
            hoffset = i * hoffsetdiff;

            voffset = (s_pHalContext->Height / 2)
                + ((int32_t) (s_pHalContext->Height / 2)
                    * Math_Qsin((uint16_t) (65536 * i / (s_pHalContext->Width / 6)) / 65536));

            EVE_CoDl_vertex2f_4(s_pHalContext, hoffset * 16, voffset * 16);
            for (int j = 0; j < 4; j++)
            {
                hdiff = random(PointSz * 10) - (PointSz * 5);
                vdiff = random(PointSz * 10) - (PointSz * 5);
                EVE_CoDl_vertex2f_4(s_pHalContext, (hoffset + hdiff) * 16, (voffset + vdiff) * 16);
            }
        }

        EVE_CoDl_end(s_pHalContext);
        Display_End(s_pHalContext);

        EVE_sleep(PRIM_DELAY_MS);

    }
    SAMAPP_DELAY;
}

/**
 * @brief usage of additive blending - draw 3 Gs
 *
 * @param None
 * @return None
 */
static void SAMAPP_Primitives_additiveBlendText()
{
    Draw_Text(s_pHalContext, "Example for: Additive blending");

    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 0, 0, 0 }, 0, 4);
    EVE_CoCmd_text(s_pHalContext, PRIM_TEXT_X, PRIM_TEXT_Y, 33, 0, "Bridgetek");
    EVE_CoDl_colorA(s_pHalContext, 128);
    EVE_CoCmd_text(s_pHalContext, PRIM_TEXT_X + PRIM_TEXT_INC, PRIM_TEXT_X + PRIM_TEXT_INC, 33, 0, "Bridgetek");
    EVE_CoDl_colorA(s_pHalContext, 64);
    EVE_CoCmd_text(s_pHalContext, PRIM_TEXT_X + 2 * PRIM_TEXT_INC, PRIM_TEXT_X + 2 * PRIM_TEXT_INC, 33, 0, "Bridgetek");
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief API to demonstrate for CMD_CALIBRATESUB
 *
 * @param None
 * @return None
 */
static void SAMAPP_Primitives_calibratesub()
{
    Draw_Text(s_pHalContext, "Example for: CMD_CALIBRATESUB");

    uint16_t clb_x = PRIM_CAL_X;
    uint16_t clb_y = PRIM_CAL_Y;

    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_Util_loadImageFile(s_pHalContext, PRIM_BITMAP_LOAD, TEST_DIR PRIM_BITMAP_CAL, NULL, OPT_TRUECOLOR);
    //Start drawing bitmap
    EVE_CoDl_saveContext(s_pHalContext);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_vertex2f_4(s_pHalContext, 0, 0);
    EVE_CoDl_end(s_pHalContext);

    EVE_CoDl_colorRgb(s_pHalContext, 0, 255, 255);
    EVE_CoCmd_text(s_pHalContext, s_pHalContext->Width / 2, clb_y + s_pHalContext->Height / 2, PRIM_TITLE_FONT, OPT_CENTER,
        "Please touch on screen");

    EVE_CoCmd_fgColor(s_pHalContext, 0xb90007);
    EVE_CoCmd_calibrateSub(s_pHalContext, clb_x, clb_y, (uint16_t)(s_pHalContext->Width / 2), (uint16_t)(s_pHalContext->Height / 2), 0);

    EVE_CoDl_restoreContext(s_pHalContext);
    EVE_Cmd_waitFlush(s_pHalContext);
}

/**
 * @brief API to demonstrate CMD_TESTCARD
 *
 * @param None
 * @return None
 */
static void SAMAPP_Primitives_testcard()
{
    Draw_Text(s_pHalContext, "Example for: CMD_TESTCARD");

    EVE_CoCmd_testCard(s_pHalContext);
    EVE_Cmd_waitFlush(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief Simple graph
 *
 * @param None
 * @return None
 */
static void SAMAPP_Primitives_simpleMap()
{
    Draw_Text(s_pHalContext, "Example for: A simple map");
    EVE_CoCmd_dlStart(s_pHalContext);
    EVE_CoDl_clearColorRgb(s_pHalContext, 236, 232, 224); //light gray #1
    EVE_CoDl_clear(s_pHalContext, 1, 1, 1); //#2
    EVE_CoDl_colorRgb(s_pHalContext, 170, 157, 136); //medium gray #3
    EVE_CoDl_lineWidth(s_pHalContext, PRIM_MAP_LINE_W1 * 16); //#4
    EVE_CoDl_call(s_pHalContext, 37); //draw the streets //#5
    EVE_CoDl_colorRgb(s_pHalContext, 250, 250, 250); //white //#6
    EVE_CoDl_lineWidth(s_pHalContext, PRIM_MAP_LINE_W2 * 16); //#7
    EVE_CoDl_call(s_pHalContext, 37); //draw the streets //#8
    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0); //#9
    EVE_CoCmd_text(s_pHalContext, PRIM_TITLE_X, PRIM_TITLE_Y, PRIM_TITLE_FONT, 0, "Main street");
    EVE_CoDl_display(s_pHalContext);

    EVE_CoDl_begin(s_pHalContext, LINES);
    EVE_CoDl_vertex2f_4(s_pHalContext, PRIM_MAP_LINE_X1 * 16, PRIM_MAP_LINE_Y1 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, PRIM_MAP_LINE_X3 * 1 / 20 * 16, PRIM_MAP_LINE_Y2 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, PRIM_MAP_LINE_X3 * 3 / 10 * 16, PRIM_MAP_LINE_Y1 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, PRIM_MAP_LINE_X3 * 7 / 20 * 16, PRIM_MAP_LINE_Y2 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, PRIM_MAP_LINE_X3 * 6 / 10 * 16, PRIM_MAP_LINE_Y1 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, PRIM_MAP_LINE_X3 * 13 / 20 * 16, PRIM_MAP_LINE_Y2 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, PRIM_MAP_LINE_X3 * 8 / 10 * 16, PRIM_MAP_LINE_Y1 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, PRIM_MAP_LINE_X3 * 17 / 20 * 16, PRIM_MAP_LINE_Y2 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, PRIM_MAP_LINE_X3 * 9 / 10 * 16, PRIM_MAP_LINE_Y1 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, PRIM_MAP_LINE_X3 * 19 / 20 * 16, PRIM_MAP_LINE_Y2 * 16);

    EVE_CoDl_vertex2f_4(s_pHalContext, PRIM_MAP_LINE_X2 * 16, PRIM_MAP_LINE_Y2 * 2 / 10 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, PRIM_MAP_LINE_X3 * 16, PRIM_MAP_LINE_Y2 * 1 / 10 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, PRIM_MAP_LINE_X2 * 16, PRIM_MAP_LINE_Y2 * 6 / 10 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, PRIM_MAP_LINE_X3 * 16, PRIM_MAP_LINE_Y2 * 5 / 10 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, PRIM_MAP_LINE_X2 * 16, PRIM_MAP_LINE_Y2 * 10 / 10 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, PRIM_MAP_LINE_X3 * 16, PRIM_MAP_LINE_Y2 * 9 / 10 * 16);
    EVE_CoDl_end(s_pHalContext);
    EVE_CoDl_return(s_pHalContext);

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief API to demonstrate the use of region commands
 *
 * @param None
 * @return None
 */
static void SAMAPP_Primitives_region()
{
#define ENABLE_REGION 1
    Draw_Text(s_pHalContext, "Example for: region");
    int32_t y;
    char string[10];
    uint16_t yoffset = 100;
    uint32_t m, p, q;

    Display_Start(s_pHalContext, (uint8_t[]) { 0, 55, 0 }, (uint8_t[]) { 200, 200, 200 }, 0, 4);
    for (uint8_t i = 0; i < 37; i++)
    {
        y = (int32_t)(s_pHalContext->Height * i / 64);
        if (i % 4 == 0)
        {
            string[0] = '\0';
            Gpu_Hal_Dec2Ascii(string, (int32_t)i);
            EVE_CoCmd_text(s_pHalContext, PRIM_REGION_X, y, PRIM_TITLE_FONT, OPT_CENTERY | OPT_RIGHTX, string);
            string[0] = '\0';
            Gpu_Hal_Dec2Ascii(string, y);
            EVE_CoCmd_text(s_pHalContext, PRIM_REGION_W - PRIM_REGION_X, y, PRIM_TITLE_FONT, OPT_CENTERY, string);
            EVE_CoDl_lineWidth(s_pHalContext, 16);
        }
        else
            EVE_CoDl_lineWidth(s_pHalContext, 8);
        EVE_CoDl_begin(s_pHalContext, LINES);
        EVE_CoDl_vertex2f_4(s_pHalContext, (PRIM_REGION_X + PRIM_REGION_TEXT_W) * 16, y * 16);
        EVE_CoDl_vertex2f_4(s_pHalContext, (PRIM_REGION_W - PRIM_REGION_X + PRIM_REGION_TEXT_W) * 16, y * 16);
        EVE_CoDl_end(s_pHalContext);
    }

    EVE_CoCmd_regRead(s_pHalContext, REG_CMD_DL, &m);

    EVE_CoDl_scissorSize(s_pHalContext, PRIM_REGION_SCISSOR_W, PRIM_REGION_H);
    EVE_CoDl_clearColorRgb(s_pHalContext, 0, 128, 128);
    EVE_CoDl_scissorXY(s_pHalContext, PRIM_REGION_X * 3, yoffset);
    EVE_CoDl_clear(s_pHalContext, 1, 1, 1);

    EVE_CoCmd_regRead(s_pHalContext, REG_CMD_DL, &p);

    EVE_CoDl_saveContext(s_pHalContext);
    EVE_CoDl_clearColorRgb(s_pHalContext, 255, 165, 0);
    EVE_CoDl_scissorXY(s_pHalContext, PRIM_REGION_X * 5, yoffset);
    EVE_CoDl_clear(s_pHalContext, 1, 1, 1);
    EVE_CoDl_restoreContext(s_pHalContext);

#if ENABLE_REGION
    EVE_CoCmd_regRead(s_pHalContext, REG_CMD_DL, &q);
    EVE_CoCmd_memWrite(s_pHalContext, RAM_DL + p, 4);
    EVE_CoDl_region(s_pHalContext, 8, 4, q/4); // 8 * 1200 / 64 = 150, (8+4) * 1200 / 64 = 225, so region is from y:150 to y:225
#endif

    EVE_CoDl_scissorXY(s_pHalContext, PRIM_REGION_X * 7, yoffset);
    EVE_CoDl_clear(s_pHalContext, 1, 1, 1);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief Main entry to run all primitive demos
 *
 * @param None
 * @return None
 */
static void SAMAPP_Primitives()
{
    SAMAPP_Primitives_points();
    SAMAPP_Primitives_lines();
    SAMAPP_Primitives_rectangles();
    SAMAPP_Primitives_bitmap();
    SAMAPP_Primitives_barGraph();
    SAMAPP_Primitives_lineStrips();
    SAMAPP_Primitives_edgeStrips();
    SAMAPP_Primitives_scissor();
    SAMAPP_Primitives_stencil();
    SAMAPP_Primitives_polygon();
    SAMAPP_Primitives_cube();
    SAMAPP_Primitives_ballStencil();
    SAMAPP_Primitives_additiveBlendPoints();
    SAMAPP_Primitives_additiveBlendText();
    SAMAPP_Primitives_calibratesub();
    SAMAPP_Primitives_testcard();
    SAMAPP_Primitives_simpleMap();
    SAMAPP_Primitives_region();
}


