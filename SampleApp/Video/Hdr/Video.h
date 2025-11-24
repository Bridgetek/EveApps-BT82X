/**
 * @file Video.h
 * @brief Sample usage of videos
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

#ifndef VIDEO_H_
#define VIDEO_H_

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

#endif /* VIDEO_H_ */
