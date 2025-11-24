/**
 * @file EVE_HalImpl_RP2040.c
 * @brief Eve_Hal framework APIs for RP2040 host platform
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
#include "EVE_HalImpl.h"
#if defined(RP2040_PLATFORM)

bool EVE_Hal_NoInit = false;
#define POLLING_BYTES 8
#define READ_TIMEOUT 5

void setSPI(EVE_HalContext *phost, EVE_SPI_CHANNELS_T numchnls);

/** @name INIT */
///@{

/**
 * @brief Initialize global HAL platform
 *
 */
void EVE_HalImpl_initialize()
{
	/* no-op */
}

/**
 * @brief Release global HAL platform
 *
 */
void EVE_HalImpl_release()
{
	/* no-op */
}

/**
 * @brief Get the default configuration parameters
 *
 * @param parameters EVE_Hal framework's parameters
 * @param deviceIdx
 * @return true True if ok
 * @return false False if error
 */
bool EVE_HalImpl_defaults(EVE_HalParameters *parameters, size_t deviceIdx)
{
	deviceIdx = EVE_DEFAULT_SPI;
	parameters->DeviceIdx = deviceIdx;
	parameters->PowerDownPin = deviceIdx ? EVE_DEFAULT_SPI1_PWD : EVE_DEFAULT_SPI0_PWD;
	parameters->SpiCsPin = deviceIdx ? EVE_DEFAULT_SPI1_CS1 : EVE_DEFAULT_SPI0_CS;
	parameters->SpiSckPin = deviceIdx ? EVE_DEFAULT_SPI1_SCK : EVE_DEFAULT_SPI0_SCK;
	parameters->SpiMosiPin = deviceIdx ? EVE_DEFAULT_SPI1_MOSI : EVE_DEFAULT_SPI0_MOSI;
	parameters->SpiMisoPin = deviceIdx ? EVE_DEFAULT_SPI1_MISO : EVE_DEFAULT_SPI0_MISO;
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
	phost->SpiPort = parameters->DeviceIdx ? spi1 : spi0;
	phost->SpiCsPin = parameters->SpiCsPin;
	phost->SpiSckPin = parameters->SpiSckPin;
	phost->SpiMosiPin = parameters->SpiMosiPin;
	phost->SpiMisoPin = parameters->SpiMisoPin;
	phost->PowerDownPin = parameters->PowerDownPin;

	eve_printf_debug("Open port %s for EVE\n", parameters->DeviceIdx ? "SPI1" : "SPI0");
	eve_printf_debug("CS: GP%i, SCK: GP%i, MOSI: GP%i, MISO: GP%i, PWD: GP%i\n",
	    (int)phost->SpiCsPin, (int)phost->SpiSckPin, (int)phost->SpiMosiPin,
	    (int)phost->SpiMisoPin, (int)phost->PowerDownPin);

	gpio_init(phost->PowerDownPin);
	gpio_set_dir(phost->PowerDownPin, GPIO_OUT);
	gpio_put(phost->PowerDownPin, 0);

	/* Initialize single channel */
	setSPI(phost, EVE_SPI_SINGLE_CHANNEL);

	gpio_put(phost->PowerDownPin, 1);

	/* Initialize the context variables */
	phost->Status = EVE_STATUS_OPENED;

	return true;
}

/**
 * @brief Close a HAL context
 *
 * @param phost Pointer to Hal context
 */
void EVE_HalImpl_close(EVE_HalContext *phost)
{
	size_t deviceIdx = phost->SpiPort == spi1 ? 1 : 0;
	phost->Status = EVE_STATUS_CLOSED;
	spi_deinit(phost->SpiPort);
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

/** @name TRANSFER */
///@{

static inline void csSelect(EVE_HalContext *phost)
{
	asm volatile("nop \n nop \n nop");
	gpio_put(phost->SpiCsPin, 0); /* Active low */
	asm volatile("nop \n nop \n nop");
}

static inline void csDeselect(EVE_HalContext *phost)
{
	asm volatile("nop \n nop \n nop");
	gpio_put(phost->SpiCsPin, 1);
	asm volatile("nop \n nop \n nop");
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

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
		uint8_t spidata[4];
		spidata[0] = (addr >> 24) & 0xFF;
		spidata[1] = (addr >> 16) & 0xFF;
		spidata[2] = (addr >> 8) & 0xFF;
		spidata[3] = addr & 0xFF;
		csSelect(phost);
		spi_write_blocking(phost->SpiPort, spidata, 4);
		phost->Status = EVE_STATUS_READING;
	}
	else
	{
		uint8_t spidata[4];
		spidata[0] = (addr >> 24) | 0x80;
		spidata[1] = (addr >> 16) & 0xFF;
		spidata[2] = (addr >> 8) & 0xFF;
		spidata[3] = addr & 0xFF;

		csSelect(phost);
		spi_write_blocking(phost->SpiPort, spidata, 4);

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

	csDeselect(phost);
	phost->Status = EVE_STATUS_OPENED;
}

/**
 * @brief Flush data
 *
 * @param phost Pointer to Hal context
 */
void EVE_Hal_flush(EVE_HalContext *phost)
{
	eve_assert(phost->Status == EVE_STATUS_OPENED);
	/* no-op */
}

/**
 * @brief Read a block data
 *
 * @param phost Pointer to Hal context
 * @param buffer Buffer to get result
 * @param size Number of bytes to read
 * @return true True if ok
 * @return false False if error
 */
static inline bool rdBuffer(EVE_HalContext *phost, uint8_t *buffer, uint32_t size)
{
	uint8_t buffer_readybyte[8];
	uint8_t retry = 0;
	bool readyRecved = false;
	uint8_t sizeTransferred;
	if (size & 3)
	{
		size = (size + 3) & ~3UL;
		eve_printf_debug("Data size should be align to 4 bytes\n");
	}
	while (1)
	{
		spi_read_blocking(phost->SpiPort, 0, buffer_readybyte, 8);

		for (uint8_t i = 0; i < POLLING_BYTES; i++)
		{
			if (buffer_readybyte[i] == 0x01)
			{
				readyRecved = true;
				sizeTransferred = POLLING_BYTES - (i + 1);
				sizeTransferred = size > sizeTransferred ? sizeTransferred : size;
				memcpy(buffer, &buffer_readybyte[i + 1], sizeTransferred);
				break;
			}
		}
		if (readyRecved)
			break;

		retry++;
		if (retry >= READ_TIMEOUT)
			return false;
	}

    if (sizeTransferred < size)
	    spi_read_blocking(phost->SpiPort, 0, buffer + sizeTransferred, size - sizeTransferred);

	return true;
}

/**
 * @brief Write a block data
 *
 * @param phost Pointer to Hal context
 * @param buffer Data buffer to write
 * @param size Size of buffer
 */
static inline void wrBuffer(EVE_HalContext *phost, const uint8_t *buffer, uint32_t size)
{
	spi_write_blocking(phost->SpiPort, buffer, size);
}

/**
 * @brief Write 4 bytes
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
 * @brief Transfer (read/write) a block data
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
	if (!size)
	{
		/* TODO: Support different padding options */
		eve_assert(padMask == 0x3);
		EVE_Hal_transfer32(phost, 0);
		return 4;
	}

	eve_assert(size <= EVE_CMD_STRING_MAX);
	uint32_t transferred = 0;
	if (phost->Status == EVE_STATUS_WRITING)
	{
		transferred += (uint32_t)strnlen(str, size) + 1;
		eve_assert(str[transferred - 1] == '\0');
		wrBuffer(phost, (uint8_t *)str, transferred);
		if (transferred & padMask)
		{
			uint32_t pad = 4 - (transferred & padMask);
			uint8_t padding[4] = { 0 };
			wrBuffer(phost, padding, pad);
			transferred += pad;
			eve_assert(!(transferred & 0x3));
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

/** @name UTILITY */
///@{

/**
 * @brief This API sends a 5byte command to the phost
 *
 * @param phost Pointer to Hal context
 * @param cmd Command to send
 */
void EVE_Hal_spiCommand(EVE_HalContext* phost, uint8_t* cmd)
{
	eve_assert(phost->Status == EVE_STATUS_OPENED);
	eve_printf_debug("SPI cmd: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);

	csSelect(phost);
	spi_write_blocking(phost->SpiPort, cmd, 5);
	csDeselect(phost);
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
	if (up)
	{
		/* Power down */
		gpio_put(phost->PowerDownPin, 0);
		EVE_sleep(20);

		setSPI(phost, EVE_SPI_SINGLE_CHANNEL);
		EVE_sleep(20);

		/* Power up */
		gpio_put(phost->PowerDownPin, 1);
		EVE_sleep(20);
	}
	else
	{
		/* Power down */
		gpio_put(phost->PowerDownPin, 0);
		EVE_sleep(20);

		setSPI(phost, EVE_SPI_SINGLE_CHANNEL);
	}
	return true;
}

/**
 * @brief Set number of SPI channel
 *
 * @param phost Pointer to Hal context
 * @param numchnls Number of channel
 * @param numdummy Number of dummy bytes
 */
void setSPI(EVE_HalContext *phost, EVE_SPI_CHANNELS_T numchnls)
{
	/* SPI initialisation. This will use SPI at 25MHz */
	spi_init(phost->SpiPort, 25 * 1000 * 1000);
	gpio_set_function(phost->SpiMisoPin, GPIO_FUNC_SPI);
	gpio_set_function(phost->SpiMosiPin, GPIO_FUNC_SPI);
	gpio_set_function(phost->SpiSckPin, GPIO_FUNC_SPI);

	/* Chip select is active-low, so we'll initialise it to a driven-high state */
	gpio_init(phost->SpiCsPin);
	gpio_set_dir(phost->SpiCsPin, GPIO_OUT);
	gpio_put(phost->SpiCsPin, 1);

	/* Only support single channel */
	phost->SpiChannels = numchnls;
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
	// RP2040 only support single SPI mode
	setSPI(phost, EVE_SPI_SINGLE_CHANNEL);
}

void EVE_Hal_restoreSPI(EVE_HalContext *phost)
{
	setSPI(phost, phost->SpiChannels);
}

/**
 * @brief Get interrupt status
 * 
 * @param phost Pointer to Hal context
 * @return True on interrupt happened or otherwise
 */
bool EVE_Hal_getInterrupt(EVE_HalContext *phost)
{
	//INT# connected to GPIO6
#define EVE_INT 6
	bool value = gpio_get(EVE_INT);
	return (~value);
}
///@}

/** @name MISC */
///@{

/**
 * @brief Init host MCU
 *
 */
void EVE_Mcu_initialize()
{
	if (!EVE_Hal_NoInit)
	{
		stdio_init_all();
	}
	sleep_ms(1000);

	eve_printf_debug("--------------------------------\n");
	eve_printf_debug("- Bridgetek EVE HAL for RP2040 -\n");
	eve_printf_debug("--------------------------------\n");
}

/**
 * @brief Release host MCU
 *
 */
void EVE_Mcu_release()
{
	/* no-op */
}
///@}


/* Globals for interrupt implementation */
static absolute_time_t s_LastTime;
static uint64_t s_TotalMilliseconds64;
static int32_t s_RemainderMicros;

/** @name MISC */
///@{
/**
 * @brief Init timer
 *
 */
void EVE_Millis_initialize()
{
	s_LastTime = get_absolute_time();
	s_TotalMilliseconds64 = 0;
	s_RemainderMicros = 0;
}

/**
 * @brief Release timer
 *
 */
void EVE_Millis_release()
{
	/* no-op */
}

static inline void updateMillis()
{
	absolute_time_t newTime = get_absolute_time();
	int64_t diff = absolute_time_diff_us(s_LastTime, newTime);
	diff += s_RemainderMicros;
	if (diff >= 1000)
	{
		++s_TotalMilliseconds64;
		diff -= 1000;
		if (diff >= 1000)
		{
			uint64_t ms = diff / 1000;
			s_TotalMilliseconds64 += ms;
			diff -= (ms * 1000);
		}
		s_LastTime = newTime;
		eve_assert(diff >= 0);
		s_RemainderMicros = (int32_t)diff;
	}
}

/**
 * @brief Get clock in miliseond
 *
 * @return uint32_t Clock number
 */
uint32_t EVE_millis()
{
	updateMillis();
	return s_TotalMilliseconds64;
}

/**
 * @brief Get clock in miliseond
 *
 * @return uint64_t Clock number
 */
uint64_t EVE_millis64()
{
	updateMillis();
	return s_TotalMilliseconds64;
}

/**
 * @brief Sleep in milisecond
 *
 * @param ms Milisecond
 */
void EVE_sleep(uint32_t ms)
{
	sleep_ms(ms);
}

///@}
#endif //RP2040_PLATFORM

/* end of file */
