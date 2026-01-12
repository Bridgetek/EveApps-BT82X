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

#define TOUCH_BITMAP_LOAD         RAM_G
#define TOUCH_LOOP                1500
#define TOUCH_DELAY_MS            5
#define TOUCH_TITLE_FONT          31
#define TOUCH_TITLE_Y             50
#define TOUCH_TITLE_Y_INC         50
#define TOUCH_RECT_X              200
#define TOUCH_RECT_Y              100
#define TOUCH_RECT_W              100
#define TOUCH_RECT_H              (s_pHalContext->Height / 2)
#define TOUCH_MRECT_H             5
#define TOUCH_RECT_INC            1
#define TOUCH_CIRCLE_R            100
#define TOUCH_CIRCLE_X            650
#define TOUCH_CIRCLE_Y            200
#define TOUCH_CIRCLE_R_INC        80
#define TOUCH_CIRCLE_TOUCH_R      50
#define TOUCH_CIRCLE_TOUCH_X      600
#define TOUCH_CIRCLE_TOUCH_Y      200
#define TOUCH_CIRCLE_COL_NO       10
#define TOUCH_CIRCLE_ROW_NO       10
#define TOUCH_POINT_SZ            100
#define TOUCH_MVPOINT_X           180
#define TOUCH_MVPOINT_Y           20
#define TOUCH_MVPOINT_SZ          50
#define TOUCH_TRACKER_TAG_R       100
#define TOUCH_TRACKER_TAG_G       101
#define TOUCH_TRACKER_TAG_B       102
#define TOUCH_TRACKER_TAG_A       103
#define TOUCH_TRACKER_DIAL_W      (s_pHalContext->Width / 10)
#define TOUCH_TRACKER_DIAL_EDGE   20
#define TOUCH_TRACKER_RECT_W      (s_pHalContext->Width / 3)
#define TOUCH_TRACKER_RECT_H      (s_pHalContext->Height / 2)
#define TOUCH_OBJ_DIAL_TAG        10
#define TOUCH_OBJ_H_TAG           11
#define TOUCH_OBJ_V_TAG           12
#define TOUCH_OBJ_DIAL_R          (s_pHalContext->Width / 8)
#define TOUCH_OBJ_SLIDE_EDGE      100
#define TOUCH_OBJ_SLIDE_W         20
#define TOUCH_OBJ_H_TEXT_Y        (s_pHalContext->Height - TOUCH_TITLE_Y_INC)
#define TOUCH_OBJ_V_TEXT_X        (s_pHalContext->Width - 150)

static EVE_HalContext s_halContext;
static EVE_HalContext* s_pHalContext;
static void SAMAPP_Touch();
static int32_t Volume;

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
        "This sample demonstrates the use of touch", 
        "",
        ""
    };
    WelcomeScreen(s_pHalContext, info);

    SAMAPP_Touch();

    EVE_Util_clearScreen(s_pHalContext);
    Gpu_Release(s_pHalContext);
    return 0;
}

/**
 * @brief touch test
 *
 * @param Sq Square positions
 * @param TouchXY TouchXY value
 * @param TouchNo Touch number order
 * @return None
 */
static void helperDrawTouchPt(SAMAPP_Squares *Sq, int32_t TouchXY, uint8_t TouchNo)
{
    if (TouchXY != 0x80008000)
    {
        Sq->x = TouchXY >> 16;
        Sq->y = (TouchXY & 0xffff);
        Volume = (TouchNo + 1) * 255 / NO_OF_TOUCH;
    }
    else
    {
        Sq->x = OFFSCREEN;
        Sq->y = OFFSCREEN;
    }
    EVE_CoDl_colorRgb(s_pHalContext, 255, 255, 255);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_bitmapHandle(s_pHalContext, 0);
    EVE_CoCmd_setBitmap(s_pHalContext, TOUCH_BITMAP_LOAD, YCBCR, TOUCH_BITMAP_W, TOUCH_BITMAP_H);
    EVE_CoDl_vertex2f_4(s_pHalContext, (Sq->x - TOUCH_BITMAP_W / 2) * 16, (Sq->y - TOUCH_BITMAP_H / 2) * 16);
    EVE_CoDl_end(s_pHalContext);
}

/**
 * @brief Calculate small rectangle and positions
 *
 * @param context BouncingSquares context
 * @param Arrayno Rectangle id
 * @return None
 */
static void helperRectangleCalc(SAMAPP_BouncingSquares *context, uint8_t Arrayno)
{
    if (context->RectTouched[Arrayno] == 1)
    {
        //the limits for the smaller rectangles forward and backward movement is set here
        if (context->My[Arrayno] <= 0)
            context->Decrease[Arrayno] = 0; //increase
        else if (context->My[Arrayno] + 10 * TOUCH_RECT_INC >= context->Ty[Arrayno])
            context->Decrease[Arrayno] = 1; //decrease

        // the smaller rectangles are moved accordingly according to the flags set above in this function call
        if (context->Decrease[Arrayno] == 1)
            context->My[Arrayno] -= TOUCH_RECT_INC; //the smaller rectangles are moved upwards
        else if (context->Decrease[Arrayno] == 0) // increase
            context->My[Arrayno] += 2 * TOUCH_RECT_INC; //the smaller rectangles are moved downwards slightly faster
    }
}

/**
 * @brief Check user touches
 *
 * @param context BouncingSquares context
 * @param val Multi touch value
 * @return None
 */
static void helperCheckTouch(SAMAPP_BouncingSquares *context, int32_t val)
{
    int8_t Arrayno = -1;

    if (val != 0x80008000)
    {
        uint16_t Tx = (val >> 16) & 0xffff;

        // Check which rectangle is being touched according to the coordinates
        if (Tx >= TOUCH_RECT_X && Tx <= (TOUCH_RECT_X + TOUCH_RECT_W))
            Arrayno = 0;
        if (Tx >= (2 * TOUCH_RECT_X) && Tx <= (2 * TOUCH_RECT_X + TOUCH_RECT_W))
            Arrayno = 1;
        if (Tx >= (3 * TOUCH_RECT_X) && Tx <= (3 * TOUCH_RECT_X + TOUCH_RECT_W))
            Arrayno = 2;
        if (Tx >= (4 * TOUCH_RECT_X) && Tx <= (4 * TOUCH_RECT_X + TOUCH_RECT_W))
            Arrayno = 3;
        if (Tx >= (5 * TOUCH_RECT_X) && Tx <= (5 * TOUCH_RECT_X + TOUCH_RECT_W))
            Arrayno = 4;

        if (Arrayno != -1)
        {
            context->RectTouched[Arrayno] = 1; // rectangle touched

            //store the touch point's Y-coordinate
            context->Ty[Arrayno] = (val & 0xffff);

            //Limit the height of the larger rectangle to reserve space for the smaller one
            if (context->Ty[Arrayno] <= TOUCH_RECT_Y)
                context->Ty[Arrayno] = TOUCH_RECT_Y;
        }
    }

    //According to the bigger rectangle values move the smaller rectangles
    for (int i = 0; i < NO_OF_RECTS; i++)
    {
        helperRectangleCalc(context, (uint8_t)i);
    }
}

/**
 * @brief Draw BouncingSquares
 *
 * @param BRx
 * @param BRy
 * @param MovingRy
 * @param SqNumber
 * @return None
 */
static void helperDrawBouncingSquares(SAMAPP_BouncingSquares *context, int16_t *RectX)
{
    int8_t R1 = 0;
    int8_t G1 = 0;
    int8_t B1 = 0;
    int8_t R2 = 0;
    int8_t G2 = 0;
    int8_t B2 = 0;

    for (int i = 0; i < NO_OF_RECTS; i++)
    {
        //different colours are set for the different rectangles
        if (i == 0)
        {
            R1 = 63;
            G1 = 72;
            B1 = 204;
            R2 = 0;
            G2 = 255;
            B2 = 255;
        }

        if (i == 1)
        {
            R1 = 255;
            G1 = 255;
            B1 = 0;
            R2 = 246;
            G2 = 89;
            B2 = 12;
        }

        if (i == 2)
        {
            R1 = 255;
            G1 = 0;
            B1 = 0;
            R2 = 200;
            G2 = 28;
            B2 = 36;
        }

        if (i == 3)
        {
            R1 = 131;
            G1 = 171;
            B1 = 9;
            R2 = 8;
            G2 = 100;
            B2 = 50;
        }

        if (i == 4)
        {
            R1 = 90;
            G1 = 40;
            B1 = 120;
            R2 = 177;
            G2 = 156;
            B2 = 217;
        }

        // Draw the rectanles here
        EVE_CoDl_begin(s_pHalContext, RECTS);
        EVE_CoDl_lineWidth(s_pHalContext, TOUCH_MRECT_H * 16);

        EVE_CoDl_colorRgb(s_pHalContext, R1, G1, B1);
        EVE_CoDl_vertex2f_4(s_pHalContext, RectX[i] * 16, (context->Ty[i]) * 16);
        EVE_CoDl_vertex2f_4(s_pHalContext, (RectX[i] + TOUCH_RECT_W) * 16, TOUCH_RECT_H * 16);

        EVE_CoDl_colorRgb(s_pHalContext, R2, G2, B2);
        EVE_CoDl_vertex2f_4(s_pHalContext, RectX[i] * 16, (context->My[i]) * 16);
        EVE_CoDl_vertex2f_4(s_pHalContext, (RectX[i] + TOUCH_RECT_W) * 16, (context->My[i] + TOUCH_MRECT_H) * 16);
        EVE_CoDl_end(s_pHalContext);
    }
}

/**
 * @brief Draw plots
 *
 * @param None
 * @return None
 */
static void helperDrawPlotXY()
{
    uint8_t i = 0;
    uint16_t PlotHt = 0;
    uint16_t PlotWth = 0;
    uint16_t X = 0;
    uint16_t Y = 0;

    PlotHt = (uint16_t)(s_pHalContext->Height / TOUCH_CIRCLE_ROW_NO);
    PlotWth = (uint16_t)(s_pHalContext->Width / TOUCH_CIRCLE_COL_NO);

    EVE_CoDl_colorRgb(s_pHalContext, 36, 54, 125);
    EVE_CoDl_begin(s_pHalContext, LINES);
    /* Horizontal Lines */
    for (i = 0; i < TOUCH_CIRCLE_ROW_NO; i++)
    {
        Y = (i + 1) * PlotHt;
        EVE_CoDl_lineWidth(s_pHalContext, 1 * 16);
        EVE_CoDl_vertex2f_4(s_pHalContext, 0, Y * 16);
        EVE_CoDl_vertex2f_4(s_pHalContext, s_pHalContext->Width * 16, Y * 16);
    }
    /* Vertical Lines */
    for (i = 0; i < TOUCH_CIRCLE_COL_NO; i++)
    {
        X = (i + 1) * PlotWth;
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
 * @return None
 */
static void helperCheckCircleTouchCood(SAMAPP_BouncingCircles *context, int32_t val, uint8_t TouchNum,
    uint8_t i)
{
    float CX = (float)(val >> 16);
    float CY = (float)(val & 0xffff);

    if (val == 0x80008000)
    {
        context->TN[i] = 0xFF;
        return;
    }

    if (context->TN[i] != TouchNum 
        && (CX > (context->CX[i] - TOUCH_CIRCLE_TOUCH_R)) && (CX < (context->CX[i] + TOUCH_CIRCLE_TOUCH_R)) 
        && (CY > (context->CY[i] - TOUCH_CIRCLE_TOUCH_R)) && (CY < context->CY[i] + TOUCH_CIRCLE_TOUCH_R))
    {
        // small circle is touched, save the information
        context->CX[i] = CX;
        context->CY[i] = CY;
        context->TN[i] = TouchNum;
    }

    // update coordinate
    if (context->TN[i] == TouchNum)
    {
        context->CX[i] = CX;
        context->CY[i] = CY;
    }
}

/**
 * @brief calculate the radius of each circle according to the touch
 *
 * @param context BouncingSquares context
 * @param Val Circle number
 * @return None
 */
static void helperCirclePlot(SAMAPP_BouncingCircles *context, uint8_t Val)
{
    uint32_t Xsq;
    uint32_t Ysq;
    Xsq = (uint32_t)((uint16_t)context->CX[Val] - (s_pHalContext->Width / 2)) * ((uint16_t)context->CX[Val] - (s_pHalContext->Width / 2));
    Ysq = (uint32_t)((uint16_t)context->CY[Val] - (s_pHalContext->Height / 2)) * ((uint16_t)context->CY[Val] - (s_pHalContext->Height / 2));
    context->R[Val] = (float)(Xsq + Ysq);
    context->R[Val] = (float)sqrt(context->R[Val]);
}

/**
 * @brief Beginning BouncingCircle section
 *
 * @param C Point size
 * @return None
 */
static void helperDrawConcentricCircles(float C)
{
    EVE_CoDl_stencilFunc(s_pHalContext, NEVER, 0x00, 0x00);
    EVE_CoDl_stencilOp(s_pHalContext, INCR, INCR);
    EVE_CoDl_begin(s_pHalContext, POINTS);
    EVE_CoDl_pointSize(s_pHalContext, (uint16_t )((C - 5) * 16)); //inner circle
    EVE_CoDl_vertex2f_4(s_pHalContext, TOUCH_CIRCLE_X * 16, TOUCH_CIRCLE_Y * 16);

    EVE_CoDl_stencilFunc(s_pHalContext, NOTEQUAL, 0x01, 0x01);
    EVE_CoDl_pointSize(s_pHalContext, (uint16_t )((C) * 16)); //outer circle
    EVE_CoDl_vertex2f_4(s_pHalContext, TOUCH_CIRCLE_X * 16, TOUCH_CIRCLE_Y * 16);

    EVE_CoDl_stencilFunc(s_pHalContext, EQUAL, 0x01, 0x01);
    EVE_CoDl_stencilOp(s_pHalContext, KEEP, KEEP);
    EVE_CoDl_colorRgb(s_pHalContext, 255, 0, 0);
    EVE_CoDl_pointSize(s_pHalContext, (uint16_t )((C) * 16));
    EVE_CoDl_vertex2f_4(s_pHalContext, TOUCH_CIRCLE_X * 16, TOUCH_CIRCLE_Y * 16);

    EVE_CoDl_stencilFunc(s_pHalContext, ALWAYS, 0x01, 0x01);
    EVE_CoDl_stencilOp(s_pHalContext, KEEP, KEEP);

    EVE_CoDl_end(s_pHalContext);
}

/**
 * @brief Draw touch points
 *
 * @param CX Point X
 * @param CY Point Y
 * @param i Point number
 * @return None
 */
static void helperDrawTouchPoints(int16_t CX, int16_t CY, uint8_t i)
{
    /* Draw the five white circles for the Touch areas with their rescpective numbers*/
    EVE_CoDl_begin(s_pHalContext, POINTS);
    EVE_CoDl_pointSize(s_pHalContext, TOUCH_CIRCLE_TOUCH_R * 16);
    EVE_CoDl_colorRgb(s_pHalContext, 255, 255, 0);
    EVE_CoDl_vertex2f_4(s_pHalContext, CX * 16, CY * 16);
    EVE_CoDl_end(s_pHalContext);

    EVE_CoDl_colorRgb(s_pHalContext, 155, 155, 0);
    EVE_CoCmd_number(s_pHalContext, CX, CY, TOUCH_TITLE_FONT, OPT_CENTERX | OPT_CENTERY, i);
}

/**
 * @brief Beginning BouncingPoints section
 *
 * @param pBInst Blob instance
 * @param TouchXY Touch value
 * @return None
 */
static void helperBlobColor(SAMAPP_BlobsInst *pBInst, int32_t TouchXY)
{
    uint8_t j = 0;
    // if there is touch store the values
    if (TouchXY != 0x80008000)
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
        EVE_CoDl_pointSize(s_pHalContext, TOUCH_POINT_SZ + (i << 3));

        // Random color for each blob, keyed from (blob_i + i)
        j = (pBInst->CurrIdx + i) & (NBLOBS - 1);

        // Draw it!
        if (pBInst->blobs[j].x != OFFSCREEN)
            EVE_CoDl_vertex2f_4(s_pHalContext, (pBInst->blobs[j].x) * 16, (pBInst->blobs[j].y) * 16);
    }
    EVE_CoDl_end(s_pHalContext);
}

/**
 * @brief Beginning MovingPoints section
 *
 * @param k Touch number
 * @param i Point number
 * @return None
 */
static void helperColorSelection(int16_t k, int16_t i)
{
    if (k == 0)
    {
        if (i & 1)
            EVE_CoDl_colorRgb(s_pHalContext, 116, 27, 124); //purple
        else
            EVE_CoDl_colorRgb(s_pHalContext, 248, 134, 173); //pink
    }
    if (k == 1)
    {
        if (i & 1)
            EVE_CoDl_colorRgb(s_pHalContext, 232, 35, 25); //red
        else
            EVE_CoDl_colorRgb(s_pHalContext, 240, 135, 132); //light red
    }
    if (k == 2)
    {
        if (i & 1)
            EVE_CoDl_colorRgb(s_pHalContext, 248, 130, 58); //orange
        else
            EVE_CoDl_colorRgb(s_pHalContext, 255, 253, 85); //yellow
    }
    if (k == 3)
    {
        if (i & 1)
            EVE_CoDl_colorRgb(s_pHalContext, 0, 35, 245); //blue
        else
            EVE_CoDl_colorRgb(s_pHalContext, 115, 251, 253); //light blue
    }
    if (k == 4)
    {
        if (i & 1)
            EVE_CoDl_colorRgb(s_pHalContext, 55, 126, 71); //green
        else
            EVE_CoDl_colorRgb(s_pHalContext, 161, 251, 142); //light green
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
    float st = (float)t / rate;
    return (int16_t)(p1 + (st * (p2 - p1)));
}

/**
 * @brief Calculate offset movements
 *
 * @param context MovingPpoints context
 * @return None
 */
static void helperPointsCalc(SAMAPP_MovingPoints* context)
{
    uint8_t touchNo = 0;
    uint16_t tempDeltaX;
    uint16_t tempDeltaY;

    /* For total number of points calculate the offsets of movement */
    for (int16_t k = 0; k < NO_OF_POINTS * NO_OF_TOUCH; k++)
    {
        touchNo = k / NO_OF_POINTS;
        if (context->t[k] > NO_OF_POINTS)
        {
            context->t[k] = 0;
            context->X[k] = (context->val[touchNo] >> 16) & 0xffff;
            context->Y[k] = (context->val[touchNo] & 0xffff);
        }
        if (context->X[k] != 0x8000)
        {
            tempDeltaX = helperLinear(context->X[k], context->StopX[touchNo], context->t[k], NO_OF_POINTS);
            tempDeltaY = helperLinear(context->Y[k], context->StopY, context->t[k], NO_OF_POINTS);
            helperColorSelection(touchNo, k);
            EVE_CoDl_vertex2f_4(s_pHalContext, tempDeltaX * 16L, tempDeltaY * 16L);
        }
        context->t[k]++;
    }
}


/**
 * @brief touch test with music play
 *
 * @param None
 * @return None
 */
static void SAMAPP_Touch_touchToPlaySong()
{
#if defined(EVE_SUPPORT_CAPACITIVE)

    Draw_Text(s_pHalContext, "Example for: Touch test\n\n\nPlease touch on screen (1-5 fingers)");
    uint32_t val[5];

    SAMAPP_Squares SqCall;

    EVE_CoCmd_regWrite(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_EXTENDED);

    EVE_Util_loadRawFile(s_pHalContext, TOUCH_BITMAP_LOAD, TEST_DIR TOUCH_BITMAP);
    /*Audio*/
    EVE_Util_loadRawFile(s_pHalContext, TOUCH_BITMAP_LOAD + TOUCH_BITMAP_W * TOUCH_BITMAP_H * 2, TEST_DIR TOUCH_AUDIO);

    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_FREQ, TOUCH_AUDIO_FREQ);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_START, TOUCH_BITMAP_LOAD + TOUCH_BITMAP_W * TOUCH_BITMAP_H * 2);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_FORMAT, TOUCH_AUDIO_FORMAT);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_LENGTH, TOUCH_AUDIO_LENGTH);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_LOOP, 1);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_L_PB, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_R_PB, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_PLAY, 1);

    for (int j = 0; j < TOUCH_LOOP; j++)
    {
        Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 0, 0, 0 }, 0, 4);
        EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), TOUCH_TITLE_Y, TOUCH_TITLE_FONT, OPT_CENTER,
            "Touch to play song"); //text info

        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCH0_XY, &val[0]);
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCHA_XY, &val[1]);
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCHB_XY, &val[2]);
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCHC_XY, &val[3]);
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCH4_XY, &val[4]);

        for (int8_t i = 0; i < NO_OF_TOUCH; i++)
        {
            helperDrawTouchPt(&SqCall, (int32_t)val[i], i);
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
    EVE_Cmd_waitFlush(s_pHalContext);
#endif
}

/**
 * @brief touch test with bouncing squares
 *
 * @param None
 * @return None
 */
static void SAMAPP_Touch_BouncingSquares()
{
#if defined(EVE_SUPPORT_CAPACITIVE)
    int16_t RectX[NO_OF_RECTS];
    int32_t Touchval[NO_OF_RECTS];

    SAMAPP_BouncingSquares context;

    Draw_Text(s_pHalContext, "Example for: Draw Bouncing squares\n\n\nPlease touch on screen (1-5 fingers)");

    //Calculate the X vertices where the five rectangles have to be placed
    for (int i = 1; i < NO_OF_RECTS; i++)
    {
        RectX[0] = TOUCH_RECT_X;
        RectX[i] = RectX[i - 1] + 2 * TOUCH_RECT_W;
    }

    for (int i = 0; i < NO_OF_RECTS; i++)
    {
        context.Ty[i] = 0;
        context.My[i] = 0;
        context.RectTouched[i] = 0;
        context.Decrease[i] = 0;
    }

    EVE_CoCmd_regWrite(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_EXTENDED);
    EVE_Cmd_waitFlush(s_pHalContext);
    for (int k = 0; k < TOUCH_LOOP; k++)
    {
        /* first touch*/
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCH0_XY, &Touchval[0]);
        /* second touch*/
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCHA_XY, &Touchval[1]);
        /* third touch*/
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCHB_XY, &Touchval[2]);
        /* fourth  touch*/
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCHC_XY, &Touchval[3]);
        /* fifth  touch*/
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCH4_XY, &Touchval[4]);

        //Check which rectangle is being touched using the coordinates and move the respective smaller rectangle
        for (int8_t i = 0; i < NO_OF_TOUCH; i++)
        {
            helperCheckTouch(&context, Touchval[i]);
        }
        Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
        helperDrawBouncingSquares(&context, RectX);
        Display_End(s_pHalContext);
        EVE_sleep(TOUCH_DELAY_MS);
    }
    EVE_CoCmd_regWrite(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_COMPATIBILITY);
#endif
}
/* End BouncingSquares section */

/**
 * @brief touch test with bouncing Circles
 *
 * @param None
 * @return None
 */
static void SAMAPP_Touch_BouncingCircles()
{
#if defined(EVE_SUPPORT_CAPACITIVE)
    int32_t Touchval[NO_OF_CIRCLE];
    SAMAPP_BouncingCircles context;

    Draw_Text(s_pHalContext, "Example for: Draw Bouncing Circles\n\n\nPlease touch on screen (1-5 fingers)");

    EVE_CoCmd_regWrite(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_EXTENDED);
    EVE_Cmd_waitFlush(s_pHalContext);
    /* calculate the intital radius of the circles before the touch happens*/
    for (int8_t i = 0; i < NO_OF_CIRCLE; i++)
    {
        context.R[i] = (float)(TOUCH_CIRCLE_R + i * TOUCH_CIRCLE_R_INC);
        context.CX[i] = (float)(TOUCH_CIRCLE_TOUCH_X - i * TOUCH_CIRCLE_TOUCH_R * 2);
        context.CY[i] = TOUCH_CIRCLE_TOUCH_Y;
    }

    for (int32_t k = 0; k < TOUCH_LOOP; k++)
    {
        /* values of the five touches are stored here */
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCH0_XY, &Touchval[0]);
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCHA_XY, &Touchval[1]);
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCHB_XY, &Touchval[2]);
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCHC_XY, &Touchval[3]);
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCH4_XY, &Touchval[4]);

        Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
        /* The plot is drawn here */
        helperDrawPlotXY();

        /* check which circle has been touched based on the coordinates and store coordinate of the circle touched*/
        for (int8_t i = 0; i < NO_OF_CIRCLE; i++)
        {
            for (int8_t j = 0; j < NO_OF_TOUCH; j++)
            {
                helperCheckCircleTouchCood(&context, Touchval[j], j, i);
            }
        }
        /* calculate the radius of each circle according to the touch of each individual circle */
        for (int8_t i = 0; i < NO_OF_CIRCLE; i++)
        {
            helperCirclePlot(&context, i);
        }
        /* with the calculated radius draw the circles as well as the Touch points */
        for (int8_t i = 0; i < (NO_OF_CIRCLE); i++)
        {
            helperDrawConcentricCircles(context.R[i]);
            helperDrawTouchPoints((int16_t) context.CX[i], (int16_t) context.CY[i], i + 1);
        }

        Display_End(s_pHalContext);
    }
    EVE_CoCmd_regWrite(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_COMPATIBILITY);
#endif // capacity EVE
}
/* End BouncingCircle section */

/**
 * @brief touch test with Bouncing points
 *
 * @param None
 * @return None
 */
static void SAMAPP_Touch_BouncingPoints()
{
#if defined(EVE_SUPPORT_CAPACITIVE)
    int32_t Touchval[NO_OF_TOUCH];
    SAMAPP_BlobsInst gBlobsInst[NO_OF_TOUCH];
    SAMAPP_BlobsInst* pBInst;

    Draw_Text(s_pHalContext, "Example for: Draw Bouncing points\n\n\nPlease touch on screen (1-5 fingers)");

    EVE_CoCmd_regWrite(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_EXTENDED);
    EVE_Cmd_waitFlush(s_pHalContext);
    pBInst = &gBlobsInst[0];

    //set all coordinates to OFFSCREEN position
    for (uint8_t j = 0; j < NO_OF_TOUCH; j++)
    {
        for (uint8_t i = 0; i < NBLOBS; i++)
        {
            pBInst->blobs[i].x = OFFSCREEN;
            pBInst->blobs[i].y = OFFSCREEN;
        }
        pBInst->CurrIdx = 0;
        pBInst++;
    }

    for (uint16_t k = 0; k < TOUCH_LOOP; k++)
    {
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCH0_XY, &Touchval[0]);
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCHA_XY, &Touchval[1]);
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCHB_XY, &Touchval[2]);
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCHC_XY, &Touchval[3]);
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCH4_XY, &Touchval[4]);

        Display_Start(s_pHalContext, (uint8_t[]) { 43, 73, 59 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
        EVE_CoDl_blendFunc(s_pHalContext, SRC_ALPHA, ONE);
        EVE_CoDl_colorMask(s_pHalContext, 1, 1, 1, 0);

        // draw blobs according to the number of touches
        for (uint16_t j = 0; j < NO_OF_TOUCH; j++)
        {
            helperBlobColor(&gBlobsInst[j], Touchval[j]);
        }
        Display_End(s_pHalContext);
    }

    EVE_CoCmd_regWrite(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_COMPATIBILITY);
#endif // capacity EVE
}
/* End BouncingPoints section */

/**
 * @brief touch test with moving points
 *
 * @param None
 * @return None
 */
static void SAMAPP_Touch_MovingPoints()
{
#if defined(EVE_SUPPORT_CAPACITIVE)
    SAMAPP_MovingPoints context;
    Draw_Text(s_pHalContext, "Example for: Draw Moving points\n\n\nPlease touch on screen (1-5 fingers)");

    EVE_CoCmd_regWrite(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_EXTENDED);
    EVE_Cmd_waitFlush(s_pHalContext);
    /* Initialize all coordinates */
    for (uint16_t j = 0; j < NO_OF_TOUCH; j++)
    {
        for (uint16_t i = 0; i < NO_OF_POINTS; i++)
        {
            context.t[i + j * NO_OF_POINTS] = (uint8_t) i;
            context.X[i + j * NO_OF_POINTS] = OFFSCREEN;
            context.Y[i + j * NO_OF_POINTS] = OFFSCREEN;
        }
    }

    /* define the loction moving points stop */
    context.StopY = TOUCH_MVPOINT_Y;
    for (uint16_t i = 0; i < NO_OF_TOUCH; i++)
    {
        context.StopX[i] = TOUCH_MVPOINT_X + i * TOUCH_MVPOINT_SZ;
    }

    for (uint16_t k = 0; k < TOUCH_LOOP; k++)
    {
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCH0_XY, &context.val[0]);
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCHA_XY, &context.val[1]);
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCHB_XY, &context.val[2]);
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCHC_XY, &context.val[3]);
        EVE_CoCmd_regRead(s_pHalContext, REG_CTOUCH_TOUCH4_XY, &context.val[4]);

        Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 0, 0, 0 }, 0, 4);
        EVE_CoDl_begin(s_pHalContext, POINTS);
        EVE_CoDl_pointSize(s_pHalContext, TOUCH_MVPOINT_SZ * 16);
        EVE_CoDl_colorA(s_pHalContext, 120);
        helperPointsCalc(&context);
        EVE_CoDl_end(s_pHalContext);
        Display_End(s_pHalContext);
        EVE_sleep(TOUCH_DELAY_MS);
    }
    EVE_CoCmd_regWrite(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_COMPATIBILITY);
#endif // capacity EVE
}

/**
 * @brief Touch test with multiple objects that can be tracked individually
 *
 * @param None
 * @return None
 */
static void SAMAPP_Touch_multiTracker()
{
#if defined(EVE_SUPPORT_CAPACITIVE)
    uint32_t trackers[NO_OF_TRACKER];
    uint32_t delayLoop = TOUCH_LOOP;
    uint32_t trackerVal;
    uint8_t tagval;
    uint8_t RDialTag = TOUCH_TRACKER_TAG_R;
    uint8_t GDialTag = TOUCH_TRACKER_TAG_G;
    uint8_t BDialTag = TOUCH_TRACKER_TAG_B;
    uint8_t ADialTag = TOUCH_TRACKER_TAG_A;
    uint8_t DialR = TOUCH_TRACKER_DIAL_W;
    uint8_t rectRed = 0;
    uint8_t rectGreen = 0;
    uint8_t rectBlue = 0;
    uint8_t rectAlpha = 255;

    uint16_t RDialX = DialR + TOUCH_TRACKER_DIAL_EDGE;
    uint16_t RDialY = DialR + 5 * TOUCH_TRACKER_DIAL_EDGE;
    uint16_t GDialX = DialR + TOUCH_TRACKER_DIAL_EDGE;
    uint16_t GDialY = (uint16_t)(s_pHalContext->Height - 5* TOUCH_TRACKER_DIAL_EDGE - DialR);
    uint16_t BDialX = (uint16_t)(s_pHalContext->Width - TOUCH_TRACKER_DIAL_EDGE - DialR);
    uint16_t BDialY = DialR + 5 * TOUCH_TRACKER_DIAL_EDGE;
    uint16_t ADialX = (uint16_t)(s_pHalContext->Width - TOUCH_TRACKER_DIAL_EDGE - DialR);
    uint16_t ADialY = (uint16_t)(s_pHalContext->Height - 5 * TOUCH_TRACKER_DIAL_EDGE - DialR);
    uint16_t rectWidth = TOUCH_TRACKER_RECT_W;
    uint16_t rectHeight = TOUCH_TRACKER_RECT_H;
    uint16_t rectX = (uint16_t) (s_pHalContext->Width / 2 - rectWidth / 2);
    uint16_t rectY = (uint16_t) (s_pHalContext->Height / 2 - rectHeight / 2);
    uint16_t RDialTrackVal = 0;
    uint16_t GDialTrackVal = 0;
    uint16_t BDialTrackVal = 0;
    uint16_t ADialTrackVal = 65535;

    Draw_Text(s_pHalContext, "Example for: Multi touch on a single tracked object\n\n\nPlease touch on screen (1-4 fingers)");

    EVE_CoCmd_regWrite(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_EXTENDED);
    EVE_CoCmd_track(s_pHalContext, RDialX, RDialY, 1, 1, RDialTag);
    EVE_CoCmd_track(s_pHalContext, GDialX, GDialY, 1, 1, GDialTag);
    EVE_CoCmd_track(s_pHalContext, BDialX, BDialY, 1, 1, BDialTag);
    EVE_CoCmd_track(s_pHalContext, ADialX, ADialY, 1, 1, ADialTag);
    while (delayLoop != 0)
    {
        EVE_CoCmd_regRead(s_pHalContext, REG_TRACKER, &trackers[0]);
        EVE_CoCmd_regRead(s_pHalContext, REG_TRACKER_1, &trackers[1]);
        EVE_CoCmd_regRead(s_pHalContext, REG_TRACKER_2, &trackers[2]);
        EVE_CoCmd_regRead(s_pHalContext, REG_TRACKER_3, &trackers[3]);

        for (uint8_t i = 0; i < NO_OF_TRACKER; i++)
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
        Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
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

        EVE_CoCmd_text(s_pHalContext, RDialX, RDialY, TOUCH_TITLE_FONT, OPT_CENTER, "Red");//text info
        EVE_CoCmd_text(s_pHalContext, GDialX, GDialY, TOUCH_TITLE_FONT, OPT_CENTER, "Green"); //text info
        EVE_CoCmd_text(s_pHalContext, BDialX, BDialY, TOUCH_TITLE_FONT, OPT_CENTER, "Blue"); //text info
        EVE_CoCmd_text(s_pHalContext, ADialX, ADialY, TOUCH_TITLE_FONT, OPT_CENTER, "Alpha"); //text info

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
 * @param None
 * @return None
 */
static void SAMAPP_Touch_touchInfo()
{
    Draw_Text(s_pHalContext, "Example for: Touch raw, touch screen, touch tag\n\n\nPlease touch on screen");

    int32_t LoopFlag = TOUCH_LOOP;
    int32_t wbutton;
    int32_t hbutton;
    char StringArray[100];
    uint32_t touchVal;
    uint16_t xvalue;
    uint16_t yvalue;

    /*************************************************************************/
    /* Below code demonstrates the usage of touch function. Display info     */
    /* touch raw, touch screen, touch tag, raw adc and resistance values     */
    /*************************************************************************/
    wbutton = s_pHalContext->Width / 8;
    hbutton = s_pHalContext->Height / 8;
    while (LoopFlag--)
    {
        Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 0, 0, 0 }, 0xff, 4);
        EVE_CoDl_tagMask(s_pHalContext, 0);
        /* Draw informative text */
        StringArray[0] = '\0';
        strcat_s(StringArray, sizeof(StringArray), "Touch Raw XY (");
        EVE_CoCmd_regRead(s_pHalContext, REG_TOUCH_RAW_XY, &touchVal);
        yvalue = (uint16_t)(touchVal & 0xffff);
        xvalue = (uint16_t)((touchVal >> 16) & 0xffff);
        Gpu_Hal_Dec2Ascii(StringArray, sizeof(StringArray), (uint32_t)xvalue);
        strcat_s(StringArray, sizeof(StringArray), ",");
        Gpu_Hal_Dec2Ascii(StringArray, sizeof(StringArray), (uint32_t)yvalue);
        strcat_s(StringArray, sizeof(StringArray), ")");
        EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), TOUCH_TITLE_Y, TOUCH_TITLE_FONT, OPT_CENTER,
            StringArray);

        StringArray[0] = '\0';
        strcat_s(StringArray, sizeof(StringArray), "Touch Screen XY (");
        EVE_CoCmd_regRead(s_pHalContext, REG_TOUCH_SCREEN_XY, &touchVal);
        yvalue = (int16_t)(touchVal & 0xffff);
        xvalue = (int16_t)((touchVal >> 16) & 0xffff);
        Gpu_Hal_Dec2Ascii(StringArray, sizeof(StringArray), (int32_t)xvalue);
        strcat_s(StringArray, sizeof(StringArray), ",");
        Gpu_Hal_Dec2Ascii(StringArray, sizeof(StringArray), (int32_t)yvalue);
        strcat_s(StringArray, sizeof(StringArray), ")");
        EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), TOUCH_TITLE_Y + TOUCH_TITLE_Y_INC, TOUCH_TITLE_FONT, OPT_CENTER,
            StringArray);

        StringArray[0] = '\0';
        strcat_s(StringArray, sizeof(StringArray), "Touch TAG (");
        EVE_CoCmd_regRead(s_pHalContext, REG_TOUCH_TAG, &touchVal);
        Gpu_Hal_Dec2Ascii(StringArray, sizeof(StringArray), touchVal);
        strcat_s(StringArray, sizeof(StringArray), ")");
        EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), TOUCH_TITLE_Y + 2 * TOUCH_TITLE_Y_INC, TOUCH_TITLE_FONT, OPT_CENTER,
            StringArray);

        EVE_CoCmd_fgColor(s_pHalContext, 0x008000);
        EVE_CoDl_tagMask(s_pHalContext, 1);
        EVE_CoDl_tag(s_pHalContext, 12);
        EVE_CoCmd_button(s_pHalContext, (int16_t) ((s_pHalContext->Width / 4) - (wbutton / 2)),
            (int16_t) ((s_pHalContext->Height * 2 / 4) - (hbutton / 2)), (int16_t) wbutton,
            (int16_t)hbutton, TOUCH_TITLE_FONT, OPT_FLAT, "Tag12");
        EVE_CoDl_tag(s_pHalContext, 13);
        EVE_CoCmd_button(s_pHalContext, (int16_t) ((s_pHalContext->Width * 3 / 4) - (wbutton / 2)),
            (int16_t) ((s_pHalContext->Height * 3 / 4) - (hbutton / 2)), (int16_t) wbutton,
            (int16_t)hbutton, TOUCH_TITLE_FONT, OPT_FLAT, "Tag13");

        Display_End(s_pHalContext);
        EVE_sleep(TOUCH_DELAY_MS);
    }
}

/**
 * @brief Sample app api to demonstrate track widget funtionality
 *
 * @param None
 * @return None
 */
static void SAMAPP_Touch_objectTrack()
{
    /*************************************************************************/
    /* Below code demonstrates the usage of track function. Track function   */
    /* tracks the pen touch on any specific object. Track function supports  */
    /* rotary and horizontal/vertical tracks. Rotary is given by rotation    */
    /* angle and horizontal/vertucal track is offset position.               */
    /*************************************************************************/
    int32_t LoopFlag = TOUCH_LOOP;
    uint32_t TrackRegisterVal = 0;
    uint16_t angleval = 0;
    uint16_t slideval = 0;
    uint16_t scrollval = 0;

    /* Set the tracker for 3 bojects */

    Draw_Text(s_pHalContext, "Example for: Rotary and horizontal/vertical tracks\n\n\nPlease touch on screen");

    EVE_CoCmd_track(s_pHalContext, (int16_t)(s_pHalContext->Width / 2),
        (int16_t)(s_pHalContext->Height / 2), 1, 1, TOUCH_OBJ_DIAL_TAG);
    EVE_CoCmd_track(s_pHalContext, TOUCH_OBJ_SLIDE_EDGE, (int16_t)(s_pHalContext->Height - TOUCH_OBJ_SLIDE_EDGE),
        (int16_t)(s_pHalContext->Width - 2 * TOUCH_OBJ_SLIDE_EDGE), TOUCH_OBJ_SLIDE_W, TOUCH_OBJ_H_TAG);
    EVE_CoCmd_track(s_pHalContext, (int16_t)(s_pHalContext->Width - TOUCH_OBJ_SLIDE_EDGE), TOUCH_OBJ_SLIDE_EDGE,
        TOUCH_OBJ_SLIDE_W, (int16_t)(s_pHalContext->Height - 2 * TOUCH_OBJ_SLIDE_EDGE), TOUCH_OBJ_V_TAG);
    /* Wait till coprocessor completes the operation */
    EVE_Cmd_waitFlush(s_pHalContext);

    /* update the background color continuously for the color change in any of the trackers */
    while (LoopFlag--)
    {
        uint8_t tagval = 0;
        EVE_CoCmd_regRead(s_pHalContext, REG_TRACKER, &TrackRegisterVal);
        tagval = TrackRegisterVal & 0xff;

        if (TOUCH_OBJ_DIAL_TAG == tagval)
        {
            angleval = TrackRegisterVal >> 16;
        }
        else if (TOUCH_OBJ_H_TAG == tagval)
        {
            slideval = TrackRegisterVal >> 16;
        }
        else if (TOUCH_OBJ_V_TAG == tagval)
        {
            // scroll bar size is 2/10 of height
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

        Display_Start(s_pHalContext, (uint8_t[]) { angval, sldval, scrlval }, (uint8_t[]) { 255, 255, 255 }, 0, 4);

        /* Draw dial with 3d effect */
        EVE_CoCmd_fgColor(s_pHalContext, 0x00ff00);
        EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
        EVE_CoDl_tag(s_pHalContext, TOUCH_OBJ_DIAL_TAG);
        EVE_CoCmd_dial(s_pHalContext, (int16_t) (s_pHalContext->Width / 2),
            (int16_t)(s_pHalContext->Height / 2), (int16_t)TOUCH_OBJ_DIAL_R, 0, angleval);

        /* Draw slider with 3d effect */
        EVE_CoCmd_fgColor(s_pHalContext, 0x00a000);
        EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
        EVE_CoDl_tag(s_pHalContext, TOUCH_OBJ_H_TAG);
        EVE_CoCmd_slider(s_pHalContext, TOUCH_OBJ_SLIDE_EDGE, (int16_t)(s_pHalContext->Height - TOUCH_OBJ_SLIDE_EDGE),
            (int16_t)(s_pHalContext->Width - 2 * TOUCH_OBJ_SLIDE_EDGE), TOUCH_OBJ_SLIDE_W, 0, slideval, 65535);

        /* Draw scroll with 3d effect */
        EVE_CoCmd_fgColor(s_pHalContext, 0x00a000);
        EVE_CoCmd_bgColor(s_pHalContext, 0x000080);
        EVE_CoDl_tag(s_pHalContext, TOUCH_OBJ_V_TAG);
        EVE_CoCmd_scrollbar(s_pHalContext, (int16_t)(s_pHalContext->Width - TOUCH_OBJ_SLIDE_EDGE), TOUCH_OBJ_SLIDE_EDGE, 
            TOUCH_OBJ_SLIDE_W, (int16_t)(s_pHalContext->Height - 2 * TOUCH_OBJ_SLIDE_EDGE), 0, scrollval, (uint16_t)(65535 * 2 / 10), 65535);

        EVE_CoCmd_fgColor(s_pHalContext, TAG_MASK(0));
        EVE_CoDl_colorRgb(s_pHalContext, 0xff, 0xff, 0xff);
        EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2),
            (int16_t)((s_pHalContext->Height / 2) + TOUCH_OBJ_DIAL_R + TOUCH_TITLE_Y_INC), TOUCH_TITLE_FONT,
            OPT_CENTER, "Rotary track");
        EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), TOUCH_OBJ_H_TEXT_Y, TOUCH_TITLE_FONT, OPT_CENTER, "Horizontal track");
        EVE_CoCmd_text(s_pHalContext, TOUCH_OBJ_V_TEXT_X, TOUCH_TITLE_Y_INC, TOUCH_TITLE_FONT, OPT_CENTER, "Vertical track");

        Display_End(s_pHalContext);

        EVE_sleep(TOUCH_DELAY_MS);
    }
}

/**
 * @brief Main entry to run all touch demos
 *
 * @param None
 * @return None
 */
static void SAMAPP_Touch()
{
    SAMAPP_Touch_touchToPlaySong();
    SAMAPP_Touch_BouncingSquares();
    SAMAPP_Touch_BouncingCircles();
    SAMAPP_Touch_BouncingPoints();
    SAMAPP_Touch_MovingPoints();
    SAMAPP_Touch_multiTracker();
    SAMAPP_Touch_touchInfo();
    SAMAPP_Touch_objectTrack();
}


