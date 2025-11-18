#ifndef APP_H_
#define APP_H_

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

#endif /* APP_H_ */
