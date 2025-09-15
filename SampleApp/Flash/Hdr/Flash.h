#ifndef APP_H_
#define APP_H_

#include "Common.h"

// Path to UI assets Folder
#if defined(MSVC_PLATFORM)
#define TEST_DIR                            "..\\..\\..\\Test\\"
#else
#define TEST_DIR                            "/EveApps/SampleApp/Flash/Test"
#endif

#define GET_CALIBRATION                     1

typedef struct Flash_Progress
{
	char fileName[10];
	char message[100];
	char stage;
	uint32_t fileSize;
} Flash_Progress_t;

typedef enum
{
	COPY_FROM_SD, /**< 0 */
	ERASE_FLASH, /**< 1 */
	COPY_TO_FLASH, /**< 2 */
	VERIFY, /**< 3 */

	LAST_STAGE = 4, /**< 4 */
} Flash_stage;

#endif /* APP_H_ */
