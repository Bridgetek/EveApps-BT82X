/**
 * @file FileTransfer.c
 * @brief File transfer interface from host to flash
 *
 * @author
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

#include "Common.h"
#include "FlashHelper.h"

#define FREAD_BLOCK           (8 * 1024)

#define BLOBSIZE              4096

#if defined(MSVC_PLATFORM)
#define EVE_FLASH_DIR         __FILE__ "\\..\\..\\..\\common\\eve_flash"
#else
#define EVE_FLASH_DIR         "/Test/common/eve_flash"
#endif

#define FILE_BLOB (EVE_FLASH_DIR "\\bt82x.blob")

#define FTF_PROGESS_READ 1
#define FTF_PROGESS_WRITE 2
#define MSG_SIZE 200

typedef struct Ftf_Progress
{
	char file[200];
	char fileName[200];
	char message[MSG_SIZE];
	uint32_t fileSize;
	uint32_t sent;
	uint32_t bytesPerPercent;
	uint32_t addr;
	uint8_t direction;
} Ftf_Progress_t;

/**
 * @brief Write blob file to flash
 * When updating a flash image, the following steps are to be expected:
 *   - Reading the first 4096 bytes in flash image into RAM_G and update the blob first
 *       using cmd_flashupdate in Basic mode.
 *   - Send the command cmd_flashfast to enter into fast mode.
 *   - If success, program the rest data of flash image by cmd_flashupdate
 *   - If not success, write a default blob.
 *
 * @param phost Pointer to Hal context
 * @param pbuff data buffer
 * @return uint32_t 1 on successful, 0 on error
 */
uint32_t Ftf_Update_Blob(EVE_HalContext* phost, const char* pbuff) 
{
	eve_printf("Updating blob\n");
	char buf[BLOBSIZE] = { 0 };

	FlashHelper_SwitchState(phost, FLASH_STATUS_BASIC); // basic mode
	EVE_Hal_wrMem(phost, RAM_G, pbuff, BLOBSIZE);
	printf("REMG: 0x%lx\n", EVE_Hal_rd32(phost, RAM_G));
	EVE_Cmd_waitFlush(phost);
	FlashHelper_Update(phost, 0, RAM_G, BLOBSIZE);
	EVE_Cmd_waitFlush(phost);
	FlashHelper_Read(phost, 4096, 0, BLOBSIZE, buf);
	printf("REMG+4096: 0x%lx, buf: 0x%lx\n", EVE_Hal_rd32(phost, 4096), buf[0]);

	int ret = FlashHelper_SwitchFullMode(phost);

	if (ret == 1) {
		eve_printf("Blob updated successful\n");
		return 1;
	}
	else {
		eve_printf("Failed to update Blob\n");
	}

	return 0;
}

/**
 * @brief Write default blob file to flash
 *
 * @param phost Pointer to Hal context
 * @return uint32_t 1 on successful, 0 on error
 */
uint32_t Ftf_Write_Blob_Default(EVE_HalContext *phost)
{
	char pBuff[BLOBSIZE];
	eve_printf("Writing blob default\n");

	EVE_Util_readFile(phost, pBuff, BLOBSIZE, FILE_BLOB);

	return Ftf_Update_Blob(phost, pBuff);
}

/**
 * @brief Write blob file to flash
 *
 * @param phost Pointer to Hal context
 * @param blobfile Blob file address
 * @return uint32_t 1 on successful, 0 on error
 */
uint32_t Ftf_Write_BlobFile(EVE_HalContext* phost, const char* blobfile)
{
	char pBuff[BLOBSIZE];

	eve_printf("Writing blob from file %s\n", blobfile);

	EVE_Util_readFile(phost, pBuff, BLOBSIZE, blobfile);

	int ret = Ftf_Update_Blob(phost, pBuff);

	/// fail to default Blob
	if (!ret) {
		return Ftf_Write_Blob_Default(phost);
	}

	return 1;
}

/**
 * @brief File transfer progress bar initialization
 *
 * @param phost Pointer to Hal context
 * @param filePath Path to the file
 * @param fileName Filename on Progress bar
 * @param addr Address on flash
 * @param direction FTF_PROGESS_READ or FTF_PROGESS_WRITE
 * @return Ftf_Progress_t*
 */
Ftf_Progress_t* Ftf_Progress_Init(EVE_HalContext* phost, const char* filePath, const char* fileName, uint32_t addr, uint8_t direction)
{
	static Ftf_Progress_t progress;
	uint32_t range = 0;
	int32_t fileSize = 0;

	progress.sent = 0;
	progress.addr = addr;
	progress.direction = direction;
#pragma warning(push)
#pragma warning(disable : 4996)
	strcpy(progress.file, filePath);
	strcpy(progress.fileName, fileName);
#pragma warning(pop)

	if (direction == FTF_PROGESS_READ) {
		snprintf(progress.message, MSG_SIZE, "Reading %s from flash", progress.fileName);
	}
	else {
#ifdef EVE_FLASH_NAND
		snprintf(progress.message, MSG_SIZE, "Writing %s to flash", progress.fileName);
		FlashHelper_Erase(phost);
		progress.fileSize = EVE_Util_loadRawFile(phost, RAM_G, filePath);
		progress.sent = progress.fileSize / 2; // assume half is done
#else
		// update blob from file first
		if (addr == 0) {
			Ftf_Write_BlobFile(phost, filePath);
		}
		else {
			Ftf_Write_Blob_Default(phost);
		}
		
		// Jump to real data
		if (addr == 0) {
			progress.sent = progress.addr = (uint32_t)EVE_Util_loadFile(phost, RAM_G, BLOBSIZE, filePath, progress.sent);
		}

		snprintf(progress.message, MSG_SIZE, "Writing %s to flash", progress.fileName);
		progress.fileSize = phost->LoadFileRemaining;

#endif
	}

#ifndef EVE_FLASH_NAND
	/// Destination address in flash memory must be 4096-byte aligned
	progress.bytesPerPercent = ALIGN(progress.fileSize / 100, 4096);

	if (progress.bytesPerPercent < FREAD_BLOCK) {
		progress.bytesPerPercent = FREAD_BLOCK;
	}
#endif

	return &progress;
}

/**
 * @brief Write a block data of file to flash
 *
 * @param phost Pointer to Hal context
 * @param progress Ftf_Progress_t struct
 * @return uint32_t Percent of data transfered, 100 mean file transfer is done
 */
uint32_t Ftf_Progress_Write_Next(EVE_HalContext* phost, Ftf_Progress_t* progress)
{
	uint32_t bytes;
	uint32_t sent = 0;
	uint32_t ramGSent = 0;
	uint32_t blockSize = 0;

	// Tranfer 1 percent of file
	blockSize = FREAD_BLOCK > progress->bytesPerPercent ? progress->bytesPerPercent : FREAD_BLOCK;

	// Tranfer to ram_g
	while (progress->sent < progress->fileSize && sent < progress->bytesPerPercent) {
		bytes = EVE_Util_loadFile(phost, ramGSent, blockSize, progress->file, NULL);
		ramGSent += bytes;
		sent += bytes;
		progress->sent += bytes;
	}

	// Update flash from ram_g
	ramGSent = (ramGSent + 4095) & (~4095);//to ensure 4KB alignment
	FlashHelper_Update(phost, progress->addr, 0, ramGSent);
	progress->addr += ramGSent;

	return progress->sent * 100 / progress->fileSize; /* Percent */
}


/**
 * @brief Default UI for the progress bar
 * User may construct their own UI for the progress bar with Ftf_Progress_Init and Ftf_Progress_Write_Next
 *
 * @param phost Pointer to Hal context
 * @param progress Ftf_Progress_t struct
 * @return uint32_t 1 on successful, 0 on error
 */
uint32_t Ftf_Progress_Ui(EVE_HalContext* phost, const Ftf_Progress_t* progress)
{
	char s[100];
	uint16_t x;
	uint16_t y;
	uint16_t w;
	uint16_t h;
	uint16_t opt;
	uint16_t font = 30;
	uint16_t val;
	const uint32_t range = 1000;
	uint64_t sent64 = progress->sent;

	if (progress->fileSize == 0) {
		return 1;
	}
	opt = 0;
	val = (uint16_t)(sent64 * 1000 / progress->fileSize);

	w = (uint16_t)(phost->Width * 8 / 10);
	h = (uint16_t)(phost->Height * 1 / 10);
	x = (uint16_t)((phost->Width - w) / 2);
	y = (uint16_t)((phost->Height - h) / 2);

	Display_StartColor(phost, (uint8_t[]) { 64, 64, 64 }, (uint8_t[]) { 255, 255, 255 });
	EVE_CoCmd_text(phost, x, y - 50, font, opt, progress->message);
	EVE_CoCmd_progress(phost, x, y, w, h, opt, val, range);

	snprintf(s, 100, "%u %%", val * 100 / range);
	EVE_CoDl_colorRgb(phost, 0, 200, 0);
	EVE_CoCmd_text(phost, x + w / 2, y + 5, font, opt, s);
	EVE_CoDl_colorRgb(phost, 255, 255, 255);

	Display_End(phost);

	return 1;
}

/**
 * @brief Write file to flash and show default progress bar on LCD
 *
 * @param phost Pointer to Hal context
 * @param filePath File to transfer
 * @param fileName File name on the progress bar
 * @param address Address on flash
 * @return uint32_t Number of bytes transfered
 */
uint32_t Ftf_Write_File_To_Flash_With_Progressbar(EVE_HalContext* phost, const char* filePath, const char* fileName, uint32_t address) 
{
#ifdef EVE_FLASH_NAND
	// display a start loading page
	Ftf_Progress_t progress_init;
	progress_init.sent = 0;
	progress_init.fileSize = 1;
	snprintf(progress_init.message, MSG_SIZE, "Writing %s to flash", fileName);
	Ftf_Progress_Ui(phost, &progress_init);
#endif
	Ftf_Progress_t* progress = Ftf_Progress_Init(phost, filePath, fileName, address, FTF_PROGESS_WRITE);

	if (!progress) {
		return -1; /// error
	}

#ifdef EVE_FLASH_NAND
	Ftf_Progress_Ui(phost, progress);
	EVE_sleep(1000);
	FlashHelper_Program(phost, 0, RAM_G, progress->fileSize);
	progress->sent = progress->fileSize;
	Ftf_Progress_Ui(phost, progress);
	EVE_sleep(1000);
#else
	while (1)
	{
		uint32_t pc = Ftf_Progress_Write_Next(phost, progress);
		Ftf_Progress_Ui(phost, progress);

		if (pc >= 100) {
			break;
		}
	}
#endif

	return progress->fileSize;
}


