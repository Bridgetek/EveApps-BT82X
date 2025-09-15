/**
 * @file Extension.c
 * @brief Sample usage of LVDS
 *
 * @author Bridgetek
 *
 * @date 2025
 * 
 * MIT License
 *
 * Copyright (c) [2025] [Bridgetek Pte Ltd (BRTChip)]
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

#include "Extension.h"
#include "FileTransfer.h"
#include "FlashHelper.h"
#include "base.h"

static EVE_HalContext s_halContext;
static EVE_HalContext* s_pHalContext;
void SAMAPP_Extension();

/**
 * @brief Main function to initialize EVE, do calibrate and start application
 * 
 * @param argc Unuse
 * @param argv Unuse
 * @return int 0
 */
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
        "This sample demonstrate the using of Extension", 
        "",
        ""
    }; 

    while (true) {
        WelcomeScreen(s_pHalContext, info);

        SAMAPP_Extension();

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

void SAMAPP_Extension_Region()
{
    Draw_Text(s_pHalContext, "Example for: EVE_CMD_REGION and EVE_CMD_ENDREGION from base package");

    Display_StartColor(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 0 });
    EVE_CoDl_scissorSize(s_pHalContext, 240, 320);
    EVE_CoDl_scissorXY(s_pHalContext, 150, 0);

    EVE_CMD_REGION(s_pHalContext);
    EVE_CoDl_colorRgb(s_pHalContext, 255, 0, 0); // red
    EVE_CoDl_clearColorRgb(s_pHalContext, 255, 165, 0); // orange
    EVE_CoDl_clear(s_pHalContext, 1, 1, 1);
    EVE_CMD_REGION(s_pHalContext);
    EVE_CoDl_colorRgb(s_pHalContext, 0, 0, 255); // blue
	EVE_CoDl_clearColorRgb(s_pHalContext, 80, 80, 200); // medium blue
	EVE_CoDl_clear(s_pHalContext, 1, 1, 1);
	EVE_CMD_REGION(s_pHalContext);
	EVE_CoDl_clearColorRgb(s_pHalContext, 250, 80, 200); // bright pink
	EVE_CoDl_clear(s_pHalContext, 1, 1, 1);
	EVE_CMD_ENDREGION(s_pHalContext, 0, 90, 0, 80);
	EVE_CMD_ENDREGION(s_pHalContext, 0, 50, 0, 170);
	EVE_CMD_ENDREGION(s_pHalContext, 0, 10, 0, 300);

	EVE_CoDl_begin(s_pHalContext, LINES);
	EVE_CoDl_vertex2f_4(s_pHalContext, 0, 0);
	EVE_CoDl_vertex2f_4(s_pHalContext, 16 * 512, 16 * 320);
	EVE_CoDl_end(s_pHalContext);
	Display_End(s_pHalContext);
	EVE_sleep(2000);
}

void SAMAPP_Extension_Touchoffset()
{
	Draw_Text(s_pHalContext, "Example for: EVE_CMD_TOUCHOFFSET and EVE_CMD_ENDTOUCHOFFSET from base package");
	uint32_t counter = 1500, frame = 0;
	uint8_t i, j;
	uint16_t win[2][2] = { {100, 100}, {800, 400} }; // subwindow location
	uint32_t render_addr[2] = { DDR_BITMAPS_STARTADDR, DDR_BITMAPS_STARTADDR1 }; // rendertarget address
	uint16_t x, y; // button location
	uint32_t tag = 0xff;
	uint32_t prev_tag[2] = { 0 };
	char buttonChList[2][4][2] = { { "a", "b", "c", "d"}, { "e", "f", "g", "h"} }; // button label text
	bool highlighted[2][4] = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }; // by default, no highlight

	while(frame < counter)
	{
		for (i = 0; i < 2; i++)
		{
			EVE_CMD_TOUCHOFFSET(s_pHalContext, win[i][0], win[i][1]);
			EVE_CoCmd_renderTarget(s_pHalContext, render_addr[i], RGB6, 512, 512);
			EVE_CoDl_clearTag(s_pHalContext, 0xFF);
			Display_StartColor(s_pHalContext, (uint8_t[]) { 100, 0, 30 }, (uint8_t[]) { 255, 255, 255 });
			for (j = 0; j < 4; j++)
			{
				x = 24 + (j % 2) * 244;
				y = 24 + (j / 2) * 244;
				EVE_CoDl_tag(s_pHalContext, 'a' + i * 4 + j);
				if (highlighted[i][j])
					EVE_CoCmd_fgColor(s_pHalContext, 0xd06038);
				else
					EVE_CoCmd_fgColor(s_pHalContext, 0x3870);
				EVE_CoCmd_button(s_pHalContext, x, y, 220, 220, 32, OPT_FLAT, buttonChList[i][j]);
			}
			EVE_CoDl_tag(s_pHalContext, 0);
			Display_End(s_pHalContext);
			EVE_CoCmd_graphicsFinish(s_pHalContext);
			EVE_CMD_ENDTOUCHOFFSET(s_pHalContext);
			EVE_Cmd_waitFlush(s_pHalContext);

			EVE_sleep(6);
			tag = EVE_Hal_rd32(s_pHalContext, REG_TOUCH_TAG);
		    if (prev_tag[i] == 0)
		    {
				if ((tag >= (uint32_t)('a' + i * 4)) && (tag < (uint32_t)('a' + i * 4 + 4)))
				{
					highlighted[i][tag - 'a' - i * 4] ^= 1;
				}
		    }
			prev_tag[i] = tag;
		}

		if (frame % 2 == 0)
			EVE_CoCmd_renderTarget(s_pHalContext, SC0_PTR0_STARTADDR, YCBCR, s_pHalContext->Width, s_pHalContext->Height);
		else
			EVE_CoCmd_renderTarget(s_pHalContext, SC0_PTR1_STARTADDR, YCBCR, s_pHalContext->Width, s_pHalContext->Height);
		Display_StartColor(s_pHalContext, (uint8_t[]) { 0, 60, 0 }, (uint8_t[]) { 255, 255, 255 });
		EVE_CoCmd_number(s_pHalContext, 0, 0, 30, 0, frame);
		EVE_CoDl_vertexFormat(s_pHalContext, 0);
		EVE_CoDl_begin(s_pHalContext, BITMAPS);
		for (i = 0; i < 2; i++)
		{
			EVE_CoCmd_setBitmap(s_pHalContext, render_addr[i], RGB6, 512, 512);
			EVE_CoDl_vertex2f(s_pHalContext, win[i][0], win[i][1]);
		}
		EVE_CoDl_end(s_pHalContext);
		EVE_CoCmd_sync(s_pHalContext);
		if (frame % 2 == 0)
			EVE_Hal_wr32(s_pHalContext, REG_SO_SOURCE, SC0_PTR0_STARTADDR);
		else
			EVE_Hal_wr32(s_pHalContext, REG_SO_SOURCE, SC0_PTR1_STARTADDR);
		Display_End(s_pHalContext);

        frame++;
	}
	LVDS_Config(s_pHalContext, YCBCR, MODE_PICTURE);
}

void SAMAPP_Extension()
{
	SAMAPP_Extension_Region();
	SAMAPP_Extension_Touchoffset();
}


