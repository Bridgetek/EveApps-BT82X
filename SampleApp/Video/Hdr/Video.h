#ifndef APP_H_
#define APP_H_

#include "Common.h"

// Path to UI assets Folder
#if defined(MSVC_PLATFORM)
#define TEST_DIR                            "..\\..\\..\\Test\\"
#else
#define TEST_DIR                            "/EveApps/SampleApp/Video/Test/"
#endif
/************** flash map ******************
 * flowers_1024x600.avi : 4096   : 4767240
 ******************************************/
#define FLASH_FILE                          TEST_DIR "/Flash/flash.bin"
#define VIDEO_FLASH_ADDR_FLOWER             4096

// assets information list
#define VIDEO_FLOWERS_1024                  "flowers_1024x600.avi"
#define VIDEO_FLOWERS_1024_W                1024
#define VIDEO_FLOWERS_1024_H                600
#define VIDEO_FLOWERS_1920                  "flowers_1920x1200.avi"
#define VIDEO_PNG_LOGO                      "images-logo.png"
#define VIDEO_PNG_LOGO_W                    236
#define VIDEO_PNG_LOGO_H                    72

// assets in SD card
#define VIDEO_JPG                           "pic_in_SD.jpg"
#define VIDEO_AVI                           "flowers_1024x600.avi"

#define GET_CALIBRATION                     1

#endif /* APP_H_ */
