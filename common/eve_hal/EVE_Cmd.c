/**
 * @file EVE_Cmd.c
 * @brief EVE's command read/write
 
 * This file defines the generic APIs of phost access layer for the BT820 or EVE compatible silicon.
 * Application shall access BT820 or EVE resources over these APIs,regardless of I2C or SPI protocol.
 * In addition, there are some helper functions defined for FT800 coprocessor engine as well as phost commands.
 *
 * @author Bridgetek
 *
 * @date 2024
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

#include "EVE_Platform.h"

/**
 * @brief End read/write to Coprocessor
 * 
 * @param phost Pointer to Hal context
 */
static inline void endFunc(EVE_HalContext *phost)
{
	if (phost->Status == EVE_STATUS_WRITING)
	{
		EVE_Hal_endTransfer(phost);
	}
}

/**
 * @brief Read from Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @return uint16_t Read pointer
 */
uint32_t EVE_Cmd_rp(EVE_HalContext *phost)
{
	uint32_t rp;
	endFunc(phost);
	rp = EVE_Hal_rd32(phost, REG_CMD_READ) & EVE_CMD_FIFO_MASK;
	if (EVE_CMD_FAULT(rp))
		phost->CmdFault = true;
	return rp;
}

/**
 * @brief Write to Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @return uint16_t Write pointer
 */
uint32_t EVE_Cmd_wp(EVE_HalContext *phost)
{
	endFunc(phost);
	return EVE_Hal_rd32(phost, REG_CMD_WRITE) & EVE_CMD_FIFO_MASK;
}

/**
 * @brief Get free space of Coprocessor's command buffer
 * 
 * @param phost Pointer to Hal context
 * @return uint16_t Free space in Bytes
 */
uint16_t EVE_Cmd_space(EVE_HalContext *phost)
{
	uint16_t space;
	endFunc(phost);
	
	space = EVE_Hal_rd16(phost, REG_CMDB_SPACE) & EVE_CMD_FIFO_MASK;
	if (EVE_CMD_FAULT(space))
		phost->CmdFault = true;
	phost->CmdSpace = space;
	return space;
}

/**
 * @brief Start transfer data to EVE
 * 
 * @param phost Pointer to Hal context
 */
static void startBufferTransfer(EVE_HalContext *phost)
{
	if (phost->Status != EVE_STATUS_WRITING)
	{
		EVE_Hal_startTransfer(phost, EVE_TRANSFER_WRITE, REG_CMDB_WRITE);
	}
}

/**
 * @brief Write string to Coprocessor's comand fifo
 * 
 * @param phost Pointer to Hal context
 * @param buffer Data pointer
 * @param size Size to write
 * @param transfer Byte to transfer
 * @param transfered Byte transfered
 * @return transfer Byte to transfer
 */
static uint32_t wrString(EVE_HalContext *phost, const void *buffer, uint32_t *size, uint32_t transfered, uint32_t transfer)
{
	uint32_t t;
	eve_assert(transfered == 0);
	eve_assert(transfer == *size); /* Cannot split string transfers */
	eve_assert(transfer <= phost->CmdSpace);
	t = EVE_Hal_transferString(phost, (const char *)buffer, transfered, transfer, 0x3);
	if (t != transfer) /* End of string */
	{
		eve_assert(t <= phost->CmdSpace);
		transfer = t;
		*size = (transfered + transfer);
	}
	eve_assert((transfered + transfer) == *size);

	return transfer;
}

/**
 * @brief Write buffer to Coprocessor's comand fifo
 * 
 * @param phost Pointer to Hal context
 * @param buffer Data pointer
 * @param size Size to write
 * @param progmem True if ProgMem 
 * @param string True is string
 * @return uint32_t Byte transfered
 */
static uint32_t wrBuffer(EVE_HalContext *phost, const void *buffer, uint32_t size, bool progmem, bool string)
{
	uint32_t transfered = 0;

	do
	{
		uint32_t transfer = (size - transfered);
		uint32_t space = phost->CmdSpace;
		uint32_t req = min((string ? (transfer + 1) : transfer), (EVE_CMD_FIFO_SIZE >> 1));
		eve_assert(!string || (req == (transfer + 1)));
		if (space < req)
		{
			if (!EVE_Cmd_waitSpace(phost, req))
				return transfered; /* Coprocessor fault */
			space = phost->CmdSpace;
		}
		if (transfer > space)
			transfer = space;
		eve_assert(transfer <= EVE_CMD_FIFO_SIZE - 4);
		if (transfer)
		{
			startBufferTransfer(phost);
			if (string)
			{
				transfer = wrString(phost, buffer, &size, transfered, transfer);
			}
			else if (progmem)
			{
				EVE_Hal_transferProgMem(phost, NULL, (eve_progmem_const uint8_t *)(uintptr_t)(&((uint8_t *)buffer)[transfered]), transfer);
			}
			else
			{
				EVE_Hal_transferMem(phost, NULL, &((uint8_t *)buffer)[transfered], transfer);
			}
			if (!string && (transfer & 0x3))
			{
				uint32_t pad = 4 - (transfer & 0x3);
				uint8_t padding[4] = { 0 };
				eve_assert((transfered + transfer) == size);
				EVE_Hal_transferMem(phost, NULL, padding, pad);
				transfer += pad;
				eve_assert(!(transfer & 0x3));
			}
			transfered += transfer;
			if (!phost->CmdFunc) /* Keep alive while writing function */
			{
				EVE_Hal_endTransfer(phost);
			}
			eve_assert(phost->CmdSpace >= transfer);
			phost->CmdSpace -= (uint16_t)transfer;
		}
	} while (transfered < size);
	return transfered;
}

/**
 * @brief Begin writing a function, keeps the transfer open
 * 
 * @param phost Pointer to Hal context
 */
void EVE_Cmd_startFunc(EVE_HalContext *phost)
{
	eve_assert(!phost->CmdWaiting);
	eve_assert(phost->CmdBufferIndex == 0);
	phost->CmdFunc = true;
}

/**
 * @brief End writing a function, closes the transfer
 * 
 * @param phost Pointer to Hal context
 */
void EVE_Cmd_endFunc(EVE_HalContext *phost)
{
	eve_assert(!phost->CmdWaiting);
	eve_assert(phost->CmdBufferIndex == 0);
	endFunc(phost);
	phost->CmdFunc = false;
}

/**
 * @brief Write buffer to Coprocessor's command fifo
 * 
 * @param phost Pointer to Hal context
 * @param buffer Data pointer
 * @param size Size to write
 * @return true Write ok
 * @return false Write error
 */
bool EVE_Cmd_wrMem(EVE_HalContext *phost, const uint8_t *buffer, uint32_t size)
{
	eve_assert(!phost->CmdWaiting);
	eve_assert(phost->CmdBufferIndex == 0);
	return wrBuffer(phost, buffer, size, false, false) == size;
}

/**
 * @brief Write buffer in ProgMem to Coprocessor's command fifo
 * 
 * @param phost Pointer to Hal context
 * @param buffer Data buffer
 * @param size Size to write
 * @return true True if ok
 * @return false False if error
 */
bool EVE_Cmd_wrProgMem(EVE_HalContext *phost, eve_progmem_const uint8_t *buffer, uint32_t size)
{
	eve_assert(!phost->CmdWaiting);
	eve_assert(phost->CmdBufferIndex == 0);
	return wrBuffer(phost, (void *)(uintptr_t)buffer, size, true, false) == size;
}

/**
 * @brief Write a string into Coprocessor's command fifo
 * 
 * @param phost Pointer to Hal context
 * @param str String to write
 * @param maxLength Length to write, up to EVE_CMD_STRING_MAX
 * @return uint32_t Number of bytes transfered
 */
uint32_t EVE_Cmd_wrString(EVE_HalContext *phost, const char *str, uint32_t maxLength)
{
	uint32_t transfered;
	eve_assert(!phost->CmdWaiting);
	eve_assert(phost->CmdBufferIndex == 0);
	transfered = wrBuffer(phost, str, maxLength, false, true);
	return transfered;
}

/**
 * @brief Write a byte to Coprocessor's command fifo
 * 
 * @param phost Pointer to Hal context
 * @param value Byte to write
 * @return true True if ok
 * @return false False if error
 */
bool EVE_Cmd_wr8(EVE_HalContext *phost, uint8_t value)
{
	eve_assert(!phost->CmdWaiting);
	eve_assert(phost->CmdBufferIndex < 4);

	if (phost->CmdBufferIndex < 4)
	{
		phost->CmdBuffer[phost->CmdBufferIndex++] = value;
	}

	if (phost->CmdBufferIndex == 4)
	{
		phost->CmdBufferIndex = 0;
		return EVE_Cmd_wrMem(phost, phost->CmdBuffer, 4);
	}

	return true;
}

/**
 * @brief Write 2 bytes to Coprocessor's command fifo
 * 
 * @param phost Pointer to Hal context
 * @param value Data to write
 * @return true True if ok
 * @return false False if error
 */
bool EVE_Cmd_wr16(EVE_HalContext *phost, uint16_t value)
{
	eve_assert(!phost->CmdWaiting);
	eve_assert(phost->CmdBufferIndex < 3);

	if (phost->CmdBufferIndex < 3)
	{
		phost->CmdBuffer[phost->CmdBufferIndex++] = value & 0xFF;
		phost->CmdBuffer[phost->CmdBufferIndex++] = value >> 8;
	}

	if (phost->CmdBufferIndex == 4)
	{
		phost->CmdBufferIndex = 0;
		return EVE_Cmd_wrMem(phost, phost->CmdBuffer, 4);
	}

	return true;
}

/**
 * @brief Write 4 bytes to Coprocessor's command fifo
 * 
 * @param phost Pointer to Hal context
 * @param value Data to write
 * @return true True if ok
 * @return false False if error
 */
bool EVE_Cmd_wr32(EVE_HalContext *phost, uint32_t value)
{
	eve_assert(!phost->CmdWaiting);
	eve_assert(phost->CmdBufferIndex == 0);

	if (phost->CmdSpace < 4 && !EVE_Cmd_waitSpace(phost, 4))
		return false;

	if (phost->Status != EVE_STATUS_WRITING)
	{
		EVE_Hal_startTransfer(phost, EVE_TRANSFER_WRITE, REG_CMDB_WRITE);
	}
	EVE_Hal_transfer32(phost, value);
	if (!phost->CmdFunc) /* Keep alive while writing function */
	{
		EVE_Hal_endTransfer(phost);
	}
	eve_assert(phost->CmdSpace >= 4);
	phost->CmdSpace -= 4;

	return true;
}

/**
 * @brief Move the write pointer forward by the specified number of bytes. Returns the previous write pointer
 * 
 * @param phost Pointer to Hal context
 * @param bytes Number of bytes to move
 * @return uint32_t Previous write pointer
 */
uint32_t EVE_Cmd_moveWp(EVE_HalContext *phost, uint16_t bytes)
{
	uint32_t wp;
	uint32_t prevWp;
	eve_assert(!phost->CmdWaiting);
	eve_assert(phost->CmdBufferIndex == 0);

	if (!EVE_Cmd_waitSpace(phost, bytes))
		return -1;

	prevWp = EVE_Cmd_wp(phost);
	wp = (prevWp + bytes) & EVE_CMD_FIFO_MASK;
	EVE_Hal_wr32(phost, REG_CMD_WRITE, wp);

	return prevWp;
}

#if defined(_DEBUG)
void debugBackupRamG(EVE_HalContext *phost);
#endif

/**
 * @brief Check for coprocessor fault
 * 
 * @param phost Pointer to Hal context
 * @param rpOrSpace Read pointer or space
 * @return true True if ok
 * @return false False if coprocessor fault
 */
static bool checkWait(EVE_HalContext *phost, uint16_t rpOrSpace)
{
	/* Check for coprocessor fault */
	if (EVE_CMD_FAULT(rpOrSpace))
	{
		/* Coprocessor fault */
		phost->CmdWaiting = false;
#if defined(_DEBUG)
		char err[128];
		err[0] = '\0';
		if (!phost->DebugMessageVisible)
		{
			eve_printf_debug("Coprocessor fault\n");
			debugBackupRamG(phost);
			EVE_Hal_rdMem(phost, (uint8_t *)err, RAM_REPORT, RAM_REPORT_MAX);
			eve_printf_debug("%s\n", err);
		}
#endif
		return false;
	}

	if (phost->Status == EVE_STATUS_ERROR || phost->Status == EVE_STATUS_CLOSED)
	{
		phost->CmdWaiting = false;
		eve_printf_debug("Host error\n");
		return false;
	}

	return true;
}

/**
 * @brief Wait handler
 * 
 * @param phost Pointer to Hal context
 * @param rpOrSpace Read pointer or space
 * @return true True if ok
 * @return false False if error
 */
static bool handleWait(EVE_HalContext *phost, uint16_t rpOrSpace)
{
	/* Check for coprocessor fault */
	if (!checkWait(phost, rpOrSpace))
		return false;

	/* Process any idling */
	EVE_Hal_idle(phost);

	/* Process user idling */
	if (phost->CbCmdWait && !phost->CbCmdWait(phost))
	{
		/* Wait aborted */
		phost->CmdWaiting = false;
		eve_printf_debug("Wait for coprocessor aborted\n");
		return false;
	}
	return true;
}

/**
 * @brief Wait till Command FIFO buffer empty
 * 
 * @param phost Pointer to Hal context
 * @return true True if ok
 * @return false False if error
 */
bool EVE_Cmd_waitFlush(EVE_HalContext *phost)
{
	uint32_t rp;
	uint32_t wp;

	eve_assert(!phost->CmdWaiting);
	phost->CmdWaiting = true;
	while ((rp = EVE_Cmd_rp(phost)) != (wp = EVE_Cmd_wp(phost)))
	{
		if (!handleWait(phost, rp))
		{
			phost->CmdSpace = (rp - wp - 4) & EVE_CMD_FIFO_MASK;
			return false;
		}
	}

	/* Command buffer empty */
	phost->CmdSpace = EVE_CMD_FIFO_SIZE - 4;
	phost->CmdWaiting = false;
	return true;
}

/** Wait for the command buffer to have at least the requested amount of free space.
 * @return 0 in case a coprocessor fault occurred 
 */
uint32_t EVE_Cmd_waitSpace(EVE_HalContext *phost, uint32_t size)
{
	uint16_t space;

	if (size > (EVE_CMD_FIFO_SIZE - 4))
	{
		eve_printf_debug("Requested free space exceeds coprocessor FIFO\n");
		return 0;
	}

	eve_assert(!phost->CmdWaiting);
	phost->CmdWaiting = true;

	space = phost->CmdSpace;

	/* Optimization. 
	Only update space if more space is needed than already known available, 
	or when not actually waiting for any space */
	if (space < size || !size)
		space = EVE_Cmd_space(phost);
	if (!checkWait(phost, space))
		return 0;

	/* Wait until there's sufficient space */
	while (space < size)
	{
		space = EVE_Cmd_space(phost);
		if (!handleWait(phost, space))
			return 0;
	}

	/* Sufficient space */
	phost->CmdWaiting = false;
	return space;
}

/**
 * @brief Wait until Coprocessor finished logo animation
 * 
 * @param phost Pointer to Hal context
 * @return true True if ok
 * @return false False if error
 */
bool EVE_Cmd_waitLogo(EVE_HalContext *phost)
{
	uint32_t rp;
	uint32_t wp;

	eve_assert(!phost->CmdWaiting);
	phost->CmdWaiting = true;

	do
	{
		rp = EVE_Cmd_rp(phost);
		wp = EVE_Cmd_wp(phost);
		if (!handleWait(phost, rp))
			return false;

	} while ((wp != 0) || (rp != 0));

	phost->CmdWaiting = false;
	return true;
}

/** Wait for a 32-bit value that was set by `EVE_CoCmd_memWrite32(phost, ptr, value)`.
 * @return true when the value is found. 
 * @return false otherwise
 * when the coprocessor has flushed, or a coprocessor fault occured. 
 */
bool EVE_Cmd_waitRead32(EVE_HalContext *phost, uint32_t ptr, uint32_t value)
{
	uint32_t rp;
	uint32_t wp;

	eve_assert(!phost->CmdWaiting);
	phost->CmdWaiting = true;
	while ((rp = EVE_Cmd_rp(phost)) != (wp = EVE_Cmd_wp(phost)))
	{
		if (EVE_Hal_rd32(phost, ptr) == value)
		{
			/* Expected value has been read */
			phost->CmdWaiting = false;
			return true;
		}
		if (!handleWait(phost, rp))
		{
			phost->CmdSpace = (rp - wp - 4) & EVE_CMD_FIFO_MASK;
			return false;
		}
	}

	/* Command buffer empty */
	phost->CmdSpace = EVE_CMD_FIFO_SIZE - 4;
	phost->CmdWaiting = false;
	return EVE_Hal_rd32(phost, ptr) == value;
}

/** Restore the internal state of EVE_Cmd.
 * Call this after manually writing to the coprocessor buffer 
 */
void EVE_Cmd_restore(EVE_HalContext *phost)
{
	EVE_Cmd_rp(phost);
	EVE_Cmd_wp(phost);
	EVE_Cmd_space(phost);
}

/* end of file */
