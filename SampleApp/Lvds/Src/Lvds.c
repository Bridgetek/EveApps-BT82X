/**
 * @file Lvds.c
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

#include "Lvds.h"
#include "Common.h"
#include "FileTransfer.h"
#include "FlashHelper.h"

static EVE_HalContext s_halContext;
static EVE_HalContext* s_pHalContext;
void SAMAPP_LvdsRx();

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

    // read and store calibration setting
#if GET_CALIBRATION == 1
    EVE_Calibrate(s_pHalContext);
    Calibration_Save(s_pHalContext);
#endif

    EVE_Util_clearScreen(s_pHalContext);

    char *info[] =
    {  "EVE Sample Application",
        "This sample demonstrate the using of LVDS", 
        "",
        ""
    }; 

    while (TRUE) {
        WelcomeScreen(s_pHalContext, info);

        SAMAPP_LvdsRx();

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


void SAMAPP_LvdsRx() {
	Draw_Text(s_pHalContext, "Example for: LVDSRX");
	LVDS_Config(s_pHalContext, YCBCR, MODE_LVDSRX);

	uint32_t lvdsrx_data_addr;
	EVE_CoCmd_regRead(s_pHalContext, REG_SC2_ADDR, &lvdsrx_data_addr);
	uint32_t valid = 0;
	while (!valid)
	{
		EVE_CoCmd_regRead(s_pHalContext, REG_SC2_STATUS, &valid);
	}
	eve_printf_debug("lvdsrx_data_addr 0x%lx, valid 0x%lx\n", lvdsrx_data_addr, valid);

	while (1)
	{
		Display_Start(s_pHalContext);
		EVE_CoCmd_setBitmap(s_pHalContext, lvdsrx_data_addr, RGB8, 1920, 1080);
		EVE_CoDl_begin(s_pHalContext, BITMAPS);
		EVE_CoDl_vertex2f_4(s_pHalContext, 0, 0);
		EVE_CoDl_end(s_pHalContext);

		EVE_CoCmd_text(s_pHalContext, 1920/2, 100, 31, OPT_CENTERX, "LVDSRX");
		Display_End(s_pHalContext);
	}
}


