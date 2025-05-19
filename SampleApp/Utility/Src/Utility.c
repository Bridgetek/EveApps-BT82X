/**
 * @file Utility.c
 * @brief Sample usage of some useful command
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
#include "Utility.h"

#define SAMAPP_DELAY           EVE_sleep(2000);

static EVE_HalContext s_halContext;
static EVE_HalContext* s_pHalContext;
void SAMAPP_Utility();

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

    Flash_Init(s_pHalContext, TEST_DIR "/Flash/flash.bin", "flash.bin");
    EVE_Util_clearScreen(s_pHalContext);

    char *info[] =
    {  "EVE Sample Application",
        "This sample demonstrate the using of utilites of EVE", 
        "",
        ""
    }; 

    while (TRUE) {
        WelcomeScreen(s_pHalContext, info);

        SAMAPP_Utility();

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
* @brief Check Coprocesor fault
*
*/
void helperCoprocessorFaultReport()
{
    char DiagMessage[128];
    uint32_t val = 0;
    if ((EVE_CoCmd_regRead(s_pHalContext, REG_CMD_READ, &val)) && (val == 0x3fff)) //Fault reported
    {
        EVE_Hal_rdMem(s_pHalContext, DiagMessage, RAM_REPORT, RAM_REPORT_MAX);
        eve_printf_debug("%s\n", DiagMessage);
        DiagMessage[0] = '\0'; //Reset message
    }
}

/**
* @brief Restore Coprocessor after fault
*
*/
void helperCoprocessorFaultRecover()
{
    uint32_t val = 0;
    /* recovery coprocessor sequence */
    /* Set REG_CMD_READ to zero */
    EVE_Hal_wr32(s_pHalContext, REG_CMD_READ, 0);
    EVE_Hal_wr32(s_pHalContext, REG_CMD_WRITE, 0);
    EVE_Cmd_waitFlush(s_pHalContext);
    eve_assert((EVE_Hal_rd32(s_pHalContext, REG_CMD_WRITE)) == 0);
    eve_assert((EVE_Hal_rd32(s_pHalContext, REG_CMD_READ)) == 0);
    EVE_sleep(100);

    EVE_CoCmd_flashFast(s_pHalContext, 0);
    EVE_CoCmd_regRead(s_pHalContext, REG_FLASH_STATUS, &val);
    if (val != FLASH_STATUS_FULL)
    {
        eve_printf_debug("Flash is not able to get into full mode\n");
    }
}

/**
* @brief Flush a command to REG_CMDB_WRITE
*
* @param command Command value
*
*/
static void helperCMDBWrite(uint32_t command)
{
    EVE_Hal_wr32(s_pHalContext, REG_CMDB_WRITE, command);
}

/**
* @brief Flush a string to REG_CMDB_WRITE
*
* @param str string to write
*
*/
static void helperCMDBWriteString(uint8_t* str)
{
    int textLen = strlen(str);
    int padding = textLen % 4;
    /* Copy the command instruction into buffer */
    uint32_t v = 0;
    for (int i = 0; i < textLen/4; i++) 
    {
        v = str[i*4 + 0] << 0 |
            str[i*4 + 1] << 8 |
            str[i*4 + 2] << 16|
            str[i*4 + 3] << 24 ;
        EVE_Hal_wr32(s_pHalContext, REG_CMDB_WRITE, v);
    }

    if (padding)
    {
        v = 0;
        for (int i = 0; i < (4 - padding); i++) 
        {
            v |= str[textLen / 4*4 + i] << i * 8;
        }
        EVE_Hal_wr32(s_pHalContext, REG_CMDB_WRITE, v);
    }
}

/**
* @brief API to demonstrate CMD_WAIT
*
*/
void SAMAPP_Utility_wait()
{
    const uint32_t delayUs = 1000000;

    Draw_Text(s_pHalContext, "Example for: Waiting/Sleeping");
    Draw_TextColor(s_pHalContext, "EVE is waiting for 1 second...", (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 });

    uint32_t sms = EVE_millis();
    eve_printf_debug("Time start= %u\n", sms);
    EVE_CoCmd_wait(s_pHalContext, delayUs);
    EVE_Cmd_waitFlush(s_pHalContext);
    uint32_t ems = EVE_millis();
    eve_printf_debug("Time end= %u\n", ems);
}

/**
* @brief API to demonstrate for CMD_CALLLIST, CMD_NEWLIST, CMD_ENDLIST, CMD_RETURN
*
*/
void SAMAPP_Utility_callList()
{
    uint32_t endPtr = 4;
    uint32_t endPtrArr[5];
    const uint32_t w = 800;
    const uint32_t h = 480;
    const uint16_t x = (s_pHalContext->Width - w) / 2;
    const uint16_t y = (s_pHalContext->Height - h) / 2;
    char str[1000];

    Draw_Text(s_pHalContext, "Example for: CMD_CALLLIST, CMD_NEWLIST, CMD_ENDLIST and CMD_RETURN");

    Draw_TextColor(s_pHalContext, "Constructing 5 lists ...", (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 });
    for (int i = 0; i < 5; i++)
    {
        endPtrArr[i] = endPtr;

        EVE_CoCmd_newList(s_pHalContext, endPtr);
        EVE_CoCmd_setBitmap(s_pHalContext, DDR_BITMAPS_STARTADDR, RGB565, w, h);

        EVE_CoDl_saveContext(s_pHalContext);
        EVE_CoDl_begin(s_pHalContext, BITMAPS);
        EVE_CoCmd_loadIdentity(s_pHalContext);
        EVE_CoCmd_rotateAround(s_pHalContext, w / 2, h / 2, i * 60 * 65536 / 360, 65536 * 1);
        EVE_CoCmd_setMatrix(s_pHalContext);
        EVE_CoDl_vertex2f_4(s_pHalContext, (x) * 16, (y) * 16);
        EVE_CoDl_end(s_pHalContext);
        EVE_CoDl_restoreContext(s_pHalContext);

        snprintf(str, 1000, "Displaying list number %u", i);
        EVE_CoCmd_text(s_pHalContext, x, y - 100, 31, 0, str);
        EVE_CoCmd_endList(s_pHalContext);

        EVE_Cmd_waitFlush(s_pHalContext);
        uint32_t endAddr;
        EVE_CoCmd_getPtr(s_pHalContext, &endAddr); //cmd_getPtr will be always 128 bytes align
        EVE_Cmd_waitFlush(s_pHalContext);

        eve_printf_debug("Compiled list %u at %u, size: %u Bytes\n", i, endPtr, endAddr - endPtr);
        endPtr = endAddr;
    }
    Draw_TextColor(s_pHalContext, "Constructed 5 lists", (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 });

    EVE_Util_loadImageFile(s_pHalContext, DDR_BITMAPS_STARTADDR, TEST_DIR "\\flower_800x480.jpg", NULL, 0);
    for (int i = 0; i < 5; i++)
    {
        snprintf(str, 1000, "Calling List number %u", i);
        Draw_TextColor(s_pHalContext, str, (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 });

        Display_Start(s_pHalContext);
        EVE_CoCmd_callList(s_pHalContext, endPtrArr[i]);
        Display_End(s_pHalContext);
        SAMAPP_DELAY;

        //check whether there is a CMD_RETURN at the end of list
        if (i == 0)
        {
            uint32_t cmd = 0;
            EVE_CoCmd_regRead(s_pHalContext, RAM_G + endPtrArr[i] + 108, &cmd);
            eve_printf_debug("Return: 0x%08x\n", cmd);

            if (cmd == CMD_RETURN)
            {
                Draw_TextColor(s_pHalContext, "CMD_RETURN is appended into the list", (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 });
                SAMAPP_DELAY;
            }
        }
    }
    SAMAPP_DELAY;
}

/**
* @brief API to demonstrate for command list in RAM_G with alignment
*
*/
void SAMAPP_Utility_callListWithAlignment()
{
    Draw_Text(s_pHalContext, "Example for: Construct a command list in RAM_G to show a button");

    //Construct a command list in RAM_G to show a button
    EVE_Hal_wr32(s_pHalContext, DDR_BITMAPS_STARTADDR2 + 0 * 4, SAVE_CONTEXT());
    EVE_Hal_wr32(s_pHalContext, DDR_BITMAPS_STARTADDR2 + 1 * 4, CLEAR_COLOR_RGB(255, 255, 255));
    EVE_Hal_wr32(s_pHalContext, DDR_BITMAPS_STARTADDR2 + 2 * 4, CLEAR(1, 1, 1));
    EVE_Hal_wr32(s_pHalContext, DDR_BITMAPS_STARTADDR2 + 3 * 4, CMD_BUTTON);

    EVE_Hal_wr32(s_pHalContext, DDR_BITMAPS_STARTADDR2 + 4 * 4, 160 << 16 | 160); //y | x coordinate of button
    EVE_Hal_wr32(s_pHalContext, DDR_BITMAPS_STARTADDR2 + 5 * 4, 200 << 16 | 324); //h | w
    EVE_Hal_wr32(s_pHalContext, DDR_BITMAPS_STARTADDR2 + 6 * 4, 0 << 16 | 31); //options | Font handle
    EVE_Hal_wr32(s_pHalContext, DDR_BITMAPS_STARTADDR2 + 7 * 4, 'T' << 24 | 'E' << 16 | 'S' << 8 | 'T');

    EVE_Hal_wr32(s_pHalContext, DDR_BITMAPS_STARTADDR2 + 8 * 4, '\0' << 24 | '\0' << 16 | '\0' << 8 | '\0');

    EVE_Hal_wr32(s_pHalContext, DDR_BITMAPS_STARTADDR2 + 8 * 4, RESTORE_CONTEXT()); //Assume 3 bytes padding bytes for alignment 
    EVE_Hal_wr32(s_pHalContext, DDR_BITMAPS_STARTADDR2 + 9 * 4, CMD_RETURN); //return to the command buffer

    //Call cmd_list with data in RAM_G
    Display_Start(s_pHalContext);
    EVE_CoCmd_callList(s_pHalContext, DDR_BITMAPS_STARTADDR2);
    Display_End(s_pHalContext);

    SAMAPP_DELAY;
}


/**
* @brief API to demonstrate the use of append commands
*
*/
void SAMAPP_Utility_appendCommand()
{
    uint32_t AppendCmds[16];
    int16_t xoffset;
    int16_t yoffset;
    uint16_t imgW = 256;
    uint16_t imgH = 256;

    Draw_Text(s_pHalContext, "Example for: Appending DL command");

    /*************************************************************************/
    /* Below code demonstrates the usage of coprocessor append command - to append any*/
    /* mcu specific graphics commands to coprocessor generated graphics commands      */
    /*************************************************************************/

    /* Bitmap construction by MCU - display at 256x256 offset */
    /* Construct the bitmap data to be copied in the temp buffer then by using
    coprocessor append command copy it into graphics processor DL memory */
    xoffset = (int16_t)((s_pHalContext->Width - imgW) / 4);
    yoffset = (int16_t)((s_pHalContext->Height / 3) - imgH / 2);

    EVE_CoCmd_dlStart(s_pHalContext);
    AppendCmds[0] = CLEAR_COLOR_RGB(255, 0, 0);
    AppendCmds[1] = CLEAR(1, 1, 1);
    AppendCmds[2] = COLOR_RGB(255, 255, 255);
    AppendCmds[3] = BEGIN(BITMAPS);
    AppendCmds[4] = BITMAP_SOURCE(DDR_BITMAPS_STARTADDR1);
    AppendCmds[5] = BITMAP_SOURCEH(DDR_BITMAPS_STARTADDR1 >> 24);
    AppendCmds[6] = BITMAP_LAYOUT(RGB565, 2 * imgW, imgH);
    AppendCmds[7] = BITMAP_SIZE(BILINEAR, BORDER, BORDER, imgW, imgH);
    AppendCmds[8] = VERTEX2F(xoffset * 16, yoffset * 16);
    AppendCmds[9] = END();

    /* Download the bitmap data*/
    EVE_Util_loadImageFile(s_pHalContext, DDR_BITMAPS_STARTADDR1, TEST_DIR "\\mandrill256.jpg", NULL, OPT_RGB565);

    /* Download the DL data constructed by the MCU to location 40*40*2 in sram */
    EVE_Hal_wrMem(s_pHalContext, DDR_BITMAPS_STARTADDR1 + imgW * imgH, (uint8_t *)AppendCmds, 9 * 4);

    /* Call the append api for copying the above generated data into graphics processor
    DL commands are stored at location 40*40*2 offset from the starting of the sram */
    EVE_CoCmd_append(s_pHalContext, DDR_BITMAPS_STARTADDR1 + imgW * imgH, 9 * 4);
    /*  Display the text information */
    xoffset -= 50;
    yoffset += 300;
    EVE_CoCmd_text(s_pHalContext, xoffset, yoffset, 31, 0, "Display bitmap by Append");
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief Draw set 11
*
*/
void SAMAPP_Utility_bulkTransfer()
{
    SAMAPP_Circle_t circles[100];
    uint32_t precision = 4;
    uint32_t CNUM = 100; // Disable circles
    uint32_t ImgW = 256;
    uint32_t ImgH = 256;
    uint32_t xoffset = (s_pHalContext->Width - ImgW) / 2;
    uint32_t yoffset = (s_pHalContext->Height - ImgH) / 2;
    int count = 0;

    Draw_Text(s_pHalContext, "Example for: Bulk transfer");
    EVE_Util_clearScreen(s_pHalContext);

    for (int i = 0; i < (int)CNUM; i++)
    {
        circles[i].visible = 0;
        circles[i].opacity = 0;
        circles[i].radius = 0;
        circles[i].step = 0;
        circles[i].visible = 0;
        circles[i].x = 0;
        circles[i].y = 0;
        circles[i].color.b = 0;
        circles[i].color.g = 0;
        circles[i].color.r = 0;
    }

    EVE_Util_loadImageFile(s_pHalContext, DDR_BITMAPS_STARTADDR1, TEST_DIR "\\mandrill256.jpg", NULL, OPT_RGB565);

    while (count++ < 60 * 10)
    {
        /*Display List start*/
        helperCMDBWrite(CMD_DLSTART);
        helperCMDBWrite(CLEAR_COLOR_RGB(0, 0, 0));
        helperCMDBWrite(CLEAR(1, 1, 1));
        helperCMDBWrite(VERTEX_FORMAT(2));
        // draw circles
        for (int i = 0; i < (int)CNUM; i++)
        {
            int visible = rand() % 3;
            int x = rand() % s_pHalContext->Width;
            int y = rand() % s_pHalContext->Height;
            int radius = rand() % 20;
            int r = rand() % 255;
            int g = rand() % 255;
            int b = rand() % 255;
            int step = rand() % 100 / 20 + 1;

            if (visible != 1)
            {
                visible = 0;
            }

            if (circles[i].visible == 0 && visible == 1)
            {
                circles[i].visible = 1;
                circles[i].x = x;
                circles[i].y = y;
                circles[i].radius = radius * 5;
                circles[i].color.r = r;
                circles[i].color.g = g;
                circles[i].color.b = b;
                circles[i].opacity = 0;
                circles[i].step = step;
            }
            else if (circles[i].opacity >= 255)
            {
                circles[i].step = -circles[i].step;
            }
            else if (circles[i].opacity <= 0)
            {
                circles[i].visible = 0;
            }
            circles[i].opacity += circles[i].step;

            if (circles[i].visible)
            {
                helperCMDBWrite(COLOR_A(circles[i].opacity));
                helperCMDBWrite(
                COLOR_RGB(circles[i].color.r, circles[i].color.g, circles[i].color.b));

                helperCMDBWrite(BEGIN(POINTS));
                helperCMDBWrite(POINT_SIZE(circles[i].radius * precision));
                helperCMDBWrite(VERTEX2F(circles[i].x * precision, circles[i].y * precision));
                helperCMDBWrite(END());
            }
        }
        // reset coloring
        helperCMDBWrite(COLOR_A(255));
        helperCMDBWrite(COLOR_RGB(255, 255, 255));

        // draw image
        helperCMDBWrite(BITMAP_HANDLE(2));
        helperCMDBWrite(BEGIN(BITMAPS));
        helperCMDBWrite(BITMAP_SOURCE(DDR_BITMAPS_STARTADDR1));
        helperCMDBWrite(BITMAP_SOURCEH(DDR_BITMAPS_STARTADDR1 >> 24));
        helperCMDBWrite(BITMAP_LAYOUT(RGB565, ImgW * 2, ImgH));
        helperCMDBWrite(BITMAP_LAYOUT_H((ImgW * 2) >> 10, ImgH >> 9));
        helperCMDBWrite(BITMAP_SIZE(BILINEAR, BORDER, BORDER, ImgW, ImgH));
        helperCMDBWrite(BITMAP_SIZE_H(ImgW >> 9, ImgH >> 9));
        helperCMDBWrite(VERTEX2F(xoffset * precision, yoffset * precision));
        helperCMDBWrite(END());

        // Draw the text in top of screen
        uint32_t x = s_pHalContext->Width / 2 - 120;
        uint32_t y = 20;
        uint32_t font = 31;
        uint32_t opt = OPT_CENTER;
        helperCMDBWrite(CMD_TEXT);
        helperCMDBWrite(((y << 16) | (x & 0xffff)));
        helperCMDBWrite(((opt << 16) | (font & 0xffff)));
        helperCMDBWriteString("Bulk transfer demostration");

        y = 100;
        helperCMDBWrite(CMD_TEXT);
        helperCMDBWrite(((y << 16) | (x & 0xffff)));
        helperCMDBWrite(((opt << 16) | (font & 0xffff)));
        helperCMDBWriteString("Commands are transferd into REG_CMDB_WRITE instead of RAM_CMD");
        helperCMDBWrite(VERTEX_FORMAT(4));

        helperCMDBWrite(DISPLAY()); //send command display - to end display commands
        helperCMDBWrite(CMD_SWAP); // draw the new screen

        // Flush all the command
        EVE_Cmd_waitFlush(s_pHalContext); // Wait until EVE is free
    }
    SAMAPP_DELAY;
}

/**
* @brief API to demonstrate display bitmap by inflate data from flash
*
*/
void SAMAPP_Utility_cmdInflateFromFlash()
{
    Draw_Text(s_pHalContext, "Example for: CMD_INFLATE with OPT_FLASH");

    /* INFLATED BITMAP information */
    uint16_t w = 320;
    uint16_t h = 240;

    FlashHelper_SwitchState(s_pHalContext, FLASH_STATUS_FULL); // full mode

    /* Now use cmd_inflate */
    EVE_CoCmd_flashSource(s_pHalContext, 4096);
    EVE_CoCmd_inflate(s_pHalContext, RAM_G, OPT_FLASH);

    /* Display inflated image */
    Display_Start(s_pHalContext);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoCmd_setBitmap(s_pHalContext, RAM_G, ARGB4, w, h);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width - w) / 2 * 16, (s_pHalContext->Height - h) / 2 * 16);
    EVE_CoDl_end(s_pHalContext);

    /*  Display the text information */
    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);
    EVE_CoCmd_text(s_pHalContext, s_pHalContext->Width / 2, 100, 31, OPT_CENTER, "Display bitmap by inflate (OPT_FLASH)");
    Display_End(s_pHalContext);

    SAMAPP_DELAY;
}

/**
* @brief API to demonstrate display bitmap by inflate data from media fifo
*
*/
void SAMAPP_Utility_CmdInflateFromFifo()
{
    Draw_Text(s_pHalContext, "Example for: CMD_INFLATE with data from media fifo");

    /* INFLATED BITMAP information */
    uint16_t w = 320;
    uint16_t h = 240;

    uint32_t mediafifo = w * h * 2; //the starting address of the media fifo
    uint32_t mediafifolen = 20 * 1024;

    /* Clear the memory at location 0 - any previous bitmap data */
    EVE_CoCmd_memSet(s_pHalContext, RAM_G, 255L, 1L * w * 2 * h);
    /*Load data to mediafifo */
    EVE_MediaFifo_set(s_pHalContext, mediafifo, mediafifolen);
    EVE_CoCmd_inflate(s_pHalContext, RAM_G, OPT_MEDIAFIFO);
    EVE_Util_loadMediaFile(s_pHalContext, TEST_DIR "\\bird_320x240_ARGB4.bin", NULL, 0);

    Display_Start(s_pHalContext);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoCmd_setBitmap(s_pHalContext, RAM_G, ARGB4, w, h);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width - w) / 2 * 16, (s_pHalContext->Height - h) / 2 * 16);
    EVE_CoDl_end(s_pHalContext);

    /*  Display the text information */
    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);
    EVE_CoCmd_text(s_pHalContext, s_pHalContext->Width / 2, 100, 31, OPT_CENTER, "Display bitmap by inflate from media fifo");
    Display_End(s_pHalContext);
    EVE_MediaFifo_close(s_pHalContext);

    SAMAPP_DELAY;
}

/**
* @brief API to demonstrate the usage of inflate command - compression done via zlib
*
*/
void SAMAPP_Utility_CmdInflateFromCommand()
{
    uint16_t w = 320;
    uint16_t h = 240;

    Draw_Text(s_pHalContext, "Example for: CMD_INFLATE with data from command fifo");

    /**********************************************************************************/
    /* Below code demonstrates the usage of inflate function                          */
    /* Download the deflated data into command buffer and in turn coprocessor inflate */
    /* the deflated data and outputs at 0 location                                    */
    /**********************************************************************************/
    /* Clear the memory at location 0 - any previous bitmap data */
    EVE_CoCmd_memSet(s_pHalContext, RAM_G, 255L, 1L * w * 2 * h);
    /* inflate the data read from binary file */
    EVE_Util_loadInflateFile(s_pHalContext, RAM_G, TEST_DIR "\\bird_320x240_ARGB4.bin");

    /* Set the display list for graphics processor */
    /* Bitmap construction by MCU */
    /* Transfer the data into coprocessor memory directly word by word */
    Display_Start(s_pHalContext);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoCmd_setBitmap(s_pHalContext, RAM_G, ARGB4, w, h);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width - w) / 2 * 16, (s_pHalContext->Height - h) / 2 * 16);
    EVE_CoDl_end(s_pHalContext);

    /*  Display the text information */
    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);
    EVE_CoCmd_text(s_pHalContext, s_pHalContext->Width / 2, 100, 31, OPT_CENTER, "Display bitmap by inflate from command fifo");
    Display_End(s_pHalContext);

    SAMAPP_DELAY;
}

/**
* @brief API to demonstrate Coprocessor fault and recover
*
*/
void SAMAPP_Utility_coprocessorFaultRecover()
{
    int32_t bitmapWidth = 128;
    int32_t bitmapHeight = 128;
    Draw_Text(s_pHalContext, "Example for: Coprocessor fault and recover");

    //Fault case: enable interlace option
    EVE_Util_loadImageFile(s_pHalContext, RAM_G, TEST_DIR "\\lenaface40_unsupported.png", NULL, 0);

    Display_Start(s_pHalContext);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width / 2 - bitmapWidth / 2) * 16,
        (s_pHalContext->Height / 2 - bitmapHeight / 2) * 16);
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);
    EVE_sleep(100);

    helperCoprocessorFaultReport();
    helperCoprocessorFaultRecover();

    //Fault case: change bit depth into 7
    EVE_Util_loadImageFile(s_pHalContext, RAM_G, TEST_DIR "\\lenaface40_corrupted.png", NULL, 0);

    Display_Start(s_pHalContext);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width / 2 - bitmapWidth / 2) * 16,
        (s_pHalContext->Height / 2 - bitmapHeight / 2) * 16);
    EVE_CoDl_end(s_pHalContext);

    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 100, 31, OPT_CENTER,
        "This PNG images is loaded before\ncoprocessor fault and recovered");
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief API to demonstrate CMD_FILLWIDTH
*
*/
void SAMAPP_Utility_fillWidth()
{
    int16_t x;
    int16_t y;
    int16_t fill_w;

    Draw_Text(s_pHalContext, "Example for: CMD_FILLWIDTH");

    Display_Start(s_pHalContext);
    fill_w = 100;
    EVE_CoCmd_fillWidth(s_pHalContext, fill_w);

    x = 100;
    y = 200;
    EVE_CoDl_colorRgb(s_pHalContext, 0, 255, 0);
    EVE_CoDl_begin(s_pHalContext, RECTS);
    EVE_CoDl_vertex2f_4(s_pHalContext, x * 16, y * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (x + fill_w) * 16, (y + 300) * 16);
    EVE_CoDl_end(s_pHalContext);
    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);
    EVE_CoCmd_text(s_pHalContext, x, y, 31, OPT_FILL, "This test doesn't fit on one line");

    x = 400;
    y = 200;
    EVE_CoDl_colorRgb(s_pHalContext, 0, 255, 0);
    EVE_CoDl_begin(s_pHalContext, RECTS);
    EVE_CoDl_vertex2f_4(s_pHalContext, (x - fill_w / 2) * 16, (y - 150) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (x + fill_w / 2) * 16, (y + 150) * 16);
    EVE_CoDl_end(s_pHalContext);
    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);
    EVE_CoCmd_text(s_pHalContext, x, y, 31, OPT_FILL | OPT_CENTER, "This test doesn't fit on one line");

    x = 100;
    y = 600;
    fill_w = 2;
    EVE_CoCmd_fillWidth(s_pHalContext, fill_w);

    EVE_CoDl_colorRgb(s_pHalContext, 0, 255, 0);
    EVE_CoDl_begin(s_pHalContext, RECTS);
    EVE_CoDl_vertex2f_4(s_pHalContext, x * 16, y * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (x + 100) * 16, (y + 200) * 16);
    EVE_CoDl_end(s_pHalContext);
    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);
    EVE_CoCmd_text(s_pHalContext, x, y, 31, OPT_FILL, "line one line two");

    x = 400;
    EVE_CoDl_colorRgb(s_pHalContext, 0, 255, 0);
    EVE_CoDl_begin(s_pHalContext, RECTS);
    EVE_CoDl_vertex2f_4(s_pHalContext, x * 16, y * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (x + 160) * 16, (y + 100) * 16);
    EVE_CoDl_end(s_pHalContext);
    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);
    EVE_CoCmd_text(s_pHalContext, x, y, 31, 0, "line one \nline two");
    Display_End(s_pHalContext);
    SAMAPP_DELAY;

    // Cover the cmd_button 
    Display_Start(s_pHalContext);
    y = 200;
    fill_w = 300;
    EVE_CoCmd_fillWidth(s_pHalContext, fill_w);
    EVE_CoCmd_button(s_pHalContext, 100, y, fill_w, 200, 31, OPT_FILL, "This test doesn't fit on one line");
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief API to demonstrate OTP_FORMAT
*
*/
void SAMAPP_Utility_printType() //must call after SAMAPP_ExtendedFormat_Font
{
    Draw_Text(s_pHalContext, "Example for: OPT_FORMAT");
    uint8_t c = 51;
    uint32_t a = 0x12a000;
    int16_t mV = 1947;
    int32_t t = 680;

    Display_StartColor(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 });
    EVE_CoCmd_text(s_pHalContext, 10, 20, 31, 0, "%");
    EVE_CoCmd_text(s_pHalContext, 10, 20 + 50 * 1, 31, 0, "%%");
    EVE_CoCmd_text(s_pHalContext, 10, 20 + 50 * 2, 31, 0, "%d%%%");
    EVE_CoCmd_text(s_pHalContext, 10, 20 + 50 * 3, 31, OPT_FORMAT, "%d * %d = %d", 100, 200, 20000);
    EVE_CoCmd_text(s_pHalContext, 10, 20 + 50 * 4, 31, OPT_FORMAT, "%3d%% complete", c); /*result:  51 % complete */
    EVE_CoCmd_text(s_pHalContext, 10, 20 + 50 * 5, 31, OPT_FORMAT, "base address %06x", a); /*result:  base address 12a000 */
    EVE_CoCmd_text(s_pHalContext, 10, 20 + 50 * 6, 31, OPT_FORMAT, "%+5.3d mV", mV); /*result:  + 1947 mV */
    EVE_CoCmd_text(s_pHalContext, 10, 20 + 50 * 7, 31, OPT_FORMAT, "Temp. %d.%.1d degrees", t / 10,
        t % 10); /*result:  Temp. 68.0 degrees */

    EVE_Hal_wrMem(s_pHalContext, RAM_G + 4, "Hello ", 8); //address and length should be aline to 4
    EVE_CoCmd_text(s_pHalContext, 10, 20 + 50 * 8, 31, OPT_FORMAT, "%s %d times", RAM_G + 4, 5); /*result:  Hello 5 times */

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief Change screen orientation from landscape to portrait mode.  
* 
* Setscratch command is also introduced, it sets the temporary bitmap handle for buttons, 
* keys, and graidents.
*
*/
void SAMAPP_Utility_screenRotate()
{
    uint8_t text[100];

    Draw_Text(s_pHalContext, "Example for: Change screen orientation from landscape to portrait mode");

    for (uint32_t rotateMode = 0; rotateMode < 8; rotateMode++)
    {
        Display_StartColor(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 });
        EVE_CoCmd_setRotate(s_pHalContext, rotateMode);
        switch (rotateMode)
        {
        case 0:
        case 1:
        case 4:
        case 5:
            snprintf(text, 100, "Landscape Mode, rotate value= %d", rotateMode);
            EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 50, 31, OPT_CENTER,
                text);
            break;
        default:
            snprintf(text, 100, "Portrait Mode\nRotate value= %d", rotateMode);
            EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Height / 2), 50, 31, OPT_CENTER,
                text);
            break;
        }

        Display_End(s_pHalContext);
        SAMAPP_DELAY;
    }

    //reset the rotation angle
    EVE_CoCmd_setRotate(s_pHalContext, 0);
}

/**
* @brief API to demonstrate number base
*
*/
void SAMAPP_Utility_numberBases()
{
    Draw_Text(s_pHalContext, "Example for: Number base");

    Display_StartColor(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 });
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 50, 32, OPT_CENTER,
        "Built-in bases conversion");
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 100, 32, OPT_CENTER,
        "bases from 2 to 32:");

    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2) - 200, 200, 31, 0,
        "Binary(2):");
    EVE_CoCmd_setBase(s_pHalContext, 2);
    EVE_CoCmd_number(s_pHalContext, (int16_t) (s_pHalContext->Width / 2 + 100), 200, 31, 0, 1000);

    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2 - 200), 300, 31, 0,
        "Octal(8):");
    EVE_CoCmd_setBase(s_pHalContext, 8);
    EVE_CoCmd_number(s_pHalContext, (int16_t) (s_pHalContext->Width / 2 + 100), 300, 31, 0, 1000);

    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2 - 200), 400, 31, 0,
        "Decimal(10):");
    EVE_CoCmd_setBase(s_pHalContext, 10);
    EVE_CoCmd_number(s_pHalContext, (int16_t) (s_pHalContext->Width / 2 + 100), 400, 31, 0, 1000);

    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2 - 200), 500, 31, 0,
        "Hex(16):");
    EVE_CoCmd_setBase(s_pHalContext, 16);
    EVE_CoCmd_number(s_pHalContext, (int16_t) (s_pHalContext->Width / 2 + 100), 500, 31, 0, 1000);

    //Revert the base back to decimal because numeric base is part of the global context.
    EVE_CoCmd_setBase(s_pHalContext, 10);

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief read back the result of a command
*
*/
void SAMAPP_Utility_crcCheck()
{
    const uint32_t memWrite = 0x66666666; // 66 = 'f'
    const uint32_t memSizeTest = 1024; // bytes
    const uint32_t crcExpected = 0x17D32620;
    uint32_t memcrcRet = 0;

    Draw_Text(s_pHalContext, "Example for: Check Crc32 for 1024 bytes on RAM_G");

    EVE_CoCmd_memSet(s_pHalContext, RAM_G, memWrite, memSizeTest);
    EVE_Cmd_waitFlush(s_pHalContext);/*reset cmd index*/

    uint32_t cmdbuff_write_ptr = EVE_Cmd_wp(s_pHalContext);
    uint32_t crc_result_addr = RAM_CMD + ((cmdbuff_write_ptr + 12) & EVE_CMD_FIFO_MASK);

    EVE_CoCmd_memCrc(s_pHalContext, 0, memSizeTest, 0);
    EVE_Cmd_waitFlush(s_pHalContext);

    EVE_CoCmd_regRead(s_pHalContext, crc_result_addr, &memcrcRet);
    eve_printf_debug("current CRC number [0,1023) is 0x%x \r\n", memcrcRet);
    if (memcrcRet == crcExpected)
    {
        eve_printf_debug("Crc return value is as expected");
        Draw_TextColor(s_pHalContext, "Crc value is as expected", (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 });
    }
    SAMAPP_DELAY;
}

void SAMAPP_Utility() {
    SAMAPP_Utility_wait();
    SAMAPP_Utility_callList();
    SAMAPP_Utility_callListWithAlignment();
    SAMAPP_Utility_appendCommand();
    SAMAPP_Utility_bulkTransfer();
    SAMAPP_Utility_cmdInflateFromFlash();
    SAMAPP_Utility_CmdInflateFromFifo();
    SAMAPP_Utility_CmdInflateFromCommand();
    SAMAPP_Utility_coprocessorFaultRecover();
    SAMAPP_Utility_fillWidth();
    SAMAPP_Utility_printType();
    SAMAPP_Utility_screenRotate();
    SAMAPP_Utility_numberBases();
    SAMAPP_Utility_crcCheck();
}


