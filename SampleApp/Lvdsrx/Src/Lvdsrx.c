/**
 * @file Lvdsrx.c
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

#include "Lvdsrx.h"
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
        "This sample demonstrate the using of LVDSRX", 
        "",
        ""
    }; 

    while (true) {
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

static uint8_t LVDS_connected = 0; // 0: disconnected, 1: connected, 2:lost connection
uint8_t check_LVDS_connection()
{
	uint32_t reg_val;
	reg_val = EVE_Hal_rd32(s_pHalContext, REG_LVDSRX_STAT);

	if (reg_val != ((CHn_DLL_LOCKED << 25) | (CHn_DLL_LOCKED << 24)))
	{
		if (LVDS_connected == 2) // lost connection
		{
			eve_printf_debug("LVDS lost connection, stop it\n");
			reg_val = EVE_Hal_rd32(s_pHalContext, REG_LVDSRX_CTRL);
			reg_val &= ~((CHn_PWDN_B_ON << 8) | CHn_PWDN_B_ON);
			EVE_Hal_wr32(s_pHalContext, REG_LVDSRX_CTRL, reg_val);
			LVDS_connected = 0;
		}
		else if (LVDS_connected == 0)
		{
			eve_printf_debug("LVDS restart\n");
			reg_val = EVE_Hal_rd32(s_pHalContext, REG_LVDSRX_CTRL);
			reg_val |= ((CHn_PWDN_B_ON << 8) | CHn_PWDN_B_ON);
			EVE_Hal_wr32(s_pHalContext, REG_LVDSRX_CTRL, reg_val);
			LVDS_connected = 1;
		}
		else
		{
			// no action
		}
	}
	else
	{
		if (LVDS_connected == 1)
		{
			eve_printf_debug("LVDS re-sync\n");
			LVDS_connected = 2;
		}
		else
		{
			// no action
		}
	}
	return LVDS_connected;

}

/**
 * @brief Use LVDSRX without swapchain
 * 
 * In this case, LVDSRX will render using a single address, but this is not recommended as there is no synchronization guarantee. 
 * It is intended only for testing the basic functionality of LVDSRX.
 */
void SAMAPP_LvdsRx_withoutSC() {
	Draw_Text(s_pHalContext, "Example for: LVDSRX without Swapchain");
	LVDS_Config(s_pHalContext, YCBCR, MODE_LVDSRX);
	LVDS_connected = 1;

	uint32_t counter = 2000;
	while (counter > 0)
	{
		if (check_LVDS_connection() != 1)
			continue;

		Display_Start(s_pHalContext);
		EVE_CoCmd_setBitmap(s_pHalContext, DDR_FRAMEBUFFER_STARTADDR, RGB8, 1920, 1080);
		EVE_CoDl_begin(s_pHalContext, BITMAPS);
		EVE_CoDl_vertex2f_4(s_pHalContext, 0, 0);
		EVE_CoDl_end(s_pHalContext);

		EVE_CoCmd_text(s_pHalContext, 1920/2, 100, 31, OPT_CENTERX, "LVDSRX without Swapchain");
		Display_End(s_pHalContext);
		counter--;
	}
	EVE_sleep(2000);
}

/**
 * @brief Use LVDSRX with swapchain
 * 
 * In this case, LVDSRX will render using a swapchain. To prevent race conditions, please follow this process:
 * - Wait for the valid bit in REG_SC2_STATUS to be set, indicating a valid frame
 * - Retrieve the address of the valid frame from REG_SC2_ADDR
 * - Render the bitmap
 * - Clear REG_SC2_STATUS to recycle the swapchain buffer
 */
void SAMAPP_LvdsRx_withSC()
{
	Draw_Text(s_pHalContext, "Example for: LVDSRX with Swapchain");
	LVDS_Config(s_pHalContext, YCBCR, MODE_LVDSRX_SC);
	LVDS_connected = 1;

	uint32_t counter = 2000;
	uint32_t lvdsrx_data_addr;

	while (counter > 0)
	{
		if (check_LVDS_connection() != 1)
			continue;

		EVE_CoCmd_waitCond(s_pHalContext, REG_SC2_STATUS, EQUAL, 0x1, 0x1);
		EVE_CoCmd_regRead(s_pHalContext, REG_SC2_ADDR, &lvdsrx_data_addr);
		EVE_Cmd_waitFlush(s_pHalContext);

		Display_Start(s_pHalContext);
		EVE_CoCmd_setBitmap(s_pHalContext, lvdsrx_data_addr, RGB8, 1920, 1080);
		EVE_CoDl_begin(s_pHalContext, BITMAPS);
		EVE_CoDl_vertex2f_4(s_pHalContext, 0, 0);
		EVE_CoDl_end(s_pHalContext);

		EVE_CoCmd_text(s_pHalContext, 1920 / 2, 100, 31, OPT_CENTERX, "LVDSRX with Swapchain");
		Display_End(s_pHalContext);
		EVE_CoCmd_regWrite(s_pHalContext, REG_SC2_STATUS, 0); // clear status
		EVE_Cmd_waitFlush(s_pHalContext);
		counter--;
	}
	EVE_sleep(2000);
}

void SAMAPP_LvdsRx()
{
	SAMAPP_LvdsRx_withoutSC();
	SAMAPP_LvdsRx_withSC();
}
