/**
 * @file Utility.h
 * @brief Sample usage of utility
 *
 * @author Bridgetek
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

#ifndef UITL_H_
#define UITL_H_

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
#define UTIL_RAW_MANDRILL                   "mandrill256_256x256_RGB8.raw"
#define UTIL_RAW_ZORDER_RGB8                "mandrill256_256x256_RGB8_z_0xaa.raw"
#define UTIL_RAW_CASE                       "case_184x192_ASTC_8X8.raw"
#define UTIL_RAW_ZORDER_ASTC                "case_184x192_ASTC_8X8_z_0xaa.raw"
#define UTIL_BITMAP_CASE_W                  184
#define UTIL_BITMAP_CASE_H                  192

#define GET_CALIBRATION                     1

/* sample app structure definitions */
typedef struct SAMAPP_ColorRGB {
    int r;
    int g;
    int b;
}SAMAPP_ColorRGB;

typedef struct SAMAPP_Circle {
    int visible;
    int x;
    int y;
    int radius;
    SAMAPP_ColorRGB color;
    int opacity; // 0 to 256
    int step;
}SAMAPP_Circle;

#endif /* UITL_H_ */
