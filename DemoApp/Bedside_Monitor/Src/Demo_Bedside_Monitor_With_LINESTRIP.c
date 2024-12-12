#if 0
/**
 * @file Bedside_Monitor.c
 * @brief Sample usage of primitives drawing
 *
 * @author Bridgetek
 *
 * @date 2019
 * 
 * MIT License
 *
 * Copyright (c) [2019] [Bridgetek Pte Ltd (BRTChip)]
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

	char *info[] = { "EVE Sample Application",
		"This sample demonstrate a Bedside monitoring ",
		"",
		"" };

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

void SAMAPP_Demo_With_Linestrip();
void SAMAPP_Demo_With_Paletted();

void SAMAPP_Bedside_Monitor()
{
	SAMAPP_Demo_With_Linestrip();
	//SAMAPP_Demo_With_Paletted();
}

/// Sample data generator //////////////////////////////////////////////////////////////////////////////////////
#define CYCLE_DURATION 100 // One heartbeat in 100 SAMPLE_INTERVAL
#define SAMPLE_INTERVAL 1  // Adjustable sampling rate in ms
#define BASELINE 200	   // Baseline (y-offset)
#define AMPLITUDE 100	   // Maximum amplitude of the waveform
#define HEARTBEAT_PER_1000_MS 1
#define SAMPLE_PER_SECOND (HEARTBEAT_PER_1000_MS * CYCLE_DURATION)

// Function to simulate an integer-based sine wave (approximation)
int approx_sine_wave(int t, int period)
{
	t = t % period;
	int segment = period / 4;

	if (t < segment)
	{
		return (4 * AMPLITUDE * t) / period; // Rising
	}
	else if (t < 2 * segment)
	{
		return (2 * AMPLITUDE) - (4 * AMPLITUDE * t) / period; // Falling
	}
	else if (t < 3 * segment)
	{
		return -(4 * AMPLITUDE * (t - 2 * segment)) / period; // Falling
	}
	else
	{
		return -(2 * AMPLITUDE) + (4 * AMPLITUDE * t) / period; // Rising
	}
}

// Function to generate a random number within a range
int random_in_range(int min, int max)
{
	return min + (rand() % (max - min + 1));
}

// Function to generate one ECG value (keeps track of time internally)
int generate_ecg_sample()
{
	static int time = 0; // Keeps track of the current time step
	int cycle_time = time % CYCLE_DURATION;
	int value;

	// Introduce small random variations
	int rand_amplitude = AMPLITUDE + random_in_range(-10, 10);
	int rand_baseline = BASELINE + random_in_range(-5, 5);

	// P-Wave (small upward bump)
	if (cycle_time >= 0 && cycle_time < 10)
	{
		value = rand_baseline + (rand_amplitude / 4) * cycle_time / 10;
	}
	// PR Segment (flat line)
	else if (cycle_time >= 10 && cycle_time < 12)
	{
		value = rand_baseline;
	}
	// Q-Wave (small downward dip)
	else if (cycle_time >= 12 && cycle_time < 15)
	{
		value = rand_baseline - (rand_amplitude / 5) * (cycle_time - 12) / 3;
	}
	// R-Wave (sharp upward spike)
	else if (cycle_time >= 15 && cycle_time < 18)
	{
		value = rand_baseline + (rand_amplitude * 3 / 2) * (18 - cycle_time) / 3;
	}
	// S-Wave (sharp downward spike)
	else if (cycle_time >= 18 && cycle_time < 21)
	{
		value = rand_baseline - (rand_amplitude / 2) * (cycle_time - 18) / 3;
	}
	// ST Segment (flat line)
	else if (cycle_time >= 21 && cycle_time < 40)
	{
		value = rand_baseline;
	}
	// T-Wave (broad upward bump)
	else if (cycle_time >= 40 && cycle_time < 60)
	{
		value = rand_baseline + approx_sine_wave(cycle_time - 40, 20, rand_amplitude / 2);
	}
	// Baseline (flat line)
	else
	{
		value = rand_baseline;
	}

	time += SAMPLE_INTERVAL; // Advance time by the sample interval
	return value;
}

#define TOTAL_SAMPLES 1000
int ecg_data[TOTAL_SAMPLES];

/// The demo area //////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
	int x, y, w, h, x_end, y_end, x_mid, y_mid;
}app_box;

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

app_box lcd_size = { 0, 0, 1920, 1200, 1920, 1200 };
app_box app_window = INIT_APP_BOX ( (1920 - 1280) / 2, (1200 - 800) / 2, 1280, 800);
#define EVE_DRAW_AT(x, y) EVE_Cmd_wr32(s_pHalContext, VERTEX2F((x) * 1, (y) * 1))
#define VERTEX2F16(x, y) VERTEX2F(x / 16, y / 16)

void draw_app_window() {
	int border = 5;
	int bx = app_window.x;
	int by = app_window.y;
	int bw = app_window.w + border;
	int bh = app_window.h + border;
	char border_color[3] = { 134, 134, 134 };

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

/// SAMAPP_Demo_With_Linestrip //////////////////////////////////////////////////////////////////////////////////////
// ************************************ application ************************************
static uint8_t rate = 2;
static int16_t x, y, tx;
static uint8_t beats[10];
static int8_t beats_Incr[10] = { -10, 10, 5, -5, -20, 20, 12, -12, -5, 5 };
static uint16_t add2write = 0;

static int8_t istouch()
{
	return !(EVE_Hal_rd16(s_pHalContext, REG_TOUCH_RAW_XY) & 0x8000);
}

void Sine_wave(uint8_t amp)
{
	static uint8_t played = 0, change = 0;
	x += rate;
	if (x > s_pHalContext->Width)
		x = 0;
	y = (app_window.h / 2) + ((int32_t)amp * Math_Qsin(-65536 * x / (25 * rate)) / 65536);
	if (played == 0 && change < y)
	{
		played = 1;
		Play_Sound(s_pHalContext, (108 << 8 | 0x10), 50, 100);
	}
	if (change > y)
		played = 0;
	change = y;
	EVE_Hal_wr16(s_pHalContext, RAM_G + (x / rate) * 4, VERTEX2F(x, y));
}

void Sawtooth_wave(uint8_t amp)
{
	static uint16_t temp = 0;
	static uint8_t pk = 0;
	x += rate;
	if (x > s_pHalContext->Width)
	{
		x = 0;
	}
	temp += 2;
	if (temp > 65535L)
		temp = 0;
	y = (temp % amp);
	pk = y / (amp - 2);
	if (pk)
		Play_Sound(s_pHalContext, (108 << 8 | 0x10), 50, 100);
	y = (app_window.h / 2) - y;
	EVE_Hal_wr16(s_pHalContext, RAM_G + (x / rate) * 4, VERTEX2F(x, y));
}

void Triangle_wave(uint8_t amp)
{
	static uint16_t temp = 0;
	static uint8_t pk = 0, dc = 0, p = 0;
	x += rate;
	if (x > s_pHalContext->Width)
	{
		x = 0;
	}
	temp += 2;
	if (temp > 65535L)
		temp = 0;
	y = (temp % amp);
	pk = (y / (amp - 2)) % 2;
	dc = (temp / amp) % 2;
	if (pk)
	{
		if (p == 0)
		{
			p = 1;
			Play_Sound(s_pHalContext, (108 << 8 | 0x10), 50, 100);
		}
		else
			p = 0;
	}
	if (dc)
		y = (app_window.h / 2) - (amp - y);
	else
		y = (app_window.h / 2) - y;
	EVE_Hal_wr16(s_pHalContext, RAM_G + (x / rate) * 4, VERTEX2F(x, y));
}
static uint16_t temp_x, temp_p, temp_y, en;

void Heartbeat()
{
	uint16_t tval;
	y = app_window.h / 2;
	for (tval = 0; tval < 10; tval++)
	{
		y = y + (beats_Incr[tval] * 5);
		beats[tval] = y;
	}
	x += rate;
	if (x > app_window.w)
	{
		x = 0;
		temp_p = 0;
		temp_y = 0;
		y = app_window.h / 2;
		en = 0;
		temp_x = 0;
	}
	tx = 5 * rate;
	tx = ((temp_p + 1) * tx) + temp_p * temp_x;
	if (tx <= x)
	{
		if (0 == en)
			en = 1;
	}
	if (en == 1)
	{
		if (y != beats[temp_y])
		{
			y += beats_Incr[temp_y] * 5;
			temp_y++;
			if (y == (app_window.h / 2) + beats_Incr[4] * 5)
				Play_Sound(s_pHalContext, (108 << 8 | 0x10), 50, 100);
		}
		else
		{
			temp_y++;
			if (temp_y > 9)
			{
				temp_y = 0;
				temp_p++;
				en = 0;
				temp_x = x - tx;
			}
		}
	}
	EVE_Hal_wr32(s_pHalContext, RAM_G + (x / rate) * 4, VERTEX2F(x, y));
}

// Heartbeat function to print ECG data
int Heartbeat2()
{
	// Simulated sinus rhythm data (adjusted to resemble the waveform)
int ecg_data2[100] = {
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    // P-wave (small bump)
    0, 2, 4, 6, 8, 6, 4, 2, 0, 
    // PR segment (flat)
    0, 0, 0, 0, 0,
    // QRS complex (sharp spike)
    2, 4, 8, 20, 40, 70, 30, 10, 5, 2,
    // ST segment (flat)
    2, 2, 2, 2, 2, 2, 2,
    // T-wave (broad bump)
    2, 4, 6, 10, 14, 16, 14, 10, 6, 4, 2,
    // Baseline (flat)
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    
};
	static unsigned long last_time = 0; // Keeps track of the last print time
	static int index = 0; // Tracks current ECG data index
	unsigned long current_time = EVE_millis();

	if (last_time == 0)
	{
		last_time = current_time;
	}

	// Adjust sample rate: one data point every 100ms
	const unsigned long sample_rate = 10;

	int duration = current_time - last_time;
	last_time = current_time;
	
	int num_index = duration / sample_rate;
	int num_vertex = min(100, num_index + index);
	printf("ECG Data num: %d\n", num_vertex);
	if(0) for (int i=0; i < num_index;i++)
	{
		// Print the current ECG data point
		printf("ECG Data Point [%d]: %d\n", index, ecg_data2[index]);
		int y = ecg_data2[index];
		EVE_Hal_wr32(s_pHalContext, RAM_G + index * 4, VERTEX2F(index, y*40));

		// Update last_time and increment index
		index++;

		// Wrap around the index if it exceeds the array size
		if (index >= 100)
		{
			index = 0;
			break;
		}
	}

	int dot_num = 1000;
	int offset_nth = 0;
	int offset = (RAM_G + dot_num * 4) * offset_nth;
	if (1)
	for (int i = 0; i < 1000; i++) {
		int xc = i % 100;
		unsigned int y = ecg_data2[xc];
		printf("ECG Data Point [%d-(%d)]: %d\n", i, xc, 200 + y * 15);
		EVE_Hal_wr32(s_pHalContext, offset + i * 4, VERTEX2F(app_window.x + i, app_window.y + 200 - y * 2));
	}
	return num_vertex;
}

int PLETH()
{
	// Simulated sinus rhythm data (adjusted to resemble the waveform)
int ecg_data2[100] = {
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    // P-wave (small bump)
    0, 2, 4, 6, 8, 6, 4, 2, 0, 
    // PR segment (flat)
    0, 0, 0, 0, 0,
    // QRS complex (sharp spike)
    2, 4, 8, 20, 40, 70, 30, 10, 5, 2,
    // ST segment (flat)
    2, 2, 2, 2, 2, 2, 2,
    // T-wave (broad bump)
    2, 4, 6, 10, 14, 16, 14, 10, 6, 4, 2,
    // Baseline (flat)
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    
};
	static unsigned long last_time = 0; // Keeps track of the last print time
	static int index = 0; // Tracks current ECG data index
	unsigned long current_time = EVE_millis();

	if (last_time == 0)
	{
		last_time = current_time;
	}

	// Adjust sample rate: one data point every 100ms
	const unsigned long sample_rate = 10;

	int duration = current_time - last_time;
	last_time = current_time;
	
	int num_index = duration / sample_rate;
	int num_vertex = min(100, num_index + index);
	printf("ECG Data num: %d\n", num_vertex);
	if(0) for (int i=0; i < num_index;i++)
	{
		// Print the current ECG data point
		printf("ECG Data Point [%d]: %d\n", index, ecg_data2[index]);
		int y = ecg_data2[index];
		EVE_Hal_wr32(s_pHalContext, RAM_G + index * 4, VERTEX2F(index, y*40));

		// Update last_time and increment index
		index++;

		// Wrap around the index if it exceeds the array size
		if (index >= 100)
		{
			index = 0;
			break;
		}
	}

	int dot_num = 1000;
	int offset_nth = 1;
	int offset = (RAM_G + dot_num * 4) * offset_nth;

	for (int i = 0; i < 1000; i++) {
		int xc = i % 100;
		unsigned int y = ecg_data2[xc];
		printf("ECG Data Point [%d-(%d)]: %d\n", i, xc, 200 + y * 15);
		EVE_Hal_wr32(s_pHalContext, offset + i * 4, VERTEX2F(app_window.x + i, app_window.y + 400 - y * 2));
	}
	return num_vertex;
}

int CO2()
{
	// Simulated sinus rhythm data (adjusted to resemble the waveform)
int ecg_data2[100] = {
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    // P-wave (small bump)
    0, 2, 4, 6, 8, 6, 4, 2, 0, 
    // PR segment (flat)
    0, 0, 0, 0, 0,
    // QRS complex (sharp spike)
    2, 4, 8, 20, 40, 70, 30, 10, 5, 2,
    // ST segment (flat)
    2, 2, 2, 2, 2, 2, 2,
    // T-wave (broad bump)
    2, 4, 6, 10, 14, 16, 14, 10, 6, 4, 2,
    // Baseline (flat)
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    
};
	static unsigned long last_time = 0; // Keeps track of the last print time
	static int index = 0; // Tracks current ECG data index
	unsigned long current_time = EVE_millis();

	if (last_time == 0)
	{
		last_time = current_time;
	}

	// Adjust sample rate: one data point every 100ms
	const unsigned long sample_rate = 10;

	int duration = current_time - last_time;
	last_time = current_time;
	
	int num_index = duration / sample_rate;
	int num_vertex = min(100, num_index + index);
	printf("ECG Data num: %d\n", num_vertex);
	if(0) for (int i=0; i < num_index;i++)
	{
		// Print the current ECG data point
		printf("ECG Data Point [%d]: %d\n", index, ecg_data2[index]);
		int y = ecg_data2[index];
		EVE_Hal_wr32(s_pHalContext, RAM_G + index * 4, VERTEX2F(index, y*40));

		// Update last_time and increment index
		index++;

		// Wrap around the index if it exceeds the array size
		if (index >= 100)
		{
			index = 0;
			break;
		}
	}

	int dot_num = 1000;
	int offset_nth = 2;
	int offset = (RAM_G + dot_num * 4) * offset_nth;

	for (int i = 0; i < 1000; i++) {
		int xc = i % 100;
		unsigned int y = ecg_data2[xc];
		printf("ECG Data Point [%d-(%d)]: %d\n", i, xc, 200 + y * 15);
		EVE_Hal_wr32(s_pHalContext, offset + i * 4, VERTEX2F(app_window.x + i, app_window.y + 600 - y * 2));
	}
	return num_vertex;
}

void Gpu_Radiobutton(int16_t x, int16_t y, uint32_t bgcolor, uint32_t fgcolor, uint8_t psize, uint8_t tag, uint8_t option)
{
	uint8_t check_size = psize / 2;
	EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB((bgcolor >> 16 & 0xff), (bgcolor >> 8 & 0xff), (bgcolor & 0xff)));
	EVE_Cmd_wr32(s_pHalContext, TAG(tag));
	EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(psize * 16));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(POINTS));
	EVE_DRAW_AT(x , y );
	if (tag == option)
	{
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB((fgcolor >> 16 & 0xff), (fgcolor >> 8 & 0xff), (fgcolor & 0xff)));
		EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(check_size * 16));
		EVE_DRAW_AT(x , y );
	}
	EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
}

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
			temp_tag = 0; //The event will be processed. Clear the tag
		}
		sk = 0;
	}
	return ret_tag;
}


// Function to calculate and return FPS without float/double
int getFPS()
{
	static unsigned long last_time_ms = 0; // Timestamp of the last frame
	static unsigned long frame_count = 0; // Total frames since start
	static int fps = 0; // Calculated FPS

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

// Integer to string conversion without reversing
void int_to_string(int value, char *buffer)
{
	char *ptr = buffer;

	// Handle negative numbers
	if (value < 0)
	{
		*ptr++ = '-'; // Add negative sign
		value = -value;
	}

	// Find the number of digits
	int temp = value;
	int num_digits = 1;
	while (temp >= 10)
	{
		num_digits++;
		temp /= 10;
	}

	// Write digits starting from the left
	ptr[num_digits] = '\0'; // Null-terminate the string
	for (int i = num_digits - 1; i >= 0; i--)
	{
		ptr[i] = (value % 10) + '0'; // Place digits
		value /= 10;
	}
}

int my_sprintf(char *output, const char *format, int value)
{
	// Initialize pointers for output and format
	char *out_ptr = output;
	const char *fmt_ptr = format;

	while (*fmt_ptr)
	{
		if (*fmt_ptr == '%' && *(fmt_ptr + 1) == 'd')
		{
			// Found %d, convert the integer to string
			char num_buffer[12];
			int_to_string(value, num_buffer);

			// Append the integer string to the output
			strcpy(out_ptr, num_buffer);
			out_ptr += strlen(num_buffer);

			// Skip the %d in the format
			fmt_ptr += 2;
		}
		else
		{
			// Copy other characters as-is
			*out_ptr++ = *fmt_ptr++;
		}
	}

	// Null-terminate the output string
	*out_ptr = '\0';

	// Return the length of the output string
	return strlen(output);
}

void SAMAPP_Demo_With_Linestrip()
{
	int16_t i, xx = 0;
	uint16_t tval, th_to, amp;
	uint8_t temp[480], hide_x = 0, fg = 1, tag, opt = 6;
	// ========  Bg =========================================================
	for (tval = 0; tval <= app_window.h / 2; tval++)
	{
		temp[app_window.h - tval] = temp[tval] = (tval * 0.90);
	}
	EVE_Hal_wrMem(s_pHalContext, s_pHalContext->Width * 4L, temp, sizeof(temp));

	y = app_window.h / 2;
	for (tval = 0; tval < 10; tval++)
	{
		y = y + (beats_Incr[tval] * 5);
		beats[tval] = y;
	}

	for (tval = 0; tval < s_pHalContext->Width; tval += rate)
	{
		EVE_Hal_wr32(s_pHalContext, RAM_G + ((tval / rate) * 4), VERTEX2F(tval , y ));
	}
	add2write = 0;

	Heartbeat2();
	PLETH();
	CO2();
	while (1)
	{
		Display_StartColor(s_pHalContext, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 0, 0, 0 });
		EVE_Cmd_wr32(s_pHalContext, VERTEX_FORMAT(0));

		EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
		EVE_DRAW_AT(0, 0);
		EVE_DRAW_AT(s_pHalContext->Width, s_pHalContext->Height);
		draw_app_window();

		if(0){
			EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(s_pHalContext->Width * 4L));
			EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L8, 1, app_window.h));
			EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT_H(0, app_window.h >> 9));
			EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, REPEAT, BORDER, s_pHalContext->Width, app_window.h));
			EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H(s_pHalContext->Width >> 9, app_window.h >> 9));
			EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
			EVE_Cmd_wr32(s_pHalContext, TAG(0));
			EVE_DRAW_AT(0, 0);
		}

		if (1)
		{
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x1B, 0xE0, 0x67));
			EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(2 * 16));
			EVE_Cmd_wr32(s_pHalContext, BEGIN(LINE_STRIP));
			EVE_CoCmd_append(s_pHalContext, RAM_G + 1000 * 4 * 0, 1000 * 4);
			EVE_Cmd_wr32(s_pHalContext, END());
		}

		if (1)
		{
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x1B, 0xE0, 0x67));
			EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(2 * 16));
			EVE_Cmd_wr32(s_pHalContext, BEGIN(LINE_STRIP));
			EVE_CoCmd_append(s_pHalContext, RAM_G + 1000 * 4 * 1, 1000 * 4);
			EVE_Cmd_wr32(s_pHalContext, END());
		}

		if (1)
		{
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x1B, 0xE0, 0x67));
			EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(2 * 16));
			EVE_Cmd_wr32(s_pHalContext, BEGIN(LINE_STRIP));
			EVE_CoCmd_append(s_pHalContext, RAM_G + 1000 * 4 * 2, 1000*4);
			EVE_Cmd_wr32(s_pHalContext, END());
		}

		if(1){
			EVE_Cmd_wr32(s_pHalContext, BEGIN(LINE_STRIP));
			if ((x / rate) < (s_pHalContext->Width / rate) - (50 / rate)) // else it screw up
				EVE_CoCmd_append(s_pHalContext, RAM_G + (x / rate) * 4 + ((50 / rate) * 4), ((s_pHalContext->Width / rate) * 4) - ((x / rate) * 4) - ((50 / rate) * 4));
			EVE_Cmd_wr32(s_pHalContext, END());

			EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(6 * 16));
			EVE_Cmd_wr32(s_pHalContext, BEGIN(POINTS));
			EVE_DRAW_AT(x, y);
			EVE_Cmd_wr32(s_pHalContext, END());
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
			EVE_Cmd_wr32(s_pHalContext, COLOR_A(100));
			EVE_Cmd_wr32(s_pHalContext, BEGIN(EDGE_STRIP_R));
			EVE_DRAW_AT((hide_x + s_pHalContext->Width - 100), 0);
			EVE_DRAW_AT((hide_x + s_pHalContext->Width - 100), app_window.h);
			EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
			Gpu_Radiobutton(hide_x + s_pHalContext->Width - 82, app_window.h - 90, 0xffffff, 0, 14, 3, opt);
			Gpu_Radiobutton(hide_x + s_pHalContext->Width - 82, app_window.h - 55, 0xffffff, 0, 14, 4, opt);
			Gpu_Radiobutton(hide_x + s_pHalContext->Width - 82, app_window.h - 20, 0xffffff, 0, 14, 5, opt);
			Gpu_Radiobutton(hide_x + s_pHalContext->Width - 82, app_window.h - 125, 0xffffff, 0, 14, 6, opt);
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 0, 0));

			char fps_str[30];
			my_sprintf(fps_str, "fps = %d", getFPS());
			EVE_CoCmd_text(s_pHalContext, app_window.x + 10, app_window.y+20, 30, OPT_CENTERY, fps_str);

			EVE_CoCmd_text(s_pHalContext, hide_x + s_pHalContext->Width - 65, app_window.h - 90, 26, OPT_CENTERY, "Sine");
			EVE_CoCmd_text(s_pHalContext, hide_x + s_pHalContext->Width - 65, app_window.h - 55, 26, OPT_CENTERY, "Sawtooth");
			EVE_CoCmd_text(s_pHalContext, hide_x + s_pHalContext->Width - 65, app_window.h - 20, 26, OPT_CENTERY, "Triangle");
			EVE_CoCmd_text(s_pHalContext, hide_x + s_pHalContext->Width - 65, app_window.h - 125, 26, OPT_CENTERY, "ECG");
			EVE_CoCmd_text(s_pHalContext, (hide_x + s_pHalContext->Width - 70), 20, 30, OPT_CENTERY | OPT_CENTERX, "-");
			EVE_CoCmd_text(s_pHalContext, (hide_x + s_pHalContext->Width - 25), 20, 30, OPT_CENTERY | OPT_CENTERX, "+");
			EVE_CoCmd_text(s_pHalContext, (hide_x + s_pHalContext->Width - 85), 50, 28, 0, "Rate:");
			EVE_CoCmd_number(s_pHalContext, (hide_x + s_pHalContext->Width - 35), 50, 28, 0, rate);
			EVE_CoCmd_text(s_pHalContext, (hide_x + s_pHalContext->Width - 85), 80, 28, 0, "Pk:");
			//EVE_CoCmd_number(s_pHalContext, (hide_x + s_pHalContext->Width - 45), 80, 28, 0, amp);
			EVE_Cmd_wr32(s_pHalContext, COLOR_A(50));
			EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(15 * 16));
			EVE_Cmd_wr32(s_pHalContext, BEGIN(POINTS));
			EVE_Cmd_wr32(s_pHalContext, TAG(1));
			EVE_DRAW_AT((hide_x + s_pHalContext->Width - 70), 20);
			EVE_Cmd_wr32(s_pHalContext, TAG(2));
			EVE_DRAW_AT((hide_x + s_pHalContext->Width - 25), 20);
		}
		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);
		//==========================End==================================================
	}
}

#endif