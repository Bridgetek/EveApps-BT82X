/**
 * @file EVE_HalImpl_BT8XXEMU.c
 * @brief Eve_Hal framework APIs for EMULATOR host platform
 *
 * @author Bridgetek
 *
 * @date 2018
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

#include "EVE_HalImpl.h"
#include "EVE_Platform.h"
#if defined(BT8XXEMU_PLATFORM)

#include <bt8xxemu.h>

/*********
** INIT **
*********/

static int s_EmulatorSerial = 0;
/** @name Init */
///@{

/**
 * @brief Initialize HAL platform
 * 
 */
void EVE_HalImpl_initialize()
{
	// TODO: g_HalPlatform.TotalDevices = 1;
}

/**
 * @brief Release HAL platform
 * 
 */
void EVE_HalImpl_release()
{
	/* no-op */
}

/**
 * @brief List the available devices
 * 
 * @return 1
 */
size_t EVE_Hal_list()
{
	return 1;
}

/**
 * @brief Get info of the specified device
 * 
 * @param deviceInfo
 * @param deviceIdx
 */
void EVE_Hal_info(EVE_DeviceInfo *deviceInfo, size_t deviceIdx)
{
	memset(deviceInfo, 0, sizeof(EVE_DeviceInfo));
	strcpy_s(deviceInfo->DisplayName, sizeof(deviceInfo->DisplayName), "BT8XX Emulator");
	sprintf_s(deviceInfo->SerialNumber, sizeof(deviceInfo->SerialNumber), "BT8XXEMU_%i", ++s_EmulatorSerial);
	s_EmulatorSerial &= 0xFFFF;
	deviceInfo->Opened = false; // Emulator can be opened multiple times
	deviceInfo->Host = EVE_HOST_BT8XXEMU;
}

/**
 * @brief Check whether the context is the specified device
 * 
 * @param phost Pointer to Hal context
 * @param deviceIdx
 * @return true
 */
bool EVE_Hal_isDevice(EVE_HalContext *phost, size_t deviceIdx)
{
	if (!phost)
		return false;
	if (EVE_HOST != EVE_HOST_BT8XXEMU)
		return false;
	return true;
}

/**
 * @brief Get the default configuration parameters
 * 
 * @param parameters EVE_Hal framework's parameters
 * @param deviceIdx
 * @return true
 */
bool EVE_HalImpl_defaults(EVE_HalParameters *parameters, size_t deviceIdx)
{
	parameters->EmulatorMode = EVE_BT820;
	return true;
}

/**
 * @brief Opens a new HAL context using the specified parameters
 * 
 * @param phost Pointer to Hal context
 * @param parameters EVE_Hal framework's parameters
 * @return true True if ok
 * @return false False if error
 */
bool EVE_HalImpl_open(EVE_HalContext *phost, const EVE_HalParameters *parameters)
{
	bool ret;
	const BT8XXEMU_EmulatorParameters *origParams;
	BT8XXEMU_EmulatorParameters *params;
	const BT8XXEMU_FlashParameters *origFlashParams;

	origParams = parameters->EmulatorParameters;
	if (origParams)
	{
		// Copy
		params = malloc(sizeof(BT8XXEMU_EmulatorParameters));
		if (!params)
			return false;
		memcpy(params, origParams, sizeof(BT8XXEMU_EmulatorParameters));
	}
	else
	{
		// Make defaults
		params = malloc(sizeof(BT8XXEMU_EmulatorParameters));
		if (!params)
			return false;
		BT8XXEMU_defaults(BT8XXEMU_VERSION_API, params, EVE_shortChipId(parameters->EmulatorMode));
		params->Flags &= (~BT8XXEMU_EmulatorEnableDynamicDegrade & ~BT8XXEMU_EmulatorEnableRegPwmDutyEmulation);
	}
	phost->EmulatorParameters = params;

	if (!params->Mode)
	{
		free(params);
		phost->EmulatorParameters = NULL;
		return false;
	}


#if defined(EVE_EMULATOR_MAIN)
	phost->Emulator = EVE_GpuEmu;
	phost->EmulatorFlash = EVE_EmuFlash;
	origFlashParams = NULL;
#else
	origFlashParams = parameters->EmulatorFlashParameters;
	if (origFlashParams)
	{
		// Create
		phost->EmulatorFlash = BT8XXEMU_Flash_create(BT8XXEMU_VERSION_API, origFlashParams);
		params->Flash = phost->EmulatorFlash;
	}
	BT8XXEMU_run(BT8XXEMU_VERSION_API, &phost->Emulator, params);
#endif

	ret = !!phost->Emulator;
	if (ret)
	{
		phost->Status = EVE_STATUS_OPENED;
		++g_HalPlatform.OpenedDevices;
	}
	else
	{
		free(params);
		phost->EmulatorParameters = NULL;
	}
	return ret;
}

/**
 * @brief Close a HAL context
 * 
 * @param phost Pointer to Hal context
 */
void EVE_HalImpl_close(EVE_HalContext *phost)
{
#if !defined(EVE_EMULATOR_MAIN)
	// Release emulator
	if (phost->Emulator)
	{
		BT8XXEMU_stop(phost->Emulator);
		BT8XXEMU_destroy(phost->Emulator);
	}
	phost->Emulator = NULL;

	// Release flash
	if (phost->EmulatorFlash)
	{
		BT8XXEMU_Flash_destroy(phost->EmulatorFlash);
	}
	phost->EmulatorFlash = NULL;

	// Release emulator parameters
	if (phost->EmulatorParameters)
	{
		free(phost->EmulatorParameters);
		phost->EmulatorParameters = NULL;
	}
#else
	phost->Emulator = NULL;
	phost->EmulatorFlash = NULL;
#endif

	phost->Status = EVE_STATUS_CLOSED;
	--g_HalPlatform.OpenedDevices;
}

/**
 * @brief Idle. Call regularly to update frequently changing internal state
 * 
 * @param phost Pointer to Hal context
 */
void EVE_HalImpl_idle(EVE_HalContext *phost)
{
	/* no-op */
}
///@}

/*************
** TRANSFER **
*************/

/** @name TRANSFER */
///@{

/**
 * @brief Start data transfer to Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @param rw Read or Write
 * @param addr Address to read/write
 */
void EVE_Hal_startTransfer(EVE_HalContext *phost, EVE_TRANSFER_T rw, uint32_t addr)
{
	eve_assert(phost->Status == EVE_STATUS_OPENED);

	if (rw == EVE_TRANSFER_READ)
	{
		BT8XXEMU_chipSelect(phost->Emulator, 1);
		BT8XXEMU_transfer(phost->Emulator, (addr >> 16) & 0xFF);
		BT8XXEMU_transfer(phost->Emulator, (addr >> 8) & 0xFF);
		BT8XXEMU_transfer(phost->Emulator, addr & 0xFF);
		BT8XXEMU_transfer(phost->Emulator, 0); // Dummy Read Byte
		phost->Status = EVE_STATUS_READING;
	}
	else
	{
		BT8XXEMU_chipSelect(phost->Emulator, 1);
		BT8XXEMU_transfer(phost->Emulator, ((addr >> 16) & 0xFF) | 0x80);
		BT8XXEMU_transfer(phost->Emulator, (addr >> 8) & 0xFF);
		BT8XXEMU_transfer(phost->Emulator, addr & 0xFF);
		phost->Status = EVE_STATUS_WRITING;
	}
}

/**
 * @brief End data transfer
 * 
 * @param phost Pointer to Hal context
 */
void EVE_Hal_endTransfer(EVE_HalContext *phost)
{
	eve_assert(phost->Status == EVE_STATUS_READING || phost->Status == EVE_STATUS_WRITING);

	BT8XXEMU_chipSelect(phost->Emulator, 0);
	phost->Status = EVE_STATUS_OPENED;
}

/**
 * @brief Flush data to Coprocessor
 * 
 * @param phost Pointer to Hal context
 */
void EVE_Hal_flush(EVE_HalContext *phost)
{
	eve_assert(phost->Status == EVE_STATUS_OPENED);
	/* no-op */
}

/**
 * @brief Write 8 bit to Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @param value Value to write
 * @return uint8_t Number of bytes transfered
 */
static inline uint8_t transfer8(EVE_HalContext *phost, uint8_t value)
{
	return BT8XXEMU_transfer(phost->Emulator, value);
}

/**
 * @brief Write 8 bits to Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @param value Value to write
 * @return uint8_t Number of bytes transfered
 */
uint8_t EVE_Hal_transfer8(EVE_HalContext *phost, uint8_t value)
{
	return transfer8(phost, value);
}

/**
 * @brief Write 2 bytes to Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @param value Value to write
 * @return uint16_t Number of bytes transfered
 */
uint16_t EVE_Hal_transfer16(EVE_HalContext *phost, uint16_t value)
{
	uint16_t retVal = 0;
	retVal = transfer8(phost, value & 0xFF);
	retVal |= (uint16_t)transfer8(phost, (value >> 8) & 0xFF) << 8;
	return retVal;
}

/**
 * @brief Write 4 bytes to Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @param value Value to write
 * @return uint32_t Number of bytes transfered
 */
uint32_t EVE_Hal_transfer32(EVE_HalContext *phost, uint32_t value)
{
	uint32_t retVal = 0;
	retVal = transfer8(phost, value & 0xFF);
	retVal |= (uint32_t)transfer8(phost, (value >> 8) & 0xFF) << 8;
	retVal |= (uint32_t)transfer8(phost, (value >> 16) & 0xFF) << 16;
	retVal |= (uint32_t)transfer8(phost, value >> 24) << 24;
	return retVal;
}

/**
 * @brief Transfer (read/write) a block data to Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @param result Buffer to get data transfered, NULL when write
 * @param buffer Buffer where data is transfered, NULL when read
 * @param size Size of buffer
 */
void EVE_Hal_transferMem(EVE_HalContext *phost, uint8_t *result, const uint8_t *buffer, uint32_t size)
{
	uint32_t i;
	if (result && buffer)
	{
		for (i = 0; i < size; ++i)
			result[i] = transfer8(phost, buffer[i]);
	}
	else if (result)
	{
		for (i = 0; i < size; ++i)
			result[i] = transfer8(phost, 0);
	}
	else if (buffer)
	{
		for (i = 0; i < size; ++i)
			transfer8(phost, buffer[i]);
	}
}

/**
 * @brief Transfer a block data from program memory
 * 
 * @param phost Pointer to Hal context
 * @param result Buffer to get data transfered, NULL when write
 * @param buffer Buffer where data is transfered, NULL when read
 * @param size Size of buffer
 */
void EVE_Hal_transferProgMem(EVE_HalContext *phost, uint8_t *result, eve_progmem_const uint8_t *buffer, uint32_t size)
{
	uint32_t i;
	if (result && buffer)
	{
		for (i = 0; i < size; ++i)
			result[i] = transfer8(phost, buffer[i]);
	}
	else if (result)
	{
		for (i = 0; i < size; ++i)
			result[i] = transfer8(phost, 0);
	}
	else if (buffer)
	{
		for (i = 0; i < size; ++i)
			transfer8(phost, buffer[i]);
	}
}

/**
 * @brief Transfer a string to Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @param str String to transfer
 * @param index Start position in the string
 * @param size Size of string
 * @param padMask Padding mask
 * @return uint32_t Numer of bytes transfered
 */
uint32_t EVE_Hal_transferString(EVE_HalContext *phost, const char *str, uint32_t index, uint32_t size, uint32_t padMask)
{
	uint32_t transferred;

	if (!size)
	{
		/* TODO: Support different padding options */
		eve_assert(padMask == 0x3);
		EVE_Hal_transfer32(phost, 0);
		return 4;
	}

	eve_assert(size <= EVE_CMD_STRING_MAX);
	transferred = 0;
	if (phost->Status == EVE_STATUS_WRITING)
	{
		for (;;)
		{
			char c = str[index + (transferred++)];
			transfer8(phost, c);
			if (!c)
			{
				break;
			}
			if (transferred >= size)
			{
				++transferred;
				transfer8(phost, 0);
				break;
			}
		}
		while (transferred & padMask)
		{
			++transferred;
			transfer8(phost, 0);
		}
	}
	else
	{
		/* not implemented */
		eve_debug_break();
	}
	return transferred;
}
///@}

/************
** UTILITY **
************/

/** @name UTILITY */
///@{
/**
 * @brief This API sends a 5byte command to the phost
 * 
 * @param phost Pointer to Hal context
 * @param cmd Command to send
 */
void EVE_Hal_spiCommand(EVE_HalContext *phost, uint8_t *cmd)
{
	eve_printf_debug("SPI cmd: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
	/* no-op */
}

/**
 * @brief Toggle PD_N pin of FT800 board for a power cycle
 * 
 * @param phost Pointer to Hal context
 * @param up Up or Down
 */
bool EVE_Hal_powerCycle(EVE_HalContext *phost, bool up)
{
#if !defined(EVE_EMULATOR_MAIN)
	BT8XXEMU_EmulatorParameters *params;

	// ESD would need to call MainReady__ESD again...
	// TODO: Implement powercycle in BT8XXEMU
	if (up)
	{
		if (phost->Emulator)
		{
			BT8XXEMU_stop(phost->Emulator);
			BT8XXEMU_destroy(phost->Emulator);
			phost->Emulator = NULL;
		}

		params = phost->EmulatorParameters;
		BT8XXEMU_run(BT8XXEMU_VERSION_API, &phost->Emulator, params);

		return phost->Emulator;
	}
	else
	{
		if (!phost->Emulator)
		{
			params = phost->EmulatorParameters;
			BT8XXEMU_run(BT8XXEMU_VERSION_API, &phost->Emulator, params);
		}

		BT8XXEMU_stop(phost->Emulator);
		BT8XXEMU_destroy(phost->Emulator);
		phost->Emulator = NULL;

		return true;
	}
#endif
}

/**
 * @brief Set number of SPI channel
 * 
 * @param phost Pointer to Hal context
 * @param numchnls Number of channel
 */
void EVE_Hal_setSPI(EVE_HalContext *phost, EVE_SPI_CHANNELS_T numchnls)
{
	/* no-op */
}

/**
 * @brief Restore platform to previously configured EVE SPI channel mode 
 * 
 * @param phost Pointer to Hal context
 */
void EVE_Hal_restoreSPI(EVE_HalContext *phost)
{
	/* no-op */
}
///@}

/*********
** MISC **
*********/

/** @name MISC */
///@{

/**
 * @brief Display GPIO pins
 * 
 * @param phost Pointer to Hal context
 * @return true True if Ok
 * @return false False if error
 */
bool EVE_UtilImpl_bootupDisplayGpio(EVE_HalContext *phost)
{
	/* no-op */
	return true;
}
///@}

#endif /* #if defined(BT8XXEMU_PLATFORM) */

/* end of file */
