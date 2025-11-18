#ifndef APP_H_
#define APP_H_

#include "Common.h"

// Path to UI assets Folder
#if defined(MSVC_PLATFORM)
#define TEST_DIR                            "..\\..\\..\\Test\\"
#else
#define TEST_DIR                            "/EveApps/SampleApp/Font/Test/"
#endif
/************** flash map******************
 * Roboto-BoldCondensed_30_L4.reloc                  : 4096    : 5864   
 * NotoSerifSC-VariableFont_wght_30_L2.reloc         : 9984    : 3094068
 * NotoSerifSC-VariableFont_wght_30_L1_ordinal.reloc : 3104064 : 1535888
 ******************************************/
#define FLASH_FILE                          TEST_DIR "/Flash/flash.bin"
#define FONT_FLASH_ROBOTO_L4_RELOC          4096
#define FONT_FLASH_NOTO_L2_RELOC            9984
#define FONT_FLASH_NOTO_L1_RELOC_ORDINAL    3104064

#define FONT_SD                             "Roboto-BoldCondensed_30_L4.reloc"

// assets information list
#define FONT_JPG                            "font16.jpg"
#define FONT_JPG_WIDTH                      16
#define FONT_JPG_FORMAT                     L8
#define FONT_JPG_FLAGS                      0
#define FONT_JPG_LINESTRIDE                 16
#define FONT_JPG_WIDTHINPIX                 16
#define FONT_JPG_HEIGHTINPIX                16
#define FONT_JPG_DATA                       1024 //data address - starts at location 1024
#define FONT_START_CHAR                     32
#define ROBOTO_L4_RAW                       "Roboto-BoldCondensed_30_L4.raw"
#define ROBOTO_L4_RELOC                     "Roboto-BoldCondensed_30_L4.reloc"
#define ROBOTO_L1_RAW                       "Roboto-BoldCondensed_30_L1.raw"
#define ROBOTO_L2_RAW                       "Roboto-BoldCondensed_30_L2.raw"

#define GET_CALIBRATION                     1

#endif /* APP_H_ */
