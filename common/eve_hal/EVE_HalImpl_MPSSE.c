/**
 * @file EVE_HalImpl_MPSSE.c
 * @brief Eve_Hal framework APIs for MPSSE host platform
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
#if defined(MPSSE_PLATFORM)

#ifdef _WIN32
#include <timeapi.h>
#pragma comment(lib, "Winmm.lib")
#endif

#define LIBMPSSE_MAX_RD_BYTES_PER_CALL_IN_SINGLE_CH 65535
#define LIBMPSSE_MAX_WR_BYTES_PER_CALL_IN_SINGLE_CH 65535

#define LIBMPSSE_MAX_RD_BYTES_PER_CALL_IN_MULTI_CH 65535
#define LIBMPSSE_MAX_WR_BYTES_PER_CALL_IN_MULTI_CH 65532 /**< 3 bytes for FT81x memory address to which data to be written */

/*********
** INIT **
*********/

uint32_t s_NumChannels = 0;
#ifdef _WIN32
bool s_SleepMitigated = false;
#endif

/** @name INIT */
///@{
/**
 * @brief Initialize HAL platform
 * 
 */
void EVE_HalImpl_initialize()
{
	/* Initialize the libmpsse */
	Init_libMPSSE();
}

/**
 * @brief Release HAL platform
 * 
 */
void EVE_HalImpl_release()
{
	/* Cleanup the MPSSE Lib */
#ifdef _WIN32
	if (s_SleepMitigated)
	{
		timeEndPeriod(1);
		s_SleepMitigated = false;
	}
#endif
	Cleanup_libMPSSE();
}

/**
 * @brief List the available devices 
 *
 */
size_t EVE_Hal_list()
{
	s_NumChannels = 0;
	SPI_GetNumChannels(&s_NumChannels);
	return s_NumChannels;
}

/**
 * @brief Get info of the specified device
 *
 * @param deviceInfo
 * @param deviceIdx
 */
void EVE_Hal_info(EVE_DeviceInfo *deviceInfo, size_t deviceIdx)
{
	FT_DEVICE_LIST_INFO_NODE chanInfo = { 0 };

	memset(deviceInfo, 0, sizeof(EVE_DeviceInfo));
	if (deviceIdx >= s_NumChannels)
		return;

	SPI_GetChannelInfo((uint32_t)deviceIdx, &chanInfo);

	strcpy_s(deviceInfo->SerialNumber, sizeof(deviceInfo->SerialNumber), chanInfo.SerialNumber);
	strcpy_s(deviceInfo->DisplayName, sizeof(deviceInfo->DisplayName), chanInfo.Description);
	deviceInfo->Host = EVE_HOST_MPSSE;
	deviceInfo->Opened = chanInfo.Flags & FT_FLAGS_OPENED;
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
	FT_DEVICE_LIST_INFO_NODE chanInfo = { 0 };

	if (!phost)
		return false;
	if (EVE_HOST != EVE_HOST_MPSSE)
		return false;
	if (deviceIdx >= s_NumChannels)
		return false;

	if (!phost->SpiHandle)
		return false;

	if (!SPI_GetChannelInfo((uint32_t)deviceIdx, &chanInfo))
		return false;

	return phost->SpiHandle == chanInfo.ftHandle;
}

/**
 * @brief Get the default configuration parameters
 * 
 * @param parameters EVE_Hal framework's parameters
 * @param deviceIdx
 */
bool EVE_HalImpl_defaults(EVE_HalParameters *parameters, size_t deviceIdx)
{
	FT_DEVICE_LIST_INFO_NODE chanInfo = { 0 };
	bool res = deviceIdx >= 0 && deviceIdx < s_NumChannels;
	if (!res)
	{
		uint32_t i;
		if (!s_NumChannels)
			EVE_Hal_list();

		// Select first open device
		deviceIdx = 0;
		for (i = 0; i < s_NumChannels; ++i)
		{
			if (SPI_GetChannelInfo((uint32_t)i, &chanInfo) != FT_OK)
				continue;
			if (!(chanInfo.Flags & FT_FLAGS_OPENED))
			{
				deviceIdx = i;
				res = true;
				break;
			}
		}
	}
	else
	{
		SPI_GetChannelInfo((uint32_t)deviceIdx, &chanInfo);
	}
	parameters->MpsseChannelNo = deviceIdx & 0xFF;
	parameters->PowerDownPin = 7;
	parameters->SpiClockrateKHz = 15000; /* in KHz */
	if (!strcmp(chanInfo.Description, "UMFTPD2A B"))
	{
		/* Settings for FT9xx GPIO passthrough */
		/* parameters->PowerDownPin = 7 + 16; */ /* Uses channel D, useful for FT9xx passthrough. See EVE_HalImpl_open */
		/* parameters->SpiClockrateKHz = 600; */

		/* Settings for FT9xx hardware SPI passthrough */
		parameters->PowerDownPin = 0x80 + 43; /* GPIO pin 43 */
		parameters->SpiClockrateKHz = 2000; // 2000;
	}
	return res;
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
	FT_STATUS status;
	FT_DEVICE_LIST_INFO_NODE chanInfo = { 0 };
	ChannelConfig channelConf; /* channel configuration */

	/* configure the spi settings */
	channelConf.ClockRate = parameters->SpiClockrateKHz * 1000;
	channelConf.LatencyTimer = 2;
	channelConf.configOptions = SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_DBUS3 | SPI_CONFIG_OPTION_CS_ACTIVELOW;
	channelConf.Pin = 0x00008080; /* FinalVal-FinalDir-InitVal-InitDir (for dir 0=in, 1=out) */ /* !!! Critical setting for system to power on */

	phost->SpiClockrateKHz = parameters->SpiClockrateKHz;
	phost->MpsseChannelNo = parameters->MpsseChannelNo;
	phost->PowerDownPin = parameters->PowerDownPin;

	/* Open the first available channel */
	status = SPI_OpenChannel(parameters->MpsseChannelNo, (FT_HANDLE *)&phost->SpiHandle);
	phost->GpioHandle = phost->SpiHandle;
	if (FT_OK != status)
	{
		eve_printf_debug("SPI open channel failed %d %p\n", parameters->MpsseChannelNo, phost->SpiHandle);
		return false;
	}
	status = SPI_InitChannel((FT_HANDLE)phost->SpiHandle, &channelConf);
	if (FT_OK != status)
	{
		eve_printf_debug("SPI init channel failed %d %p\n", parameters->MpsseChannelNo, phost->SpiHandle);
		return false;
	}

	eve_printf_debug("\nhandle=0x%p status=0x%x\n", phost->SpiHandle, (unsigned int)status);

	/* Special case, when connecting through UMFTPD2A, use channel D for GPIO */
	if (phost->PowerDownPin >= 8 && ((phost->PowerDownPin & 0x80) == 0))
	{
		SPI_GetChannelInfo((uint32_t)phost->MpsseChannelNo, &chanInfo);
		if (!strcmp(chanInfo.Description, "UMFTPD2A B"))
		{
			size_t slen = strlen(chanInfo.SerialNumber);
			char cn = 'B' + (phost->PowerDownPin >> 3);
			eve_printf_debug("%s (%s): Adding channel %c for GPIO\n", chanInfo.Description, chanInfo.SerialNumber, cn);
			phost->PowerDownPin &= 7;
			chanInfo.SerialNumber[slen - 1] = cn;
			phost->GpioHandle = NULL;
			if (FT_OpenEx(chanInfo.SerialNumber, FT_OPEN_BY_SERIAL_NUMBER, &phost->GpioHandle) != FT_OK)
			{
				eve_printf_debug("Failed to open channel D\n");
				FT_Close(phost->SpiHandle);
				return false;
			}
			else
			{
				eve_printf_debug("Channel D open OK\n");
			}
			if (FT_ResetDevice(phost->GpioHandle) != FT_OK
			    || FT_SetBitMode(phost->GpioHandle, (1 << phost->PowerDownPin), FT_BITMODE_SYNC_BITBANG) != FT_OK)
			{
				eve_printf_debug("Failed to prepare channel D\n");
				FT_Close(phost->GpioHandle);
				FT_Close(phost->SpiHandle);
				return false;
			}
		}
	}

	/* Initialize the context variables */
	phost->SpiChannels = EVE_SPI_SINGLE_CHANNEL;
	phost->Status = EVE_STATUS_OPENED;
	++g_HalPlatform.OpenedDevices;

#ifdef _WIN32
	/* Performance degradation workaround. Mitigate SPI_ToggleCS sleep, if it's detected. */
	/* SPI_ToggleCS may be fixed to remove the sleep call in future versions of libMPSSE. */
	/* This workaround applies only if the affected libMPSSE library is being used. */
	if (!s_SleepMitigated)
	{
		/* This will sleep twice, once on each FALSE */
		uint32_t ms, delta;
		ms = EVE_millis();
		SPI_ToggleCS((FT_HANDLE)phost->SpiHandle, TRUE);
		SPI_ToggleCS((FT_HANDLE)phost->SpiHandle, FALSE);
		SPI_ToggleCS((FT_HANDLE)phost->SpiHandle, TRUE);
		SPI_ToggleCS((FT_HANDLE)phost->SpiHandle, FALSE);
		delta = EVE_millis() - ms;
		/* If the sleep time is more than a single full tick (either 15ms or 16ms),
		we know for sure that it's sleeping a full tick twice. */
		if (delta > 16)
		{
			/* Set thread scheduler for process to 1ms. */
			timeBeginPeriod(1);
			s_SleepMitigated = true;
			/* Verify that it worked. */
			eve_printf_debug("Mitigating libMPSSE sleep issue (%u ms)\n", (unsigned int)delta);
			ms = EVE_millis();
			SPI_ToggleCS((FT_HANDLE)phost->SpiHandle, TRUE);
			SPI_ToggleCS((FT_HANDLE)phost->SpiHandle, FALSE);
			SPI_ToggleCS((FT_HANDLE)phost->SpiHandle, TRUE);
			SPI_ToggleCS((FT_HANDLE)phost->SpiHandle, FALSE);
			delta = EVE_millis() - ms;
			/* EVE_millis may jump a whole tick at most */
			if (delta > 16)
			{
				eve_printf_debug("MPSSE sleep issue may still persist (%u ms)\n", (unsigned int)delta);
			}
			else
			{
				eve_printf_debug("MPSSE sleep issue mitigated\n");
			}
		}
	}
#endif

	return true;
}

/**
 * @brief Close a HAL context
 * 
 * @param phost Pointer to Hal context
 */
void EVE_HalImpl_close(EVE_HalContext *phost)
{
	phost->Status = EVE_STATUS_CLOSED;
	--g_HalPlatform.OpenedDevices;
	if (phost->GpioHandle != phost->SpiHandle)
		SPI_CloseChannel(phost->GpioHandle);
	SPI_CloseChannel(phost->SpiHandle);
	phost->GpioHandle = NULL;
	phost->SpiHandle = NULL;
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

#if defined(EVE_BUFFER_WRITES)
static bool flush(EVE_HalContext *phost);
#endif

/**
 * @brief Increase RAM_G adress
 * 
 * @param phost Pointer to Hal context
 * @param addr Address offset
 * @param inc Number of bytes to increase
 * @return uint32_t New address in RAM_G
 */
static inline uint32_t incrementRamGAddr(EVE_HalContext *phost, uint32_t addr, uint32_t inc)
{
	if (addr != REG_CMDB_WRITE)
	{
		bool wrapCmdAddr = (addr >= RAM_CMD) && (addr < (RAM_CMD + EVE_CMD_FIFO_SIZE));
		addr += inc;
		if (wrapCmdAddr)
			addr = RAM_CMD + (addr & EVE_CMD_FIFO_MASK);
	}
	return addr;
}

/**
 * @brief Read a block data from Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @param buffer Buffer to get result
 * @param size Number of bytes to read
 * @return true True if ok
 * @return false False if error
 */
static inline bool rdBuffer(EVE_HalContext *phost, uint8_t *buffer, uint32_t size)
{
	uint32_t sizeTransferred = 0;
	uint32_t sizeRemaining = size;
	uint8_t buffer_readybyte;
	uint8_t countTimout = 0;
	do
	{
		SPI_Read(phost->SpiHandle, &buffer_readybyte, 1, &sizeTransferred, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
		countTimout++;
	} while ((buffer_readybyte != 0x01) && (countTimout < READ_TIMOUT));

	while (sizeRemaining)
	{
		FT_STATUS status = SPI_Read(phost->SpiHandle, buffer, min(0xFFFF, sizeRemaining), &sizeTransferred, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
		sizeRemaining -= sizeTransferred;
		buffer += sizeTransferred;

		if (status != FT_OK || !sizeTransferred)
		{
			eve_printf_debug("%d SPI_Read failed, sizeTransferred is %d with status %d\n", __LINE__, sizeTransferred, (int)status);
			phost->Status = EVE_STATUS_ERROR;
			return false;
		}
	}

	return true;
}

/**
 * @brief Write a block data to Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @param buffer Data buffer to write
 * @param size Size of buffer
 * @return true True if ok
 * @return false False if error
 */
static inline bool wrBuffer(EVE_HalContext *phost, const uint8_t *buffer, uint32_t size)
{
#if defined(EVE_BUFFER_WRITES)
	if (buffer && (size < (sizeof(phost->SpiWrBuf) - phost->SpiWrBufIndex)))
	{
		/* Write to buffer */
		memcpy(&phost->SpiWrBuf[phost->SpiWrBufIndex], buffer, size);
		phost->SpiWrBufIndex += size;
		return true;
	}
	else
	{
		if (buffer && phost->SpiWrBufIndex)
		{
			/* Buffer is over size, flush now */
			if (!flush(phost))
				return false;

			/* Write to buffer */
			if (size < sizeof(phost->SpiWrBuf))
				return wrBuffer(phost, buffer, size);
		}

		if (buffer || phost->SpiWrBufIndex)
		{
			uint32_t sizeTransferred;
			uint8_t hrdpkt[8];
			uint32_t addr = phost->SpiRamGAddr;
			FT_STATUS status;
			uint32_t sizeRemaining;

			if (!buffer)
			{
				/* Flushing */
				buffer = phost->SpiWrBuf;
				size = phost->SpiWrBufIndex;
				phost->SpiWrBufIndex = 0;
			}

			/* Compose the HOST MEMORY WRITE packet */
			hrdpkt[0] = (addr >> 24) | 0x80; /* MSB bit 1 for WRITE */
			hrdpkt[1] = (addr >> 16) & 0xFF;
			hrdpkt[2] = (addr >> 8) & 0xFF;
			hrdpkt[3] = addr & 0xFF;

			status = SPI_Write((FT_HANDLE)phost->SpiHandle, hrdpkt, 4, &sizeTransferred, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);

			if ((status != FT_OK) || (sizeTransferred != 4))
			{
				eve_printf_debug("%d SPI_Write failed, sizeTransferred is %d with status %d\n", __LINE__, sizeTransferred, (int)status);
				if (sizeTransferred != 4)
					phost->Status = EVE_STATUS_ERROR;
				return false;
			}

			sizeRemaining = size;
			while (sizeRemaining)
			{
				uint32_t transferSize = min(0xFFFF, sizeRemaining);
				FT_STATUS status = SPI_Write(phost->SpiHandle, (uint8 *)buffer, transferSize, &sizeTransferred,
				    (transferSize == sizeRemaining) ? (SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE) : SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
				sizeRemaining -= sizeTransferred;
				buffer += sizeTransferred;

				if (status != FT_OK || !sizeTransferred)
				{
					eve_printf_debug("%d SPI_Write failed, sizeTransferred is %d with status %d\n", __LINE__, sizeTransferred, (int)status);
					phost->Status = EVE_STATUS_ERROR;
					return false;
				}
			}

			addr = incrementRamGAddr(phost, addr, size);
			phost->SpiRamGAddr = addr;
		}

		return true;
	}
#else
	uint32_t sizeTransferred = 0;
	uint32_t sizeRemaining = size;

	while (sizeRemaining)
	{
		FT_STATUS status = SPI_Write(phost->SpiHandle, (uint8 *)buffer, min(0xFFFF, sizeRemaining), &sizeTransferred, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
		sizeRemaining -= sizeTransferred;
		buffer += sizeTransferred;

		if (status != FT_OK || !sizeTransferred)
		{
			eve_printf_debug("%d SPI_Write failed, sizeTransferred is %d with status %d\n", __LINE__, sizeTransferred, status);
			phost->Status = EVE_STATUS_ERROR;
			return false;
		}
	}

	return true;
#endif
}

/**
 * @brief Write 8 bits to Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @param value Value to write
 * @return uint8_t Number of bytes transfered
 */
static inline uint8_t transfer8(EVE_HalContext *phost, uint8_t value)
{
	if (phost->Status == EVE_STATUS_WRITING)
	{
		wrBuffer(phost, &value, sizeof(value));
	}
	else
	{
		rdBuffer(phost, &value, sizeof(value));
	}
	return value;
}

#if defined(EVE_BUFFER_WRITES)
/**
 * @brief Flush data to Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @return true True if ok
 * @return false False if error
 */
static bool flush(EVE_HalContext *phost)
{
	bool res = true;
	if (phost->SpiWrBufIndex)
	{
		res = wrBuffer(phost, NULL, 0);
	}
	eve_assert(!phost->SpiWrBufIndex);
	return res;
}
#endif

/**
 * @brief Flush data to Coprocessor
 * 
 * @param phost Pointer to Hal context
 */
void EVE_Hal_flush(EVE_HalContext *phost)
{
	eve_assert(phost->Status == EVE_STATUS_OPENED);
#if defined(EVE_BUFFER_WRITES)
	flush(phost);
#endif
}

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

#if defined(EVE_BUFFER_WRITES)
	if (addr != incrementRamGAddr(phost, phost->SpiRamGAddr, phost->SpiWrBufIndex) || rw == EVE_TRANSFER_READ)
	{
		/* Close any write transfer that was left open, if the address changed */
		flush(phost);
		phost->SpiRamGAddr = addr;
	}
#endif

	if (rw == EVE_TRANSFER_READ)
	{
		uint8_t transferArray[4];
		uint32_t sizeTransferred;

#if defined(EVE_BUFFER_WRITES)
		eve_assert(!phost->SpiWrBufIndex);
#endif

		/* Compose the read packet */
		transferArray[0] = (addr >> 24) & 0xFF;
		transferArray[1] = (addr >> 16) & 0xFF;
		transferArray[2] = (addr >> 8) & 0xFF;
		transferArray[3] = addr & 0xFF;

		SPI_Write((FT_HANDLE)phost->SpiHandle, transferArray, 4, &sizeTransferred, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);

		if (phost->Status != EVE_STATUS_ERROR)
			phost->Status = EVE_STATUS_READING;
	}
	else
	{
#if defined(EVE_BUFFER_WRITES)
		if (phost->Status != EVE_STATUS_ERROR)
			phost->Status = EVE_STATUS_WRITING;
#else
		uint8_t transferArray[4];
		uint32_t sizeTransferred;

		/* Compose the write packet */
		transferArray[0] = (addr >> 24) | 0x80;
		transferArray[1] = (addr >> 16) & 0xFF;
		transferArray[2] = (addr >> 8) & 0xFF;
		transferArray[3] = addr & 0xFF;

		SPI_Write((FT_HANDLE)phost->SpiHandle, transferArray, 4, &sizeTransferred, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);

		if (phost->Status != EVE_STATUS_ERROR)
			phost->Status = EVE_STATUS_WRITING;
#endif
	}
}

/**
 * @brief End data transfer
 * 
 * @param phost Pointer to Hal context
 */
void EVE_Hal_endTransfer(EVE_HalContext *phost)
{
	FT_STATUS status;
#if defined(EVE_BUFFER_WRITES)
	uint32_t addr;
#endif

	eve_assert(phost->Status == EVE_STATUS_READING || phost->Status == EVE_STATUS_WRITING);

#if defined(EVE_BUFFER_WRITES)
	/* Transfers to FIFO and DL are kept open */
	addr = phost->SpiRamGAddr;
	if ((addr != REG_CMDB_WRITE)
	    && !((addr >= RAM_CMD) && (addr < (RAM_CMD + EVE_CMD_FIFO_SIZE)))
	    && !((addr >= RAM_DL) && (addr < (RAM_DL + EVE_DL_SIZE))))
	{
		flush(phost);
	}

	if (phost->Status == EVE_STATUS_READING)
	{
		/* just disable the CS */
		status = SPI_ToggleCS((FT_HANDLE)phost->SpiHandle, FALSE);

		if (status != FT_OK)
			phost->Status = EVE_STATUS_ERROR;
		else
			phost->Status = EVE_STATUS_OPENED;
	}
	else if (phost->Status == EVE_STATUS_WRITING)
	{
		phost->Status = EVE_STATUS_OPENED;
	}
#else
	/* just disable the CS */
	status = SPI_ToggleCS((FT_HANDLE)phost->SpiHandle, FALSE);

	if (phost->Status != EVE_STATUS_ERROR)
	{
		if (status != FT_OK)
			phost->Status = EVE_STATUS_ERROR;
		else
			phost->Status = EVE_STATUS_OPENED;
	}
#endif
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
	uint8_t buffer[2];
	if (phost->Status == EVE_STATUS_READING)
	{
		rdBuffer(phost, buffer, 2);
		return (uint16_t)buffer[0]
		    | (uint16_t)buffer[1] << 8;
	}
	else
	{
		buffer[0] = value & 0xFF;
		buffer[1] = value >> 8;
		wrBuffer(phost, buffer, 2);
		return 0;
	}
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
	uint8_t buffer[4];
	if (phost->Status == EVE_STATUS_READING)
	{
		rdBuffer(phost, buffer, 4);
		return (uint32_t)buffer[0]
		    | (uint32_t)buffer[1] << 8
		    | (uint32_t)buffer[2] << 16
		    | (uint32_t)buffer[3] << 24;
	}
	else
	{
		buffer[0] = value & 0xFF;
		buffer[1] = (value >> 8) & 0xFF;
		buffer[2] = (value >> 16) & 0xFF;
		buffer[3] = value >> 24;
		wrBuffer(phost, buffer, 4);
		return 0;
	}
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
	if (!size)
		return;

	if (result && buffer)
	{
		/* not implemented */
		eve_debug_break();
	}
	else if (result)
	{
		rdBuffer(phost, result, size);
	}
	else if (buffer)
	{
		wrBuffer(phost, buffer, size);
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
	if (!size)
		return;

	if (result && buffer)
	{
		/* not implemented */
		eve_debug_break();
	}
	else if (result)
	{
		rdBuffer(phost, result, size);
	}
	else if (buffer)
	{
		wrBuffer(phost, buffer, size);
	}
}

/**
 * @brief Transfer a string to EVE platform
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
		uint8_t buffer[EVE_CMD_STRING_MAX + 1];

		for (;;)
		{
			char c = str[index + (transferred)];
			buffer[transferred++] = c;
			// putchar(c ? c : '_');
			if (!c)
			{
				break;
			}
			if (transferred >= size)
			{
				buffer[transferred++] = 0;
				// putchar('_');
				break;
			}
		}
		while (transferred & padMask)
		{
			buffer[transferred++] = 0;
			// putchar('_');
		}

		eve_assert(transferred);

		wrBuffer(phost, buffer, transferred);
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
	uint8_t transferArray[5];
	uint32_t sizeTransferred;
	eve_printf_debug("SPI cmd: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);

	transferArray[0] = cmd[0];
	transferArray[1] = cmd[1];
	transferArray[2] = cmd[2];
	transferArray[3] = cmd[3];
	transferArray[4] = cmd[4];

#if defined(EVE_BUFFER_WRITES)
	flush(phost);
#endif
	SPI_Write(phost->SpiHandle, transferArray, sizeof(transferArray), &sizeTransferred, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
}

static FT_STATUS EVE_HalImpl_passthroughGpio(EVE_HalContext *phost, uint8_t gpio, uint8_t val)
{
	/* Special behaviour for FT9xx passthrough to control GPIO pins on FT9xx.
	This produces a fake read */

	int tries = 0;

	uint8_t recvArray[2];
	uint8_t transferArray[8];
	uint32_t sizeTransferred;
	FT_STATUS ftRes;

	transferArray[0] = 0;
	transferArray[1] = 0;
	transferArray[2] = 0;
	transferArray[3] = 0;
	transferArray[4] = 0xA5;
	transferArray[5] = gpio;
	transferArray[6] = val;
	transferArray[7] = 0xA5 ^ gpio ^ val;

	recvArray[1] = 0;

#if defined(EVE_BUFFER_WRITES)
	flush(phost);
#endif
	do
	{
		EVE_sleep(20);
		ftRes = SPI_Write(phost->SpiHandle, transferArray, sizeof(transferArray), &sizeTransferred, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE)
		    || SPI_Read(phost->SpiHandle, recvArray, sizeof(recvArray), &sizeTransferred, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
		++tries;
	} while (ftRes == FT_OK && recvArray[1] != 0x5A && tries < 128);

	if (tries >= 128 && ftRes == FT_OK)
		ftRes = FT_IO_ERROR;

	return ftRes;
}

/**
 * @brief Toggle PD_N pin of FT800 board for a power cycle
 * 
 * @param phost Pointer to Hal context
 * @param up Up or Down
 * @return true True if ok
 * @return false False if error
 */
bool EVE_Hal_powerCycle(EVE_HalContext *phost, bool up)
{
	bool res = true;
	FT_STATUS ftRes;
	uint8_t pin = (1 << phost->PowerDownPin);
	uint8_t pwd1 = pin;
	uint8_t pwd0 = 0;
	DWORD written;
	uint8_t byOutputBuffer[3];
	DWORD dwNumBytesToSend = 0;
	DWORD dwNumBytesSent;
	eve_assert(phost->PowerDownPin < 8 || phost->PowerDownPin & 0x80);
#if defined(EVE_BUFFER_WRITES)
	flush(phost);
#endif
	if (up)
	{
		if (phost->PowerDownPin & 0x80)
			ftRes = EVE_HalImpl_passthroughGpio(phost, phost->PowerDownPin & 0x7F, 0);
		else if (phost->GpioHandle == phost->SpiHandle)
#if 1 // test Power down pin
		{
			byOutputBuffer[dwNumBytesToSend++] = 0x80; // #define MPSSE_CMD_SET_DATA_BITS_LOWBYTE		0x80
			byOutputBuffer[dwNumBytesToSend++] = 0x00; // value: set PD_N to L
			byOutputBuffer[dwNumBytesToSend++] = 0x80; // direction
			ftRes = FT_Write(phost->GpioHandle, byOutputBuffer, dwNumBytesToSend, &dwNumBytesSent);
			if (dwNumBytesToSend != dwNumBytesSent)
			{
				res = false;
			}
		}
#else
			ftRes = FT_OK;
#endif
		else
			ftRes = FT_Write(phost->GpioHandle, &pwd0, 1, &written);
		if (ftRes != FT_OK)
			res = false;

		eve_assert(ftRes == FT_OK);
		EVE_sleep(20);

		if (phost->PowerDownPin & 0x80)
			ftRes = EVE_HalImpl_passthroughGpio(phost, phost->PowerDownPin & 0x7F, 1);
		else if (phost->GpioHandle == phost->SpiHandle)
#if 1 // test Power down pin
		{
			dwNumBytesToSend = 0;
			byOutputBuffer[dwNumBytesToSend++] = 0x80; // #define MPSSE_CMD_SET_DATA_BITS_LOWBYTE		0x80
			byOutputBuffer[dwNumBytesToSend++] = 0x80; // value:: set PD_N to H
			byOutputBuffer[dwNumBytesToSend++] = 0x80; // direction
			ftRes = FT_Write(phost->GpioHandle, byOutputBuffer, dwNumBytesToSend, &dwNumBytesSent);
			if (dwNumBytesToSend != dwNumBytesSent)
			{
				res = false;
			}
		}
#else
			ftRes = FT_OK;
#endif
		else
			ftRes = FT_Write(phost->GpioHandle, &pwd1, 1, &written);
		if (ftRes != FT_OK)
			res = false;

		eve_assert(ftRes == FT_OK);
		EVE_sleep(20);
	}
	else
	{
		if (phost->PowerDownPin & 0x80)
			ftRes = EVE_HalImpl_passthroughGpio(phost, phost->PowerDownPin & 0x7F, 1);
		else if (phost->GpioHandle == phost->SpiHandle)
#if 1 // test Power down pin
		{
			byOutputBuffer[dwNumBytesToSend++] = 0x80; // #define MPSSE_CMD_SET_DATA_BITS_LOWBYTE		0x80
			byOutputBuffer[dwNumBytesToSend++] = 0x80; // value: set PD_N to H
			byOutputBuffer[dwNumBytesToSend++] = 0x80; // direction
			ftRes = FT_Write(phost->GpioHandle, byOutputBuffer, dwNumBytesToSend, &dwNumBytesSent);
			if (dwNumBytesToSend != dwNumBytesSent)
			{
				res = false;
			}
		}
#else
			ftRes = FT_OK;
#endif
		else
			ftRes = FT_Write(phost->GpioHandle, &pwd1, 1, &written);
		if (ftRes != FT_OK)
			res = false;

		eve_assert(ftRes == FT_OK);
		EVE_sleep(20);

		if (phost->PowerDownPin & 0x80)
			ftRes = EVE_HalImpl_passthroughGpio(phost, phost->PowerDownPin & 0x7F, 0);
		else if (phost->GpioHandle == phost->SpiHandle)
#if 1 // test Power down pin
		{
			dwNumBytesToSend = 0;
			byOutputBuffer[dwNumBytesToSend++] = 0x80; // #define MPSSE_CMD_SET_DATA_BITS_LOWBYTE		0x80
			byOutputBuffer[dwNumBytesToSend++] = 0x00; // value: set PD_N to L
			byOutputBuffer[dwNumBytesToSend++] = 0x80; // direction
			ftRes = FT_Write(phost->GpioHandle, byOutputBuffer, dwNumBytesToSend, &dwNumBytesSent);
			if (dwNumBytesToSend != dwNumBytesSent)
			{
				res = false;
			}
		}
#else
			ftRes = FT_OK;
#endif
		else
			ftRes = FT_Write(phost->GpioHandle, &pwd0, 1, &written);
		if (ftRes != FT_OK)
			res = false;

		eve_assert(ftRes == FT_OK);
		EVE_sleep(20);
	}
	return res;
}

/**
 * @brief Set number of SPI channel
 * 
 * @param phost Pointer to Hal context
 * @param numchnls Number of channel
 * @param numdummy Number of dummy bytes
 */
void EVE_Hal_setSPI(EVE_HalContext *phost, EVE_SPI_CHANNELS_T numchnls)
{
#if defined(EVE_BUFFER_WRITES)
	flush(phost);
#endif
	/* no-op */
}

void EVE_Hal_restoreSPI(EVE_HalContext *phost)
{
#if defined(EVE_BUFFER_WRITES)
	flush(phost);
#endif
	/* no-op */
}

/**
 * @brief get interrupt status from GPIO pin
 * ADBUS5 is connected with interrupt pin which is low active
 * 
 * @param phost Pointer to Hal context
 */
bool EVE_Hal_getInterrupt(EVE_HalContext *phost)
{
	uint8_t byOutputBuffer[3];
	DWORD dwNumBytesToSend = 0;
	DWORD dwNumBytesSent;
	FT_STATUS status;

#if 0 // ADBUS5 by default is set as an input pin
	dwNumBytesToSend = 0;
	byOutputBuffer[dwNumBytesToSend++] = 0x80; // Configure data bits low-byte of MPSSE port
	byOutputBuffer[dwNumBytesToSend++] = 0x80; // value
	byOutputBuffer[dwNumBytesToSend++] = 0x80; // direction: set ADBUS5 to input, keep ADBUS7 output since it is PD_N pin
	status = FT_Write(phost->GpioHandle, byOutputBuffer, dwNumBytesToSend, &dwNumBytesSent);
	if (dwNumBytesToSend != dwNumBytesSent)
	{
		return false;
	}
#endif

	// read GPIO status
	BYTE command = 0x81; // 0x81 = Read Low byte GPIO (ADBUS)
	status = FT_Write(phost->GpioHandle, &command, 1, &dwNumBytesSent);
	if (status != FT_OK)
	{
		return false;
	}

	// Read response
	DWORD bytesRead;
	BYTE inputBuffer[2];
	status = FT_Read(phost->GpioHandle, inputBuffer, 1, &bytesRead);
	if (status == FT_OK && bytesRead == 1)
	{
		if ((inputBuffer[0] & 0x20) == 0x0)
			return true;
	}

	return false;
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

#endif /* #if defined(MPSSE_PLATFORM) */

/* end of file */