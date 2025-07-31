#ifndef APP_H_
#define APP_H_

// Path to UI assets Folder
#define TEST_DIR                            "..\\..\\..\\Test\\"
#define GET_CALIBRATION                     1

#define ALIGN_TWO_POWER_N(Value, alignval)  (((Value) + (alignval - 1)) & (~(alignval - 1)))
#define random(x)                           (rand() % (x))

#endif /* APP_H_ */
