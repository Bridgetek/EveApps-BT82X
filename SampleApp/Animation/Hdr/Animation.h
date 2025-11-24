/**
 * @file Animation.h
 * @brief Sample usage of animation
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

#ifndef ANIM_H_
#define ANIM_H_

#include "Common.h"

// Path to UI assets Folder
#if defined(MSVC_PLATFORM)
#define TEST_DIR                            "..\\..\\..\\Test\\"
#else
#define TEST_DIR                            "/EveApps/SampleApp/Animation/Test/"
#endif
/************** flash map******************
 * abstract.anim.reloc : 4096    : 1479808
 * bicycle.anim.raw    : 1483904 : 154220 
 ******************************************/
#define FLASH_FILE                          TEST_DIR "/Flash/flash.bin"
#define ANIM_ADDR_FLASH_ABSTRACT            4096
#define ANIM_ADDR_FLASH_BICYCLE             1483904
#define ANIM_LEN_FLASH_BICYCLE              154220

//assets information list
#define ANIM_ABSTRACT                       "abstract.anim.reloc"
#define ANIMATION_FRAMES                    40

#define GET_CALIBRATION                     1

#endif /* ANIM_H_ */
