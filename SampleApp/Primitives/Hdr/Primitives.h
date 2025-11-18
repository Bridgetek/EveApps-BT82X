#ifndef APP_H_
#define APP_H_

#include "Common.h"

// Path to UI assets Folder
#if defined(MSVC_PLATFORM)
#define TEST_DIR                            "..\\..\\..\\Test\\"
#else
#define TEST_DIR                            "/EveApps/SampleApp/Primitives/Test/"
#endif

//assets information list
#define PRIM_BITMAP                         "mandrill256.jpg"
#define PRIM_BITMAP_W                       256
#define PRIM_BITMAP_H                       256
#define PRIM_BITMAP_CAL                     "lavender-1920x1200.jpg"

#define GET_CALIBRATION                     1

#define ALIGN_TWO_POWER_N(Value, alignval)  (((Value) + (alignval - 1)) & (~(alignval - 1)))
#define random(x)                           (rand() % (x))

#endif /* APP_H_ */
