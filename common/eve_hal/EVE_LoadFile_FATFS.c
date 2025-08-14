/**
 * @file EVE_LoadFile_FATFS.c
 * @brief Eve_Hal framework APIs for loading file with FATFS
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

#include "EVE_LoadFile.h"
#include "EVE_MediaFifo.h"

#if defined(RP2040_PLATFORM)
#define MOUNT_POINT ""
#if EVE_ENABLE_FATFS
static bool s_FatFSLoaded = false;
static FATFS s_FatFS;
#endif

/**
 * @brief SD card detect 
 *
 * @return SDHOST_STATUS
 */
SDHOST_STATUS sdhost_card_detect(void)
{
	FRESULT fr; /* FatFs return code */
	UINT br;
	UINT bw;

	fr = f_mount(&s_FatFS, MOUNT_POINT, 1);
	if (fr != FR_OK)
	{
		eve_printf_debug("mount error %d\n", fr);
		return SDHOST_CARD_REMOVED;
	}
	eve_printf_debug("mount ok\n");
	return SDHOST_CARD_INSERTED;
}

/**
 * @brief SD card init 
 */
void sdhost_init(void)
{
	FRESULT fr; /* FatFs return code */
	UINT br;
	UINT bw;

	fr = f_mount(&s_FatFS, MOUNT_POINT, 1);
	if (fr != FR_OK)
	{
		eve_printf_debug("mount error %d\n", fr);
	}
	eve_printf_debug("mount ok\n");

	switch (s_FatFS.fs_type)
	{
	case FS_FAT12:
		printf("Type is FAT12\n");
		break;
	case FS_FAT16:
		printf("Type is FAT16\n");
		break;
	case FS_FAT32:
		printf("Type is FAT32\n");
		break;
	case FS_EXFAT:
		printf("Type is EXFAT\n");
		break;
	default:
		printf("Type is unknown\n");
		break;
	}
	eve_printf_debug("Card size: %7.2f GB (GB = 1E9 bytes)\n\n", s_FatFS.csize * s_FatFS.n_fatent * 512E-9);
}

/**
 * @brief Mount the SDcard
 *
 * @param phost Pointer to Hal context
 * @return true True if ok
 * @return false False if error
 */
bool EVE_Util_loadSdCard(EVE_HalContext *phost)
{
#if EVE_ENABLE_FATFS
	SDHOST_STATUS status = sdhost_card_detect();
	if (status == SDHOST_CARD_INSERTED)
	{
		if (!s_FatFSLoaded && (f_mount(&s_FatFS, MOUNT_POINT, 1) != FR_OK))
		{
			printf("FatFS SD card mount failed\n");
		}
		else
		{
			printf("FatFS SD card mounted successfully\n");
			s_FatFSLoaded = true;
		}
	}
	else
	{
		printf("SD card not detected\n");
		s_FatFSLoaded = false;
	}
	return s_FatFSLoaded;
#else
	return false;
#endif
}

/**
 * @brief Check SDcard status
 *
 * @param phost Pointer to Hal context
 * @return true True if ready
 * @return false False if not
 */
bool EVE_Util_sdCardReady(EVE_HalContext *phost)
{
	/* no-op */
#if EVE_ENABLE_FATFS
	return s_FatFSLoaded;
#else
	return false;
#endif
}

/**
 * @brief Load a raw file into RAM_G
 *
 * @param phost  Pointer to Hal context
 * @param address Address in RAM_G
 * @param filename File to load
 * @return uint32_t File size
 */
uint32_t EVE_Util_loadRawFile(EVE_HalContext *phost, uint32_t address, const char *filename)
{
#if EVE_ENABLE_FATFS
	FRESULT fResult;
	FIL InfSrc;

	UINT blocklen;
	int32_t filesize;
	uint8_t buffer[512L];
	uint32_t addr = address;
	uint32_t file_size = 0;

	if (!s_FatFSLoaded)
	{
		eve_printf_debug("SD card not ready\n");
		return file_size;
	}

	fResult = f_open(&InfSrc, filename, FA_READ | FA_OPEN_EXISTING);
	if (fResult == FR_DISK_ERR)
	{
		eve_printf_debug("Re-mount SD card\n");
		s_FatFSLoaded = false;
		sdhost_init();
		EVE_Util_loadSdCard(phost);
		fResult = f_open(&InfSrc, filename, FA_READ | FA_OPEN_EXISTING);
	}
	if (fResult == FR_OK)
	{
		filesize = f_size(&InfSrc);
		file_size = filesize;
		while (filesize > 0)
		{
			fResult = f_read(&InfSrc, buffer, 512, &blocklen); // read a chunk of src file
			filesize -= blocklen;
			EVE_Hal_wrMem(phost, addr, buffer, blocklen);
			addr += blocklen;
		}
		f_close(&InfSrc);
		return file_size;
	}
	else
	{
		eve_printf_debug("Unable to open file: \"%s\"\n", filename);
		return file_size;
	}
#else
	eve_printf_debug("No filesystem support, cannot open: \"%s\"\n", filename);
	return file_size;
#endif
}

/**
 * @brief Load file into RAM_G by CMD_INFLATE
 *
 * @param phost  Pointer to Hal context
 * @param address Address to write
 * @param filename File to load
 * @return true True if ok
 * @return false False if error
 */
bool EVE_Util_loadInflateFile(EVE_HalContext *phost, uint32_t address, const char *filename)
{
#if EVE_ENABLE_FATFS
	FRESULT fResult;
	FIL InfSrc;

	UINT blocklen;
	int32_t filesize;
	uint8_t buffer[512L];

	if (!s_FatFSLoaded)
	{
		eve_printf_debug("SD card not ready\n");
		return false;
	}

	fResult = f_open(&InfSrc, filename, FA_READ | FA_OPEN_EXISTING);
	if (fResult == FR_DISK_ERR)
	{
		eve_printf_debug("Re-mount SD card\n");
		s_FatFSLoaded = false;
		sdhost_init();
		EVE_Util_loadSdCard(phost);
		fResult = f_open(&InfSrc, filename, FA_READ | FA_OPEN_EXISTING);
	}
	if (fResult == FR_OK)
	{
		EVE_Cmd_wr32(phost, CMD_INFLATE);
		EVE_Cmd_wr32(phost, address);
		EVE_Cmd_wr32(phost, 0); // options
		filesize = f_size(&InfSrc);
		while (filesize > 0)
		{
			fResult = f_read(&InfSrc, buffer, 512, &blocklen); // read a chunk of src file
			filesize -= blocklen;
			blocklen += 3;
			blocklen &= ~3U;
			if (!EVE_Cmd_wrMem(phost, (uint8_t *)buffer, blocklen))
				break;
		}
		f_close(&InfSrc);
		return EVE_Cmd_waitFlush(phost);
	}
	else
	{
		eve_printf_debug("Unable to open file: \"%s\"\n", filename);
		return false;
	}
#else
	eve_printf_debug("No filesystem support, cannot open: \"%s\"\n", filename);
	return false;
#endif
}

/**
 * @brief Load image into RAM_G
 *
 * @param phost  Pointer to Hal context
 * @param address Address in RAM_G
 * @param filename File to load
 * @param format Target format of image
 * @return true True if ok
 * @return false False if error
 */
bool EVE_Util_loadImageFile(EVE_HalContext *phost, uint32_t address, const char *filename, uint32_t *format, uint32_t options)
{
#if EVE_ENABLE_FATFS
	FRESULT fResult;
	FIL InfSrc;

	UINT blocklen;
	int32_t filesize;
	uint8_t buffer[512L];

	if (!s_FatFSLoaded)
	{
		eve_printf_debug("SD card not ready\n");
		return false;
	}

	if (phost->CmdFault)
		return false;

	fResult = f_open(&InfSrc, filename, FA_READ | FA_OPEN_EXISTING);
	if (fResult == FR_DISK_ERR)
	{
		eve_printf_debug("Re-mount SD card\n");
		s_FatFSLoaded = false;
		sdhost_init();
		EVE_Util_loadSdCard(phost);
		fResult = f_open(&InfSrc, filename, FA_READ | FA_OPEN_EXISTING);
	}
	if (fResult == FR_OK)
	{
		EVE_Cmd_wr32(phost, CMD_LOADIMAGE);
		EVE_Cmd_wr32(phost, address);
		EVE_Cmd_wr32(phost, options);
		filesize = f_size(&InfSrc);
		while (filesize > 0)
		{
			fResult = f_read(&InfSrc, buffer, 512, &blocklen); // read a chunk of src file
			filesize -= blocklen;
			blocklen += 3;
			blocklen &= ~3U;
			if (!EVE_Cmd_wrMem(phost, (uint8_t *)buffer, blocklen))
				break;
		}
		f_close(&InfSrc);

		if (!EVE_Cmd_waitFlush(phost))
			return false;

		if (format)
			*format = EVE_Hal_rd32(phost, 0x3097e8);

		return true;
	}
	else
	{
		eve_printf_debug("Unable to open file: \"%s\"\n", filename);
		return false;
	}
#else
	eve_printf_debug("No filesystem support, cannot open: \"%s\"\n", filename);
	return false;
#endif
}

/**
 * @brief Load a file into the coprocessor FIFO
 * 
 * @param phost Pointer to Hal context
 * @param filename File to load
 * @param transfered transfered length
 * @return true True if ok
 * @return false False if error
 */
bool EVE_Util_loadCmdFile(EVE_HalContext *phost, const char *filename, uint32_t *transfered)
{
#if EVE_ENABLE_FATFS
	FRESULT fResult;
	FIL InfSrc;

	UINT blocklen;
	int32_t filesize;
	uint8_t buffer[512L];

	if (!s_FatFSLoaded)
	{
		eve_printf_debug("SD card not ready\n");
		return false;
	}

	fResult = f_open(&InfSrc, filename, FA_READ | FA_OPEN_EXISTING);
	if (fResult == FR_DISK_ERR)
	{
		eve_printf_debug("Re-mount SD card\n");
		s_FatFSLoaded = false;
		sdhost_init();
		EVE_Util_loadSdCard(phost);
		fResult = f_open(&InfSrc, filename, FA_READ | FA_OPEN_EXISTING);
	}
	if (fResult == FR_OK)
	{
		filesize = f_size(&InfSrc);
		while (filesize > 0)
		{
			fResult = f_read(&InfSrc, buffer, 512, &blocklen); // read a chunk of src file
			filesize -= blocklen;
			blocklen += 3;
			blocklen &= ~3U;
			if (!EVE_Cmd_wrMem(phost, (uint8_t *)buffer, blocklen))
				break;
			if (transfered)
				*transfered += blocklen;
		}
		f_close(&InfSrc);
		return EVE_Cmd_waitFlush(phost);
	}
	else
	{
		eve_printf_debug("Unable to open file: \"%s\"\n", filename);
		return false;
	}
#else
	eve_printf_debug("No filesystem support, cannot open: \"%s\"\n", filename);
	return false;
#endif
}

/**
 * @brief Read a file into a buffer, returns the number of bytes read
 * 
 * @param phost Pointer to Hal context
 * @param buffer Read buffer
 * @param size Read size
 * @param filename File to read
 * @return read length
 */
size_t EVE_Util_readFile(EVE_HalContext *phost, uint8_t *buffer, size_t size, const char *filename)
{
	// Read up to `size` number of bytes from the file into `buffer`, then return the number of read bytes
#if EVE_ENABLE_FATFS
	FRESULT fResult;
	FIL InfSrc;

	if (!s_FatFSLoaded)
	{
		eve_printf_debug("SD card not ready\n");
		return false;
	}

	fResult = f_open(&InfSrc, filename, FA_READ | FA_OPEN_EXISTING);
	if (fResult == FR_DISK_ERR)
	{
		eve_printf_debug("Re-mount SD card\n");
		s_FatFSLoaded = false;
		sdhost_init();
		EVE_Util_loadSdCard(phost);
		fResult = f_open(&InfSrc, filename, FA_READ | FA_OPEN_EXISTING);
	}
	if (fResult == FR_OK)
	{
		size_t read;
		fResult = f_read(&InfSrc, buffer, size, &read);
		f_close(&InfSrc);
		return read;
	}
	else
	{
		eve_printf_debug("Unable to open file: \"%s\"\n", filename);
		return 0;
	}

#else
	eve_printf_debug("No filesystem support, cannot open: \"%s\"\n", filename);
	return 0;
#endif
}

/**
 * @brief Load a file into the media FIFO.
 * 
 * If transfered is set, the file may be streamed partially,
 * and will be kept open until EVE_Util_closeFile is called, 
 * and stop once the coprocessor has processed it.\n
 * Filename may be omitted in subsequent calls
 * 
 * @param phost Pointer to Hal context
 * @param filename File to load
 * @param transfered transfered length
 * @return true True if ok
 * @return false False if error
 */
bool EVE_Util_loadMediaFile(EVE_HalContext *phost, const char *filename, uint32_t *transfered)
{

#if EVE_ENABLE_FATFS && defined(EVE_SUPPORT_MEDIAFIFO)
	FRESULT fResult = FR_INVALID_OBJECT;

	UINT blocklen;
	int32_t filesize;
	uint32_t blockSize = min(512, ((phost->MediaFifoSize >> 3) << 2) - 4);
	uint8_t buffer[512L];

	if (!s_FatFSLoaded)
	{
		eve_printf_debug("SD card not ready\n");
		return false;
	}

	if (phost->CmdFault)
		return false;

	if (transfered && phost->LoadFileRemaining)
	{
		filesize = phost->LoadFileRemaining;
	}
	else
	{
		if (!transfered)
		{
			EVE_Util_closeFile(phost);
		}
		fResult = f_open(&phost->LoadFileObj, filename, FA_READ | FA_OPEN_EXISTING);
		if (fResult == FR_DISK_ERR)
		{
			eve_printf_debug("Re-mount SD card\n");
			s_FatFSLoaded = false;
			sdhost_init();
			EVE_Util_loadSdCard(phost);
			fResult = f_open(&phost->LoadFileObj, filename, FA_READ | FA_OPEN_EXISTING);
		}
		if (fResult == FR_OK)
		{
			filesize = f_size(&phost->LoadFileObj);
			if (transfered)
			{
				phost->LoadFileRemaining = filesize;
			}
			if (filesize == 0)
			{
				/* Empty file, no-op */
				eve_printf_debug("Empty file: \"%s\"\n", filename);
				f_close(&phost->LoadFileObj);
				return true;
			}
		}
		else
		{
			eve_printf_debug("Unable to open file: \"%s\"\n", filename);
			return false;
		}
	}

	while (filesize > 0)
	{
		fResult = f_read(&phost->LoadFileObj, buffer, blockSize, &blocklen); // read a chunk of src file
		if (fResult != FR_OK)
		{
			if (fResult == FR_DISK_ERR)
			{
				eve_printf_debug("Lost SD card\n");
				s_FatFSLoaded = false;
				sdhost_init();
			}
			break;
		}

		filesize -= blocklen;
		blocklen += 3;
		blocklen &= ~3U;

		if (transfered)
		{
			uint32_t transferedPart = 0;
			bool wrRes = EVE_MediaFifo_wrMem(phost, buffer, blocklen, &transferedPart); /* copy data continuously into media fifo memory */
			*transfered += transferedPart;
			if (transferedPart < blocklen)
			{
				long offset = (long)transferedPart - (long)blocklen; /* Negative */
				f_lseek(&phost->LoadFileObj, f_tell(&phost->LoadFileObj) + offset); /* Seek back */
				filesize -= offset; /* Increments remaining (double negative) */
				break; /* Early exit, processing done */
			}
			if (!wrRes)
				break;
		}
		else
		{
			if (!EVE_MediaFifo_wrMem(phost, buffer, blocklen, NULL)) /* copy data continuously into media fifo memory */
				break; /* Coprocessor fault */
		}
	}

	if (!transfered)
	{
		f_close(&phost->LoadFileObj); /* Close the opened file */
	}
	else if (filesize)
	{
		phost->LoadFileRemaining = filesize; /* Save remaining */
	}
	else
	{
		f_close(&phost->LoadFileObj);
		phost->LoadFileRemaining = 0;
	}

	return (fResult == FR_OK) && (transfered ? EVE_Cmd_waitFlush(phost) : EVE_MediaFifo_waitFlush(phost, false));

#else
	eve_printf_debug("No filesystem support, cannot open: \"%s\"\n", filename);
	return false;
#endif
}

/**
 * @brief Load a file into RAM_G block by block, returns the number of bytes loaded at address
 * 
 * The file will be kept open until EVE_Util_closeFile is called \n
 * If transfered is set, can skip some data when first time to read
 * 
 * @param phost  Pointer to Hal context
 * @param address Address in RAM_G
 * @param size Load size
 * @param filename File to load
 * @param transfered transfered length
 * @return load length
 */
uint16_t EVE_Util_loadFile(EVE_HalContext *phost, uint32_t address, uint32_t size, const char *filename, uint32_t *transfered)
{
#if EVE_ENABLE_FATFS
	FRESULT fResult = FR_INVALID_OBJECT;

	UINT blocklen;
	int32_t filesize;
	uint16_t blockSize = (uint16_t)size;
	uint16_t loaded = 0;
	uint32_t remaining = 0;
	uint8_t buffer[512L];

	if (!s_FatFSLoaded)
	{
		eve_printf_debug("SD card not ready\n");
		return false;
	}

	if (phost->CmdFault)
		return false;

	if (transfered && phost->LoadFileRemaining)
	{
		remaining = phost->LoadFileRemaining;
	}
	else
	{
		if (!transfered)
		{
			EVE_Util_closeFile(phost);
		}
		fResult = f_open(&phost->LoadFileObj, filename, FA_READ | FA_OPEN_EXISTING);
		if (fResult == FR_DISK_ERR)
		{
			eve_printf_debug("Re-mount SD card\n");
			s_FatFSLoaded = false;
			sdhost_init();
			EVE_Util_loadSdCard(phost);
			fResult = f_open(&phost->LoadFileObj, filename, FA_READ | FA_OPEN_EXISTING);
		}
		if (fResult == FR_OK)
		{
			filesize = f_size(&phost->LoadFileObj);
			remaining = filesize;
			if (filesize == 0)
			{
				/* Empty file, no-op */
				eve_printf_debug("Empty file: \"%s\"\n", filename);
				f_close(&phost->LoadFileObj);
				return true;
			}
		}
		else
		{
			eve_printf_debug("Unable to open file: \"%s\"\n", filename);
			return false;
		}
	}

	if (!phost->LoadFileRemaining) // first time open, skip '*transfered' size
	{
		while (transfered && (*transfered > 0))
		{
			fResult = f_read(&phost->LoadFileObj, buffer, min(*transfered, 512), &blocklen); // read a chunk of src file
			if (fResult != FR_OK)
			{
				if (fResult == FR_DISK_ERR)
				{
					eve_printf_debug("Lost SD card\n");
					s_FatFSLoaded = false;
					sdhost_init();
				}
				break;
			}
			*transfered -= blocklen;
			remaining -= blocklen;
		}
	}
	blockSize = min(blockSize, remaining);
	while (blockSize > 0)
	{
		fResult = f_read(&phost->LoadFileObj, buffer, min(blockSize, 512), &blocklen); // read a chunk of src file
		if (fResult != FR_OK)
		{
			if (fResult == FR_DISK_ERR)
			{
				eve_printf_debug("Lost SD card\n");
				s_FatFSLoaded = false;
				sdhost_init();
			}
			break;
		}

		blockSize -= blocklen;
		remaining -= blocklen;
		loaded += blocklen;
		EVE_Hal_wrMem(phost, address, buffer, blocklen);
		EVE_Cmd_waitFlush(phost);
	}

	if (remaining)
	{
		phost->LoadFileRemaining = remaining; /* Save remaining */
	}
	else
	{
		f_close(&phost->LoadFileObj);
		phost->LoadFileRemaining = 0;
	}

	return loaded;

#else
	eve_printf_debug("No filesystem support, cannot open: \"%s\"\n", filename);
	return 0;
#endif
}

/**
 * @brief Close opened file.
 * 
 * @param phost  Pointer to Hal context
 */
void EVE_Util_closeFile(EVE_HalContext *phost)
{
#if defined(EVE_SUPPORT_MEDIAFIFO)
	if (phost->LoadFileRemaining)
	{
#if EVE_ENABLE_FATFS
		f_close(&phost->LoadFileObj);
#endif
		phost->LoadFileRemaining = 0;
	}
#endif
}

#endif

/* end of file */
