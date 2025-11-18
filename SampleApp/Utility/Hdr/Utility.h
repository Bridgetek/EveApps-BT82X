#ifndef APP_H_
#define APP_H_

#include "Common.h"

// Path to UI assets Folder
#if defined(MSVC_PLATFORM)
#define TEST_DIR                            "..\\..\\..\\Test\\"
#else
#define TEST_DIR                            "/EveApps/SampleApp/Utility/Test/"
#endif
/************** flash map ******************
 * mandrill256_256x256_YCBCR.zlib : 4096   : 59304
 ******************************************/
#define FLASH_FILE                          TEST_DIR "/Flash/flash.bin"
#define UTIL_ADDR_FLASH_MANDRILL            4096

// assets information list
#define UTIL_JPG_FLOWER                     "flower_800x480.jpg"
#define UTIL_JPG_FLOWER_W                   800
#define UTIL_JPG_FLOWER_H                   480
#define UTIL_JPG_MANDRILL                   "mandrill256.jpg"
#define UTIL_JPG_MANDRILL_W                 256
#define UTIL_JPG_MANDRILL_H                 256
#define UTIL_JPG_MANDRILL_INFLATE           "mandrill256_256x256_YCBCR.zlib"
#define UTIL_PNG_UNSUPPORT                  "lenaface40_unsupported.png"
#define UTIL_PNG_CORRUPT                    "lenaface40_corrupted.png"
#define UTIL_PNG_FAILED_W                   128
#define UTIL_PNG_FAILED_H                   128
#define UTIL_WAV                            "perfect_beauty.wav"

#define GET_CALIBRATION                     1

/* sample app structure definitions */
typedef struct SAMAPP_ColorRGB {
    int r;
    int g;
    int b;
}SAMAPP_ColorRGB_t;

typedef struct SAMAPP_Circle {
    int visible;
    int x;
    int y;
    int radius;
    SAMAPP_ColorRGB_t color;
    int opacity; // 0 to 256
    int step;
}SAMAPP_Circle_t;

#endif /* APP_H_ */
