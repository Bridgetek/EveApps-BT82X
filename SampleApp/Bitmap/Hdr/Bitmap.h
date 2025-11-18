#ifndef APP_H_
#define APP_H_

#include "Common.h"

// Path to UI assets Folder
#if defined(MSVC_PLATFORM)
#define TEST_DIR                            "..\\..\\..\\Test\\"
#else
#define TEST_DIR                            "/EveApps/SampleApp/Bitmap/Test/"
#endif
/************** flash map******************
 * mandrill256.jpg : 4096   : 14368
 ******************************************/
#define FLASH_FILE                          TEST_DIR "/Flash/flash.bin"
#define BITMAP_FLASH_ADDR                   4096

// assets in SD card
#define BITMAP_SD                           "pic_in_SD.jpg"

// assets information list
#define BITMAP_SUNFLOWER                    "sunflower.png"
#define BITMAP_SUNFLOWER_W                  800
#define BITMAP_SUNFLOWER_H                  600
#define BITMAP_DITHER                       "loadimage-dither-testcase.png"
#define BITMAP_DITHER_W                     800
#define BITMAP_DITHER_H                     480
#define BITMAP_ASTC_CELL                    "numbers_astc12x10.raw"
#define BITMAP_ASTC_CELL_W                  12
#define BITMAP_ASTC_CELL_H                  10
#define BITMAP_ASTC_FU                      "fu_168x960_ASTC_4X4.raw"
#define BITMAP_ASTC_FU_W                    168
#define BITMAP_ASTC_FU_H                    960
#define BITMAP_MANDRILL                     "mandrill256.jpg"
#define BITMAP_MANDRILL_LA1                 "mandrill256_256x256_LA1.raw"
#define BITMAP_MANDRILL_LA2                 "mandrill256_256x256_LA2.raw"
#define BITMAP_MANDRILL_LA4                 "mandrill256_256x256_LA4.raw"
#define BITMAP_MANDRILL_LA8                 "mandrill256_256x256_LA8.raw"
#define BITMAP_MANDRILL_RGB6                "mandrill256_256x256_RGB6.raw"
#define BITMAP_MANDRILL_RGB8                "mandrill256_256x256_RGB8.raw"
#define BITMAP_MANDRILL_YCBCR               "mandrill256_256x256_YCBCR.raw"
#define BITMAP_MANDRILL_W                   256
#define BITMAP_MANDRILL_H                   256
#define BITMAP_CASE_ARGB6                   "case_180x180_ARGB6.raw"
#define BITMAP_CASE_ARGB8                   "case_180x180_ARGB8.raw"
#define BITMAP_CASE_W                       180
#define BITMAP_CASE_H                       180
#define BITMAP_CASE_P_ARGB8_INDEX           "case_181x185_PALETTEDARGB8.index.raw"
#define BITMAP_CASE_P_ARGB8_LUT             "case_181x185_PALETTEDARGB8.lut.raw"
#define BITMAP_CASE_P_W                     181
#define BITMAP_CASE_P_H                     185
#define BITMAP_CASE_P_LUT_SZ                1024

#define GET_CALIBRATION                     1

#endif /* APP_H_ */
