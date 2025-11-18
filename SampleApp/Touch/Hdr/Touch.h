#ifndef APP_H_
#define APP_H_

#include "Common.h"

// Path to UI assets Folder
#if defined(MSVC_PLATFORM)
#define TEST_DIR                            "..\\..\\..\\Test\\"
#else
#define TEST_DIR                            "/EveApps/SampleApp/Touch/Test/"
#endif

//assets information list
#define TOUCH_BITMAP                        "yellow_66x66_YCBCR.raw"
#define TOUCH_BITMAP_W                      60
#define TOUCH_BITMAP_H                      60
#define TOUCH_AUDIO                         "Devil_Ride_30_44100_ulaw.raw"
#define TOUCH_AUDIO_FORMAT                  ULAW_SAMPLES
#define TOUCH_AUDIO_FREQ                    44100
#define TOUCH_AUDIO_LENGTH                  1324958

#define GET_CALIBRATION                     1

#define NO_OF_TOUCH                         (5)
#define OFFSCREEN                           0x8000

typedef struct SAMAPP_Squares {
	uint16_t x; /**< X-coordinate */
	uint16_t y; /**< Y-coordinate */
}SAMAPP_Squares_t;

//bouncing squares
#define NO_OF_RECTS (5)
typedef struct SAMAPP_BouncingSquares {
	int16_t Ty[NO_OF_RECTS]; /**< Y-coordinate for touch point */
	int16_t My[NO_OF_RECTS]; /**< Y-coordinate for moving rectangle */
	uint8_t Decrease[NO_OF_RECTS]; /**< flag for decrease/increase */
	uint8_t RectTouched[NO_OF_RECTS]; /**< Touch flag */
}SAMAPP_BouncingSquares_t;

//Bouncing Circle
#define NO_OF_CIRCLE                        (5)
typedef struct SAMAPP_BouncingCircles {
	float R[NO_OF_CIRCLE]; /**< circle radius */
	float CX[NO_OF_CIRCLE]; /**< X-coordinate for small circle (touch point) */
    float CY[NO_OF_CIRCLE]; /**< Y-coordinate for small circle (touch point) */
	uint8_t TN[NO_OF_CIRCLE]; /**< touch number */
}SAMAPP_BouncingCircles_t;

//bouncing points structures
#define NBLOBS                              (64)
typedef struct SAMAPP_Blobs {
    uint16_t x; /**< X-coordinate */
    uint16_t y; /**< Y-coordinate */
}SAMAPP_Blobs_t;
typedef struct SAMAPP_BlobsInst {
	SAMAPP_Blobs_t blobs[NBLOBS]; /**< location of points */
	uint8_t CurrIdx; /**< current index */
}SAMAPP_BlobsInst_t;


//moving points structures
#define NO_OF_POINTS                        (64)
typedef struct SAMAPP_MovingPoints {
	uint16_t StopX[NO_OF_TOUCH]; /**< The x-coordinate of the moving points is no longer updating */
	uint16_t StopY; /**< The y-coordinate of the moving points is no longer updating */
	uint32_t val[NO_OF_TOUCH]; /**< Touch location value */
	uint16_t X[(NO_OF_POINTS) * (NO_OF_TOUCH)]; /**< The x-coordinate of the moving points */
	uint16_t Y[(NO_OF_POINTS) * (NO_OF_TOUCH)]; /**< The y-coordinate of the moving points */
	uint8_t t[(NO_OF_POINTS) * (NO_OF_TOUCH)]; /**< point number */
}SAMAPP_MovingPoints_t;

//multitracker
#define NO_OF_TRACKER                       4


#endif /* APP_H_ */
