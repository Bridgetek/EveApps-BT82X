/**
 * @file EVE_Util.c
 * @brief EVE HAL framework utilities
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

#include "EVE_Util.h"

static const uint8_t c_DlCodeBootup[12] = {
	0, 0, 0, 2, // GPU instruction CLEAR_COLOR_RGB: BLUE, GREEN, RED, ID(0x02)
	7, 0, 0, 38, // GPU instruction CLEAR: Tag/Stencil/Color, ID(0x26)
	0, 0, 0, 0, // GPU instruction DISPLAY: ID(0x00)
};

/** @note Update `EVE_DISPLAY_T` in `EVE_Util.h` when adding display presets. */
static const uint16_t s_DisplayResolutions[EVE_DISPLAY_NB][4] = {
	/* width, height, refreshRate, hsfWidth */

	{ 480, 272, 60, 0 }, /* EVE_DISPLAY_DEFAULT (values ignored) */

	/* Landscape */
	{ 1920, 1080, 60, 0 }, /* EVE_DISPLAY_FHD_1920x1080_60Hz */
	{ 1920, 1200, 60, 0 }, /* EVE_DISPLAY_WUXGA_1920x1200_60Hz */
};

#if defined(_WIN32)

/** Interactive platform selection */
static const char *s_HostDisplayNames[EVE_HOST_NB] = {
	"<Unknown>",

	"FT4222",
	"MPSSE",
};

#endif

/**
 * @brief Clear the screen
 * 
 * @param phost Pointer to Hal context
 */
void EVE_Util_clearScreen(EVE_HalContext *phost)
{
	EVE_Hal_wrMem(phost, RAM_DL, c_DlCodeBootup, sizeof(c_DlCodeBootup));
	EVE_Hal_wr32(phost, REG_DLSWAP, DLSWAP_FRAME);
}

/**
 * @brief Get the default bootup parameters
 *
 * @param phost Pointer to Hal context
 * @param bootup Default bootup parameters
 */
void EVE_Util_bootupDefaults(EVE_HalContext *phost, EVE_BootupParameters *bootup)
{
	int32_t chipId = EVE_CHIPID;
	(void)chipId;

	memset(bootup, 0, sizeof(EVE_BootupParameters));
	bootup->ExternalOsc = true;

	if (chipId == EVE_BT820)
	{
		bootup->SystemClock = EVE_SYSCLK_72M; /* 72Mhz is default for BT82x */
	}
	else
	{
		/* If the chipid is not known yet, this will reconfigure the system clock during `EVE_Util_bootup`.
		Additional default clock overrides need to be implemented there as well. */
		bootup->SystemClock = EVE_SYSCLK_DEFAULT;
	}

#if defined(ENABLE_SPI_QUAD)
	bootup->SpiChannels = EVE_SPI_QUAD_CHANNEL;
#elif defined(ENABLE_SPI_DUAL)
	bootup->SpiChannels = EVE_SPI_DUAL_CHANNEL;
#else
	bootup->SpiChannels = EVE_SPI_SINGLE_CHANNEL;
#endif
}

#ifndef EVE_HARDCODED_DISPLAY_TIMINGS
#define EVE_HARDCODED_DISPLAY_TIMINGS 1
#endif

/**
 * @brief Confirm resolution support
 *
 * @param phost Pointer to Hal context
 * @param config Configuration parameters
 * @param width Screen width
 * @param height Screen height
 * @param refreshRate Screen refresh rate
 * @param hsfWidth Screen HSF width
 * @param freq Coprocess frequency
 * @return true True if successful
 * @return false False if error
 */
static bool configDefaultsEx(EVE_HalContext *phost, EVE_ConfigParameters *config, uint32_t width, uint32_t height, uint32_t refreshRate, uint32_t hsfWidth, uint32_t freq)
{
	/* Only false if the output resolution will be wrong.
	Not affected by unsupported HSF or refreshRate */
	bool supportedResolution = true;

	uint32_t screenWidth;
	uint32_t pixels;
	uint32_t minCycles;
	uint32_t maxRate;
	uint32_t pclk;

	uint32_t cycles;
	uint32_t vcycle;
	uint32_t hcycle;
	uint32_t hoffset;
	uint32_t voffset;
	uint32_t hsync1;
	uint32_t vsync1;

	memset(config, 0, sizeof(EVE_ConfigParameters));

	/*
	Refresh rate is as follows:
	REG_VCYCLE * REG_HCYCLE * REG_PCLK is the number of clocks each frame takes,
	Divide REG_FREQUENCY by this number to get the frame refresh rate in Hz.
	REG_HCYCLE must be higher than REG_HSIZE, REG_VCYCLE must be higher than REG_VSIZE.

	By default, BT820 is at 72MHz.
	User may configure this differently in `EVE_Util_bootup`.

	Clocks per frame by clock frequency at 60Hz:
	- 84MHz: 1400k
	- 72MHz: 1200k
	- 60MHz: 1000k
	- 48MHz: 800k
	- 36MHz: 600k
	- 24MHz: 400k
	*/

	/* Trim oversize displays */
	if (width > 2047)
	{
		eve_printf_debug("Line buffer width %i is too large for this configuration\n", (int)config->Width);
		width = 2047;
		supportedResolution = false;
	}
	if (height > 2047)
	{
		eve_printf_debug("Screen height %i is too large for this configuration\n", (int)config->Height);
		height = 2047;
		supportedResolution = false;
	}

	if (hsfWidth)
	{
		/* Use the screen width if HSF is not supported */
		config->Width = (int16_t)hsfWidth;
		eve_printf_debug("Display config specifies HSF but not supported on this platform\n");
	}
	else
	{
		config->Width = (int16_t)width;
	}
	config->Height = (int16_t)height;

	screenWidth = hsfWidth ? hsfWidth : width; /* Use screen width for calculation */
	pixels = screenWidth * height;

	/* Calculate maximum refresh rate */
	minCycles = pixels + (pixels >> 2); /* pixels * 1.25 */
	maxRate = freq / minCycles;

	/* If the refresh rate is too low, try with tighter settings */
	if (refreshRate > maxRate)
	{
		minCycles = pixels + (pixels >> 3); /* pixels * 1.125 */
		maxRate = freq / minCycles;
	}

	if (maxRate < refreshRate)
	{
		/* Trim unsupported framerate */
		eve_printf_debug("Frame rate limited to %d\n", (unsigned int)maxRate);
		refreshRate = maxRate;
		pclk = 1;
	}
	else
	{
		pclk = maxRate / refreshRate;
	}
	config->PCLK = (uint8_t)pclk;

	/* Change PCLKPol if the right edge of the display is noisy,
	or if the display is showing jagged colors. Values 0 or 1.
	This toggles the polarity of the PCLK. */
	config->PCLKPol = 1; /* non-default */

	/* Approximate an average good setting */
	cycles = freq / (refreshRate * pclk);
	vcycle = height * cycles / pixels; /* max vcycle without blanking, on average 1.125x height, or 1.25x */
	vcycle = (vcycle + ((vcycle + height) >> 1) + height + height) >> 2; /* vcycle is 1.5 max vcycle + 2.5 height / 4, so on average 1.046875x height, or 1.09375x */
	hcycle = cycles / vcycle; /* cycles div by vcycle, on average 1.07375x, or 1.1425x */
	hoffset = (hcycle - screenWidth) >> 1;
	voffset = (vcycle - height) >> 1;
	hsync1 = hoffset >> 1;
	vsync1 = voffset >> 1;
	hoffset += hsync1;
	voffset += vsync1;
	config->HCycle = (int16_t)hcycle;
	config->HSync0 = 0;
	config->HSync1 = (int16_t)hsync1;
	config->HOffset = (int16_t)hoffset;
	config->VCycle = (int16_t)vcycle;
	config->VSync0 = 0;
	config->VSync1 = (int16_t)vsync1;
	config->VOffset = (int16_t)voffset;

	/* Verify */
	eve_assert(config->HSync1);
	eve_assert(config->HOffset > config->HSync1);
	eve_assert(config->HCycle > config->Width);
	eve_assert((config->HCycle - config->Width) > config->HOffset);
	eve_assert(config->VSync1);
	eve_assert(config->VOffset > config->VSync1);
	eve_assert(config->VCycle > config->Height);
	eve_assert((config->VCycle - config->Height) > config->VOffset);

	return supportedResolution;
}

/**
 * @brief Get the default configuration parameters for the specified display
 *
 * @param phost Pointer to Hal context
 * @param config Configuration parameters
 * @param display Display resolution presets
 */
void EVE_Util_configDefaults(EVE_HalContext *phost, EVE_ConfigParameters *config, EVE_DISPLAY_T display)
{
	bool supportedResolution;
	uint32_t freq = EVE_Hal_rd32(phost, REG_FREQUENCY);
	uint32_t width;
	uint32_t height;
	uint32_t refreshRate;
	uint32_t hsfWidth;

	/* NOTE: Update `EVE_DISPLAY_T` in `EVE_Util.h` when adding display presets. */
	if (!display)
	{
		/* Default displays if none was explicitly chosen */
		display = EVE_DISPLAY_WUXGA_1920x1200_60Hz;
	}

	width = s_DisplayResolutions[display][0];
	height = s_DisplayResolutions[display][1];
	refreshRate = s_DisplayResolutions[display][2];
	hsfWidth = s_DisplayResolutions[display][3];
	supportedResolution = configDefaultsEx(phost, config, width, height, refreshRate, hsfWidth, freq);

	/* Known values for specific display models */
	if (display == EVE_DISPLAY_WUXGA_1920x1200_60Hz)
	{
		/*
		BT820 known values:
		REG_FREQUENCY: 72MHz
		Resolution: 1920x1200
		Refresh rate: 60Hz
		*/
#if EVE_HARDCODED_DISPLAY_TIMINGS
		if (supportedResolution)
		{
			config->Width = 1920;
			config->Height = 1200;
			config->HCycle = 2080;
			config->HOffset = 50;
			config->HSync0 = 0;
			config->HSync1 = 30;
			config->VCycle = 1244;
			config->VOffset = 10;
			config->VSync0 = 0;
			config->VSync1 = 3;
			config->PCLK = 1;
		}
#endif
		config->PCLKPol = 0;
    }


#ifdef _DEBUG
	eve_printf_debug("Display refresh rate set to %f Hz\n", (float)((double)freq / ((double)config->HCycle * (double)config->VCycle * (double)config->PCLK)));
#endif

#if 0
	eve_printf_debug("Width: %i\n", (int)config->Width);
	eve_printf_debug("Height: %i\n", (int)config->Height);
	eve_printf_debug("HCycle: %i\n", (int)config->HCycle);
	eve_printf_debug("HOffset: %i\n", (int)config->HOffset);
	eve_printf_debug("HSync0: %i\n", (int)config->HSync0);
	eve_printf_debug("HSync1: %i\n", (int)config->HSync1);
	eve_printf_debug("VCycle: %i\n", (int)config->VCycle);
	eve_printf_debug("VOffset: %i\n", (int)config->VOffset);
	eve_printf_debug("VSync0: %i\n", (int)config->VSync0);
	eve_printf_debug("VSync1: %i\n", (int)config->VSync1);
	eve_printf_debug("PCLK: %i\n", (int)config->PCLK);
	eve_printf_debug("PCLKPol: %i\n", (int)config->PCLKPol);
#endif
}

#define EXTRACT_CHIPID(romChipId) EVE_extendedChipId(((romChipId) >> 16) & 0xFFFF)

/**
 * @brief Boot up the device. Obtains the chip Id. Sets up clock and SPI speed.
 *
 * @param phost Pointer to Hal context
 * @param bootup Bootup paramters
 * @return true True if successful
 * @return false False if error
 */
bool EVE_Util_bootup(EVE_HalContext *phost, EVE_BootupParameters *bootup)
{
	/* IMPORTANT: Do not use EVE_CoCmd functions here, as they can be overridden by hooks */

	const uint32_t expectedChipId = EVE_CHIPID;
	uint8_t engineStatus;
	uint32_t chipId;
	uint8_t id;
	double syspll;
	uint32_t freqbyte;
	EVE_CMD_SYS_CLK_DIV sys_clk_div = { 0 };
	EVE_CMD_BOOT_CFG boot_cfg = { 0 };
	EVE_CMD_BOOT_CFG_EN boot_cfg_en = { 0 };
	EVE_CMD_DDR_TYPE ddr_type = { 0 };

	do
	{
		/* EVE will be in SPI Single channel after POR */
		if (!EVE_Hal_powerCycle(phost, true))
			return false;

		boot_cfg_en.boot = 1;
		boot_cfg_en.DDRtype = 1;
		boot_cfg_en.GPREG = 0;
		boot_cfg_en.enable = 1;
		EVE_Hal_SPICmd_bootcfgen(phost, boot_cfg_en);
		boot_cfg.DDR = 1;
		boot_cfg.touch = 1;
		boot_cfg.audio = 1;
		boot_cfg.watchdog = 1;
		boot_cfg.source = 0;
		EVE_Hal_SPICmd_setbootcfg(phost, boot_cfg);
		ddr_type.speed = 0; //1333
		ddr_type.type = 1;
		ddr_type.size = EVE_DDR_SIZE;
		EVE_Hal_SPICmd_setddrtype(phost, ddr_type);
		boot_cfg_en.boot = 1;
		boot_cfg_en.DDRtype = 1;
		boot_cfg_en.GPREG = 0;
		boot_cfg_en.enable = 0;
		EVE_Hal_SPICmd_bootcfgen(phost, boot_cfg_en); // enable(0)
		syspll = 576000000;
		freqbyte = (uint32_t)((round(syspll / FREQUENCY) - 1));
		sys_clk_div.SYSPLL_CPS = 1; // default b'001'
		sys_clk_div.freq = freqbyte; // system clock: 72MHz or 36MHz
		EVE_Hal_SPICmd_sysclkdiv(phost, sys_clk_div);
		EVE_Hal_SPICmd_pwr_state(phost, EVE_PWR_STATE_ACTIVE);
		EVE_sleep(40);

		while (EVE_Hal_rd32(phost, REG_BOOT_STATUS) != 0x522e2e2e) // Polling to indicate EVE is in normal running operation state
		{
			EVE_sleep(10);
			eve_printf_debug("Boot status %lx \n", EVE_Hal_rd32(phost, REG_BOOT_STATUS));
		}
		eve_printf_debug("Boot status %lx \n", EVE_Hal_rd32(phost, REG_BOOT_STATUS));
		chipId = EVE_Hal_rd32(phost, REG_CHIP_ID);
		eve_printf_debug("chipID %lx \n", chipId);

		while (EXTRACT_CHIPID(chipId) != EVE_BT820)
		{
			eve_printf_debug("EVE REG_CHIP_ID after wake up %lx\n", (unsigned long)chipId);

			EVE_sleep(20);
			if (phost->CbCmdWait && !phost->CbCmdWait(phost))
				return false;

			chipId = EVE_Hal_rd32(phost, REG_CHIP_ID);
		}
	} while (!chipId);

	/* Validate chip ID to ensure the correct HAL is used */
	/* ROM_CHIPID is valid across all EVE devices */
	if (expectedChipId && EXTRACT_CHIPID(chipId) != expectedChipId)
		eve_printf_debug("Mismatching EVE chip id %lx, expect model %lx\n", (unsigned long)((chipId >> 8) & 0xFF) | ((chipId & 0xFF) << 8), (unsigned long)expectedChipId);
	eve_printf_debug("EVE chip id %lx %lx.%lx (EVE gen %i)\n", (unsigned long)EVE_shortChipId(EXTRACT_CHIPID(chipId)), (unsigned long)((chipId >> 16) & 0xFF), (unsigned long)((chipId >> 24) & 0xFF), EVE_gen(EXTRACT_CHIPID(chipId)));

	/* Read Register ID to check if EVE is ready. */
	while ((id = (uint8_t)EVE_Hal_rd32(phost, REG_ID)) != 0x7C)
	{
		eve_printf_debug("EVE register ID after wake up %x\n", (unsigned int)id);

		EVE_sleep(20);
		if (phost->CbCmdWait && !phost->CbCmdWait(phost))
			return false;
	}
	eve_printf_debug("EVE register ID after wake up %x\n", (unsigned int)id);
	eve_assert(chipId == EVE_Hal_rd32(phost, REG_CHIP_ID));


	/* Read REG_CPURESET to check if engines are ready.
		Bit 0 for coprocessor engine,
		Bit 1 for touch engine,
		Bit 2 for audio engine.
	*/
	while ((engineStatus = (uint8_t)EVE_Hal_rd32(phost, REG_CPURESET)) != 0x00)
	{
		if (engineStatus & 0x01)
		{
			eve_printf_debug("Coprocessor engine is not ready\n");
		}
		if (engineStatus & 0x02)
		{
			eve_printf_debug("Touch engine is not ready\n");
		}
		if (engineStatus & 0x04)
		{
			eve_printf_debug("Audio engine is not ready\n");
		}

		EVE_sleep(20);
		if (phost->CbCmdWait && !phost->CbCmdWait(phost))
			return false;
	}
	eve_printf_debug("All engines are ready\n");

	/* Update REG_FREQUENCY as per user selected */
	if (bootup->SystemClock != EVE_SYSCLK_DEFAULT)
	{
		uint32_t clockMHz = bootup->SystemClock * 12;
		EVE_Hal_wr32(phost, REG_FREQUENCY, clockMHz * 1000 * 1000);
		EVE_Hal_flush(phost);
		eve_printf_debug("EVE clock frequency set to %d MHz\n", (unsigned int)clockMHz);
	}
	else
	{
		eve_printf_debug("EVE default clock is %d MHz\n", (unsigned int)(EVE_Hal_rd32(phost, REG_FREQUENCY) / 1000000));
	}

	/* Switch to configured default SPI channel mode */
	EVE_Hal_setSPI(phost, bootup->SpiChannels);
#ifdef _DEBUG
	const char *spiChannels = (phost->SpiChannels == EVE_SPI_QUAD_CHANNEL)
		? "Quad"
		: ((phost->SpiChannels == EVE_SPI_DUAL_CHANNEL) ? "Dual" : "Single");
	const char *requested = (bootup->SpiChannels == EVE_SPI_QUAD_CHANNEL)
		? "Quad"
		: ((bootup->SpiChannels == EVE_SPI_DUAL_CHANNEL) ? "Dual" : "Single");
	if (bootup->SpiChannels == phost->SpiChannels)
		eve_printf_debug("%s channel SPI\n", spiChannels);
	else
		eve_printf_debug("%s channel SPI (%s unsupported)\n", spiChannels, requested);
#endif

	EVE_Hal_wr32(phost, REG_GPIO_DIR, 0xffffffff);
	EVE_Hal_wr32(phost, REG_GPIO, 0xffffffff);

	return true;
}

/**
 * @brief Boot up the device. Configures the display, resets or initializes coprocessor state.
 *
 * @param phost Pointer to Hal context
 * @param config Configuration parameter
 * @return true True if successful
 * @return false False if error
 */
bool EVE_Util_config(EVE_HalContext *phost, EVE_ConfigParameters *config)
{
	/* IMPORTANT: Do not use EVE_CoCmd functions here, as they can be overridden by hooks */

	bool reconfigure = phost->PCLK;
	if (reconfigure)
	{
		/* If PCLK is already set, we're reconfiguring. Quiet flush first */
		eve_printf_debug("Reconfiguring\n");
		EVE_Cmd_waitFlush(phost);
	}

	uint32_t wp;
	uint32_t rp;

	eve_assert(config->Width < 2048);
	eve_assert(config->Height < 2048);

	phost->PCLK = 0;

	EVE_Hal_wr32(phost, REG_HCYCLE, config->HCycle);
	EVE_Hal_wr32(phost, REG_HOFFSET, config->HOffset);
	EVE_Hal_wr32(phost, REG_HSYNC0, config->HSync0);
	EVE_Hal_wr32(phost, REG_HSYNC1, config->HSync1);
	EVE_Hal_wr32(phost, REG_VCYCLE, config->VCycle);
	EVE_Hal_wr32(phost, REG_VOFFSET, config->VOffset);
	EVE_Hal_wr32(phost, REG_VSYNC0, config->VSync0);
	EVE_Hal_wr32(phost, REG_VSYNC1, config->VSync1);
	EVE_Hal_wr32(phost, REG_PCLK_POL, config->PCLKPol);
	EVE_Hal_wr32(phost, REG_HSIZE, config->Width);
	EVE_Hal_wr32(phost, REG_VSIZE, config->Height);
	EVE_Hal_wr32(phost, REG_DISP, 1);
	EVE_Hal_wr32(phost, REG_PWM_DUTY, 128);

#if defined(DISPLAY_RESOLUTION_WSVGA)
	EVE_Hal_wr32(phost, REG_DISP, 1);
	EVE_Hal_wr32(phost, REG_PCLK_POL, 1);

	EVE_Hal_wr32(phost, REG_GPIO, 0x808b | (1 << 12));
	EVE_Hal_wr32(phost, REG_DISP, 1);
#endif

	EVE_Util_clearScreen(phost);

	/* Refresh fifo */
	wp = EVE_Cmd_wp(phost);
	rp = EVE_Cmd_rp(phost);
	EVE_Cmd_space(phost);
	phost->MediaFifoAddress = 0;
	phost->MediaFifoSize = 0;

	/* Coprocessor needs a reset */
	if (reconfigure ? EVE_CMD_FAULT(rp) : (wp || rp))
	{
		/* If this occurs after powerdown and bootup, powerdown is not working */
		eve_printf_debug("Coprocessor fifo not empty\n");
		if (!EVE_Util_resetCoprocessor(phost))
			return false;
	}
	else
	{
		/* Setup coprocessor defaults */
		eve_printf_debug("Configure coprocessor defaults\n");

		/* Reattach flash to avoid inconsistent state */
		EVE_Cmd_wr32(phost, CMD_FLASHDETACH);
		EVE_Cmd_wr32(phost, CMD_FLASHATTACH);
	}


	EVE_Hal_flush(phost);
	if (!EVE_Cmd_waitFlush(phost))
		return false;

	phost->Width = config->Width;
	phost->Height = config->Height;

	if (EVE_DDR_SIZE == 0)
	    phost->DDR_RamSize = 128 * 1024 * 1024;
	else if (EVE_DDR_SIZE == 2)
		phost->DDR_RamSize = 512 * 1024 * 1024;
	else
		phost->DDR_RamSize = 128 * 1024 * 1024;

#if 0
	eve_printf_debug("REG_HSIZE: %i\n", (int)EVE_Hal_rd32(phost, REG_HSIZE));
	eve_printf_debug("REG_VSIZE: %i\n", (int)EVE_Hal_rd32(phost, REG_VSIZE));
	eve_printf_debug("REG_HCYCLE: %i\n", (int)EVE_Hal_rd32(phost, REG_HCYCLE));
	eve_printf_debug("REG_HOFFSET: %i\n", (int)EVE_Hal_rd32(phost, REG_HOFFSET));
	eve_printf_debug("REG_HSYNC0: %i\n", (int)EVE_Hal_rd32(phost, REG_HSYNC0));
	eve_printf_debug("REG_HSYNC1: %i\n", (int)EVE_Hal_rd32(phost, REG_HSYNC1));
	eve_printf_debug("REG_VCYCLE: %i\n", (int)EVE_Hal_rd32(phost, REG_VCYCLE));
	eve_printf_debug("REG_VOFFSET: %i\n", (int)EVE_Hal_rd32(phost, REG_VOFFSET));
	eve_printf_debug("REG_VSYNC0: %i\n", (int)EVE_Hal_rd32(phost, REG_VSYNC0));
	eve_printf_debug("REG_VSYNC1: %i\n", (int)EVE_Hal_rd32(phost, REG_VSYNC1));
	eve_printf_debug("REG_PCLK_POL: %i\n", (int)EVE_Hal_rd32(phost, REG_PCLK_POL));
	eve_printf_debug("REG_DISP: %i\n", (int)EVE_Hal_rd32(phost, REG_DISP));
	eve_printf_debug("REG_PWM_DUTY: %i\n", (int)EVE_Hal_rd32(phost, REG_PWM_DUTY));
	eve_printf_debug("DDR_RamSize: %x\n", (int)phost->DDR_RamSize);
#endif

	eve_printf_debug("EVE configuration ready\n");
	return true;
}

/**
 * @brief Complementary of bootup. Does not close the HAL context.
 *
 * @param phost Pointer to Hal context
 */
void EVE_Util_shutdown(EVE_HalContext *phost)
{
	EVE_Hal_wr32(phost, REG_GPIO_DIR, 0xffff);
	EVE_Hal_wr32(phost, REG_GPIO, 0);

	EVE_Hal_flush(phost);
	EVE_Hal_powerCycle(phost, false);
}

#if defined(_DEBUG)
/**
 * @brief Backup the last 128 bytes of RAM_G, which may be used for an error message
 *
 * @param phost Pointer to Hal context
 */
void debugBackupRamG(EVE_HalContext *phost)
{
	if (!phost->DebugMessageVisible)
	{
		EVE_Hal_rdMem(phost, phost->DebugBackup, RAM_REPORT + RAM_REPORT_MAX - sizeof(phost->DebugBackup), sizeof(phost->DebugBackup));
		phost->DebugMessageVisible = true;
	}
}
/**
 * @brief Restore the last 128 bytes of RAM_G
 *
 * @param phost Pointer to Hal context
 */
static void debugRestoreRamG(EVE_HalContext *phost)
{
	if (phost->DebugMessageVisible)
	{
		EVE_Hal_wrMem(phost, RAM_REPORT + RAM_REPORT_MAX - sizeof(phost->DebugBackup), phost->DebugBackup, sizeof(phost->DebugBackup));
		phost->DebugMessageVisible = false;
	}
}
#endif

/**
 * @brief Reset Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @return true True if successful
 * @return false False if error
 */
bool EVE_Util_resetCoprocessor(EVE_HalContext *phost)
{
	/* IMPORTANT: Do not use EVE_CoCmd functions here, as they can be overridden by hooks */
	bool ready;
	uint16_t rd;
	uint16_t wr;
	(void)rd;
	(void)wr;

	eve_printf_debug("Reset coprocessor\n");

	/* Set REG_CPURESET to 1, to hold the coprocessor in the reset condition */
	EVE_Hal_wr32(phost, REG_CPURESET, 1);
	EVE_Hal_flush(phost);
	EVE_sleep(100);

	/* Set REG_CMD_READ and REG_CMD_WRITE to zero */
	EVE_Hal_wr32(phost, REG_CMD_READ, 0);
	EVE_Hal_wr32(phost, REG_CMD_WRITE, 0);
	EVE_Hal_wr32(phost, REG_CMD_DL, 0);

	/* Stop playing audio in case video with audio was playing during reset */
	EVE_Hal_wr32(phost, REG_PLAYBACK_PLAY, 0);

	/* Default */
	phost->CmdFault = false;

	/* Set REG_CPURESET to 0, to restart the coprocessor */
	EVE_Hal_wr32(phost, REG_CPURESET, 0);
	EVE_Hal_flush(phost);
	EVE_sleep(100);
	eve_assert((wr = EVE_Hal_rd32(phost, REG_CMD_WRITE)) == 0);
	eve_assert((rd = EVE_Hal_rd32(phost, REG_CMD_READ)) == 0);

	/* Refresh fifo */
	EVE_Cmd_waitFlush(phost);
	phost->MediaFifoAddress = 0;
	phost->MediaFifoSize = 0;

	/* Cold start. Ensure that the coprocessor is ready. */
	EVE_Cmd_wr32(phost, CMD_DLSTART);
	EVE_Cmd_wr32(phost, CMD_COLDSTART);

	/* Reattach flash to avoid inconsistent state */
	EVE_Cmd_wr32(phost, CMD_FLASHATTACH);

	/* Wait for coprocessor to be ready */
	ready = EVE_Cmd_waitFlush(phost);
	if (phost->CbCoprocessorReset) /* Notify */
		phost->CbCoprocessorReset(phost, !ready);

#if defined(_DEBUG)
	debugRestoreRamG(phost);
#endif
	return ready;
}

/**
 * @brief Recover from the coprocessor fault
 * 
 * @param phost Pointer to Hal context
 */
void EVE_Util_coprocessorFaultRecover(EVE_HalContext *phost)
{
	EVE_Hal_wr32(phost, REG_CMD_READ, 0);
	while (EVE_Hal_rd32(phost, REG_CMD_WRITE) != 0)
		;

	eve_assert((EVE_Hal_rd32(phost, REG_CMD_WRITE)) == 0);
	eve_assert((EVE_Hal_rd32(phost, REG_CMD_READ)) == 0);

	/* Default */
	phost->CmdFault = false;

#if defined(_DEBUG)
	debugRestoreRamG(phost);
#endif
}

/**
 * @brief Bootup Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @return true True if successful
 * @return false False if error
 */
bool EVE_Util_bootupConfig(EVE_HalContext *phost)
{
	EVE_BootupParameters bootup;
	EVE_ConfigParameters config;

	EVE_Util_bootupDefaults(phost, &bootup);
	if (!EVE_Util_bootup(phost, &bootup))
	{
		return false;
	}

	EVE_Util_configDefaults(phost, &config, EVE_DISPLAY_DEFAULT);
	if (!EVE_Util_config(phost, &config))
	{
		EVE_Util_shutdown(phost);
		return false;
	}

	return true;
}

/**
 * @brief Get current system clock of Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @return uint32_t Frequency of Coprocessor
 */
uint32_t EVE_Util_currentFrequency(EVE_HalContext *phost)
{
	uint32_t t0;
	uint32_t t1;
	uint32_t interval = 0;

	t0 = EVE_Hal_rd32(phost, REG_CLOCK); /* t0 read */
	/* may not be precise */
	EVE_sleep(1000);

	t1 = EVE_Hal_rd32(phost, REG_CLOCK); /* t1 read */

	if (t1 > t0)
		interval = t1 - t0;
	else
		interval = 0xFFFFFFFF - t0 + t1;
	return (interval / 1000000);
}
/* end of file */
