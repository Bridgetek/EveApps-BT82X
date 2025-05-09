/**
 * @file libMPSSE_spi.h
 * @brief MPSSE APIs
 *
 * @author FTDI
 *
 * @date 27/05/2011
 * 
 * MIT License
 *
 * Copyright (c) [2011] [Future Technology Devices International Limited (FTDI)]
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
 *
 * Project: libMPSSE
 * Module: SPI
 *
 * Rivision History:
 * 0.1  - initial version
 * 0.2  - 20110708 - added FT_ReadGPIO, FT_WriteGPIO & SPI_ChangeCS
 * 0.3  - 20111025 - modified for supporting 64bit linux
 * 0.41 - 20140903 - modified for compilation issues with either C application/C++ application
 */

#ifndef LIBMPSSE_SPI_H
#define LIBMPSSE_SPI_H

#include "ftd2xx.h"

/******************************************************************************/
/*								Macro defines								  */
/******************************************************************************/

#ifdef __cplusplus
#define FTDI_API extern "C"
#else
#define FTDI_API
#endif

/** @name Bit defination of the transferOptions parameter in SPI_Read, SPI_Write & SPI_Transfer */
///@{

/** transferOptions-Bit0: If this bit is 0 then it means that the transfer size provided is in bytes */
#define SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES 0x00000000
/** transferOptions-Bit0: If this bit is 1 then it means that the transfer size provided is in bytes */
#define SPI_TRANSFER_OPTIONS_SIZE_IN_BITS 0x00000001
/** transferOptions-Bit1: if BIT1 is 1 then CHIP_SELECT line will be enabled at start of transfer */
#define SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE 0x00000002
/** transferOptions-Bit2: if BIT2 is 1 then CHIP_SELECT line will be disabled at end of transfer */
#define SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE 0x00000004
///@}

/** @name Bit defination of the Options member of configOptions structure*/
///@{
#define SPI_CONFIG_OPTION_MODE_MASK 0x00000003
#define SPI_CONFIG_OPTION_MODE0 0x00000000
#define SPI_CONFIG_OPTION_MODE1 0x00000001
#define SPI_CONFIG_OPTION_MODE2 0x00000002
#define SPI_CONFIG_OPTION_MODE3 0x00000003

#define SPI_CONFIG_OPTION_CS_MASK 0x0000001C /**< 111 00*/
#define SPI_CONFIG_OPTION_CS_DBUS3 0x00000000 /**< 000 00*/
#define SPI_CONFIG_OPTION_CS_DBUS4 0x00000004 /**< 001 00*/
#define SPI_CONFIG_OPTION_CS_DBUS5 0x00000008 /**< 010 00*/
#define SPI_CONFIG_OPTION_CS_DBUS6 0x0000000C /**< 011 00*/
#define SPI_CONFIG_OPTION_CS_DBUS7 0x00000010 /**< 100 00*/

#define SPI_CONFIG_OPTION_CS_ACTIVELOW 0x00000020
///@}

/******************************************************************************/
/*								Type defines								  */
/******************************************************************************/

#ifndef _FTDI_LIB_FT4222_H_
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long long uint64;

typedef signed char int8;
typedef signed short int16;
typedef signed long long int64;

#ifdef __x86_64__
/*20111025: 64bit linux doesn't work is uint32 is unsigned long*/
typedef unsigned int uint32;
typedef signed int int32;
#else
typedef unsigned long uint32;
typedef signed long int32;
#endif
#endif

typedef enum I2C_ClockRate_t
{
	I2C_CLOCK_STANDARD_MODE = 100000, /**< 100kb/sec */
	I2C_CLOCK_FAST_MODE = 400000, /**< 400kb/sec */
	I2C_CLOCK_FAST_MODE_PLUS = 1000000, /**< 1000kb/sec */
	I2C_CLOCK_HIGH_SPEED_MODE = 3400000 /**< 3.4Mb/sec */
} I2C_CLOCKRATE;

typedef struct ChannelConfig_t
{
	uint32 ClockRate;

	uint8 LatencyTimer;

	uint32 configOptions; /**< This member provides a way to enable/disable features
	specific to the protocol that are implemented in the chip
	\n BIT1-0=CPOL-CPHA:	00 - MODE0 - data captured on rising edge, propagated on falling
 	\n					01 - MODE1 - data captured on falling edge, propagated on rising
 	\n					10 - MODE2 - data captured on falling edge, propagated on rising
 	\n					11 - MODE3 - data captured on rising edge, propagated on falling
	\n BIT4-BIT2: 000 - A/B/C/D_DBUS3=ChipSelect
	\n		 : 001 - A/B/C/D_DBUS4=ChipSelect
 	\n		 : 010 - A/B/C/D_DBUS5=ChipSelect
 	\n		 : 011 - A/B/C/D_DBUS6=ChipSelect
 	\n		 : 100 - A/B/C/D_DBUS7=ChipSelect
 	\n BIT5: ChipSelect is active high if this bit is 0
	\n BIT6 -BIT31		: Reserved
	*/

	uint32 Pin; /**< BIT7   -BIT0:   Initial direction of the pins
	\n BIT15 -BIT8:   Initial values of the pins
	\n BIT23 -BIT16: Final direction of the pins
	\n BIT31 -BIT24: Final values of the pins */
	uint16 reserved;
} ChannelConfig;

/******************************************************************************/
/*								External variables							  */
/******************************************************************************/

/******************************************************************************/
/*								Function declarations						  */
/******************************************************************************/
FTDI_API FT_STATUS SPI_GetNumChannels(uint32 *numChannels);
FTDI_API FT_STATUS SPI_GetChannelInfo(uint32 index,
    FT_DEVICE_LIST_INFO_NODE *chanInfo);
FTDI_API FT_STATUS SPI_OpenChannel(uint32 index, FT_HANDLE *handle);
FTDI_API FT_STATUS SPI_InitChannel(FT_HANDLE handle, ChannelConfig *config);
FTDI_API FT_STATUS SPI_CloseChannel(FT_HANDLE handle);
FTDI_API FT_STATUS SPI_Read(FT_HANDLE handle, uint8 *buffer,
    uint32 sizeToTransfer, uint32 *sizeTransfered, uint32 options);
FTDI_API FT_STATUS SPI_Write(FT_HANDLE handle, uint8 *buffer,
    uint32 sizeToTransfer, uint32 *sizeTransfered, uint32 options);
FTDI_API FT_STATUS SPI_ReadWrite(FT_HANDLE handle, uint8 *inBuffer,
    uint8 *outBuffer, uint32 sizeToTransfer, uint32 *sizeTransferred,
    uint32 transferOptions);
FTDI_API FT_STATUS SPI_IsBusy(FT_HANDLE handle, unsigned char *state);
FTDI_API void Init_libMPSSE(void);
FTDI_API void Cleanup_libMPSSE(void);
FTDI_API FT_STATUS SPI_ChangeCS(FT_HANDLE handle, uint32 configOptions);
FTDI_API FT_STATUS FT_WriteGPIO(FT_HANDLE handle, uint8 dir, uint8 value);
FTDI_API FT_STATUS FT_ReadGPIO(FT_HANDLE handle, uint8 *value);
FTDI_API FT_STATUS SPI_ToggleCS(FT_HANDLE handle, unsigned char state);

/******************************************************************************/

#endif /*LIBMPSSE_SPI_H*/
