#ifndef APP_H_
#define APP_H_

#include "EVE_Platform.h"

// Path to UI assets Folder
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
#define TEST_DIR                            "..\\..\\..\\Test\\"
#else
#define TEST_DIR                            "/EveApps/SampleApp/Bedside_Monitor/Test"
#endif

#define GET_CALIBRATION                     1

#define GRAPH_W 160
#define GRAPH_H 1000
#define GRAPH_SIZE (GRAPH_W * GRAPH_H)

#define CHAR_BIT 8
#define BITS_IN_TYPE(type) (sizeof(type) * CHAR_BIT)
#define BIT_PER_CHAR (BITS_IN_TYPE(char))

#define MSG printf
#define SIGNALS_DATA_TYPE unsigned char 

extern int g_graph_zoom_lv;
#define GRAPH_ZOOM_LV_MAX 8

#define TAG_ZOOM_UP 1
#define TAG_ZOOM_DOWN 2

#endif /* APP_H_ */
