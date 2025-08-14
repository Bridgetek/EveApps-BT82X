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


void SAMAPP_Extension() {
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


