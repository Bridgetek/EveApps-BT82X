#ifndef APP_H_
#define APP_H_

#include "Common.h"

// Path to UI assets Folder
#if defined(MSVC_PLATFORM)
#define TEST_DIR                            "..\\..\\..\\Test\\"
#else
#define TEST_DIR                            "/EveApps/SampleApp/Audio/Test/"
#endif
/************** flash map******************
 * perfect_beauty.wav : 4096   : 960080
 ******************************************/
#define FLASH_FILE                          TEST_DIR "/Flash/flash.bin"
#define AUDIO_FLASH_ADDR                    4096

// assets information list
#define AUDIO_RAW_FILE                      "perfect_beauty.raw"
#define AUDIO_WAV_FREQ                      48000
#define AUDIO_WAV_LEN                       3840000
#define AUDIO_WAV_FILE                      "perfect_beauty.wav"

#define GET_CALIBRATION                     1

#endif /* APP_H_ */
