/**
 * @file Primitives.c
 * @brief Sample usage of primitives drawing
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
#include "Primitives.h"

#define SAMAPP_DELAY EVE_sleep(3000)

static EVE_HalContext s_halContext;
static EVE_HalContext* s_pHalContext;
void SAMAPP_Primitives();

int main(int argc, char* argv[])
{
    s_pHalContext = &s_halContext;
    Gpu_Init(s_pHalContext);
    LVDS_Config(s_pHalContext, YCBCR, MODE_PICTURE);

    // read and store calibration setting
#if !defined(BT8XXEMU_PLATFORM) && GET_CALIBRATION == 1
    EVE_Calibrate(s_pHalContext);
    Calibration_Save(s_pHalContext);
#endif

    EVE_Util_clearScreen(s_pHalContext);

    char *info[] =
    {  "EVE Sample Application",
        "This sample demonstrate the using of primitives graphics", 
        "",
        ""
    }; 

    while (TRUE) {
        WelcomeScreen(s_pHalContext, info);

        SAMAPP_Primitives();

        EVE_Util_clearScreen(s_pHalContext);

        Gpu_Release(s_pHalContext);

        /* Init HW Hal for next loop*/
        Gpu_Init(s_pHalContext);
        LVDS_Config(s_pHalContext, YCBCR, MODE_PICTURE);
#if !defined(BT8XXEMU_PLATFORM) && GET_CALIBRATION == 1
        Calibration_Restore(s_pHalContext);
#endif
    }

    return 0;
}

/**
* @brief linear interpolation
*
* @param t deltaTime
* @param a transform position
* @param b target position
* @return float
*/
float helperLerp(float t, float a, float b)
{
    return (1 - t) * a + t * b;
}

/**
* @brief Smooth linear interpolation
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
*/
void SAMAPP_Primitives_points()
{
    Draw_Text(s_pHalContext, "Example for: Points");

    Display_Start(s_pHalContext);
    /* Construct DL of points */
    EVE_CoDl_begin(s_pHalContext, POINTS);
    EVE_CoDl_colorRgb(s_pHalContext, 128, 0, 0);
    EVE_CoDl_pointSize(s_pHalContext, 25 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width / 5) * 16, (s_pHalContext->Height / 2) * 16);

    EVE_CoDl_colorRgb(s_pHalContext, 0, 128, 0);
    EVE_CoDl_pointSize(s_pHalContext, 50 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width * 2 / 5) * 16, (s_pHalContext->Height / 2) * 16);

    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 128);
    EVE_CoDl_pointSize(s_pHalContext, 100 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width * 3 / 5) * 16, (s_pHalContext->Height / 2) * 16);

    EVE_CoDl_colorRgb(s_pHalContext, 128, 128, 0);
    EVE_CoDl_pointSize(s_pHalContext, 150 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width * 4 / 5) * 16, (s_pHalContext->Height / 2) * 16);
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief Display Lines on screen
*
*/
void SAMAPP_Primitives_lines()
{
    Draw_Text(s_pHalContext, "Example for: Lines");

    int16_t LineHeight = 50;

    Display_Start(s_pHalContext);
    EVE_CoDl_begin(s_pHalContext, LINES);
    EVE_CoDl_colorRgb(s_pHalContext, 128, 0, 0);
    EVE_CoDl_lineWidth(s_pHalContext, 5 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width / 4) * 16, ((s_pHalContext->Height - LineHeight) / 2) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width / 4) * 16, ((s_pHalContext->Height + LineHeight) / 2) * 16);

    LineHeight = 200;
    EVE_CoDl_colorRgb(s_pHalContext, 0, 128, 0);
    EVE_CoDl_lineWidth(s_pHalContext, 10 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width * 2 / 4) * 16, ((s_pHalContext->Height - LineHeight) / 2) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width * 2 / 4) * 16, ((s_pHalContext->Height + LineHeight) / 2) * 16);

    LineHeight = 500;
    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 128);
    EVE_CoDl_lineWidth(s_pHalContext, 20 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width * 3 / 4) * 16, ((s_pHalContext->Height - LineHeight) / 2) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width * 3 / 4) * 16, ((s_pHalContext->Height + LineHeight) / 2) * 16);
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief Display rectangles on screen
*
*/
void SAMAPP_Primitives_rectangles()
{
    Draw_Text(s_pHalContext, "Example for: Rectangles");

    int16_t RectWidth;
    int16_t RectHeight;

    Display_Start(s_pHalContext);
    EVE_CoDl_begin(s_pHalContext, RECTS);
    RectWidth = 50;
    RectHeight = 100;
    EVE_CoDl_colorRgb(s_pHalContext, 128, 0, 0);
    EVE_CoDl_lineWidth(s_pHalContext, 5 * 16); //LINE_WIDTH is used for corner curvature
    EVE_CoDl_vertex2f_4(s_pHalContext, ((s_pHalContext->Width / 4) - (RectWidth / 2)) * 16,
            ((s_pHalContext->Height - RectHeight) / 2) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, ((s_pHalContext->Width / 4) + (RectWidth / 2)) * 16,
            ((s_pHalContext->Height + RectHeight) / 2) * 16);

    RectWidth = 100;
    RectHeight = 200;
    EVE_CoDl_colorRgb(s_pHalContext, 0, 128, 0);
    EVE_CoDl_lineWidth(s_pHalContext, 10 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, ((s_pHalContext->Width * 2 / 4) - (RectWidth / 2)) * 16,
            ((s_pHalContext->Height - RectHeight) / 2) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, ((s_pHalContext->Width * 2 / 4) + (RectWidth / 2)) * 16,
            ((s_pHalContext->Height + RectHeight) / 2) * 16);

    RectWidth = 200;
    RectHeight = 400;
    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 128);
    EVE_CoDl_lineWidth(s_pHalContext, 40 * 16);
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
*/
void SAMAPP_Primitives_bitmap()
{
    Draw_Text(s_pHalContext, "Example for: Display image");

    uint16_t x, y;
    uint16_t imgW = 256;
    uint16_t imgH = 256;
    EVE_Util_loadImageFile(s_pHalContext, DDR_BITMAPS_STARTADDR, TEST_DIR "\\mandrill256.jpg", NULL, OPT_RGB565);

    Display_Start(s_pHalContext);
    EVE_CoCmd_setBitmap(s_pHalContext, DDR_BITMAPS_STARTADDR, RGB565, imgW, imgH);
    EVE_CoDl_begin(s_pHalContext, BITMAPS); // start drawing bitmaps
    x = ((s_pHalContext->Width / 4) - (imgW / 2));
    y = ((s_pHalContext->Height / 2) - (imgH / 2));
    EVE_CoDl_vertex2f_4(s_pHalContext, x * 16, y * 16);

    EVE_CoDl_colorRgb(s_pHalContext, 255, 64, 64); // red
    x = ((s_pHalContext->Width * 2 / 4) - (imgW / 2));
    y = ((s_pHalContext->Height / 2) - (imgH / 2));
    EVE_CoDl_vertex2f_4(s_pHalContext, x * 16, y * 16);

    EVE_CoDl_colorRgb(s_pHalContext, 64, 180, 64); // green
    x += (imgW / 2);
    y += (imgH / 2);
    EVE_CoDl_vertex2f_4(s_pHalContext, x * 16, y * 16);

    EVE_CoDl_colorRgb(s_pHalContext, 255, 255, 64); // transparent yellow
    EVE_CoDl_colorA(s_pHalContext, 150);
    x += (imgW / 2);
    y += (imgH / 2);
    EVE_CoDl_vertex2f_4(s_pHalContext, x * 16, y * 16);

    EVE_CoDl_colorRgb(s_pHalContext, 255, 255, 255);
    EVE_CoDl_colorA(s_pHalContext, 255);
    EVE_CoDl_vertex2f_4(s_pHalContext, -50 * 16, -50 * 16); //for -ve coordinates use vertex2f instruction
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief Display a Bar Graph
*
*/
void SAMAPP_Primitives_barGraph()
{
#define SAMAPP_BARGRAPH_ARRAY_SIZE (256)
    /* Write the data into RAM_G */
    uint8_t Y_Array[SAMAPP_BARGRAPH_ARRAY_SIZE];
    uint32_t numchunks = 0;
    int32_t String_size;
    uint32_t hoffset = 0;
    uint32_t voffset = 0;
    uint32_t widthaligh;

    Draw_Text(s_pHalContext, "Example for: Display a Bar Graph");

    hoffset = s_pHalContext->Width / 4;
    voffset = (s_pHalContext->Height - 256) / 2; //centre of the screen

    // Draw 1/4 of screen width, and zoom to 2 times larger
    widthaligh = ALIGN_TWO_POWER_N(s_pHalContext->Width / 2, SAMAPP_BARGRAPH_ARRAY_SIZE);
    numchunks = widthaligh / SAMAPP_BARGRAPH_ARRAY_SIZE;
    String_size = SAMAPP_BARGRAPH_ARRAY_SIZE;
    for (uint32_t j = 0; j < numchunks; j++)
    {
        for (int i = 0; i < SAMAPP_BARGRAPH_ARRAY_SIZE; i++)
        {
            Y_Array[i] = random(128) + 64; //within range
        }
        EVE_Hal_wrMem(s_pHalContext, RAM_G + j * SAMAPP_BARGRAPH_ARRAY_SIZE, &Y_Array[0],
            String_size);
    }

    Display_Start(s_pHalContext);
    EVE_CoDl_colorRgb(s_pHalContext, 128, 0, 0);
    EVE_CoDl_bitmapSource(s_pHalContext, RAM_G);
    EVE_CoDl_bitmapLayout(s_pHalContext, BARGRAPH, 256, 1);
    EVE_CoDl_bitmapSize(s_pHalContext, NEAREST, BORDER, BORDER, 256, 256);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    /* Display text 8x8 at hoffset, voffset location */
    for (uint32_t i = 0; i < numchunks; i++)
    {
        EVE_CoDl_cell(s_pHalContext, i);
        EVE_CoDl_vertex2f_4(s_pHalContext, hoffset * 16, voffset * 16);
        hoffset += SAMAPP_BARGRAPH_ARRAY_SIZE;
    }

    Display_End(s_pHalContext);
    SAMAPP_DELAY;

    /* drawing of sine wave with rising amplitude */
    String_size = SAMAPP_BARGRAPH_ARRAY_SIZE;
    for (uint32_t j = 0; j < numchunks; j++)
    {
        for (int i = 0; i < SAMAPP_BARGRAPH_ARRAY_SIZE; i++)
        {
            int32_t tmpval;
            int16_t tmpidx;
            tmpidx = (int16_t) (i + j * SAMAPP_BARGRAPH_ARRAY_SIZE);
            tmpval = 128 + ((tmpidx / 4) * Math_Qsin(65536 * tmpidx / 48) / 65536); //within range

            Y_Array[i] = tmpval & 0xff;
        }
        EVE_Hal_wrMem(s_pHalContext, RAM_G + j * SAMAPP_BARGRAPH_ARRAY_SIZE, Y_Array, String_size);
    }

    Display_Start(s_pHalContext);
    EVE_CoDl_bitmapSource(s_pHalContext, RAM_G);
    EVE_CoDl_bitmapLayout(s_pHalContext, BARGRAPH, 256, 1);
    EVE_CoDl_bitmapSize(s_pHalContext, NEAREST, BORDER, BORDER, 256, 256);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);

    /* Display bargraph at hoffset, voffset location */
    voffset = (s_pHalContext->Height - 256) / 2; //centre of the screen
    hoffset = s_pHalContext->Width / 4;
    for (uint32_t i = 0; i < numchunks; i++)
    {
        EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0, 0);
        EVE_CoDl_cell(s_pHalContext, i);
        EVE_CoDl_vertex2f_4(s_pHalContext, hoffset * 16, voffset * 16);
        EVE_CoDl_colorRgb(s_pHalContext, 255, 255, 255);
        EVE_CoDl_vertex2f_4(s_pHalContext, hoffset * 16, (voffset + 4) * 16);
        hoffset += SAMAPP_BARGRAPH_ARRAY_SIZE;
    }
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief Display Linestrips
*
*/
void SAMAPP_Primitives_lineStrips()
{
    Draw_Text(s_pHalContext, "Example for: Linestrips");

    Display_Start(s_pHalContext);
    EVE_CoDl_colorRgb(s_pHalContext, 255, 168, 64);
    EVE_CoDl_begin(s_pHalContext, LINE_STRIP);
    EVE_CoDl_vertex2f_4(s_pHalContext, 16 * 16, 16 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width * 2 / 3) * 16, (s_pHalContext->Height * 2 / 3) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width - 300) * 16, (s_pHalContext->Height - 20) * 16);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief Display Edgestrips
*
*/
void SAMAPP_Primitives_edgeStrips()
{
    Draw_Text(s_pHalContext, "Example for: Edgestrips");

    Display_Start(s_pHalContext);
    EVE_CoDl_colorRgb(s_pHalContext, 255, 168, 64);
    EVE_CoDl_begin(s_pHalContext, EDGE_STRIP_R);
    EVE_CoDl_vertex2f_4(s_pHalContext, 16 * 16, 16 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width * 2 / 3) * 16, (s_pHalContext->Height * 2 / 3) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width - 300) * 16, (s_pHalContext->Height - 20) * 16);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief Scissor sample
*
*/
void SAMAPP_Primitives_scissor()
{
    Draw_Text(s_pHalContext, "Example for: Scissor");

    Display_Start(s_pHalContext);
    EVE_CoDl_scissorXY(s_pHalContext, 100, 200); // Scissor rectangle top left at (100, 200)
    EVE_CoDl_scissorSize(s_pHalContext, 400, 400); // Scissor rectangle is 400 x 400 pixels
    EVE_CoDl_clearColorRgb(s_pHalContext, 255, 255, 0); // Clear to yellow
    EVE_CoDl_clear(s_pHalContext, 1, 1, 1);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief Stencil sample
*
*/
void SAMAPP_Primitives_stencil()
{
    Draw_Text(s_pHalContext, "Example for: Stencil");

    int16_t PointSize = 200;
    int16_t DispBtwPoints = 250;

    Display_Start(s_pHalContext);
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
*/
void SAMAPP_Primitives_polygon()
{
    Draw_Text(s_pHalContext, "Example for: Polygons");

    Display_Start(s_pHalContext); // clear screen
    EVE_CoDl_colorRgb(s_pHalContext, 255, 0, 0);
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
*/
void SAMAPP_Primitives_cube()
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

    // Cube dimention is of 100*100*100
    CubeEdgeSz = 200;
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

    Display_Start(s_pHalContext);

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
*/
void SAMAPP_Primitives_ballStencil()
{
    Draw_Text(s_pHalContext, "Example for: Display a ball");

    int32_t xball = (s_pHalContext->Width / 2);
    int32_t yball = (s_pHalContext->Height / 2);
    int32_t rball = 100;
    int32_t numpoints = 6;
    int32_t numlines = 8;
    int32_t asize;
    int32_t aradius;
    int32_t gridsize = 50;
    int32_t asmooth;
    int32_t loopflag = 1;
    int32_t displ = 50;
    int32_t dispr = (s_pHalContext->Width - displ);
    int32_t dispa = 50;
    int32_t dispb = (s_pHalContext->Height - dispa);
    int32_t xflag = 1;
    int32_t yflag = 1;

    dispr -= ((dispr - displ) % gridsize);
    dispb -= ((dispb - dispa) % gridsize);
    /* write the play sound */
    EVE_CoCmd_regWrite(s_pHalContext, REG_SOUND, 0x50);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_SOUND, 100);
    loopflag = 100;
    while (loopflag-- )
    {
        if (((xball + rball + 10) >= dispr) || ((xball - rball - 10) <= displ))
        {
            xflag ^= 1;
            EVE_CoCmd_regWrite(s_pHalContext, REG_PLAY, 1);
        }
        if (((yball + rball + 8) >= dispb) || ((yball - rball - 8) <= dispa))
        {
            yflag ^= 1;
            EVE_CoCmd_regWrite(s_pHalContext, REG_PLAY, 1);
        }
        if (xflag)
        {
            xball += 10;
        }
        else
        {
            xball -= 10;
        }
        if (yflag)
        {
            yball += 8;
        }
        else
        {
            yball -= 8;
        }

        Display_Start(s_pHalContext);
        EVE_CoDl_clearColorRgb(s_pHalContext, 128, 128, 0);
        EVE_CoDl_clear(s_pHalContext, 1, 1, 1); // clear screen
        EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);
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

        for (int i = 1; i <= numpoints; i++)
        {
            asize = i * rball * 2 / (numpoints + 1);
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
        for (int i = 1; i <= numlines; i++)
        {
            asize = (i * rball * 2 / numlines);
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
        EVE_CoDl_vertex2f_4(s_pHalContext, (xball + 20) * 16, (yball + 10) * 16);

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
* @brief demonstrated display of point and text
*
*/
void SAMAPP_Primitives_string()
{
    int16_t hoffset;
    int16_t voffset;

    Draw_Text(s_pHalContext, "Example for: Display a text");

    voffset = (int16_t) ((s_pHalContext->Height - 74) / 2); //the max height of inbuilt font handle 33
    hoffset = (int16_t) ((s_pHalContext->Width - 4 * 80) / 2);

    Display_Start(s_pHalContext); // clear screen
    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);

    // only support handle 16, 18 20~25
    EVE_CoDl_begin(s_pHalContext, BITMAPS); // start drawing bitmaps
    EVE_CoDl_bitmapHandle(s_pHalContext, 25);
    EVE_CoDl_cell(s_pHalContext, 'B');
    EVE_CoDl_vertex2f_4(s_pHalContext, hoffset * 16, voffset * 16); // ascii B in font 33
    hoffset += 24;
    EVE_CoDl_cell(s_pHalContext, 'R');
    EVE_CoDl_vertex2f_4(s_pHalContext, hoffset * 16, voffset * 16); // ascii R
    hoffset += 26;
    EVE_CoDl_cell(s_pHalContext, 'I');
    EVE_CoDl_vertex2f_4(s_pHalContext, hoffset * 16, voffset * 16); // ascii I
    hoffset += 10;
    EVE_CoDl_cell(s_pHalContext, 'D');
    EVE_CoDl_vertex2f_4(s_pHalContext, hoffset * 16, voffset * 16); // ascii D
    hoffset += 24;
    EVE_CoDl_cell(s_pHalContext, 'G');
    EVE_CoDl_vertex2f_4(s_pHalContext, hoffset * 16, voffset * 16); // ascii G
    hoffset += 24;
    EVE_CoDl_cell(s_pHalContext, 'E');
    EVE_CoDl_vertex2f_4(s_pHalContext, hoffset * 16, voffset * 16); // ascii E
    hoffset += 24;
    EVE_CoDl_cell(s_pHalContext, 'T');
    EVE_CoDl_vertex2f_4(s_pHalContext, hoffset * 16, voffset * 16); // ascii T
    hoffset += 24;
    EVE_CoDl_cell(s_pHalContext, 'E');
    EVE_CoDl_vertex2f_4(s_pHalContext, hoffset * 16, voffset * 16); // ascii E
    hoffset += 24;
    EVE_CoDl_cell(s_pHalContext, 'K');
    EVE_CoDl_vertex2f_4(s_pHalContext, hoffset * 16, voffset * 16); // ascii K
    EVE_CoDl_end(s_pHalContext);

    voffset += 100;
    hoffset = (int16_t)((s_pHalContext->Width - 4 * 80) / 2);
    EVE_CoCmd_text(s_pHalContext, hoffset, voffset, 33, 0, "BRIDGETEK");
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief Additive blending of points - 1000 points
*
*/
void SAMAPP_Primitives_additiveBlendPoints()
{
    int32_t hoffset;
    int32_t hoffsetdiff;
    int32_t voffset;
    int32_t flagloop = 1;
    int32_t hdiff;
    int32_t vdiff;
    int32_t PointSz;

    Draw_Text(s_pHalContext, "Example for: Additive blending of points");

#define MSVC_PI (3.141592653)
    PointSz = s_pHalContext->Width / 100;
    flagloop = 10;
    hoffsetdiff = s_pHalContext->Width / 160;
    while (flagloop--)
    {
        Display_Start(s_pHalContext);
        EVE_CoDl_colorRgb(s_pHalContext, 20, 91, 20); // green color for additive blending
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

        /* next 640 are sine values of two cycles */
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

        EVE_sleep(10);

    }
    SAMAPP_DELAY;
}

/**
* @brief usage of additive blending - draw 3 Gs
*
*/
void SAMAPP_Primitives_additiveBlendText()
{
    Draw_Text(s_pHalContext, "Example for: Additive blending");

    Display_Start(s_pHalContext); // clear screen
    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoCmd_text(s_pHalContext, 200, 200, 33, 0, "Bridgetek");
    EVE_CoDl_colorA(s_pHalContext, 128);
    EVE_CoCmd_text(s_pHalContext, 220, 220, 33, 0, "Bridgetek");
    EVE_CoDl_colorA(s_pHalContext, 64);
    EVE_CoCmd_text(s_pHalContext, 240, 240, 33, 0, "Bridgetek");
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief API to demonstrate for CMD_CALIBRATESUB
*
*/
void SAMAPP_Primitives_calibratesub() {
    Draw_Text(s_pHalContext, "Example for: CMD_CALIBRATESUB");

    uint16_t clb_x = 20;
    uint16_t clb_y = 20;
    uint16_t clb_w = (uint16_t)(s_pHalContext->Width / 2);
    uint16_t clb_h = (uint16_t)(s_pHalContext->Height / 2);
    uint16_t format = ARGB8;
    uint16_t w = 1920;
    uint16_t h = 1200;

    Display_Start(s_pHalContext);
    EVE_Util_loadImageFile(s_pHalContext, DDR_BITMAPS_STARTADDR, TEST_DIR "\\lavender-1920x1200.jpg", NULL, OPT_TRUECOLOR);
    EVE_CoCmd_setBitmap(s_pHalContext, DDR_BITMAPS_STARTADDR, format, w, h);
    //Start drawing bitmap
    EVE_CoDl_saveContext(s_pHalContext);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_vertex2f_4(s_pHalContext, 0, 0);
    EVE_CoDl_end(s_pHalContext);

    EVE_CoDl_colorRgb(s_pHalContext, 0, 255, 255);
    EVE_CoCmd_text(s_pHalContext, clb_x + 260, clb_y + clb_h / 2, 31, 0,
        "Please touch on screen");

    EVE_CoCmd_fgColor(s_pHalContext, 0xb90007);
    EVE_CoCmd_calibrateSub(s_pHalContext, clb_x, clb_y, clb_w, clb_h, 0);

    EVE_CoDl_restoreContext(s_pHalContext);
    EVE_Cmd_waitFlush(s_pHalContext);
}

/**
* @brief API to demonstrate CMD_TESTCARD
*
*/
void SAMAPP_Primitives_testcard() {
    Draw_Text(s_pHalContext, "Example for: CMD_TESTCARD");

    EVE_CoCmd_testCard(s_pHalContext);
    EVE_Cmd_waitFlush(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief Simple graph
*
*/
void SAMAPP_Primitives_simpleMap()
{
    Draw_Text(s_pHalContext, "Example for: A simple map");
    EVE_CoCmd_dlStart(s_pHalContext);
    EVE_CoDl_clearColorRgb(s_pHalContext, 236, 232, 224); //light gray #1
    EVE_CoDl_clear(s_pHalContext, 1, 1, 1); //#2
    EVE_CoDl_colorRgb(s_pHalContext, 170, 157, 136); //medium gray #3
    EVE_CoDl_lineWidth(s_pHalContext, 20 * 16); //#4
    EVE_CoDl_call(s_pHalContext, 37); //draw the streets //#5
    EVE_CoDl_colorRgb(s_pHalContext, 250, 250, 250); //white //#6
    EVE_CoDl_lineWidth(s_pHalContext, 15 * 16); //#7
    EVE_CoDl_call(s_pHalContext, 37); //draw the streets //#8
    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0); //#9
    EVE_CoCmd_text(s_pHalContext, 240, 90, 31, 0, "Main street"); //draw "Main steet" at (240,90)
    EVE_CoDl_display(s_pHalContext);

    EVE_CoDl_begin(s_pHalContext, LINES);
    EVE_CoDl_vertex2f_4(s_pHalContext, -10 * 16, -1 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, 50 * 16, 600 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, 290 * 16, -1 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, 350 * 16, 600 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, 590 * 16, -1 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, 650 * 16, 600 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, 790 * 16, -1 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, 850 * 16, 600 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, 890 * 16, -1 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, 950 * 16, 600 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, 990 * 16, -1 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, 1050 * 16, 600 * 16);

    EVE_CoDl_vertex2f_4(s_pHalContext, -1 * 16, 100 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, 1200 * 16, 50 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, -1 * 16, 300 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, 1200 * 16, 250 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, -1 * 16, 500 * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, 1200 * 16, 450 * 16);
    EVE_CoDl_end(s_pHalContext);
    EVE_CoDl_return(s_pHalContext);

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief API to demonstrate the use of region commands
*
*/
void SAMAPP_Primitives_region()
{
	Draw_Text(s_pHalContext, "Example for: region");

	Display_Start(s_pHalContext);
	EVE_CoDl_scissorSize(s_pHalContext, 50, 500);
	EVE_CoDl_clearColorRgb(s_pHalContext, 0, 128, 128);
	EVE_CoDl_scissorXY(s_pHalContext, 100, 100);
	EVE_CoDl_clear(s_pHalContext, 1, 1, 1);
	EVE_CoDl_region(s_pHalContext, 8, 4, 13); // 8 * 1200 / 64 = 150, (8+4) * 1200 / 64 = 225, so region is from y:150 to y:225
	EVE_CoDl_clearColorRgb(s_pHalContext, 255, 165, 0);
	EVE_CoDl_scissorXY(s_pHalContext, 200, 100);
	EVE_CoDl_clear(s_pHalContext, 1, 1, 1);
	EVE_CoDl_restoreContext(s_pHalContext);
	EVE_CoDl_scissorXY(s_pHalContext, 300, 100);
	EVE_CoDl_clear(s_pHalContext, 1, 1, 1);
	Display_End(s_pHalContext);
	SAMAPP_DELAY;
}

void SAMAPP_Primitives() 
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
    SAMAPP_Primitives_string();
    SAMAPP_Primitives_additiveBlendPoints();
    SAMAPP_Primitives_additiveBlendText();
    SAMAPP_Primitives_calibratesub();
    SAMAPP_Primitives_testcard();
    SAMAPP_Primitives_simpleMap();
	SAMAPP_Primitives_region();
}


