/**
 * @file EVE_HalDefs.h
 * @brief Eve_Hal framework APIs
 *
 *  This file defines the generic APIs of phost access layer for the BT820 or EVE compatible silicon.
 *  Application shall access BT820 or EVE resources over these APIs,regardless of I2C or SPI protocol.
 *  In addition, there are some helper functions defined for BT820 coprocessor engine as well as phost commands.
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

#ifndef EVE_HAL_DEFS__H
#define EVE_HAL_DEFS__H
/*************
** INCLUDES **
*************/
#include "EVE_GpuTypes.h"
#include "EVE_GpuDefs.h"
#if EVE_ENABLE_FATFS
#include "ff.h"
#endif

/***********
** MARCOS **
***********/

#define RAM_REPORT_MAX 128

/*************
** TYPEDEFS **
*************/


typedef enum EVE_STATUS_T
{
	EVE_STATUS_CLOSED = 0, /**< 0 */
	EVE_STATUS_OPENED, /**< 1 */
	EVE_STATUS_READING, /**< 2 */
	EVE_STATUS_WRITING, /**< 3 */
	EVE_STATUS_ERROR, /**< 4 */
} EVE_STATUS_T;

typedef enum EVE_TRANSFER_T
{
	EVE_TRANSFER_NONE = 0, /**< 0 */
	EVE_TRANSFER_READ, /**< 1 */
	EVE_TRANSFER_WRITE, /**< 2 */
} EVE_TRANSFER_T;

typedef enum EVE_CHIPID_T
{
	EVE_CHIPID_BT820 = EVE_BT820, /**< EVE_BT820 */
} EVE_CHIPID_T;

typedef enum EVE_HOST_T
{
	EVE_HOST_UNKNOWN = 0, /**< 0 */
	EVE_HOST_FT4222, /**< 1 */
	EVE_HOST_MPSSE, /**< 2 */

	EVE_HOST_NB /**< 3 */
} EVE_HOST_T;

typedef enum EVE_PWR_STATE_T
{
	EVE_PWR_STATE_ACTIVE = 0, /**< 0 */
	EVE_PWR_STATE_STANDBY, /**< 1 */
	EVE_PWR_STATE_SLEEP, /**< 2 */
	EVE_PWR_STATE_POWERDOWN /**< 3 */
} EVE_PWR_STATE_T;

/** SETPLLSP1 */
typedef struct EVE_CMD_SYS_PLL_FREQ
{
	uint8_t PLL_NS : 7; /**< PLL frequency multiplier factor.Default : 15 */
	uint8_t PLL_CKS : 1; /**< PLL range */
} EVE_CMD_SYS_PLL_FREQ;

/** SYSCLKDIV */
typedef struct EVE_CMD_SYS_CLK_DIV
{
	uint8_t freq : 4; /**< system clock division factor. Default: 7 */
	uint8_t SYSPLL_CPS : 3; /**< b'001' */
	uint8_t rev : 1; /**< reserved */
} EVE_CMD_SYS_CLK_DIV;

/** SETBOOTCFG */
typedef struct EVE_CMD_BOOT_CFG
{
	uint8_t source : 3; /**< source field */
	uint8_t rev : 1; /**< reserved */
	uint8_t watchdog : 1; /**< 1 - watchdog system will init */
	uint8_t audio : 1; /**< 1 - audio system will init */
	uint8_t touch : 1; /**< 1 - touch will init */
	uint8_t DDR : 1; /**< 1 - DDR subsystem will init */
} EVE_CMD_BOOT_CFG;

/** BOOTCFGEN */
typedef struct EVE_CMD_BOOT_CFG_EN
{
	uint8_t enable : 1; /**< 1 - allow update of boot control settings; 0 (default) - ignore updates to boot */
	uint8_t rev : 4; /**< reserved */
	uint8_t GPREG : 1; /**< GPREG_USER_SETTING; 0 - DEFAULT_GPREG */
	uint8_t DDRtype : 1; /**< 1 - DDRTYPE_USER_SETTING; 0 - DEFAULT_DDRTYPE */
	uint8_t boot : 1; /**< 1 - BOOT_USER_SETTING; 0 - DEFAULT_BOOTCFG */
} EVE_CMD_BOOT_CFG_EN;

/** SETDDRTYPE - ref: REG_DDR_TYPE */
typedef struct EVE_CMD_DDR_TYPE
{
	uint8_t size : 3; /**< DDR_SIZEDDR DRAM size */
	uint8_t type : 2; /**< DDR_TYPE */
	uint8_t speed : 3; /**< DDR_SPEEDGRADE */
} EVE_CMD_DDR_TYPE;

/** SETGPREG - ref: REG_GPREG */
typedef struct EVE_CMD_GPREG
{
	uint8_t mode : 1; /**< SPI master slow mode */
	uint8_t rev : 7; /**< reserved */
} EVE_CMD_GPREG;

typedef struct EVE_HalContext EVE_HalContext;
typedef bool (*EVE_Callback)(EVE_HalContext *phost);

/** Hook into coprocessor commands. Return 1 to abort the command. Useful for an optimization routine */
typedef int (*EVE_CoCmdHook)(EVE_HalContext *phost, uint32_t cmd, uint32_t state);
/** Hook into coprocessor reset */
typedef void (*EVE_ResetCallback)(EVE_HalContext *phost, bool fault);

typedef struct EVE_DeviceInfo
{
	char DisplayName[256];
	char SerialNumber[16];
	EVE_HOST_T Host;
	bool Opened;

} EVE_DeviceInfo;

/** Hal parameters */
typedef struct EVE_HalParameters
{
	void *UserContext;

	/** Called anytime the code is waiting during CMD write. Return false to abort wait */
	EVE_Callback CbCmdWait;

#if defined(MPSSE_PLATFORM)
	uint32_t MpsseChannelNo; /**< MPSSE channel number */
#endif

#if defined(FT4222_PLATFORM)
	uint32_t DeviceIdx;
	uint8_t SpiCsPin; /**< SPI chip select number of FT8XX chip */
#endif

#if defined(FT4222_PLATFORM) || defined(MPSSE_PLATFORM)
	uint8_t PowerDownPin; /**< FT8XX power down pin number */
#endif

#if defined(FT4222_PLATFORM) || defined(MPSSE_PLATFORM)
	uint16_t SpiClockrateKHz; /**< In kHz */
#endif

#if defined(RP2040_PLATFORM)
	uint32_t DeviceIdx;
	uint8_t SpiCsPin; /**< SPI chip select number of BT8XX chip */
	uint8_t SpiSckPin;
	uint8_t SpiMosiPin;
	uint8_t SpiMisoPin;
	uint8_t PowerDownPin; /**< BT8XX power down pin number */
#endif

} EVE_HalParameters;

typedef struct EVE_HalContext
{
	/** Pointer to user context */
	void *UserContext;

	/** Called anytime the code is waiting during CMD write. Return false to abort wait */
	EVE_Callback CbCmdWait;
	/** Callback hook called anytime the coprocessor is reset through the EVE_Util interface */
	EVE_ResetCallback CbCoprocessorReset;

#if EVE_CMD_HOOKS
	/* Hook into coprocessor commands. Called when EVE_CoCmd interface is used. Return 1 to abort the command. Useful for an optimization routine */
	EVE_CoCmdHook CoCmdHook;
#endif

	EVE_STATUS_T Status;

	uint8_t PCLK;

	/** @name User space width and height, based on REG_HSIZE, REG_VSIZE */
	///@{
	uint32_t Width;
	uint32_t Height;
	///@}

	/** @name Handles to external context */
	///@{
#if defined(FT4222_PLATFORM) | defined(MPSSE_PLATFORM)
	void *SpiHandle;
	void *GpioHandle; /**< LibFT4222 uses this member to store GPIO handle */
#endif

#if defined(FT4222_PLATFORM) | defined(MPSSE_PLATFORM)
	/** Currently configured SPI clock rate. In kHz.
	May be different from requested the clock rate in parameters */
	uint16_t SpiClockrateKHz;
#endif
	EVE_SPI_CHANNELS_T SpiChannels; /**< Variable to contain single/dual/quad channels */

#if defined(MPSSE_PLATFORM)
	uint32_t MpsseChannelNo; /**< MPSSE channel number */
#endif
#if defined(FT4222_PLATFORM)
	uint8_t SpiCsPin; /**< SPI chip select number of BT8XX chip */
#endif
#if defined(FT4222_PLATFORM) || defined(MPSSE_PLATFORM)
	uint8_t PowerDownPin; /**< BT8XX power down pin number */
#endif
#if defined(RP2040_PLATFORM)
	void *SpiPort; /**< SPI port */
	uint8_t SpiCsPin; /**< SPI chip select number of BT8XX chip */
	uint8_t SpiSckPin;
	uint8_t SpiMosiPin;
	uint8_t SpiMisoPin;
	uint8_t PowerDownPin; /**< BT8XX power down pin number */
#endif
	///@}

	/** @name Write buffer to optimize writes into larger batches */
	///@{
#if defined(EVE_BUFFER_WRITES) || defined(FT4222_PLATFORM)
	uint8_t SpiWrBuf[65536];
	uint32_t SpiWrBufIndex;
	uint32_t SpiRamGAddr; /**< Current RAM_G address of ongoing SPI write transaction */
#endif
	///@}

	/** @name Buffer cmd smaller than a full cmd command */
	///@{
	uint8_t CmdBuffer[4];
	uint8_t CmdBufferIndex;

	uint32_t CmdSpace; /**< Free space, cached value */
	///@}

	/** @name Media FIFO state */
	///@{
#if defined(EVE_SUPPORT_MEDIAFIFO)
	uint32_t MediaFifoAddress;
	uint32_t MediaFifoSize;
#if EVE_ENABLE_FATFS
	FIL LoadFileObj;
#else
	void *LoadFileHandle;
#endif
	ptrdiff_t LoadFileRemaining;
#endif
	///@}

#if defined(_DEBUG)
	bool DebugMessageVisible;
	uint8_t DebugBackup[RAM_REPORT_MAX];
#endif

	/** @name Status flags */
	///@{
	bool CmdFunc; /**< Flagged while transfer to cmd is kept open */
	bool CmdFault; /**< Flagged when coprocessor is in fault mode and needs to be reset */
	bool CmdWaiting; /**< Flagged while waiting for CMD write (to check during any function that may be called by CbCmdWait) */
	///@}

	uint32_t DDR_RamSize; /**< DDR RAM SIZE from REG_RAM_SIZE For BT82X */

} EVE_HalContext;

typedef struct EVE_HalPlatform
{
	uint32_t OpenedDevices;

} EVE_HalPlatform;

/**********************
 *   INLINE FUNCTIONS
 **********************/

/** Include the EVE generation in the chip ID value to simplify feature set comparisons(BT880 support) */
static inline EVE_CHIPID_T EVE_extendedChipId(int chipId)
{
	switch (chipId & 0xFFFF)
	{
	    case EVE_BT820 & 0xFFFF:
		    return (EVE_CHIPID_T)((chipId & 0xFFFF) | 0x50000);
	    default:
		    break;
	}
	return (EVE_CHIPID_T)(chipId & 0xFFFF);
}

/** Remove EVE generation from the chip ID */
static inline int EVE_shortChipId(EVE_CHIPID_T chipId)
{
	return chipId & 0xFFFF;
}

static inline int EVE_gen(EVE_CHIPID_T chipId)
{
	switch (chipId)
	{
	    case EVE_BT820:
		    return EVE5;
	    default:
		    break;
	}
	return 0;
}

#endif /* #ifndef EVE_HAL_DEFS__H */

/* end of file */
