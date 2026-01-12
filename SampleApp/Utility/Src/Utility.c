/**
 * @file Utility.c
 * @brief Sample usage of some useful command
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

#include "Utility.h"
#include "FileTransfer.h"
#include "FlashHelper.h"

#define SAMAPP_DELAY_MS         2000
#define SAMAPP_DELAY            EVE_sleep(SAMAPP_DELAY_MS)
#define UTIL_DELAY_MS           10
#define UTIL_LOAD_ADDR          DDR_BITMAPS_STARTADDR
#define UTIL_TITLE_FONT         31
#define UTIL_TITLE_X            10
#define UTIL_TITLE_Y            50
#define UTIL_TITLE_Y_INC        50
#define UTIL_TEXT_LEN           100
#define UTIL_NO_OF_CALLLIST     5
#define UTIL_CALLLIST_ADDR      RAM_G
#define UTIL_CALLLIST_ANGLE     60
#define UTIL_COPYLIST_X         500
#define UTIL_COPYLIST_Y         500
#define UTIL_APPEND_CMD_LEN     16
#define UTIL_APPEND_TEXT_Y_INC  300
#define UTIL_BULK_CIRCLE_NUM    100
#define UTIL_BULK_LOOP          600
#define UTIL_BULK_PREC          4
#define UTIL_MEDIAFIFO_ADDR     RAM_G
#define UTIL_MEDIAFIFO_SZ       (20 * 1024)
#define UTIL_FILLW_X1           100
#define UTIL_FILLW_Y1           200
#define UTIL_FILLW_X2           400
#define UTIL_FILLW_Y2           600
#define UTIL_FILLW_W1           100
#define UTIL_FILLW_W2           2
#define UTIL_FILLW_W3           300
#define UTIL_FILLW_H1           300
#define UTIL_FILLW_H2           200
#define UTIL_ROTATE_MODE        8
#define UTIL_NUM_TEXT_X1        (s_pHalContext->Width / 2 - 200)
#define UTIL_NUM_TEXT_X2        (s_pHalContext->Width / 2 + 100)
#define UTIL_SKIPCOND_BTN_TAG   1
#define UTIL_SKIPCOND_EXIT_TAG  2
#define UTIL_INT_PIC_TAG        1
#define UTIL_LOAD_ADDR1         DDR_BITMAPS_STARTADDR1
#define UTIL_ZORDER_X1          50
#define UTIL_ZORDER_X2          500
#define UTIL_ZORDER_Y           100

static EVE_HalContext s_halContext;
static EVE_HalContext* s_pHalContext;
static void SAMAPP_Utility();
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
        "This sample demonstrates the use of utilites of EVE", 
        "",
        ""
    };
    WelcomeScreen(s_pHalContext, info);

    SAMAPP_Utility();

    EVE_Util_clearScreen(s_pHalContext);
    Gpu_Release(s_pHalContext);
    return 0;
}

/**
 * @brief Flush a command to REG_CMDB_WRITE
 *
 * @param command Command value
 * @return None
 */
static void helperCMDBWrite(uint32_t command)
{
    EVE_Hal_wr32(s_pHalContext, REG_CMDB_WRITE, command);
}

/**
 * @brief Flush a string to REG_CMDB_WRITE
 *
 * @param str string to write
 * @return None
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
 * @param None
 * @return None
 */
static void SAMAPP_Utility_wait()
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
 * @param None
 * @return None
 */
static void SAMAPP_Utility_callList()
{
    uint32_t endPtr = UTIL_CALLLIST_ADDR + 4;
    uint32_t endPtrArr[UTIL_NO_OF_CALLLIST];
    const uint16_t x = (s_pHalContext->Width - UTIL_JPG_FLOWER_W) / 2;
    const uint16_t y = (s_pHalContext->Height - UTIL_JPG_FLOWER_H) / 2;
    char str[UTIL_TEXT_LEN];

    Draw_Text(s_pHalContext, "Example for: CMD_CALLLIST, CMD_NEWLIST, CMD_ENDLIST and CMD_RETURN");

    Draw_TextColor(s_pHalContext, "Constructing 5 lists ...", (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 });
    for (int i = 0; i < UTIL_NO_OF_CALLLIST; i++)
    {
        endPtrArr[i] = endPtr;

        EVE_CoCmd_newList(s_pHalContext, endPtr);
        EVE_CoCmd_setBitmap(s_pHalContext, UTIL_LOAD_ADDR, RGB565, UTIL_JPG_FLOWER_W, UTIL_JPG_FLOWER_H);

        EVE_CoDl_saveContext(s_pHalContext);
        EVE_CoDl_begin(s_pHalContext, BITMAPS);
        EVE_CoCmd_loadIdentity(s_pHalContext);
        EVE_CoCmd_rotateAround(s_pHalContext, UTIL_JPG_FLOWER_W / 2, UTIL_JPG_FLOWER_H / 2, i * UTIL_CALLLIST_ANGLE * 65536 / 360, 65536 * 1);
        EVE_CoCmd_setMatrix(s_pHalContext);
        EVE_CoDl_vertex2f_4(s_pHalContext, (x) * 16, (y) * 16);
        EVE_CoDl_end(s_pHalContext);
        EVE_CoDl_restoreContext(s_pHalContext);

        snprintf(str, UTIL_TEXT_LEN, "Displaying list number %u", i);
        EVE_CoCmd_text(s_pHalContext, x, UTIL_TITLE_Y, UTIL_TITLE_FONT, 0, str);
        EVE_CoCmd_endList(s_pHalContext);

        EVE_Cmd_waitFlush(s_pHalContext);
        uint32_t endAddr;
        EVE_CoCmd_getPtr(s_pHalContext, &endAddr); //cmd_getPtr will be always 128 bytes align
        EVE_Cmd_waitFlush(s_pHalContext);

        eve_printf_debug("Compiled list %u at %u, size: %u Bytes\n", i, endPtr, endAddr - endPtr);
        endPtr = endAddr;
    }
    Draw_TextColor(s_pHalContext, "Constructed 5 lists", (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 });

    EVE_Util_loadImageFile(s_pHalContext, UTIL_LOAD_ADDR, TEST_DIR UTIL_JPG_FLOWER, NULL, 0);
    for (int i = 0; i < UTIL_NO_OF_CALLLIST; i++)
    {
        snprintf(str, UTIL_TEXT_LEN, "Calling List number %u", i);
        Draw_TextColor(s_pHalContext, str, (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 });

        Display_Start(s_pHalContext, (uint8_t[]) { 125, 125, 125 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
        EVE_CoCmd_callList(s_pHalContext, endPtrArr[i]);
        Display_End(s_pHalContext);
        SAMAPP_DELAY;

        //check whether there is a CMD_RETURN at the end of list
        if (i == 0)
        {
            uint32_t cmd = 0;
            EVE_CoCmd_regRead(s_pHalContext, UTIL_CALLLIST_ADDR + endPtrArr[i] + 108, &cmd); // 108 is the length of the command list from CMD_NEWLIST to CMD_ENDLIST
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
 * @param None
 * @return None
 */
static void SAMAPP_Utility_callListWithAlignment()
{
    Draw_Text(s_pHalContext, "Example for: Construct a command list in RAM_G to show a button");

    //Construct a command list in RAM_G to show a button
    EVE_Hal_wr32(s_pHalContext, UTIL_LOAD_ADDR + 0 * 4, SAVE_CONTEXT());
    EVE_Hal_wr32(s_pHalContext, UTIL_LOAD_ADDR + 1 * 4, CLEAR_COLOR_RGB(255, 255, 255));
    EVE_Hal_wr32(s_pHalContext, UTIL_LOAD_ADDR + 2 * 4, CLEAR(1, 1, 1));
    EVE_Hal_wr32(s_pHalContext, UTIL_LOAD_ADDR + 3 * 4, CMD_BUTTON);

    EVE_Hal_wr32(s_pHalContext, UTIL_LOAD_ADDR + 4 * 4, 160 << 16 | 160); //y | x coordinate of button
    EVE_Hal_wr32(s_pHalContext, UTIL_LOAD_ADDR + 5 * 4, 200 << 16 | 324); //h | w
    EVE_Hal_wr32(s_pHalContext, UTIL_LOAD_ADDR + 6 * 4, 0 << 16 | 31); //options | Font handle
    EVE_Hal_wr32(s_pHalContext, UTIL_LOAD_ADDR + 7 * 4, 'T' << 24 | 'S' << 16 | 'E' << 8 | 'T');

    EVE_Hal_wr32(s_pHalContext, UTIL_LOAD_ADDR + 8 * 4, '\0' << 24 | '\0' << 16 | '\0' << 8 | '\0');

    EVE_Hal_wr32(s_pHalContext, UTIL_LOAD_ADDR + 8 * 4, RESTORE_CONTEXT()); //Assume 3 bytes padding bytes for alignment 
    EVE_Hal_wr32(s_pHalContext, UTIL_LOAD_ADDR + 9 * 4, CMD_RETURN); //return to the command buffer

    //Call cmd_list with data in RAM_G
    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_CoCmd_callList(s_pHalContext, UTIL_LOAD_ADDR);
    Display_End(s_pHalContext);

    SAMAPP_DELAY;
}

/**
 * @brief API to demonstrate copying a display list
 *
 * @param None
 * @return None
 */
static void SAMAPP_Utility_copyList()
{
    Draw_Text(s_pHalContext, "Example for: Copy a display list to RAM_G");

    /* Create a display list and copy to RAM_G */
    Display_Start(s_pHalContext, (uint8_t[]) { 100, 0, 0 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_CoCmd_text(s_pHalContext, UTIL_COPYLIST_X, UTIL_COPYLIST_Y, UTIL_TITLE_FONT, OPT_CENTER, "This is a cached display list");
    EVE_CoCmd_copyList(s_pHalContext, UTIL_CALLLIST_ADDR);

    /* Display a new display list */
    Display_Start(s_pHalContext, (uint8_t[]) { 0, 0, 100 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_CoCmd_text(s_pHalContext, UTIL_COPYLIST_X, UTIL_COPYLIST_Y, UTIL_TITLE_FONT, OPT_CENTER, "Displaying a new display list");
    Display_End(s_pHalContext);
    SAMAPP_DELAY;

    /* Display cached display list */
    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_CoCmd_callList(s_pHalContext, UTIL_CALLLIST_ADDR);
    Display_End(s_pHalContext);

    SAMAPP_DELAY;
}

/**
 * @brief API to demonstrate the use of append commands
 *
 * @param None
 * @return None
 */
static void SAMAPP_Utility_appendCommand()
{
    uint32_t AppendCmds[16];
    int16_t xoffset;
    int16_t yoffset;

    Draw_Text(s_pHalContext, "Example for: Appending DL command");

    /*************************************************************************/
    /* Below code demonstrates the usage of coprocessor append command - to append any*/
    /* mcu specific graphics commands to coprocessor generated graphics commands      */
    /*************************************************************************/

    /* Bitmap construction by MCU */
    /* Construct the bitmap data to be copied in the temp buffer then by using
    coprocessor append command copy it into graphics processor DL memory */
    xoffset = (int16_t)((s_pHalContext->Width - UTIL_JPG_MANDRILL_W) / 4);
    yoffset = (int16_t)((s_pHalContext->Height / 3) - UTIL_JPG_MANDRILL_H / 2);

    EVE_CoCmd_dlStart(s_pHalContext);
    AppendCmds[0] = CLEAR_COLOR_RGB(255, 0, 0);
    AppendCmds[1] = CLEAR(1, 1, 1);
    AppendCmds[2] = COLOR_RGB(255, 255, 255);
    AppendCmds[3] = BEGIN(BITMAPS);
    AppendCmds[4] = BITMAP_SOURCE(UTIL_LOAD_ADDR);
    AppendCmds[5] = BITMAP_SOURCEH(UTIL_LOAD_ADDR >> 24);
    AppendCmds[6] = BITMAP_LAYOUT(RGB565, 2 * UTIL_JPG_MANDRILL_W, UTIL_JPG_MANDRILL_H);
    AppendCmds[7] = BITMAP_SIZE(BILINEAR, BORDER, BORDER, UTIL_JPG_MANDRILL_W, UTIL_JPG_MANDRILL_H);
    AppendCmds[8] = VERTEX2F(xoffset * 16, yoffset * 16);
    AppendCmds[9] = END();

    /* Download the bitmap data*/
    EVE_Util_loadImageFile(s_pHalContext, UTIL_LOAD_ADDR, TEST_DIR UTIL_JPG_MANDRILL, NULL, OPT_RGB565);

    /* Download the DL data constructed by the MCU to ram */
    EVE_Hal_wrMem(s_pHalContext, UTIL_LOAD_ADDR + UTIL_JPG_MANDRILL_W * UTIL_JPG_MANDRILL_H * 2, (uint8_t *)AppendCmds, 9 * 4);

    /* Call the append api for copying the above generated data into graphics processor DL commands */
    EVE_CoCmd_append(s_pHalContext, UTIL_LOAD_ADDR + UTIL_JPG_MANDRILL_W * UTIL_JPG_MANDRILL_H * 2, 9 * 4);
    /*  Display the text information */
    yoffset += UTIL_APPEND_TEXT_Y_INC;
    EVE_CoCmd_text(s_pHalContext, xoffset, yoffset, UTIL_TITLE_FONT, 0, "Display bitmap by Append");
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief write bulk of data to CMDB
 *
 * @param None
 * @return None
 */
static void SAMAPP_Utility_bulkTransfer()
{
    SAMAPP_Circle circles[UTIL_BULK_CIRCLE_NUM];
    uint32_t xoffset = (s_pHalContext->Width - UTIL_JPG_MANDRILL_W) / 2;
    uint32_t yoffset = (s_pHalContext->Height - UTIL_JPG_MANDRILL_H) / 2;
    int count = 0;

    Draw_Text(s_pHalContext, "Example for: Bulk transfer");
    EVE_Util_clearScreen(s_pHalContext);

    for (int i = 0; i < UTIL_BULK_CIRCLE_NUM; i++)
    {
        circles[i].visible = 0;
        circles[i].opacity = 0;
        circles[i].radius = 0;
        circles[i].step = 0;
        circles[i].x = 0;
        circles[i].y = 0;
        circles[i].color.b = 0;
        circles[i].color.g = 0;
        circles[i].color.r = 0;
    }

    EVE_Util_loadImageFile(s_pHalContext, UTIL_LOAD_ADDR, TEST_DIR UTIL_JPG_MANDRILL, NULL, OPT_RGB565);

    while (count++ < UTIL_BULK_LOOP)
    {
        /*Display List start*/
        helperCMDBWrite(CMD_DLSTART);
        helperCMDBWrite(CLEAR_COLOR_RGB(0, 0, 0));
        helperCMDBWrite(CLEAR(1, 1, 1));
        helperCMDBWrite(VERTEX_FORMAT(2));
        // draw circles
        for (int i = 0; i < UTIL_BULK_CIRCLE_NUM; i++)
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
                helperCMDBWrite(COLOR_RGB(circles[i].color.r, circles[i].color.g, circles[i].color.b));

                helperCMDBWrite(BEGIN(POINTS));
                helperCMDBWrite(POINT_SIZE(circles[i].radius * UTIL_BULK_PREC));
                helperCMDBWrite(VERTEX2F(circles[i].x * UTIL_BULK_PREC, circles[i].y * UTIL_BULK_PREC));
                helperCMDBWrite(END());
            }
        }
        // reset coloring
        helperCMDBWrite(COLOR_A(255));
        helperCMDBWrite(COLOR_RGB(255, 255, 255));

        // draw image
        helperCMDBWrite(BITMAP_HANDLE(2));
        helperCMDBWrite(BEGIN(BITMAPS));
        helperCMDBWrite(BITMAP_SOURCE(UTIL_LOAD_ADDR));
        helperCMDBWrite(BITMAP_SOURCEH(UTIL_LOAD_ADDR >> 24));
        helperCMDBWrite(BITMAP_LAYOUT(RGB565, UTIL_JPG_MANDRILL_W * 2, UTIL_JPG_MANDRILL_H));
        helperCMDBWrite(BITMAP_LAYOUT_H((UTIL_JPG_MANDRILL_W * 2) >> 10, UTIL_JPG_MANDRILL_H >> 9));
        helperCMDBWrite(BITMAP_SIZE(BILINEAR, BORDER, BORDER, UTIL_JPG_MANDRILL_W, UTIL_JPG_MANDRILL_H));
        helperCMDBWrite(BITMAP_SIZE_H(UTIL_JPG_MANDRILL_W >> 9, UTIL_JPG_MANDRILL_H >> 9));
        helperCMDBWrite(VERTEX2F(xoffset * UTIL_BULK_PREC, yoffset * UTIL_BULK_PREC));
        helperCMDBWrite(END());

        // Draw the text in top of screen
        uint32_t x = s_pHalContext->Width / 2;
        uint32_t y = UTIL_TITLE_Y;
        helperCMDBWrite(CMD_TEXT);
        helperCMDBWrite(((y << 16) | (x & 0xffff)));
        helperCMDBWrite(((OPT_CENTER << 16) | (UTIL_TITLE_FONT & 0xffff)));
        helperCMDBWriteString("Bulk transfer demostration");

        y += UTIL_TITLE_Y_INC;
        helperCMDBWrite(CMD_TEXT);
        helperCMDBWrite(((y << 16) | (x & 0xffff)));
        helperCMDBWrite(((OPT_CENTER << 16) | (UTIL_TITLE_FONT & 0xffff)));
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
 * @param None
 * @return None
 */
static void SAMAPP_Utility_cmdInflateFromFlash()
{
    Draw_Text(s_pHalContext, "Example for: CMD_INFLATE with OPT_FLASH");

    FlashHelper_SwitchState(s_pHalContext, FLASH_STATUS_FULL); // full mode

    EVE_CoCmd_flashSource(s_pHalContext, addr_flash + UTIL_ADDR_FLASH_MANDRILL);
    EVE_CoCmd_inflate(s_pHalContext, UTIL_LOAD_ADDR, OPT_FLASH);

    /* Display inflated image */
    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoCmd_setBitmap(s_pHalContext, UTIL_LOAD_ADDR, YCBCR, UTIL_JPG_MANDRILL_W, UTIL_JPG_MANDRILL_H);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width - UTIL_JPG_MANDRILL_W) / 2 * 16, (s_pHalContext->Height - UTIL_JPG_MANDRILL_H) / 2 * 16);
    EVE_CoDl_end(s_pHalContext);

    /* Display the text information */
    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);
    EVE_CoCmd_text(s_pHalContext, s_pHalContext->Width / 2, UTIL_TITLE_Y, UTIL_TITLE_FONT, OPT_CENTER, "Display bitmap by inflate (OPT_FLASH)");
    Display_End(s_pHalContext);

    SAMAPP_DELAY;
}

/**
 * @brief API to demonstrate display bitmap by inflate data from media fifo
 *
 * @param None
 * @return None
 */
static void SAMAPP_Utility_CmdInflateFromFifo()
{
    Draw_Text(s_pHalContext, "Example for: CMD_INFLATE with data from media fifo");

    /* Clear the memory - any previous bitmap data */
    EVE_CoCmd_memSet(s_pHalContext, UTIL_LOAD_ADDR, 255L, 1L * UTIL_JPG_MANDRILL_W * 2 * UTIL_JPG_MANDRILL_H);
    /* Load data to mediafifo */
    EVE_MediaFifo_set(s_pHalContext, UTIL_MEDIAFIFO_ADDR, UTIL_MEDIAFIFO_SZ);
    EVE_CoCmd_inflate(s_pHalContext, UTIL_LOAD_ADDR, OPT_MEDIAFIFO);
    EVE_Util_loadMediaFile(s_pHalContext, TEST_DIR UTIL_JPG_MANDRILL_INFLATE, NULL);

    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoCmd_setBitmap(s_pHalContext, UTIL_LOAD_ADDR, YCBCR, UTIL_JPG_MANDRILL_W, UTIL_JPG_MANDRILL_H);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width - UTIL_JPG_MANDRILL_W) / 2 * 16, (s_pHalContext->Height - UTIL_JPG_MANDRILL_H) / 2 * 16);
    EVE_CoDl_end(s_pHalContext);

    /* Display the text information */
    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);
    EVE_CoCmd_text(s_pHalContext, s_pHalContext->Width / 2, UTIL_TITLE_Y, UTIL_TITLE_FONT, OPT_CENTER, "Display bitmap by inflate from media fifo");
    Display_End(s_pHalContext);
    EVE_MediaFifo_close(s_pHalContext);

    SAMAPP_DELAY;
}

/**
 * @brief API to demonstrate the usage of inflate command - compression done via zlib
 *
 * @param None
 * @return None
 */
static void SAMAPP_Utility_CmdInflateFromCommand()
{
    Draw_Text(s_pHalContext, "Example for: CMD_INFLATE with data from command fifo");

    /**********************************************************************************/
    /* Below code demonstrates the usage of inflate function                          */
    /**********************************************************************************/
    /* Clear the memory - any previous bitmap data */
    EVE_CoCmd_memSet(s_pHalContext, UTIL_LOAD_ADDR, 255L, 1L * UTIL_JPG_MANDRILL_W * 2 * UTIL_JPG_MANDRILL_H);
    /* inflate the data read from binary file */
    EVE_Util_loadInflateFile(s_pHalContext, UTIL_LOAD_ADDR, TEST_DIR UTIL_JPG_MANDRILL_INFLATE);

    /* Set the display list for graphics processor */
    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoCmd_setBitmap(s_pHalContext, UTIL_LOAD_ADDR, YCBCR, UTIL_JPG_MANDRILL_W, UTIL_JPG_MANDRILL_H);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width - UTIL_JPG_MANDRILL_W) / 2 * 16, (s_pHalContext->Height - UTIL_JPG_MANDRILL_H) / 2 * 16);
    EVE_CoDl_end(s_pHalContext);

    /* Display the text information */
    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);
    EVE_CoCmd_text(s_pHalContext, s_pHalContext->Width / 2, UTIL_TITLE_Y, UTIL_TITLE_FONT, OPT_CENTER, "Display bitmap by inflate from command fifo");
    Display_End(s_pHalContext);

    SAMAPP_DELAY;
}

/**
 * @brief API to demonstrate Coprocessor fault and recover
 *
 * @param None
 * @return None
 */
static void SAMAPP_Utility_coprocessorFaultRecover()
{
    Draw_Text(s_pHalContext, "Example for: Coprocessor fault and recover");

    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    //Fault case: enable interlace option
    EVE_Util_loadImageFile(s_pHalContext, RAM_G, TEST_DIR UTIL_PNG_UNSUPPORT, NULL, 0);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width / 2 - UTIL_PNG_FAILED_W / 2) * 16,
        (s_pHalContext->Height / 2 - UTIL_PNG_FAILED_H / 2) * 16);
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);
    EVE_sleep(100);

    EVE_Util_coprocessorFaultRecover(s_pHalContext);

    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    //Fault case: change bit depth into 7
    EVE_Util_loadImageFile(s_pHalContext, RAM_G, TEST_DIR UTIL_PNG_CORRUPT, NULL, 0);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_vertex2f_4(s_pHalContext, (s_pHalContext->Width / 2 - UTIL_PNG_FAILED_W / 2) * 16,
        (s_pHalContext->Height / 2 - UTIL_PNG_FAILED_H / 2) * 16);
    EVE_CoDl_end(s_pHalContext);
    Display_End(s_pHalContext);

    EVE_Util_coprocessorFaultRecover(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief API to demonstrate CMD_FILLWIDTH
 *
 * @param None
 * @return None
 */
static void SAMAPP_Utility_fillWidth()
{
    int16_t x;
    int16_t y;
    int16_t fill_w;

    Draw_Text(s_pHalContext, "Example for: CMD_FILLWIDTH");

    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    fill_w = UTIL_FILLW_W1;
    EVE_CoCmd_fillWidth(s_pHalContext, fill_w);

    x = UTIL_FILLW_X1;
    y = UTIL_FILLW_Y1;
    EVE_CoDl_colorRgb(s_pHalContext, 0, 255, 0);
    EVE_CoDl_begin(s_pHalContext, RECTS);
    EVE_CoDl_vertex2f_4(s_pHalContext, x * 16, y * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (x + fill_w) * 16, (y + UTIL_FILLW_H1) * 16);
    EVE_CoDl_end(s_pHalContext);
    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);
    EVE_CoCmd_text(s_pHalContext, x, y, UTIL_TITLE_FONT, OPT_FILL, "This test doesn't fit on one line");

    x = UTIL_FILLW_X2;
    y = UTIL_FILLW_Y1;
    EVE_CoDl_colorRgb(s_pHalContext, 0, 255, 0);
    EVE_CoDl_begin(s_pHalContext, RECTS);
    EVE_CoDl_vertex2f_4(s_pHalContext, (x - fill_w / 2) * 16, (y - UTIL_FILLW_H1 / 2) * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (x + fill_w / 2) * 16, (y + UTIL_FILLW_H1 / 2) * 16);
    EVE_CoDl_end(s_pHalContext);
    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);
    EVE_CoCmd_text(s_pHalContext, x, y, UTIL_TITLE_FONT, OPT_FILL | OPT_CENTER, "This test doesn't fit on one line");

    x = UTIL_FILLW_X1;
    y = UTIL_FILLW_Y2;
    fill_w = UTIL_FILLW_W2;
    EVE_CoCmd_fillWidth(s_pHalContext, fill_w);

    EVE_CoDl_colorRgb(s_pHalContext, 0, 255, 0);
    EVE_CoDl_begin(s_pHalContext, RECTS);
    EVE_CoDl_vertex2f_4(s_pHalContext, x * 16, y * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (x + UTIL_FILLW_W1) * 16, (y + UTIL_FILLW_H1) * 16);
    EVE_CoDl_end(s_pHalContext);
    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);
    EVE_CoCmd_text(s_pHalContext, x, y, UTIL_TITLE_FONT, OPT_FILL, "line one line two with fill");

    x = UTIL_FILLW_X2;
    EVE_CoDl_colorRgb(s_pHalContext, 0, 255, 0);
    EVE_CoDl_begin(s_pHalContext, RECTS);
    EVE_CoDl_vertex2f_4(s_pHalContext, x * 16, y * 16);
    EVE_CoDl_vertex2f_4(s_pHalContext, (x + UTIL_FILLW_W1) * 16, (y + UTIL_FILLW_H1) * 16);
    EVE_CoDl_end(s_pHalContext);
    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 0);
    EVE_CoCmd_text(s_pHalContext, x, y, UTIL_TITLE_FONT, 0, "line one \nline two \nwithout fill");
    Display_End(s_pHalContext);
    SAMAPP_DELAY;

    // Cover the cmd_button 
    Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    y = UTIL_FILLW_Y1;
    fill_w = UTIL_FILLW_W3;
    EVE_CoCmd_fillWidth(s_pHalContext, fill_w);
    EVE_CoCmd_button(s_pHalContext, UTIL_FILLW_X1, y, fill_w, UTIL_FILLW_H2, UTIL_TITLE_FONT, OPT_FILL, "This test doesn't fit on one line");
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief API to demonstrate OTP_FORMAT
 *
 * @param None
 * @return None
 */
static void SAMAPP_Utility_printType() //must call after SAMAPP_ExtendedFormat_Font
{
    Draw_Text(s_pHalContext, "Example for: OPT_FORMAT");
    uint8_t c = 51;
    uint32_t a = 0x12a000;
    int16_t mV = 1947;
    int32_t t = 680;

    Display_Start(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_CoCmd_text(s_pHalContext, UTIL_TITLE_X, UTIL_TITLE_Y, UTIL_TITLE_FONT, 0, "%");
    EVE_CoCmd_text(s_pHalContext, UTIL_TITLE_X, UTIL_TITLE_Y + UTIL_TITLE_Y_INC * 1, UTIL_TITLE_FONT, 0, "%%");
    EVE_CoCmd_text(s_pHalContext, UTIL_TITLE_X, UTIL_TITLE_Y + UTIL_TITLE_Y_INC * 2, UTIL_TITLE_FONT, 0, "%d%%%");
    EVE_CoCmd_text(s_pHalContext, UTIL_TITLE_X, UTIL_TITLE_Y + UTIL_TITLE_Y_INC * 3, UTIL_TITLE_FONT, OPT_FORMAT, "%d * %d = %d", 100, 200, 20000);
    EVE_CoCmd_text(s_pHalContext, UTIL_TITLE_X, UTIL_TITLE_Y + UTIL_TITLE_Y_INC * 4, UTIL_TITLE_FONT, OPT_FORMAT, "%3d%% complete", c); /*result:  51 % complete */
    EVE_CoCmd_text(s_pHalContext, UTIL_TITLE_X, UTIL_TITLE_Y + UTIL_TITLE_Y_INC * 5, UTIL_TITLE_FONT, OPT_FORMAT, "base address %06x", a); /*result:  base address 12a000 */
    EVE_CoCmd_text(s_pHalContext, UTIL_TITLE_X, UTIL_TITLE_Y + UTIL_TITLE_Y_INC * 6, UTIL_TITLE_FONT, OPT_FORMAT, "%+5.3d mV", mV); /*result:  + 1947 mV */
    EVE_CoCmd_text(s_pHalContext, UTIL_TITLE_X, UTIL_TITLE_Y + UTIL_TITLE_Y_INC * 7, UTIL_TITLE_FONT, OPT_FORMAT, "Temp. %d.%.1d degrees", t / 10,
        t % 10); /*result:  Temp. 68.0 degrees */

    EVE_Hal_wrMem(s_pHalContext, RAM_G + 4, "Hello ", 8); //address and length should be aline to 4
    EVE_CoCmd_text(s_pHalContext, UTIL_TITLE_X, UTIL_TITLE_Y + UTIL_TITLE_Y_INC * 8, UTIL_TITLE_FONT, OPT_FORMAT, "%s %d times", RAM_G + 4, 5); /*result:  Hello 5 times */

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief Change screen orientation from landscape to portrait mode.
 *
 * @param None
 * @return None
 */
static void SAMAPP_Utility_screenRotate()
{
    uint8_t text[UTIL_TEXT_LEN];

    Draw_Text(s_pHalContext, "Example for: Change screen orientation from landscape to portrait mode");

    for (uint32_t rotateMode = 0; rotateMode < UTIL_ROTATE_MODE; rotateMode++)
    {
        Display_Start(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
        EVE_CoCmd_setRotate(s_pHalContext, rotateMode);
        switch (rotateMode)
        {
        case 0:
        case 1:
        case 4:
        case 5:
            snprintf(text, UTIL_TEXT_LEN, "Landscape Mode, rotate value= %d", rotateMode);
            break;
        default:
            snprintf(text, UTIL_TEXT_LEN, "Portrait Mode\nRotate value= %d", rotateMode);

            break;
        }
        EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), UTIL_TITLE_Y, UTIL_TITLE_FONT, OPT_CENTER, text);
        Display_End(s_pHalContext);
        SAMAPP_DELAY;
    }

    //reset the rotation angle
    EVE_CoCmd_setRotate(s_pHalContext, 0);
}

/**
 * @brief API to demonstrate number base
 *
 * @param None
 * @return None
 */
static void SAMAPP_Utility_numberBases()
{
    Draw_Text(s_pHalContext, "Example for: Number base");

    Display_Start(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), UTIL_TITLE_Y, UTIL_TITLE_FONT, OPT_CENTER,
        "Built-in bases conversion");
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), UTIL_TITLE_Y + UTIL_TITLE_Y_INC, UTIL_TITLE_FONT, OPT_CENTER,
        "bases from 2 to 32:");

    EVE_CoCmd_text(s_pHalContext, UTIL_NUM_TEXT_X1, UTIL_TITLE_Y + 3 * UTIL_TITLE_Y_INC, UTIL_TITLE_FONT, 0,
        "Binary(2):");
    EVE_CoCmd_setBase(s_pHalContext, 2);
    EVE_CoCmd_number(s_pHalContext, UTIL_NUM_TEXT_X2, UTIL_TITLE_Y + 3 * UTIL_TITLE_Y_INC, UTIL_TITLE_FONT, 0, 1000);

    EVE_CoCmd_text(s_pHalContext, UTIL_NUM_TEXT_X1, UTIL_TITLE_Y + 5 * UTIL_TITLE_Y_INC, UTIL_TITLE_FONT, 0,
        "Octal(8):");
    EVE_CoCmd_setBase(s_pHalContext, 8);
    EVE_CoCmd_number(s_pHalContext, UTIL_NUM_TEXT_X2, UTIL_TITLE_Y + 5 * UTIL_TITLE_Y_INC, UTIL_TITLE_FONT, 0, 1000);

    EVE_CoCmd_text(s_pHalContext, UTIL_NUM_TEXT_X1, UTIL_TITLE_Y + 7 * UTIL_TITLE_Y_INC, UTIL_TITLE_FONT, 0,
        "Decimal(10):");
    EVE_CoCmd_setBase(s_pHalContext, 10);
    EVE_CoCmd_number(s_pHalContext, UTIL_NUM_TEXT_X2, UTIL_TITLE_Y + 7 * UTIL_TITLE_Y_INC, UTIL_TITLE_FONT, 0, 1000);

    EVE_CoCmd_text(s_pHalContext, UTIL_NUM_TEXT_X1, UTIL_TITLE_Y + 9 * UTIL_TITLE_Y_INC, UTIL_TITLE_FONT, 0,
        "Hex(16):");
    EVE_CoCmd_setBase(s_pHalContext, 16);
    EVE_CoCmd_number(s_pHalContext, UTIL_NUM_TEXT_X2, UTIL_TITLE_Y + 9 * UTIL_TITLE_Y_INC, UTIL_TITLE_FONT, 0, 1000);

    //Revert the base back to decimal because numeric base is part of the global context.
    EVE_CoCmd_setBase(s_pHalContext, 10);

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief CRC check
 *
 * @param None
 * @return None
 */
static void SAMAPP_Utility_crcCheck()
{
    const uint32_t memWrite = 0x66666666; // 66 = 'f'
    const uint32_t memSizeTest = 1024; // bytes
    const uint32_t crcExpected = 0x17D32620;
    uint32_t memcrcRet = 0;

    Draw_Text(s_pHalContext, "Example for: Check Crc32 for 1024 bytes on RAM_G");

    EVE_CoCmd_memSet(s_pHalContext, RAM_G, memWrite, memSizeTest);
    EVE_Cmd_waitFlush(s_pHalContext);/*reset cmd index*/

    EVE_CoCmd_memCrc(s_pHalContext, RAM_G, memSizeTest, &memcrcRet);
    EVE_Cmd_waitFlush(s_pHalContext);
    eve_printf_debug("current CRC number [0,1023) is 0x%x \r\n", memcrcRet);
    if (memcrcRet == crcExpected)
    {
        eve_printf_debug("Crc return value is as expected\n");
        Draw_TextColor(s_pHalContext, "Crc value is as expected", (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 });
    }
    SAMAPP_DELAY;
}

/**
 * @brief API to demonstrate CMD_SKIPCOND
 *
 * @param None
 * @return None
 */
static void SAMAPP_Utility_skipcond()
{
    Draw_Text(s_pHalContext, "Example for: Construct a command list with CMD_SKIPCOND");
    uint32_t tag = 0;
    bool pressed = 0;
    bool exit = 0;
    uint8_t txtPause[2][UTIL_TEXT_LEN] = { "PRESS TO CHANGE COLOR", "PRESSED" };

    do
    {
        EVE_CoCmd_regRead(s_pHalContext, REG_TOUCH_TAG, &tag);
        //only change the button when a release happened
        if ((tag & 0xFFFFFF) == UTIL_SKIPCOND_BTN_TAG)
        {
            pressed = 1;
        }
        else if ((tag & 0xFFFFFF) == UTIL_SKIPCOND_EXIT_TAG)
        {
            exit = 1;
        }
        else
        {
            if (pressed)
            {
                pressed = 0;
            }
        }

        Display_Start(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);

        /*** Show a button ***/
        EVE_CoCmd_skipCond(s_pHalContext, REG_TOUCH_TAG, NOTEQUAL, 0x1, 0xFFFFFF, 4 + 24); // colorRgb 4 bytes, CMD_SKIPCOND 24 bytes, so skip to the second colorRgb command if not equal to 1
        EVE_CoDl_colorRgb(s_pHalContext, 255, 0, 0); // red
        EVE_CoCmd_skipCond(s_pHalContext, 0, ALWAYS, 0x0, 0x0, 4); // unconditional branch
        EVE_CoDl_colorRgb(s_pHalContext, 0, 255, 0); // green
        EVE_CoDl_tag(s_pHalContext, UTIL_SKIPCOND_BTN_TAG);
        EVE_CoCmd_button(s_pHalContext, 160, 160, 600, 200, 31, 0, txtPause[pressed]);
        /*** Show exit button ***/
        EVE_CoDl_tag(s_pHalContext, UTIL_SKIPCOND_EXIT_TAG);
        EVE_CoDl_colorRgb(s_pHalContext, 255, 255, 255); // white
        EVE_CoCmd_button(s_pHalContext, 160, 460, 300, 200, 31, 0, "EXIT");
        /*** Done button ***/
        Display_End(s_pHalContext);

        EVE_sleep(UTIL_DELAY_MS);
    } while (!exit);
}

/**
 * @brief API to demonstrate CMD_WAITCOND
 *
 * @param None
 * @return None
 */
static void SAMAPP_Utility_waitcond()
{
    Draw_Text(s_pHalContext, "Example for: Construct a command list with CMD_WAITCOND");

    Draw_Text(s_pHalContext, "Now you will hear the music ");
    EVE_CoCmd_loadWav(s_pHalContext, UTIL_LOAD_ADDR, 0);
    EVE_Util_loadCmdFile(s_pHalContext, TEST_DIR UTIL_WAV, NULL);

    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_LOOP, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_L_PB, 155);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_R_PB, 155);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_PLAY, 1);
    // wait until the music play end
    EVE_CoCmd_waitCond(s_pHalContext, REG_PLAYBACK_PLAY, EQUAL, 0x0, 0x1);
    Draw_Text(s_pHalContext, "Music play done.");

    //The file is done, mute the audio.
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_L_PB, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_VOL_R_PB, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_LENGTH, 0);
    EVE_CoCmd_regWrite(s_pHalContext, REG_PLAYBACK_PLAY, 1);
}

/**
 * @brief API to demonstrate interrupt usage
 *
 * @param None
 * @return None
 */
static void SAMAPP_Utility_interrupt()
{
    Draw_Text(s_pHalContext, "Example for: Interrupt driven rendering");

    uint32_t int_flags = 0;

    /* Copy the display list */
    Display_Start(s_pHalContext, (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 }, 0, 2);
    /* Send command screen saver */
    EVE_CoDl_tagMask(s_pHalContext, 1);
    EVE_CoDl_tag(s_pHalContext, UTIL_INT_PIC_TAG);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_Util_loadImageFile(s_pHalContext, UTIL_LOAD_ADDR, TEST_DIR UTIL_JPG_MANDRILL, NULL, OPT_RGB565);
    EVE_CoDl_macro(s_pHalContext, 0);
    EVE_CoDl_end(s_pHalContext);
    EVE_CoDl_tagMask(s_pHalContext, 0);
    /* Display the text */
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), (int16_t)(s_pHalContext->Height / 2), UTIL_TITLE_FONT, OPT_CENTER, "Touch the picture to exit ...");
    EVE_CoDl_display(s_pHalContext);
    EVE_CoCmd_screenSaver(s_pHalContext); //screen saver command will continuously update the macro0 with vertex2f command

    EVE_CoCmd_regRead(s_pHalContext, REG_INT_FLAGS, &int_flags); // clear flags
    EVE_CoCmd_regWrite(s_pHalContext, REG_INT_MASK, 1 << INT_TAG); // set int mask
    EVE_CoCmd_regWrite(s_pHalContext, REG_INT_EN, 1); // enable int
    EVE_Cmd_waitFlush(s_pHalContext);

    while ((!EVE_Hal_getInterrupt(s_pHalContext)) 
       || ((EVE_Hal_rd32(s_pHalContext, REG_INT_FLAGS) & (1 << INT_TAG)) != (1 << INT_TAG)) 
       || (EVE_Hal_rd32(s_pHalContext, REG_TOUCH_TAG) != UTIL_INT_PIC_TAG))
        ;

    /* Send the stop command */
    EVE_CoCmd_stop(s_pHalContext);
    EVE_CoDl_vertexFormat(s_pHalContext, 4);
    Draw_Text(s_pHalContext, "Tag touched interrupt detected");
}

/**
 * @brief API to demonstrate zorder usage for RGB8 format
 *
 * @param None
 * @return None
 */
static void SAMAPP_Utility_zorder_RGB()
{
    Draw_Text(s_pHalContext, "Example for: simple zorder exercise for RGB format");
    uint8_t z = 0xAA;

    EVE_Util_loadRawFile(s_pHalContext, UTIL_LOAD_ADDR, TEST_DIR UTIL_RAW_MANDRILL); // normal bitmap
    EVE_Util_loadRawFile(s_pHalContext, UTIL_LOAD_ADDR1, TEST_DIR UTIL_RAW_ZORDER_RGB8); //z-ordered bitmap with zorder = 0xaa

    Display_Start(s_pHalContext, (uint8_t[]) { 20, 0, 30 }, (uint8_t[]) { 255, 255, 255 }, 0, 0);

    EVE_CoDl_bitmapHandle(s_pHalContext, 0);
    EVE_CoCmd_text(s_pHalContext, UTIL_ZORDER_X1, UTIL_ZORDER_Y + UTIL_JPG_MANDRILL_H + UTIL_TITLE_Y_INC, UTIL_TITLE_FONT, 0, "normal bitmap");
    EVE_CoCmd_setBitmap(s_pHalContext, UTIL_LOAD_ADDR, RGB8, UTIL_JPG_MANDRILL_W, UTIL_JPG_MANDRILL_H);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_vertex2f(s_pHalContext, UTIL_ZORDER_X1, UTIL_ZORDER_Y);
    EVE_CoDl_end(s_pHalContext);

    EVE_CoDl_bitmapHandle(s_pHalContext, 1);
    EVE_CoDl_bitmapZorder(s_pHalContext, z); // after set bitmap handler
    EVE_CoCmd_text(s_pHalContext, UTIL_ZORDER_X2, UTIL_ZORDER_Y + UTIL_JPG_MANDRILL_H + UTIL_TITLE_Y_INC, UTIL_TITLE_FONT, 0, "z-ordered bitmap");
    EVE_CoCmd_setBitmap(s_pHalContext, UTIL_LOAD_ADDR1, RGB8, UTIL_JPG_MANDRILL_W, UTIL_JPG_MANDRILL_H);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_vertex2f(s_pHalContext, UTIL_ZORDER_X2, UTIL_ZORDER_Y);
    EVE_CoDl_end(s_pHalContext);

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief API to demonstrate zorder usage for ASTC format
 *
 * @param None
 * @return None
 */
static void SAMAPP_Utility_zorder_ASTC()
{
    Draw_Text(s_pHalContext, "Example for: simple zorder exercise for ASTC format");
    uint8_t z = 0xAA;

    EVE_Util_loadRawFile(s_pHalContext, UTIL_LOAD_ADDR, TEST_DIR UTIL_RAW_CASE); // normal bitmap
    EVE_Util_loadRawFile(s_pHalContext, UTIL_LOAD_ADDR1, TEST_DIR UTIL_RAW_ZORDER_ASTC); // z-ordered bitmap

    Display_Start(s_pHalContext, (uint8_t[]) { 20, 0, 30 }, (uint8_t[]) { 255, 255, 255 }, 0, 0);

    EVE_CoDl_bitmapHandle(s_pHalContext, 0);
    EVE_CoCmd_text(s_pHalContext, UTIL_ZORDER_X1, UTIL_ZORDER_Y + UTIL_BITMAP_CASE_H + UTIL_TITLE_Y_INC, UTIL_TITLE_FONT, 0, "normal bitmap");
    EVE_CoCmd_setBitmap(s_pHalContext, UTIL_LOAD_ADDR, COMPRESSED_RGBA_ASTC_8x8_KHR, UTIL_BITMAP_CASE_W, UTIL_BITMAP_CASE_H);
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_vertex2f(s_pHalContext, UTIL_ZORDER_X1, UTIL_ZORDER_Y);
    EVE_CoDl_end(s_pHalContext);

    EVE_CoDl_bitmapHandle(s_pHalContext, 1);
    EVE_CoDl_bitmapZorder(s_pHalContext, z); // after set bitmap handler
    EVE_CoCmd_text(s_pHalContext, UTIL_ZORDER_X2, UTIL_ZORDER_Y + UTIL_BITMAP_CASE_H + UTIL_TITLE_Y_INC, UTIL_TITLE_FONT, 0, "z-ordered bitmap");
    EVE_CoCmd_setBitmap(s_pHalContext, UTIL_LOAD_ADDR1, COMPRESSED_RGBA_ASTC_8x8_KHR, UTIL_BITMAP_CASE_W, UTIL_BITMAP_CASE_H);
    EVE_CoDl_bitmapLayout(s_pHalContext, 31, 447 * 16, UTIL_BITMAP_CASE_H / 8); // The value 447 is the z-order width, as obtained from the z-order conversion
    EVE_CoDl_begin(s_pHalContext, BITMAPS);
    EVE_CoDl_vertex2f(s_pHalContext, UTIL_ZORDER_X2, UTIL_ZORDER_Y);
    EVE_CoDl_end(s_pHalContext);

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
 * @brief Main entry to run all utility demos
 *
 * @param None
 * @return None
 */
static void SAMAPP_Utility()
{
    SAMAPP_Utility_wait();
    SAMAPP_Utility_callList();
    SAMAPP_Utility_callListWithAlignment();
    SAMAPP_Utility_copyList();
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
    SAMAPP_Utility_skipcond();
    SAMAPP_Utility_waitcond();
    SAMAPP_Utility_interrupt();
    SAMAPP_Utility_zorder_RGB();
    SAMAPP_Utility_zorder_ASTC();
}


