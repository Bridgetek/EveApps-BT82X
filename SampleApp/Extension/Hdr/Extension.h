#ifndef APP_H_
#define APP_H_

#include "Common.h"

// Path to UI assets Folder
#if defined(MSVC_PLATFORM)
#define TEST_DIR "..\\..\\..\\Test\\"
#else
#define TEST_DIR "/EveApps/SampleApp/Extension/Test"
#endif

#define GET_CALIBRATION                     1

#endif /* APP_H_ */
