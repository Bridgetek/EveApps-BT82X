#if 1
/**
 * @file Bedside_Monitor.c
 * @brief A bedside monitoring demo application
 *
 * @author Bridgetek
 *
 * @date 2024
 *
 * MIT License
 *
 * Copyright (c) [2019] [Bridgetek Pte Ltd (BRTChip)]
 */

#include "Common.h"
#include "EVE_Platform.h"
#include "EVE_CoCmd.h"
#include "Bedside_Monitor.h"

static EVE_HalContext s_halContext;
static EVE_HalContext *s_pHalContext;
void SAMAPP_Bedside_Monitor();
#define SAMAPP_DELAY EVE_sleep(2000);
#define SCANOUT_FORMAT RGB8

int main(int argc, char *argv[])
{
	s_pHalContext = &s_halContext;
	Gpu_Init(s_pHalContext);
	LVDS_Config(s_pHalContext, SCANOUT_FORMAT, TESTCASE_PICTURE);

	// read and store calibration setting
#if !defined(BT8XXEMU_PLATFORM) && GET_CALIBRATION == 0
	EVE_Calibrate(s_pHalContext);
	Calibration_Save(s_pHalContext);
#endif

	EVE_Util_clearScreen(s_pHalContext);

	char *info[] = {"EVE Sample Application",
					"This sample demonstrate a Beside Monitoring",
					"",
					""};

	while (TRUE)
	{
		// WelcomeScreen(s_pHalContext, info);

		SAMAPP_Bedside_Monitor();

		EVE_Util_clearScreen(s_pHalContext);

		EVE_Hal_close(s_pHalContext);
		EVE_Hal_release();

		/* Init HW Hal for next loop*/
		Gpu_Init(s_pHalContext);
#if !defined(BT8XXEMU_PLATFORM) && GET_CALIBRATION == 1
		Calibration_Restore(s_pHalContext);
#endif
	}

	return 0;
}

/// The demo area //////////////////////////////////////////////////////////////////////////////////////
#define INIT_APP_BOX(x_val, y_val, w_val, h_val) \
	{                                            \
		(x_val),                                 \
		(y_val),                                 \
		(w_val),                                 \
		(h_val),                                 \
		(x_val) + (w_val),                       \
		(y_val) + (h_val),                       \
		(x_val) + (w_val) / 2,                   \
		(y_val) + (h_val) / 2}

#define DRAW_RECT(x, y, w, h)                      \
	{                                              \
		EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS)); \
		EVE_DRAW_AT(x, y);                         \
		EVE_DRAW_AT(x + w, y + h);                 \
	}

#define DRAW_RECT_BORDER(x, y, w, h, color, border, bcolor)                                                       \
	{                                                                                                             \
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB((bcolor) & 0xFF, ((bcolor) >> 8) & 0xFF, ((bcolor) >> 16) & 0xFF)); \
		EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));                                                                \
		EVE_DRAW_AT(x, y);                                                                                        \
		EVE_DRAW_AT(x + w, y + h);                                                                                \
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB((color) & 0xFF, ((color) >> 8) & 0xFF, ((color) >> 16) & 0xFF));    \
		EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));                                                                \
		EVE_DRAW_AT(x + border, y + border);                                                                      \
		EVE_DRAW_AT(x + w - boder * 2, y + h - boder * 2);                                                        \
	}
#define DRAW_BOX(box)                              \
	{                                              \
		EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS)); \
		EVE_DRAW_AT(box.x, box.y);                 \
		EVE_DRAW_AT(box.x_end, box.y_end);         \
	}

#define DRAW_BOX_BORDER(box, color, border, bcolor)                                                               \
	{                                                                                                             \
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB((bcolor) & 0xFF, ((bcolor) >> 8) & 0xFF, ((bcolor) >> 16) & 0xFF)); \
		EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));                                                                \
		EVE_DRAW_AT(box.x, box.y);                                                                                \
		EVE_DRAW_AT(box.x_end, box.y_end);                                                                        \
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB((color) & 0xFF, ((color) >> 8) & 0xFF, ((color) >> 16) & 0xFF));    \
		EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));                                                                \
		EVE_DRAW_AT(box.x + border, box.y + border);                                                              \
		EVE_DRAW_AT(box.x_end - border, box.y_end - border);                                                      \
	}

#define EVE_DRAW_AT(x, y) EVE_Cmd_wr32(s_pHalContext, VERTEX2F((x) * 1, (y) * 1))

typedef struct
{
	int x, y, w, h, x_end, y_end, x_mid, y_mid;
} app_box;

uint8_t Gesture_GetTag(EVE_HalContext *phost)
{
	static uint8_t Read_tag = 0;
	static uint8_t temp_tag = 0;
	static uint8_t ret_tag = 0;
	static uint8_t sk = 0;

	Read_tag = EVE_Hal_rd8(phost, REG_TOUCH_TAG);
	ret_tag = 0;
	if (Read_tag != 0) // Allow if the Key is released
	{
		if (temp_tag != Read_tag)
		{
			temp_tag = Read_tag;
			sk = Read_tag; // Load the Read tag to temp variable
		}
	}
	else
	{
		if (temp_tag != 0)
		{
			ret_tag = temp_tag;
			temp_tag = 0; // The event will be processed. Clear the tag
		}
		sk = 0;
	}
	return ret_tag;
}

// Function to calculate and return FPS without float/double
int getFPS()
{
	static unsigned long last_time_ms = 0; // Timestamp of the last frame
	static unsigned long frame_count = 0;  // Total frames since start
	static int fps = 0;					   // Calculated FPS

	unsigned long current_time_ms = EVE_millis();

	if (last_time_ms == 0)
	{
		// Initialize on the first frame
		last_time_ms = current_time_ms;
		return 0;
	}

	// Calculate time since last frame
	unsigned long duration_ms = max(1, current_time_ms - last_time_ms);
	fps = (frame_count * 1000) / duration_ms;
	frame_count++;

	return fps;
}

void draw_app_window(app_box app_window)
{
	int border = 5;
	int bx = app_window.x;
	int by = app_window.y;
	int bw = app_window.w + border;
	int bh = app_window.h + border;
	char border_color[3] = {134, 134, 134};

	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
	EVE_DRAW_AT(0, 0);
	EVE_DRAW_AT(s_pHalContext->Width, s_pHalContext->Height);

	// Camera border
	// start draw a small rectangle with alpha value 255
	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(0, 0, 0, 1));
	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(ZERO, ZERO));
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(0));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
	EVE_DRAW_AT(bx + border, by + border);
	EVE_DRAW_AT(bx + bw - border, by + bh - border);

	// finally draw the bigger rECTANGE he desired rgb valu);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(border_color[0], border_color[1], border_color[2]));
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(1, 1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(DST_ALPHA, ONE_MINUS_DST_ALPHA));
	EVE_Cmd_wr32(s_pHalContext, TAG(30));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
	EVE_DRAW_AT(bx, by);
	EVE_DRAW_AT(bx + bw, by + bh);
	EVE_Cmd_wr32(s_pHalContext, END());
	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA));

	EVE_CoCmd_number(s_pHalContext, app_window.x_mid, app_window.y - 30, 30, OPT_CENTERX, app_window.w);
	EVE_CoCmd_number(s_pHalContext, app_window.x - 40, app_window.y_mid, 30, OPT_CENTERX, app_window.h);
}

static int8_t istouch()
{
	return !(EVE_Hal_rd16(s_pHalContext, REG_TOUCH_RAW_XY) & 0x8000);
}

// Function to increase the seconds and return the "mm:ss" time string
char *mm_ss(int start_mm, int start_ss)
{
	static int mm = 0;		   // Minutes
	static int ss = 0;		   // Seconds
	static char timeString[6]; // Static buffer to hold "mm:ss"

	unsigned long prevMillis = EVE_millis(); // Store the initial time

	mm = start_mm; // Set the initial minutes
	ss = start_ss; // Set the initial seconds

	// Check if 1 second has passed and update time
	unsigned long currentMillis = EVE_millis();
	if (currentMillis - prevMillis >= 1000)
	{
		prevMillis += 1000; // Update previous time by 1 second

		// Increment seconds and handle overflow to minutes
		ss++;
		if (ss == 60)
		{
			ss = 0;
			mm++;
		}

		// Format the time as "mm:ss" into the static buffer
		snprintf(timeString, sizeof(timeString), "%02d:%02d", mm, ss);
	}

	return timeString;
}

/// Function to handle time
char *hh_mm(int hh, int mm)
{
	static int hours = 0;				 // Current hour
	static int minutes = 0;				 // Current minute
	static int seconds = 0;				 // Track seconds internally
	static unsigned long prevMillis = 0; // Store the previous time
	static char timeString[6];			 // Static buffer for "hh:mm" format

	// Initialize time only during the first call
	if (prevMillis == 0)
	{
		hours = hh;
		minutes = mm;
		prevMillis = EVE_millis();
	}

	unsigned long currentMillis = EVE_millis();

	// Check if 1000 milliseconds (1 second) have passed
	if (currentMillis - prevMillis >= 1000)
	{
		prevMillis += 1000; // Update previous time by 1 second

		// Increment seconds and handle overflow to minutes and hours
		seconds++;
		if (seconds == 60)
		{
			seconds = 0;
			minutes++;
			if (minutes == 60)
			{
				minutes = 0;
				hours++;
				if (hours == 24)
				{
					hours = 0; // Reset to 00 after 23:59
				}
			}
		}
	}

	// Format the time as "hh:mm" into the static buffer
	snprintf(timeString, sizeof(timeString), "%02d:%02d", hours, minutes);

	return timeString;
}

// Function to handle the date, takes starting date as input
char *dd_mm_yyyy(int start_dd, int start_mm, int start_yyyy)
{
#define MILLIS_PER_DAY (24 * 60 * 60 * 1000)
	static char dateString[18]; // Static buffer for "dd - mm - yyyy" format

	// Days in each month (non-leap year for simplicity, adjusted later for leap years)
	int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	// Calculate total elapsed days from milliseconds
	unsigned long elapsed_millis = EVE_millis();
	int elapsed_days = elapsed_millis / MILLIS_PER_DAY;

	// Start with the provided start date
	int dd = start_dd + elapsed_days;
	int mm = start_mm;
	int yyyy = start_yyyy;

	// Adjust for months and years
	while (1)
	{
		// Handle leap years for February
		if ((yyyy % 4 == 0 && yyyy % 100 != 0) || (yyyy % 400 == 0))
			days_in_month[1] = 29; // Leap year
		else
			days_in_month[1] = 28;

		if (dd > days_in_month[mm - 1])
		{
			dd -= days_in_month[mm - 1];
			mm++;
			if (mm > 12)
			{
				mm = 1;
				yyyy++;
			}
		}
		else
		{
			break;
		}
	}

	// Format the date as "dd - mm - yyyy"
	snprintf(dateString, sizeof(dateString), "%02d - %02d - %04d", dd, mm, yyyy);

	return dateString;
}

int rand(int range)
{
	if (range <= 0)
		return 0; 

	unsigned long millis = EVE_millis() + 17; 
	return millis % range;					  
}

// ************************************ application ************************************
#define BIT_PER_CHAR 8
#define USE_BITMAP_L1 0
#define USE_BITMAP_PALETTED 1

#define USEBITMAP USE_BITMAP_L1

#if  USEBITMAP == USE_BITMAP_L1
#define GRAPH_BIT_PER_PIXEL 1
#else
#define GRAPH_BIT_PER_PIXEL (BIT_PER_CHAR)
#endif

#define GRAPH_W 160
#define GRAPH_H 1000
#define GRAPH_SIZE (GRAPH_W *GRAPH_H)
#define GRAPH_BYTE (GRAPH_SIZE * GRAPH_BIT_PER_PIXEL / BIT_PER_CHAR)

// coloring
uint8_t color_table[] = {
	// blue, green, red, alpha
	// blue, green, red, alpha
	0, 0, 0, 255,		// Black
	255, 255, 255, 255, // White
	0, 0, 255, 255,		// Red
	0, 255, 0, 255,		// Green
	255, 0, 0, 255,		// Blue
	0, 255, 255, 255,	// Yellow
	255, 255, 0, 255,	// Cyan
	255, 0, 255, 255,	// Magenta
	192, 192, 192, 255, // Light Gray
	128, 128, 128, 255, // Dark Gray
	0, 0, 128, 255,		// Dark Red
	0, 128, 0, 255,		// Dark Green
	128, 0, 0, 255,		// Dark Blue
	0, 128, 128, 255,	// Dark Yellow
	128, 128, 0, 255,	// Dark Cyan
	128, 0, 128, 255,	// Dark Magenta
	128, 128, 255, 255, // Light Red
	128, 255, 128, 255, // Light Green
	255, 128, 128, 255, // Light Blue
	128, 255, 255, 255, // Light Yellow
	255, 255, 128, 255, // Light Cyan
	255, 128, 255, 255, // Light Magenta
	64, 64, 64, 255,	// Very Dark Gray
	64, 64, 192, 255,	// Warm Red
	64, 192, 64, 255,	// Warm Green
	192, 64, 64, 255,	// Warm Blue
	64, 192, 192, 255,	// Olive
	192, 192, 64, 255,	// Teal
	192, 64, 192, 255,	// Purple
	64, 64, 0, 255,		// Deep Teal
	64, 0, 64, 255		// Deep Purple
};

// Define indices for easier access
enum ColorIndex
{
	ARGB_BLACK = 0,
	ARGB_WHITE,
	ARGB_RED,
	ARGB_GREEN,
	ARGB_BLUE,
	ARGB_YELLOW,
	ARGB_CYAN,
	ARGB_MAGENTA,
	ARGB_LIGHT_GRAY,
	ARGB_DARK_GRAY,
	ARGB_DARK_RED,
	ARGB_DARK_GREEN,
	ARGB_DARK_BLUE,
	ARGB_DARK_YELLOW,
	ARGB_DARK_CYAN,
	ARGB_DARK_MAGENTA,
	ARGB_LIGHT_RED,
	ARGB_LIGHT_GREEN,
	ARGB_LIGHT_BLUE,
	ARGB_LIGHT_YELLOW,
	ARGB_LIGHT_CYAN,
	ARGB_LIGHT_MAGENTA,
	ARGB_VERY_DARK_GRAY,
	ARGB_WARM_RED,
	ARGB_WARM_GREEN,
	ARGB_WARM_BLUE,
	ARGB_OLIVE,
	ARGB_TEAL,
	ARGB_PURPLE,
	ARGB_DEEP_TEAL,
	ARGB_DEEP_PURPLE
};

// graph palleted buffer
char buffer_paletted1_x2[GRAPH_BYTE * 2] = {{0}}; 
char buffer_paletted2_x2[GRAPH_BYTE * 2] = {{0}};
char buffer_paletted3_x2[GRAPH_BYTE * 2] = {{0}};

// graph background color
#if USEBITMAP == USE_BITMAP_PALETTED
#define PALETTED_BG (ARGB_BLACK * 4)
#elif USEBITMAP == USE_BITMAP_L1
#define PALETTED_BG 0
#endif

void set_color(char* a, int n, char color) {
#if USEBITMAP == USE_BITMAP_L1
	int byte_index = n / 8;    // byte nth
	int bit_index = 7 - n % 8;    // bit nth 
	a[byte_index] |= (1 << bit_index);
#else
	a[n] = color;
#endif
}
void draw_pixel(char *buffer, int x, int y, int color_offset)
{
	if (y > GRAPH_H * 2)
		return;
	if (x < 0)
		printf("Error: Drawing pixel at (%d, %d)\n", x, y);


	if (x < 0 || x>= GRAPH_W)
	{
		return;
	}
	set_color(buffer, x + y * GRAPH_W, color_offset);
}
void bresenham_line(char *buffer, int x1, int y1, int x2, int y2, int color_offset)
{
	int sx = (x2 > x1) ? 1 : -1;
	int sy = (y2 > y1) ? 1 : -1;
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	while (dx-- > -1)
	{
		draw_pixel(buffer, x1, y1, color_offset);
		x1 += sx;
	}
}
int read_time_simulate(int sample_rate, int time_start_ms)
{
	int now_ms = EVE_millis();
	int duration_ms = now_ms - time_start_ms;

	int num_samples = duration_ms * sample_rate / 1000;
	//return 1;
	return num_samples;
}

// put a realtime data into ramg
// return the paletted bitmap adderss
int heartbeat(int ramg_paletted, char *buffer_paletted, int color_offset)
{
	// Simulated sinus rhythm data (adjusted to resemble the waveform)
	const int ecg_data[] = {
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2,															  // P-wave (small bump)
		0, 2, 4, 6, 8, 6, 4, 2, 0,																  // PR segment (flat)
		0, 0, 0, 0, 0,																			  // QRS complex (sharp spike)
		2, 4, 8, 20, 40, 70, 30, 10, 5, 2,														  // ST segment (flat)
		2, 2, 2, 2, 2, 2, 2,																	  // T-wave (broad bump)
		2, 4, 6, 10, 14, 16, 14, 10, 6, 4, 2,													  // Baseline (flat)
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // end
	};
	int dividor = 2;
	int multipler = 3;
	int data_duration = 1; // unit: second
	const int sample_num = sizeof(ecg_data) / sizeof(int);
	const int sample_per_second = sample_num / data_duration;
	static int sample_offset = 0;

	static int lastx = 2;
	static int lasty = GRAPH_H;
	static int line_next = GRAPH_H; // start from buffer 2nd
	static int paletted_addr_start = 0;

	static uint32_t time_start_ms = 0;
	if (time_start_ms == 0)
	{
		time_start_ms = EVE_millis();
	}
	int num_samples = read_time_simulate(sample_per_second, time_start_ms);
	if (num_samples > 0)
		time_start_ms = EVE_millis();
	else {
		return paletted_addr_start;
	}

	int copy_start = line_next * GRAPH_W;
	int copy_bytes = 0;
	// one sample take 1 line
	for (int i = 0; i < num_samples; i++)
	{
		int paletted_x = ecg_data[(sample_offset + line_next) % sample_num] * multipler / dividor;

		for (int ii = 0; ii < GRAPH_W * GRAPH_BIT_PER_PIXEL / BIT_PER_CHAR; ii++)
		{
			int bytenth = line_next * GRAPH_W * GRAPH_BIT_PER_PIXEL / BIT_PER_CHAR + ii;
			buffer_paletted[bytenth] = PALETTED_BG;
		}
		bresenham_line(buffer_paletted, paletted_x, line_next, lastx, lasty, color_offset);
		lastx = paletted_x;
		lasty = line_next;

		line_next++;
		copy_bytes += GRAPH_W;

		if (line_next == GRAPH_H * 2) // if buffer 2nd full
		{
			memcpy(buffer_paletted, &buffer_paletted[GRAPH_BYTE], GRAPH_BYTE); // copy buffer 2nd to buffer 1st
			line_next = GRAPH_H;											   // continue writing to buffer 2nd, while displaying buffer 1st
			//copy_start = line_next * GRAPH_W;
			//copy_bytes = 0;
			sample_offset = (line_next - 1) % sample_num;
			break;
		}
	}

	copy_start = copy_start * GRAPH_BIT_PER_PIXEL / BIT_PER_CHAR;
	copy_bytes = copy_bytes * GRAPH_BIT_PER_PIXEL / BIT_PER_CHAR;

	// copy to buffer 2nd
	EVE_Hal_wrMem(s_pHalContext, ramg_paletted + copy_start, &buffer_paletted[copy_start], copy_bytes);

	// duplicate to buffer 1st for next cycle
	int duplicate_to = copy_start - GRAPH_BYTE - copy_bytes;
	int duplicate_from = copy_start;
	EVE_CoCmd_memCpy(s_pHalContext, ramg_paletted + duplicate_to, ramg_paletted + duplicate_from, copy_bytes);

	paletted_addr_start = copy_start - GRAPH_BYTE + copy_bytes;
	return paletted_addr_start;
}
int pleth(int ramg_paletted, char *buffer_paletted, int color_offset)
{
	// Simulated sinus rhythm data (adjusted to resemble the waveform)
	const int pleth_data[] = {
		0, 6, 12, 18, 30, 42, 60, 78, 102, 126,			  // Cycle 1 - Inhalation
		150, 174, 192, 210, 228, 240, 252, 264, 270, 276, // Cycle 1 - Continued Inhalation
		282, 288, 285, 282, 288, 285, 276, 270, 264, 252, // Cycle 1 - Peak
		240, 228, 210, 192, 174, 160, 140, 120, 130, 110, // Cycle 1 - Exhalation with fluctuations
		90, 100, 85, 75, 95, 65, 50, 42, 30, 35,		  // Cycle 1 - Continued Exhalation
		20, 15, 12, 10, 8, 6, 9, 12, 18, 30,			  // Cycle 1 - Trough with fluctuations
		42, 70, 100, 120, 140, 160, 180, 200, 210, 220,	  // Cycle 2 - Inhalation
		225, 230, 235, 240, 238, 236, 240, 235, 225, 210, // Cycle 2 - Peak and decline
		195, 180, 165, 150, 130, 110, 90, 70, 50, 30,	  // Cycle 2 - Faster Exhalation
		20, 10, 5, 0, 0, 0, 0, 0, 0, 0					  // Cycle 2 - End
	};
	int dividor = 2;
	int multipler = 1;
	int data_duration = 1; // unit: second
	const int sample_num = sizeof(pleth_data) / sizeof(int);
	const int sample_per_second = sample_num / data_duration;
	static int sample_offset = 0;

	static int lastx = 2;
	static int lasty = GRAPH_H;
	static int line_next = GRAPH_H; // start from buffer 2nd
	static int paletted_addr_start = 0;

	static uint32_t time_start_ms = 0;
	if (time_start_ms == 0)
	{
		time_start_ms = EVE_millis();
	}
	int num_samples = read_time_simulate(sample_per_second, time_start_ms);
	if (num_samples > 0)
		time_start_ms = EVE_millis();
	else {
		return paletted_addr_start;
	}

	int copy_start = line_next * GRAPH_W;
	int copy_bytes = 0;
	// one sample take 1 line
	for (int i = 0; i < num_samples; i++)
	{
		int paletted_x = pleth_data[(sample_offset + line_next) % sample_num] * multipler / dividor;

		for (int ii = 0; ii < GRAPH_W * GRAPH_BIT_PER_PIXEL / BIT_PER_CHAR; ii++)
		{
			int bytenth = line_next * GRAPH_W * GRAPH_BIT_PER_PIXEL / BIT_PER_CHAR + ii;
			buffer_paletted[bytenth] = PALETTED_BG;
		}
		bresenham_line(buffer_paletted, paletted_x, line_next, lastx, lasty, color_offset);
		lastx = paletted_x;
		lasty = line_next;

		line_next++;
		copy_bytes += GRAPH_W;

		if (line_next == GRAPH_H * 2) // if buffer 2nd full
		{
			memcpy(buffer_paletted, &buffer_paletted[GRAPH_BYTE], GRAPH_BYTE); // copy buffer 2nd to buffer 1st
			line_next = GRAPH_H;											   // continue writing to buffer 2nd, while displaying buffer 1st
			//copy_start = line_next * GRAPH_W;
			//copy_bytes = 0;
			sample_offset = (line_next - 1) % sample_num;
			break;
		}
	}

	copy_start = copy_start * GRAPH_BIT_PER_PIXEL / BIT_PER_CHAR;
	copy_bytes = copy_bytes * GRAPH_BIT_PER_PIXEL / BIT_PER_CHAR;

	// copy to buffer 2nd
	EVE_Hal_wrMem(s_pHalContext, ramg_paletted + copy_start, &buffer_paletted[copy_start], copy_bytes);

	// duplicate to buffer 1st for next cycle
	int duplicate_to = copy_start - GRAPH_BYTE - copy_bytes;
	int duplicate_from = copy_start;
	EVE_CoCmd_memCpy(s_pHalContext, ramg_paletted + duplicate_to, ramg_paletted + duplicate_from, copy_bytes);

	paletted_addr_start = copy_start - GRAPH_BYTE + copy_bytes;
	return paletted_addr_start;
}
int co2(int ramg_paletted, char *buffer_paletted, int color_offset)
{
	// Simulated sinus rhythm data (adjusted to resemble the waveform)
	const int co2_data[] = {												  // 1 second sample
							30, 45, 60, 75, 90, 105, 120, 135, 150, 165,	  //
							180, 195, 210, 225, 240, 255, 255, 255, 255, 255, //
							255, 255, 255, 255, 255, 240, 225, 210, 195, 180, //
							165, 150, 135, 120, 105, 90, 75, 60, 45, 30,	  //
							15, 10, 5, 0, 0, 0, 0, 0, 0, 0,					  //
							0, 0, 0, 0, 0, 5, 10, 15, 15};
	int dividor = 2;
	int multipler = 1;
	int data_duration = 1; // unit: second
	const int sample_num = sizeof(co2_data) / sizeof(int);
	const int sample_per_second = sample_num / data_duration;
	static int sample_offset = 0;

	static int lastx = 2;
	static int lasty = GRAPH_H;
	static int line_next = GRAPH_H; // start from buffer 2nd
	static int paletted_addr_start = 0;

	static uint32_t time_start_ms = 0;
	if (time_start_ms == 0)
	{
		time_start_ms = EVE_millis();
	}
	int num_samples = read_time_simulate(sample_per_second, time_start_ms);
	if (num_samples > 0)
		time_start_ms = EVE_millis();
	else {
		return paletted_addr_start;
	}

	int copy_start = line_next * GRAPH_W;
	int copy_bytes = 0;
	// one sample take 1 line
	for (int i = 0; i < num_samples; i++)
	{
		int paletted_x = co2_data[(sample_offset + line_next) % sample_num] * multipler / dividor;

		for (int ii = 0; ii < GRAPH_W * GRAPH_BIT_PER_PIXEL / BIT_PER_CHAR; ii++)
		{
			int bytenth = line_next * GRAPH_W * GRAPH_BIT_PER_PIXEL / BIT_PER_CHAR + ii;
			buffer_paletted[bytenth] = PALETTED_BG;
		}
		bresenham_line(buffer_paletted, paletted_x, line_next, lastx, lasty, color_offset);
		lastx = paletted_x;
		lasty = line_next;

		line_next++;
		copy_bytes += GRAPH_W;

		if (line_next == GRAPH_H * 2) // if buffer 2nd full
		{
			memcpy(buffer_paletted, &buffer_paletted[GRAPH_BYTE], GRAPH_BYTE); // copy buffer 2nd to buffer 1st
			line_next = GRAPH_H;											   // continue writing to buffer 2nd, while displaying buffer 1st
			//copy_start = line_next * GRAPH_W;
			//copy_bytes = 0;
			sample_offset = (line_next - 1) % sample_num;
			break;
		}
	}

	copy_start = copy_start * GRAPH_BIT_PER_PIXEL / BIT_PER_CHAR;
	copy_bytes = copy_bytes * GRAPH_BIT_PER_PIXEL / BIT_PER_CHAR;

	// copy to buffer 2nd
	EVE_Hal_wrMem(s_pHalContext, ramg_paletted + copy_start, &buffer_paletted[copy_start], copy_bytes);

	// duplicate to buffer 1st for next cycle
	int duplicate_to = copy_start - GRAPH_BYTE - copy_bytes;
	int duplicate_from = copy_start;
	EVE_CoCmd_memCpy(s_pHalContext, ramg_paletted + duplicate_to, ramg_paletted + duplicate_from, copy_bytes);

	paletted_addr_start = copy_start - GRAPH_BYTE + copy_bytes;
	return paletted_addr_start;
}

void graph_L1(int bitmap_addr, int color_source, int x, int y) {
	int bformat = L1;
	int lw = max(GRAPH_H, GRAPH_W);
	int lh = max(GRAPH_H, GRAPH_W);
#define MAX_ANGLE 360
#define MAX_CIRCLE_UNIT 65536
	int rotation_angle = -90;

	EVE_CoCmd_setBitmap(s_pHalContext, bitmap_addr, bformat, GRAPH_W, GRAPH_H);
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, lw, lh));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H(lw >> 9, lh >> 9));
	EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());
	EVE_CoCmd_loadIdentity(s_pHalContext);
	EVE_CoCmd_translate(s_pHalContext, 0, GRAPH_W * MAX_CIRCLE_UNIT);
	EVE_CoCmd_rotate(s_pHalContext, rotation_angle * MAX_CIRCLE_UNIT / MAX_ANGLE);
	EVE_CoCmd_setMatrix(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_DRAW_AT(x, y);
	EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
}

void graph_paletted8(int bitmap_addr, int color_source, int x, int y) {
	int bformat = PALETTED8;
#ifdef BT82X_ENABLE
	bformat = PALETTEDARGB8;
#endif
	int lw = max(GRAPH_H, GRAPH_W);
	int lh = max(GRAPH_H, GRAPH_W);
#define MAX_ANGLE 360
#define MAX_CIRCLE_UNIT 65536
	int rotation_angle = -90;

	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	EVE_CoCmd_setBitmap(s_pHalContext, bitmap_addr , bformat, GRAPH_W, GRAPH_H);
	EVE_Cmd_wr32(s_pHalContext, PALETTE_SOURCE(color_source));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, lw, lh));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H(lw >> 9, lh >> 9));
	EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());
	EVE_CoCmd_loadIdentity(s_pHalContext);
	EVE_CoCmd_translate(s_pHalContext, 0, GRAPH_W * MAX_CIRCLE_UNIT);
	EVE_CoCmd_rotate(s_pHalContext, rotation_angle * MAX_CIRCLE_UNIT / MAX_ANGLE);
	EVE_CoCmd_setMatrix(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_DRAW_AT(x, y);
	EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
}

void SAMAPP_Bedside_Monitor()
{
	int color_table_addr = 0;
	int paletted_size = GRAPH_BYTE * 3; // buffer 0 to duplicate data from buffer 1, buffer 1 to append new data

	int paletted_adr = color_table_addr + sizeof(color_table);
	int paletted_adr2 = paletted_adr + paletted_size;
	int paletted_adr3 = paletted_adr2 + paletted_size;

	
	app_box lcd_size = {0, 0, 1920, 1200, 1920, 1200};
	app_box app_window = INIT_APP_BOX((1920 - 1280) / 2, (1200 - 800) / 2, 1280, 800);
	int graph_start = app_window.x + 35;

	app_box box_menu_top = INIT_APP_BOX(app_window.x, app_window.y, GRAPH_H, app_window.h * 8 / 100);
	app_box box_graph_ecg = INIT_APP_BOX(graph_start, box_menu_top.y_end, box_menu_top.w, app_window.h * 28 / 100);
	app_box box_graph_pleth = INIT_APP_BOX(graph_start, box_graph_ecg.y_end, box_menu_top.w, box_graph_ecg.h);
	app_box box_graph_co2 = INIT_APP_BOX(graph_start, box_graph_pleth.y_end, box_menu_top.w, box_graph_ecg.h);
	app_box box_menu_bottom = INIT_APP_BOX(app_window.x, box_graph_co2.y_end, box_menu_top.w, box_menu_top.h);

	int x = box_menu_top.x_end;
	int w = app_window.w - box_menu_top.w;
	int h = (app_window.h - box_menu_top.h - box_menu_bottom.h) / 4;
	int y = box_menu_top.y_end;
	int border = 2;

	app_box box_right1 = INIT_APP_BOX(x, y + h * 0, w, h);
	app_box box_right2 = INIT_APP_BOX(x, y + h * 1, w, h);
	app_box box_right3 = INIT_APP_BOX(x, y + h * 2, w, h);
	app_box box_right4 = INIT_APP_BOX(x, y + h * 3, w, h);

	// prepare a paletted buffer
	EVE_Hal_wrMem(s_pHalContext, color_table_addr, color_table, sizeof(color_table));

	// init graph palleted buffer 1
	memset(buffer_paletted1_x2, 0, sizeof(buffer_paletted1_x2));
	EVE_Hal_wrMem(s_pHalContext, paletted_adr, buffer_paletted1_x2, sizeof(buffer_paletted1_x2));

	// init graph palleted buffer 2
	memset(buffer_paletted2_x2, 0, sizeof(buffer_paletted2_x2));
	EVE_Hal_wrMem(s_pHalContext, paletted_adr2, buffer_paletted2_x2, sizeof(buffer_paletted2_x2));

	// init graph palleted buffer 3
	memset(buffer_paletted3_x2, 0, sizeof(buffer_paletted3_x2));
	EVE_Hal_wrMem(s_pHalContext, paletted_adr3, buffer_paletted3_x2, sizeof(buffer_paletted3_x2));

	int time_start_ms = 0;
	int val_hr = 66;
	int val_spo2 = 96;
	int val_co2 = 22;
	int val_sys = 156;
	int val_dias = 93;

	while (1)
	{
		int buffer_offset = heartbeat(paletted_adr, buffer_paletted1_x2, ARGB_GREEN);
		int buffer_offset2 = pleth(paletted_adr2, buffer_paletted2_x2, ARGB_CYAN);
		int buffer_offset3 = co2(paletted_adr3, buffer_paletted3_x2, ARGB_YELLOW);

		Display_Start(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, VERTEX_FORMAT(0));
		draw_app_window(app_window);

#if  USEBITMAP == USE_BITMAP_L1
#define GRAPH graph_L1
#else
#define GRAPH graph_paletted8
#endif

		// Graph ecg
#if  USEBITMAP == USE_BITMAP_L1
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 255, 0)); // green
#endif
		GRAPH(paletted_adr + buffer_offset, color_table_addr, graph_start + 10, box_graph_ecg.y + 50);

		// Graph pleth
#if  USEBITMAP == USE_BITMAP_L1
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 255, 255)); // cyan
#endif
		GRAPH(paletted_adr2 + buffer_offset2, color_table_addr, graph_start + 10, box_graph_pleth.y + 50);

		// Graph co2
#if  USEBITMAP == USE_BITMAP_L1
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 0)); // yellow
#endif
		GRAPH(paletted_adr3 + buffer_offset3, color_table_addr, graph_start + 10, box_graph_co2.y + 50);

		// Top menu box
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(43, 132, 135));
		DRAW_BOX(box_menu_top);
		// Bottom menu box
		DRAW_BOX(box_menu_bottom);

		int graph_border = 3, graph_magin = 36;
		int brace_y = box_graph_ecg.y / 10, brace_w = 5, brace_h = box_graph_ecg.h - brace_y * 2;

		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		// Graph ECG braces outside
		DRAW_RECT(box_graph_ecg.x, box_graph_ecg.y + brace_y, brace_w, brace_h);
		// Graph pleth braces outside
		DRAW_RECT(box_graph_pleth.x, box_graph_pleth.y + brace_y, brace_w, brace_h);
		// Graph co2 braces outside
		DRAW_RECT(box_graph_co2.x, box_graph_co2.y + brace_y, brace_w, brace_h);

		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
		// Graph ecg braces inside
		DRAW_RECT(box_graph_ecg.x + graph_border, box_graph_ecg.y + brace_y + border, brace_w , brace_h - border * 2);
		// Graph pleth braces inside
		DRAW_RECT(box_graph_pleth.x + graph_border, box_graph_pleth.y + brace_y + border, brace_w , brace_h - border * 2);
		// Graph co2 braces inside
		DRAW_RECT(box_graph_co2.x + graph_border, box_graph_co2.y + brace_y + border, brace_w , brace_h - border * 2);

		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(249, 254, 143));
		// right menu top
		DRAW_RECT(box_menu_top.x_end, box_menu_top.y, 1280 - box_menu_top.w, box_menu_top.h);
		// right menu bottom
		DRAW_RECT(box_menu_bottom.x_end, box_menu_bottom.y, 1280 - box_menu_bottom.w, box_menu_bottom.h);

		// right menu HR
		DRAW_BOX_BORDER(box_right1, 0x000000, border, 0xffffff);
		// right menu spO2
		DRAW_BOX_BORDER(box_right2, 0x000000, border, 0xffffff);
		// right menu etCO2
		DRAW_BOX_BORDER(box_right3, 0x000000, border, 0xffffff);
		// right menu NIBP
		DRAW_BOX_BORDER(box_right4, 0x000000, border, 0xffffff);

		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
		EVE_CoCmd_fgColor(s_pHalContext, 0xF3B475);
		// Record button
		EVE_CoCmd_button(s_pHalContext, box_menu_top.x + 180 * 1, box_menu_bottom.y + 7, 180, 50, 30, 0, "RECORD");
		EVE_CoCmd_fgColor(s_pHalContext, 0x144344);
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		// Stop button
		EVE_CoCmd_button(s_pHalContext, box_menu_top.x + 180 * 2 + 50, box_menu_bottom.y + 7, 180, 50, 30, 0, "STOP");
		// NIBP button
		EVE_CoCmd_button(s_pHalContext, box_menu_top.x + 180 * 3 + 99, box_menu_bottom.y + 7, 180, 50, 30, 0, "NIBP");
		// EXIT button
		EVE_CoCmd_button(s_pHalContext, box_menu_top.x + 180 * 3 + 510, box_menu_bottom.y + 7, 180, 50, 30, 0, "EXIT");

		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
		// Frame per second measurement
		EVE_CoCmd_text(s_pHalContext, app_window.x + 10, app_window.y_end - 40, 30, OPT_FORMAT, "fps = %d", getFPS());

		// register big font 32 33 34
		EVE_CoCmd_romFont(s_pHalContext, 32, 32);
		EVE_CoCmd_romFont(s_pHalContext, 33, 33);
		EVE_CoCmd_romFont(s_pHalContext, 34, 34);
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));

		int x = box_menu_top.x;
		int y = box_menu_top.y_mid;
		EVE_CoCmd_text(s_pHalContext, x + 5, y, 32, OPT_CENTERY, "BED");
		EVE_CoCmd_text(s_pHalContext, x + 155, y, 31, OPT_CENTERY, "no 5");
		EVE_CoCmd_text(s_pHalContext, box_menu_top.x_end, y, 31, OPT_CENTERY | OPT_RIGHTX, hh_mm(9, 1));
		EVE_CoCmd_text(s_pHalContext, box_menu_top.x_mid, y, 31, OPT_CENTER, dd_mm_yyyy(11, 12, 2024));

		// Graph title text information
		EVE_CoCmd_text(s_pHalContext, box_graph_ecg.x + 15, box_graph_ecg.y + 30, 30, 0, "ECG");
		EVE_CoCmd_text(s_pHalContext, box_graph_pleth.x + 15, box_graph_pleth.y + 30, 30, 0, "PLETH");
		EVE_CoCmd_text(s_pHalContext, box_graph_co2.x + 15, box_graph_co2.y + 30, 30, 0, "CO2");

		// create random data change
		int time_end_ms = EVE_millis();
		int duration = time_end_ms - time_start_ms;
		if (duration > (200 + rand(100) - 50))
		{
			if (rand(10) % 9 == 0)
				val_hr = 66 + rand(8) - 5;
			if (rand(10) % 5 == 0)
				val_spo2 = 96 + rand(12) - 5;
			if (rand(10) % 5 == 0)
				val_co2 = 22 + rand(4) - 2;
			if (rand(10) % 2 == 0)
				val_sys = 156 + rand(8) - 4;
			if (rand(10) % 3 == 0)
				val_dias = 93 + rand(8) - 4;

			time_start_ms = time_end_ms;
		}

		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 255, 0));
		// Heart rate
		EVE_CoCmd_text(s_pHalContext, box_right1.x + 5, box_right1.y + 5, 29, 0, "HR");
		EVE_CoCmd_number(s_pHalContext, box_right1.x_mid, box_right1.y_mid, 34, OPT_CENTER, val_hr);
		EVE_CoCmd_text(s_pHalContext, box_right1.x_mid + 90, box_right1.y_mid, 29, OPT_CENTER, "bpm");

		// SPO2
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 255, 255));
		EVE_CoCmd_text(s_pHalContext, box_right2.x + 5, box_right2.y + 5, 29, 0, "spO2");
		EVE_CoCmd_number(s_pHalContext, box_right2.x_mid, box_right2.y_mid, 34, OPT_CENTER, val_spo2);
		EVE_CoCmd_text(s_pHalContext, box_right2.x_mid + 90, box_right2.y_mid, 29, OPT_CENTER, "%");

		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 0));
		// ETCO2
		EVE_CoCmd_text(s_pHalContext, box_right3.x + 5, box_right3.y + 5, 29, 0, "etCO2");
		EVE_CoCmd_number(s_pHalContext, box_right3.x_mid, box_right3.y_mid, 34, OPT_CENTER, val_co2);
		EVE_CoCmd_text(s_pHalContext, box_right3.x_mid + 90, box_right3.y_mid, 29, OPT_CENTER, "mmHg");

		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		// NIBP
		EVE_CoCmd_text(s_pHalContext, box_right4.x + 5, box_right4.y + 5, 29, 0, "NIBP");
		EVE_CoCmd_number(s_pHalContext, box_right4.x + 2, box_right4.y + 50, 33, 0, val_sys);
		EVE_CoCmd_text(s_pHalContext, box_right4.x_mid + 20, box_right4.y_mid + 10, 28, OPT_CENTERX, "mmHg");
		EVE_CoCmd_number(s_pHalContext, box_right4.x_mid + 50, box_right4.y + 50, 33, 0, val_dias);
		EVE_CoCmd_text(s_pHalContext, box_right4.x + 35, box_right4.y_end - 40, 29, 0, "sys");
		EVE_CoCmd_text(s_pHalContext, box_right4.x_end - 70, box_right4.y_end - 40, 29, 0, "dias");

		Display_End(s_pHalContext);
	}
	return 0;
};
#endif