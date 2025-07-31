/**
 * @file Touch.c
 * @brief Sample usage of touching
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

#include "Touch.h"
#include "FileTransfer.h"
#include "FlashHelper.h"

static EVE_HalContext s_halContext;
static EVE_HalContext* s_pHalContext;
void SAMAPP_Touch();
static int32_t Volume;

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

    EVE_Util_clearScreen(s_pHalContext);

    char *info[] =
    {  "EVE Sample Application",
        "This sample demonstrate the using of touch", 
        "",
        ""
    }; 

    while (TRUE) {
        WelcomeScreen(s_pHalContext, info);

        SAMAPP_Touch();

        EVE_Util_clearScreen(s_pHalContext);

        Gpu_Release(s_pHalContext);

        /* Init HW Hal for next loop*/
        Gpu_Init(s_pHalContext);
        LVDS_Config(s_pHalContext, YCBCR, MODE_PICTURE);
#if GET_CALIBRATION == 1
        Calibration_Restore(s_pHalContext);
#endif
    }

    return 0;
}

/**
* @brief API to demonstrate moving rectangle
*
* @param BRy Deprecated
* @param MovingRy Moving rectangle y
* @param EndPtReach Is end moving
* @return int16_t Moving rectangle y
*/
static int16_t helperMovingRect(int16_t MovingRy, uint8_t EndPtReach)
{
    if (MovingRy <= 0)
    {
        EndPtReach = 0;
        MovingRy = 1;
    }

    if (EndPtReach == 1 && MovingRy > 0)
        MovingRy -= 1; //the smaller rectangles are moved behind
    else if (EndPtReach == 0)
        MovingRy += 2; //the smaller rectangles are moved forward slightly faster
    return MovingRy;
}

/**
* @brief Calculate rectangle limits and positions
*
* @param context BouncingSquares context
* @param Arrayno Rectangle id
*/
static void helperRectangleCalc(SAMAPP_BouncingSquares_t* context, uint8_t Arrayno)
{
    uint8_t Arr;
    int16_t MovingRy1;
    int16_t leap = 0;

    if (context->RectNo[Arrayno] == 1)
    {
        Arr = Arrayno;
        //the limits for the smaller rectangles forward and backward movement is set here
        if (context->My[Arr] == 0 && (context->My[Arr] + 25) < context->BRy[Arr])
            context->E[Arr] = 0; //inc
        else if (context->My[Arr] + 25 >= context->BRy[Arr])
            context->E[Arr] = 1; //dec

        // the smaller rectangles are moved accordingly according to the flags set above ion this function call
        MovingRy1 = helperMovingRect(context->My[Arr], context->E[Arr]);

        if (context->BRy[Arr] == 0)
            MovingRy1 = 10;
        context->My[Arr] = MovingRy1;

        if (context->My[Arr] > (context->BRy[Arr] - 15))
        {
            leap = context->My[Arr] - context->BRy[Arr];
            context->My[Arr] = context->My[Arr] - (leap + 25);
        }
    }
}

/**
* @brief Beginning BouncingCircle section
*
* @param C1 Point size
* @param R Deprecated
* @param G Deprecated
* @param B Deprecated
*/
static void helperConcentricCircles(float C1, uint8_t R, uint8_t G, uint8_t B)
{
    EVE_CoDl_stencilFunc(s_pHalContext, NEVER, 0x00, 0x00);
    EVE_CoDl_stencilOp(s_pHalContext, INCR, INCR);
    EVE_CoDl_begin(s_pHalContext, POINTS);
    EVE_CoDl_pointSize(s_pHalContext, (uint16_t )((C1 - 5) * 16)); //inner circle
    EVE_CoDl_vertex2f_4(s_pHalContext, 650 * 16, 200 * 16);

    EVE_CoDl_stencilFunc(s_pHalContext, NOTEQUAL, 0x01, 0x01);
    EVE_CoDl_pointSize(s_pHalContext, (uint16_t )((C1) * 16)); //outer circle
    EVE_CoDl_vertex2f_4(s_pHalContext, 650 * 16, 200 * 16);

    EVE_CoDl_stencilFunc(s_pHalContext, EQUAL, 0x01, 0x01);
    EVE_CoDl_stencilOp(s_pHalContext, KEEP, KEEP);
    EVE_CoDl_colorRgb(s_pHalContext, R, G, B);
    EVE_CoDl_pointSize(s_pHalContext, (uint16_t )((C1) * 16));
    EVE_CoDl_vertex2f_4(s_pHalContext, 650 * 16, 200 * 16);

    EVE_CoDl_stencilFunc(s_pHalContext, ALWAYS, 0x01, 0x01);
    EVE_CoDl_stencilOp(s_pHalContext, KEEP, KEEP);

    EVE_CoDl_end(s_pHalContext);
}

/**
* @brief Draw touch points
*
* @param C1X Point X
* @param C1Y Point Y
* @param i Point number
*/
static void helperTouchPoints(int16_t C1X, int16_t C1Y, uint8_t i)
{
    /* Draw the five white circles for the Touch areas with their rescpective numbers*/
    EVE_CoDl_begin(s_pHalContext, POINTS);
    EVE_CoDl_pointSize(s_pHalContext, (40) * 16);
    EVE_CoDl_colorRgb(s_pHalContext, 255, 255, 0);
    EVE_CoDl_vertex2f_4(s_pHalContext, C1X * 16, C1Y * 16);
    EVE_CoDl_colorRgb(s_pHalContext, 155, 155, 0);
    EVE_CoCmd_number(s_pHalContext, C1X, C1Y, 31, OPT_CENTERX | OPT_CENTERY, i);
    EVE_CoDl_end(s_pHalContext);
}

/**
* @brief Draw plots
*
*/
static void helperPlotXY()
{
    uint8_t i = 0;
    uint16_t PlotHt = 0;
    uint16_t PlotWth = 0;
    uint16_t X = 0;
    uint16_t Y = 0;

    PlotHt = (uint16_t) (s_pHalContext->Height / 10);
    PlotWth = (uint16_t) (s_pHalContext->Width / 10);

    EVE_CoDl_colorRgb(s_pHalContext, 36, 54, 125);
    EVE_CoDl_begin(s_pHalContext, LINES);
    /* Horizontal Lines */
    for (i = 1; i < 11; i++)
    {
        Y = i * PlotHt;
        EVE_CoDl_lineWidth(s_pHalContext, 1 * 16);
        EVE_CoDl_vertex2f_4(s_pHalContext, 0, Y * 16);
        EVE_CoDl_vertex2f_4(s_pHalContext, s_pHalContext->Width * 16, Y * 16);
    }
    /* Vertical Lines */
    for (i = 1; i < 11; i++)
    {
        X = i * PlotWth;
        EVE_CoDl_lineWidth(s_pHalContext, 1 * 16);
        EVE_CoDl_vertex2f_4(s_pHalContext, X * 16, 0);
        EVE_CoDl_vertex2f_4(s_pHalContext, X * 16, s_pHalContext->Height * 16);
    }
    EVE_CoDl_end(s_pHalContext);
}

/**
* @brief check which circle has been touched based on the coordinates
*
* @param context BouncingSquares context
* @param val Touch value
* @param TouchNum Touch number
* @param i Circle number
*/
static void helperCheckCircleTouchCood(SAMAPP_BouncingCircles_t* context, int32_t val, uint8_t TouchNum,
    uint8_t i)
{
    float CX = 0;
    float CY = 0;

    uint8_t AllClear = 0;

    if ((val >> 16) == -32768)
    {
        context->TN[TouchNum].F[i] = 0;
        return;
    }

    CX = (float)(val >> 16);
    CY = (float)(val & 0xffff);

    for (int8_t j = 0; j < NO_OF_CIRCLE; j++)
    {
        if (context->TN[TouchNum].F[j] == 0)
        {
            if (AllClear != 10)
                AllClear = j;
        }
        else
            AllClear = 10;
    }

    if (AllClear != 10)
        AllClear = 1;

    if (AllClear == 1 && context->TN[TouchNum].F[i] != 1 && (CX > (context->C1X[i] - 15))
        && (CX < (context->C1X[i] + 15)) && (CY > (context->C1Y[i] - 30))
        && (CY < context->C1Y[i] + 30))
    {
        context->C1X[i] = CX;
        context->C1Y[i] = CY;
        context->TN[TouchNum].F[i] = 1;
    }

    if (context->TN[TouchNum].F[i] == 1)
    {
        context->C1X[i] = CX;
        context->C1Y[i] = CY;
    }
}

/**
* @brief calculate the radius of each circle according to the touch
*
* @param context BouncingSquares context
* @param X Touch X
* @param Y Touch Y
* @param Val Circle number
* @return uint16_t radius of circle
*/
static uint16_t helperCirclePlot(SAMAPP_BouncingCircles_t* context, uint16_t X, uint16_t Y, uint8_t Val)
{
    uint32_t Xsq1[NO_OF_CIRCLE];
    uint32_t Ysq1[NO_OF_CIRCLE];
    Xsq1[Val] = (uint32_t)(X - (s_pHalContext->Width / 2)) * (X - (s_pHalContext->Width / 2));
    Ysq1[Val] = (uint32_t)(Y - (s_pHalContext->Height / 2)) * (Y - (s_pHalContext->Height / 2));
    context->Tsq1[Val] = (float)(Xsq1[Val] + Ysq1[Val]);
    context->Tsq1[Val] = (float)sqrt(context->Tsq1[Val]);
    return (uint16_t) context->Tsq1[Val];
}

/**
* @brief Store touches to context
*
* @param context BouncingSquares context
* @param Touchval Touch value
* @param TouchNo Touch number
*/
static void helperStoreTouch(SAMAPP_BouncingCircles_t* context, int32_t Touchval, uint8_t TouchNo)
{
    if (Touchval >> 16 != -32768)
    {
        context->TouchX[TouchNo] = (float) (Touchval >> 16);
        context->TouchY[TouchNo] = (float) (Touchval & 0xffff);
    }
}

/**
* @brief Linear function
*
* @param p1
* @param p2
* @param t
* @param rate
* @return int16_t
*/
static int16_t helperLinear(float p1, float p2, uint16_t t, uint16_t rate)
{
    float st = (float) t / rate;
    return (int16_t) (p1 + (st * (p2 - p1)));
}

/**
* @brief Beginning MovingPoints section
*
* @param k Point set
* @param i POint number
*/
static void helperColorSelection(int16_t k, int16_t i)
{
    if (k == 0)
    {
        if (i & 1)
            EVE_CoDl_colorRgb(s_pHalContext, 128, 0, 255); //purple
        else
            EVE_CoDl_colorRgb(s_pHalContext, 255, 43, 149); //pink
    }
    if (k == 1)
    {
        if (i & 1)
            EVE_CoDl_colorRgb(s_pHalContext, 255, 0, 0); //red
        else
            EVE_CoDl_colorRgb(s_pHalContext, 0, 255, 0); //green
    }
    if (k == 2)
    {
        if (i & 1)
            EVE_CoDl_colorRgb(s_pHalContext, 255, 128, 64); //orange
        else
            EVE_CoDl_colorRgb(s_pHalContext, 0, 255, 255); //blue
    }
    if (k == 3)
    {
        if (i & 1)
            EVE_CoDl_colorRgb(s_pHalContext, 128, 0, 0); //orange
        else
            EVE_CoDl_colorRgb(s_pHalContext, 255, 255, 128); //blue
    }
}

/**
* @brief Calculate offset movements
*
* @param context MovingPpoints context
* @param TouchNo Touch number
* @param X Touch X
* @param Y Touch Y
* @param t Point number
*/
static void helperPointsCalc(SAMAPP_MovingPoints_t* context, uint8_t* t)
{
    int16_t pointset = 0;
    int16_t tempDeltaX;
    int16_t tempDeltaY;

    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);
    EVE_CoDl_pointSize(s_pHalContext, 50 * 16);
    EVE_CoDl_colorA(s_pHalContext, 120);

    /* For total number of points calculate the offsets of movement */
    for (int16_t k = 0; k < NO_OF_POINTS * 4L; k++)
    {
        pointset = k / NO_OF_POINTS;
        if (t[k] > NO_OF_POINTS)
        {
            context->t[k] = 0;
            context->X[k] = (context->val[pointset] >> 16) & 0xffff;
            context->Y[k] = (context->val[pointset] & 0xffff);
        }

        helperColorSelection(pointset, k);

        if (context->X[k] != -32768)
        {
            tempDeltaX = helperLinear(context->X[k], context->SmallX[pointset], context->t[k],
                NO_OF_POINTS);
            tempDeltaY = helperLinear(context->Y[k], context->SmallY, context->t[k], NO_OF_POINTS);
            EVE_CoDl_vertex2f_4(s_pHalContext, tempDeltaX * 16L, tempDeltaY * 16L);
        }
        t[k]++;
    }
}

/**
* @brief Beginning BouncingPoints section
*
* @param pBInst Blob instance
* @param TouchXY Touch value
*/
static void helperBlobColor(SAMAPP_BlobsInst_t* pBInst, int32_t TouchXY)
{
    uint8_t j = 0;
    // if there is touch store the values
    if ((TouchXY >> 16) != -32768)
    {
        pBInst->blobs[pBInst->CurrIdx].x = (TouchXY >> 16) & 0xffff;
        pBInst->blobs[pBInst->CurrIdx].y = (TouchXY & 0xffff);
    }
    else
    {
        pBInst->blobs[pBInst->CurrIdx].x = OFFSCREEN;
        pBInst->blobs[pBInst->CurrIdx].y = OFFSCREEN;
    }

    //calculate the current index
    pBInst->CurrIdx = (pBInst->CurrIdx + 1) & (NBLOBS - 1);

    EVE_CoDl_begin(s_pHalContext, POINTS);
    EVE_CoDl_colorRgb(s_pHalContext, 60, 166, 117);
    for (uint8_t i = 0; i < NBLOBS; i++)
    {
        // Blobs fade away and swell as they age
        EVE_CoDl_colorA(s_pHalContext, i << 1);

        EVE_CoDl_pointSize(s_pHalContext, (100) + (i << 3));

        // Random color for each blob, keyed from (blob_i + i)
        j = (pBInst->CurrIdx + i) & (NBLOBS - 1);

        // Draw it!
        if (pBInst->blobs[j].x != OFFSCREEN)
            EVE_CoDl_vertex2f_4(s_pHalContext, (pBInst->blobs[j].x) * 16, (pBInst->blobs[j].y) * 16);
    }
    EVE_CoDl_end(s_pHalContext);
}

/**
* @brief Check user touches
*
* @param context BouncingSquares context
* @param Tx1 Touch x position
* @param val1 Multi touch value
*/
static void helperCheckTouch(SAMAPP_BouncingSquares_t* context, int16_t Tx1, int32_t val1)
{
    uint8_t Arrayno = -1;

    // Check which rectangle is being touched according to the coordinates
    if (Tx1 >= 200 && Tx1 <= 300)
        Arrayno = 0;
    if (Tx1 >= 400 && Tx1 <= 500)
        Arrayno = 1;
    if (Tx1 >= 600 && Tx1 <= 700)
        Arrayno = 2;
    if (Tx1 >= 800 && Tx1 <= 900)
        Arrayno = 3;
    if (Tx1 >= 1000 && Tx1 <= 1100)
        Arrayno = 4;

    //Set the flag for the rectangle being touched
    if (Arrayno < 255)
    {
        context->RectNo[Arrayno] = 1;

        //store the vertices of the rectangle selected according to the flag
        if ((val1 >> 16) != -32768)
        {
            context->BRx[Arrayno] = (val1 >> 16) & 0xffff;
            context->BRy[Arrayno] = (val1 & 0xffff);
        }

        //limit the Bigger rectangle's height
        if (context->BRy[Arrayno] <= 60)
            context->BRy[Arrayno] = 60;
    }

    //According to the bigger rectangle values move the smaller rectangles
    for (int i = 0; i < NO_OF_RECTS; i++)
    {
        helperRectangleCalc(context, (uint8_t) i);
    }
}

/**
* @brief touch test
*
* @param Sq Square positions
* @param TouchXY TouchXY value
* @param TouchNo Touch number order
*/
static void helperTouchTest(SAMAPP_Squares_t* Sq, int32_t TouchXY, uint8_t TouchNo)
{
    static int32_t RowNo[5];
    static int32_t ColNo[5];

    if ((TouchXY >> 16) != -32768)
    {
        Sq->x = TouchXY >> 16;
        Sq->y = (TouchXY & 0xffff);
        Volume = (TouchNo + 1) * 51;
        for (uint16_t i = 0; i < s_pHalContext->Width / 66; i++)
        {
            /* find row number*/
            if ((Sq->y > i * 66) && (Sq->y < (i + 1) * 66))
                RowNo[TouchNo] = i;
            if (((Sq->x) > (i * 66)) && ((Sq->x) < ((i + 1) * 66)))
                ColNo[TouchNo] = i;
        }
    }
    else
    {
        RowNo[TouchNo] = -1000;
        ColNo[TouchNo] = -1000;
    }
    EVE_CoDl_colorRgb(s_pHalContext, 255, 255, 255);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_bitmapHandle(s_pHalContext, 0);
    EVE_CoCmd_setBitmap(s_pHalContext, RAM_G, RGB565, 66, 66);
    EVE_CoDl_vertex2f_4(s_pHalContext, (66 * ColNo[TouchNo]) * 16, (66 * RowNo[TouchNo]) * 16);
    EVE_CoDl_end(s_pHalContext);
}

/**
* @brief Beginning BouncingSquares section
*
* @param BRx
* @param BRy
* @param MovingRy
* @param SqNumber
*/
static void helperBouncingSquaresCall(int16_t BRx, int16_t BRy, int16_t MovingRy, uint8_t SqNumber)
{
    int16_t MovingRx;

    int8_t R1 = 0;
    int8_t G1 = 0;
    int8_t B1 = 0;
    int8_t R2 = 0;
    int8_t G2 = 0;
    int8_t B2 = 0;
    MovingRx = BRx;

    if (BRy <= 60)
        BRy = 60;
    if (BRy >= 400)
        BRy = 400;

    //different colours are set for the different rectangles
    if (SqNumber == 0)
    {
        R1 = 63;
        G1 = 72;
        B1 = 204;
        R2 = 0;
        G2 = 255;
        B2 = 255;
    }

    if (SqNumber == 1)
    {
        R1 = 255;
        G1 = 255;
        B1 = 0;
        R2 = 246;
        G2 = 89;
        B2 = 12;
    }

    if (SqNumber == 2)
    {
        R1 = 255;
        G1 = 0;
        B1 = 0;
        R2 = 237;
        G2 = 28;
        B2 = 36;
    }

    if (SqNumber == 3)
    {
        R1 = 131;
        G1 = 171;
        B1 = 9;
        R2 = 8;
        G2 = 145;
        B2 = 76;
    }

    if (SqNumber == 4)
    {
        R1 = 141;
        G1 = 4;
        B1 = 143;
        R2 = 176;
        G2 = 3;
        B2 = 89;
    }

    // Draw the rectanles here
    EVE_CoDl_begin(s_pHalContext, RECTS);
    EVE_CoDl_colorRgb(s_pHalContext, R1, G1, B1);
    EVE_CoDl_lineWidth(s_pHalContext, 10 * 16);

    EVE_CoDl_vertex2f_4(s_pHalContext, BRx * 16, (BRy) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (BRx + 100) * 16, (400) * 16);

    EVE_CoDl_colorRgb(s_pHalContext, R2, G2, B2);
    EVE_CoDl_lineWidth(s_pHalContext, 5 * 16);

    EVE_CoDl_vertex2f_4(s_pHalContext, MovingRx * 16, (MovingRy) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (MovingRx + 100) * 16, (MovingRy + 5) * 16);
    EVE_CoDl_end(s_pHalContext);
}

/**
* @brief Draw set 6
*
*/
void SAMAPP_Touch_touchToPlaySong()
{
#if defined(EVE_SUPPORT_CAPACITIVE)

    Draw_Text(s_pHalContext, "Example for: Touch test\n\n\nPlease touch on screen (1-5 fingers)");
    uint32_t val[5];

    SAMAPP_Squares_t SqCall;

    EVE_Util_loadInflateFile(s_pHalContext, RAM_G, TEST_DIR "\\tile3.bin");
    EVE_CoCmd_regWrite(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_EXTENDED);

    /*Audio*/
    EVE_Util_loadRawFile(s_pHalContext, RAM_G + 66 * 66 *2, TEST_DIR "\\Devil_Ride_30_44100_ulaw.wav");

    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_FREQ, 44100);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_START, RAM_G + 66 * 66 * 2);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_FORMAT, ULAW_SAMPLES);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_LENGTH, 1324958);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_LOOP, 1);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_L_PB, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_R_PB, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_PLAY, 1);

    for (int j = 0; j < 1500; j++)
    {
        Display_Start(s_pHalContext);
        EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);
        EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 50, 31, OPT_CENTER,
            "Touch to play song"); //text info

        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCH0_XY, &val[0]);
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCHA_XY, &val[1]);
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCHB_XY, &val[2]);
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCHC_XY, &val[3]);
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCH4_XY, &val[4]);

        for (int8_t i = 0; i < NO_OF_TOUCH; i++)
        {
            helperTouchTest(&SqCall, (int32_t) val[i], i);
        }
        if ((val[0] == 0x80008000) && (val[1] == 0x80008000) && (val[2] == 0x80008000)
            && (val[3] == 0x80008000) && (val[4] == 0x80008000))
            Volume = 0;
        EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_L_PB, (uint8_t)Volume);
        EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_R_PB, (uint8_t)Volume);
        Display_End(s_pHalContext);
    }
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_L_PB, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_R_PB, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_LENGTH, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_PLAY, 1);

    EVE_CoCmd_regWrite(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_COMPATIBILITY);
#endif
}

/**
* @brief Draw Bouncing squares
*
*/
void SAMAPP_Touch_BouncingSquares()
{
#if defined(EVE_SUPPORT_CAPACITIVE)
    int16_t RectX[5];
    int32_t val[5];

    SAMAPP_BouncingSquares_t context;

    Draw_Text(s_pHalContext, "Example for: Draw Bouncing squares\n\n\nPlease touch on screen (1-5 fingers)");

    //Calculate the X vertices where the five rectangles have to be placed
    for (int i = 1; i < 5; i++)
    {
        RectX[0] = 200;
        RectX[i] = RectX[i - 1] + 200;
    }

    for (int i = 0; i < 5; i++)
    {
        context.BRy[i] = 0;
        context.My[i] = 0;
        context.RectNo[i] = 0;
        context.E[i] = 0;
    }

    EVE_CoCmd_regWrite(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_EXTENDED);
    EVE_sleep(30);
    for (int k = 0; k < 1500; k++)
    {
        /* first touch*/
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCH0_XY, &val[0]);
        /* second touch*/
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCHA_XY, &val[1]);
        /* third touch*/
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCHB_XY, &val[2]);
        /* fourth  touch*/
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCHC_XY, &val[3]);
        /* fifth  touch*/
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCH4_XY, &val[4]);

        Display_Start(s_pHalContext);
        //Check which rectangle is being touched using the coordinates and move the respective smaller rectangle
        for (int8_t i = 0; i < NO_OF_RECTS; i++)
        {
            helperCheckTouch(&context, (val[i] >> 16) & 0xffffL, val[i]);
            helperBouncingSquaresCall(RectX[i], context.BRy[i], context.My[i], i);
        }

        Display_End(s_pHalContext);
        context.Count++;

    }
    EVE_CoCmd_regWrite(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_COMPATIBILITY);
#endif
}
/* End BouncingSquares section */

/**
* @brief Draw Bouncing Circles
*
*/
void SAMAPP_Touch_BouncingCircles()
{
#if defined(EVE_SUPPORT_CAPACITIVE)
    int32_t Touchval[NO_OF_CIRCLE];
    SAMAPP_BouncingCircles_t context;

    Draw_Text(s_pHalContext, "Example for: Draw Bouncing Circles\n\n\nPlease touch on screen (1-5 fingers)");

    EVE_CoCmd_regWrite(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_EXTENDED);
    EVE_sleep(30);
    /* calculate the intital radius of the circles before the touch happens*/
    context.Tsq1[0] = 100;
    context.C1X[0] = 600;
    context.C1Y[0] = 200;
    for (int8_t i = 1; i < NO_OF_CIRCLE; i++)
    {
        context.Tsq1[i] = context.Tsq1[i - 1] + 80;
        context.C1X[i] = context.C1X[i - 1] - 80;
        context.C1Y[i] = 200;
    }

    for (int32_t k = 0; k < 1500; k++)
    {
        Display_Start(s_pHalContext);
        /* values of the five touches are stored here */
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCH0_XY, &Touchval[0]);
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCHA_XY, &Touchval[1]);
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCHB_XY, &Touchval[2]);
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCHC_XY, &Touchval[3]);
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCH4_XY, &Touchval[4]);

        for (int8_t i = 0; i < NO_OF_CIRCLE; i++)
        {
            helperStoreTouch(&context, Touchval[i], i);
        }
        /* The plot is drawn here */
        helperPlotXY();

        /* check which circle has been touched based on the coordinates and store the[0] number of the circle touched*/

        for (int8_t i = 0; i < NO_OF_CIRCLE; i++)
        {
            helperCheckCircleTouchCood(&context, Touchval[0], 0, i);
            helperCheckCircleTouchCood(&context, Touchval[1], 1, i);
            helperCheckCircleTouchCood(&context, Touchval[2], 2, i);
            helperCheckCircleTouchCood(&context, Touchval[3], 3, i);
            helperCheckCircleTouchCood(&context, Touchval[4], 4, i);
        }
        /* calculate the radius of each circle according to the touch of each individual circle */

        for (int8_t i = 0; i < NO_OF_CIRCLE; i++)
        {
            context.Tsq1[i] = (float) helperCirclePlot(&context, (uint16_t) context.C1X[i],
                (uint16_t) context.C1Y[i], i);
        }
        /* with the calculated radius draw the circles as well as the Touch points */

        for (int8_t i = 0; i < (NO_OF_CIRCLE); i++)
        {
            helperConcentricCircles(context.Tsq1[i], 255, 0, 0);
            helperTouchPoints((int16_t) context.C1X[i], (int16_t) context.C1Y[i], i + 1);
        }

        Display_End(s_pHalContext);
    }
    EVE_CoCmd_regWrite(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_COMPATIBILITY);
#endif // capacity EVE
}
/* End BouncingCircle section */

/**
* @brief Draw Bouncing points
*
*/
void SAMAPP_Touch_BouncingPoints()
{
#if defined(EVE_SUPPORT_CAPACITIVE)
    int32_t val[5];
    SAMAPP_BlobsInst_t gBlobsInst[APP_BLOBS_NUMTOUCH];
    SAMAPP_BlobsInst_t* pBInst;

    Draw_Text(s_pHalContext, "Example for: Draw Bouncing points\n\n\nPlease touch on screen (1-5 fingers)");

    EVE_CoCmd_regWrite(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_EXTENDED);
    EVE_sleep(30);
    pBInst = &gBlobsInst[0];

    //set all coordinates to OFFSCREEN position
    for (uint8_t j = 0; j < APP_BLOBS_NUMTOUCH; j++)
    {
        for (uint8_t i = 0; i < NBLOBS; i++)
        {
            pBInst->blobs[i].x = OFFSCREEN;
            pBInst->blobs[i].y = OFFSCREEN;
        }
        pBInst->CurrIdx = 0;
        pBInst++;
    }

    for (uint16_t k = 0; k < 1500; k++)
    {
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCH0_XY, &val[0]);
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCHA_XY, &val[1]);
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCHB_XY, &val[2]);
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCHC_XY, &val[3]);
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCH4_XY, &val[4]);

        Display_StartColor(s_pHalContext, (uint8_t[]) { 43, 73, 59 }, (uint8_t[]) { 255, 255, 255 });
        EVE_CoDl_blendFunc(s_pHalContext, SRC_ALPHA, ONE);
        EVE_CoDl_colorMask(s_pHalContext, 1, 1, 1, 0);

        // draw blobs according to the number of touches
        for (uint16_t j = 0; j < APP_BLOBS_NUMTOUCH; j++)
        {
            helperBlobColor(&gBlobsInst[j], val[j]);
        }

        Display_End(s_pHalContext);
    }

    EVE_CoCmd_regWrite(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_COMPATIBILITY);
#endif // capacity EVE
}
/* End BouncingPoints section */

/**
* @brief Move points
*
*/
void SAMAPP_Touch_MovingPoints()
{
#if defined(EVE_SUPPORT_CAPACITIVE)
    SAMAPP_MovingPoints_t context;
    context.Flag = 1;

    Draw_Text(s_pHalContext, "Example for: Draw Moving points\n\n\nPlease touch on screen (1-4 fingers)");

    EVE_CoCmd_regWrite(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_EXTENDED);
    EVE_sleep(30);
    /* Initialize all coordinates */
    for (uint16_t j = 0; j < 4; j++)
    {
        for (uint16_t i = 0; i < NO_OF_POINTS; i++)
        {
            context.t[i + j * NO_OF_POINTS] = (uint8_t) i;
            context.X[i + j * NO_OF_POINTS] = OFFSCREEN;
            context.Y[i + j * NO_OF_POINTS] = OFFSCREEN;
        }
    }

    context.SmallX[0] = 180;
    context.SmallY = 20;
    for (uint16_t i = 0; i < 5; i++)
    {
        context.SmallX[i + 1] = context.SmallX[i] + 50;
    }

    for (uint16_t k = 0; k < 1500; k++)
    {
        Display_Start(s_pHalContext);

        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCH0_XY, &context.val[0]);
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCHA_XY, &context.val[1]);
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCHB_XY, &context.val[2]);
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCHC_XY, &context.val[3]);
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCH4_XY, &context.val[4]);

        EVE_CoDl_begin(s_pHalContext, POINTS);

        EVE_CoDl_colorRgb(s_pHalContext, 255, 0, 0);
        helperPointsCalc(&context, &context.t[0]);
        EVE_CoDl_end(s_pHalContext);
        Display_End(s_pHalContext);
    }
    EVE_CoCmd_regWrite(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_COMPATIBILITY);
#endif // capacity EVE
}
/* End MovingPoints section */

/**
* @brief Multi touch on a single tracked object can be individually tracked to save the MCU calculations on rotary and linear tracking. Maximum of 5 trackers.
*
*/
void SAMAPP_Touch_multiTracker()
{
#if defined(EVE_SUPPORT_CAPACITIVE)
    uint32_t trackers[5];
    uint32_t delayLoop = 1500;
    uint32_t trackerVal;
    uint8_t tagval;
    uint8_t RDialTag = 100;
    uint8_t GDialTag = 101;
    uint8_t BDialTag = 102;
    uint8_t ADialTag = 103;
    uint8_t DialR = (uint8_t) (s_pHalContext->Width / 10);
    uint8_t rectRed = 0;
    uint8_t rectGreen = 0;
    uint8_t rectBlue = 0;
    uint8_t rectAlpha = 255;

    uint16_t RDialX = DialR + 20;
    uint16_t RDialY = (uint16_t) s_pHalContext->Height / 4;
    uint16_t GDialX = DialR + 20;
    uint16_t GDialY = RDialY * 3;
    uint16_t BDialX = (uint16_t) (s_pHalContext->Width - 20 - DialR);
    uint16_t BDialY = (uint16_t) (s_pHalContext->Height / 4);
    uint16_t ADialX = (uint16_t) (s_pHalContext->Width - 20 - DialR);
    uint16_t ADialY = BDialY * 3;
    uint16_t rectWidth = (uint16_t) (s_pHalContext->Width / 2.6);
    uint16_t rectHeight = (uint16_t) (s_pHalContext->Height / 2);
    uint16_t rectX = (uint16_t) (s_pHalContext->Width / 2 - rectWidth / 2);
    uint16_t rectY = (uint16_t) (s_pHalContext->Height / 2 - rectHeight / 2);
    uint16_t RDialTrackVal = 0;
    uint16_t GDialTrackVal = 0;
    uint16_t BDialTrackVal = 0;
    uint16_t ADialTrackVal = 65535;

    Draw_Text(s_pHalContext, "Example for: Multi touch on a single tracked object\n\n\nPlease touch on screen (1-5 fingers)");

    EVE_CoCmd_track(s_pHalContext, RDialX, RDialY, 1, 1, RDialTag);
    EVE_CoCmd_track(s_pHalContext, GDialX, GDialY, 1, 1, GDialTag);
    EVE_CoCmd_track(s_pHalContext, BDialX, BDialY, 1, 1, BDialTag);
    EVE_CoCmd_track(s_pHalContext, ADialX, ADialY, 1, 1, ADialTag);
    EVE_CoCmd_regWrite(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_EXTENDED);
    while (delayLoop != 0)
    {
        EVE_CoCmd_regRead(s_pHalContext, REG_TRACKER, &trackers[0]);
        EVE_CoCmd_regRead(s_pHalContext, REG_TRACKER_1, &trackers[1]);
        EVE_CoCmd_regRead(s_pHalContext, REG_TRACKER_2, &trackers[2]);
        EVE_CoCmd_regRead(s_pHalContext, REG_TRACKER_3, &trackers[3]);

        for (uint8_t i = 0; i < 4; i++)
        {
            tagval = (trackers[i] & 0xff);
            trackerVal = (trackers[i] >> 16) & 0xffff;
            if (tagval == RDialTag)
            {
                rectRed = (uint8_t) (trackerVal * 255 / 65536);
                RDialTrackVal = (uint16_t) trackerVal;
            }
            else if (tagval == GDialTag)
            {
                rectGreen = (uint8_t) (trackerVal * 255 / 65536);
                GDialTrackVal = (uint16_t) trackerVal;
            }
            else if (tagval == BDialTag)
            {
                rectBlue = (uint8_t) (trackerVal * 255 / 65536);
                BDialTrackVal = (uint16_t) trackerVal;
            }
            else if (tagval == ADialTag)
            {
                rectAlpha = (uint8_t) (trackerVal * 255 / 65536);
                ADialTrackVal = (uint16_t) trackerVal;
            }
        }
        Display_Start(s_pHalContext); // clear screen
        EVE_CoDl_colorA(s_pHalContext, 255);
        EVE_CoDl_tagMask(s_pHalContext, 1);
        EVE_CoDl_tag(s_pHalContext, RDialTag);
        EVE_CoCmd_dial(s_pHalContext, RDialX, RDialY, DialR, 0, RDialTrackVal);
        EVE_CoDl_tag(s_pHalContext, GDialTag);
        EVE_CoCmd_dial(s_pHalContext, GDialX, GDialY, DialR, 0, GDialTrackVal);
        EVE_CoDl_tag(s_pHalContext, BDialTag);
        EVE_CoCmd_dial(s_pHalContext, BDialX, BDialY, DialR, 0, BDialTrackVal);
        EVE_CoDl_tag(s_pHalContext, ADialTag);
        EVE_CoCmd_dial(s_pHalContext, ADialX, ADialY, DialR, 0, ADialTrackVal);
        EVE_CoDl_tagMask(s_pHalContext, 0);

        EVE_CoCmd_text(s_pHalContext, RDialX, RDialY, 28, OPT_CENTER, "Red");//text info
        EVE_CoCmd_text(s_pHalContext, GDialX, GDialY, 28, OPT_CENTER, "Green");//text info
        EVE_CoCmd_text(s_pHalContext, BDialX, BDialY, 28, OPT_CENTER, "Blue");//text info
        EVE_CoCmd_text(s_pHalContext, ADialX, ADialY, 28, OPT_CENTER, "Alpha");//text info

        EVE_CoDl_begin(s_pHalContext, RECTS);
        EVE_CoDl_colorRgb(s_pHalContext, rectRed, rectGreen, rectBlue);
        EVE_CoDl_colorA(s_pHalContext, rectAlpha);
        EVE_CoDl_vertex2f_4(s_pHalContext, rectX * 16, rectY * 16);
        EVE_CoDl_vertex2f_4(s_pHalContext, (rectX + rectWidth) * 16, (rectY + rectHeight) * 16);
        EVE_CoDl_end(s_pHalContext);

        Display_End(s_pHalContext);

        delayLoop--;
    }
    EVE_CoCmd_regWrite(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_COMPATIBILITY);
#endif
}

/**
* @brief explain the usage of touch engine of EVE
* 
*/
void SAMAPP_Touch_touchInfo()
{
    Draw_Text(s_pHalContext, "Example for: Touch raw, touch screen, touch tag\n\n\nPlease touch on screen");

    int32_t LoopFlag = 0;
    int32_t wbutton;
    int32_t hbutton;
    int32_t tagval;
    int32_t tagoption;
    char StringArray[100];
    uint32_t ReadWord;
    uint16_t xvalue;
    uint16_t yvalue;

    /*************************************************************************/
    /* Below code demonstrates the usage of touch function. Display info     */
    /* touch raw, touch screen, touch tag, raw adc and resistance values     */
    /*************************************************************************/
    LoopFlag = 300;
    wbutton = s_pHalContext->Width / 8;
    hbutton = s_pHalContext->Height / 8;
    while (LoopFlag--)
    {
        Display_Start(s_pHalContext);
        EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);
        EVE_CoDl_tagMask(s_pHalContext, 0);
        /* Draw informative text at width/2,20 location */
        StringArray[0] = '\0';
        strcat_s(StringArray, sizeof(StringArray), "Touch Raw XY (");
        EVE_CoCmd_regRead(s_pHalContext, REG_TOUCH_RAW_XY, &ReadWord);
        yvalue = (uint16_t) (ReadWord & 0xffff);
        xvalue = (uint16_t) ((ReadWord >> 16) & 0xffff);
        Gpu_Hal_Dec2Ascii(StringArray, (uint32_t) xvalue);
        strcat_s(StringArray, sizeof(StringArray), ",");
        Gpu_Hal_Dec2Ascii(StringArray, (uint32_t) yvalue);
        strcat_s(StringArray, sizeof(StringArray), ")");
        EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 30, 30, OPT_CENTER,
            StringArray);

        StringArray[0] = '\0';
        strcat_s(StringArray, sizeof(StringArray), "Touch Screen XY (");
        EVE_CoCmd_regRead(s_pHalContext, REG_TOUCH_SCREEN_XY, &ReadWord);
        yvalue = (int16_t) (ReadWord & 0xffff);
        xvalue = (int16_t) ((ReadWord >> 16) & 0xffff);
        Gpu_Hal_Dec2Ascii(StringArray, (int32_t) xvalue);
        strcat_s(StringArray, sizeof(StringArray), ",");
        Gpu_Hal_Dec2Ascii(StringArray, (int32_t) yvalue);
        strcat_s(StringArray, sizeof(StringArray), ")");
        EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 60, 30, OPT_CENTER,
            StringArray);

        StringArray[0] = '\0';
        strcat_s(StringArray, sizeof(StringArray), "Touch TAG (");
        EVE_CoCmd_regRead(s_pHalContext, REG_TOUCH_TAG, &ReadWord);
        Gpu_Hal_Dec2Ascii(StringArray, ReadWord);
        strcat_s(StringArray, sizeof(StringArray), ")");
        EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 90, 30, OPT_CENTER,
            StringArray);
        tagval = ReadWord;

        EVE_CoCmd_fgColor(s_pHalContext, 0x008000);
        EVE_CoDl_tagMask(s_pHalContext, 1);
        tagoption = 0;
        if (12 == tagval)
        {
            tagoption = OPT_FLAT;
        }

        EVE_CoDl_tag(s_pHalContext, 12);
        EVE_CoCmd_button(s_pHalContext, (int16_t) ((s_pHalContext->Width / 4) - (wbutton / 2)),
            (int16_t) ((s_pHalContext->Height * 2 / 4) - (hbutton / 2)), (int16_t) wbutton,
            (int16_t) hbutton, 30, (int16_t) tagoption, "Tag12");
        EVE_CoDl_tag(s_pHalContext, 13);
        tagoption = 0;
        if (13 == tagval)
        {
            tagoption = OPT_FLAT;
        }
        EVE_CoCmd_button(s_pHalContext, (int16_t) ((s_pHalContext->Width * 3 / 4) - (wbutton / 2)),
            (int16_t) ((s_pHalContext->Height * 3 / 4) - (hbutton / 2)), (int16_t) wbutton,
            (int16_t) hbutton, 30, (int16_t) tagoption, "Tag13");

        Display_End(s_pHalContext);
        EVE_sleep(30);
    }
}

/**
* @brief Sample app api to demonstrate track widget funtionality
*
*/
void SAMAPP_Touch_objectTrack()
{
    /*************************************************************************/
    /* Below code demonstrates the usage of track function. Track function   */
    /* tracks the pen touch on any specific object. Track function supports  */
    /* rotary and horizontal/vertical tracks. Rotary is given by rotation    */
    /* angle and horizontal/vertucal track is offset position.               */
    /*************************************************************************/
    int32_t LoopFlag = 0;
    uint32_t TrackRegisterVal = 0;
    uint16_t angleval = 0;
    uint16_t slideval = 0;
    uint16_t scrollval = 0;

    /* Set the tracker for 3 bojects */

    Draw_Text(s_pHalContext, "Example for: Rotary and horizontal/vertical tracks\n\n\nPlease touch on screen");

    EVE_CoCmd_track(s_pHalContext, (int16_t) (s_pHalContext->Width / 2),
        (int16_t) (s_pHalContext->Height / 2), 1, 1, 10);
    EVE_CoCmd_track(s_pHalContext, 80, (int16_t) (s_pHalContext->Height - 80),
        (int16_t) (s_pHalContext->Width - 160), 18, 11);
    EVE_CoCmd_track(s_pHalContext, (int16_t) (s_pHalContext->Width - 80), 80, 18,
        (int16_t) (s_pHalContext->Height - 160), 12);
    /* Wait till coprocessor completes the operation */
    EVE_Cmd_waitFlush(s_pHalContext);

    LoopFlag = 1500;
    /* update the background color continuously for the color change in any of the trackers */
    while (LoopFlag--)
    {
        uint8_t tagval = 0;
        EVE_CoCmd_regRead(s_pHalContext, REG_TRACKER, &TrackRegisterVal);
        tagval = TrackRegisterVal & 0xff;

        if (10 == tagval)
        {
            angleval = TrackRegisterVal >> 16;
        }
        else if (11 == tagval)
        {
            slideval = TrackRegisterVal >> 16;
        }
        else if (12 == tagval)
        {
            scrollval = TrackRegisterVal >> 16;
            if ((scrollval + 65535 / 10) > (9 * 65535 / 10))
            {
                scrollval = (8 * 65535 / 10);
            }
            else if (scrollval < (1 * 65535 / 10))
            {
                scrollval = 0;
            }
            else
            {
                scrollval -= (1 * 65535 / 10);
            }
        }

        /* Display a rotary dial, horizontal slider and vertical scroll */

        int32_t tmpval0;
        int32_t tmpval1;
        int32_t tmpval2;
        uint8_t angval;
        uint8_t sldval;
        uint8_t scrlval;

        tmpval0 = (int32_t) angleval * 255 / 65536;
        tmpval1 = (int32_t) slideval * 255 / 65536;
        tmpval2 = (int32_t) scrollval * 255 / 65536;

        angval = tmpval0 & 0xff;
        sldval = tmpval1 & 0xff;
        scrlval = tmpval2 & 0xff;

        Display_StartColor(s_pHalContext, (uint8_t[]) { angval, sldval, scrlval }, (uint8_t[]) { 255, 255, 255 });

        /* Draw dial with 3d effect */
        EVE_CoCmd_fgColor(s_pHalContext, 0x00ff00);
        EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
        EVE_CoDl_tag(s_pHalContext, 10);
        EVE_CoCmd_dial(s_pHalContext, (int16_t) (s_pHalContext->Width / 2),
            (int16_t) (s_pHalContext->Height / 2), (int16_t) (s_pHalContext->Width / 8), 0,
            angleval);

        /* Draw slider with 3d effect */
        EVE_CoCmd_fgColor(s_pHalContext, 0x00a000);
        EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
        EVE_CoDl_tag(s_pHalContext, 11);
        EVE_CoCmd_slider(s_pHalContext, 80, (int16_t) (s_pHalContext->Height - 80),
            (int16_t) (s_pHalContext->Width - 160), 18, 0, slideval, 65535);

        /* Draw scroll with 3d effect */
        EVE_CoCmd_fgColor(s_pHalContext, 0x00a000);
        EVE_CoCmd_bgColor(s_pHalContext, 0x000080);
        EVE_CoDl_tag(s_pHalContext, 12);
        EVE_CoCmd_scrollbar(s_pHalContext, (int16_t) (s_pHalContext->Width - 80), 80, 18,
            (int16_t) (s_pHalContext->Height - 160), 0, scrollval, (uint16_t) (65535 * 0.2), 65535);

        EVE_CoCmd_fgColor(s_pHalContext, TAG_MASK(0));
        EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0xff, 0xff);
        EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2),
            (int16_t) ((s_pHalContext->Height / 2) + (s_pHalContext->Width / 8) + 20), 30,
            OPT_CENTER, "Rotary track");
        EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2),
            (int16_t) (s_pHalContext->Height - 80 + 40), 30, OPT_CENTER, "Horizontal track");
        EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width - 90), 20, 30, OPT_CENTER,
            "Vertical track");

        Display_End(s_pHalContext);

        EVE_sleep(10);
    }

    /* Set the tracker for 3 bojects */

    EVE_CoCmd_track(s_pHalContext, 240, 136, 0, 0, 10);
    EVE_CoCmd_track(s_pHalContext, 40, 232, 0, 0, 11);
    EVE_CoCmd_track(s_pHalContext, 400, 40, 0, 0, 12);

    /* Wait till coprocessor completes the operation */
    EVE_Cmd_waitFlush(s_pHalContext);
}

void SAMAPP_Touch() {
    SAMAPP_Touch_touchToPlaySong();
    SAMAPP_Touch_BouncingSquares();
    SAMAPP_Touch_BouncingCircles();
    SAMAPP_Touch_BouncingPoints();
    SAMAPP_Touch_MovingPoints();
    SAMAPP_Touch_multiTracker();
    SAMAPP_Touch_touchInfo();
    SAMAPP_Touch_objectTrack();
}


