#ifndef HELPERS_H_
#define HELPERS_H_

#include "EVE_Platform.h"

#define ALIGN_TWO_POWER_N(Value, alignval)  (((Value) + (alignval - 1)) & (~(alignval - 1)))

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

// Function to calculate and return FPS without float/double
int getFPS();

// Function to format the date as "dd-mm-yyyy"
char* dd_mm_yyyy();

// Function to format the time as "hh:mm"
char* hh_mm();

// Function to calculate and return the formatted date-time string
char* dd_mm_yyyy_hh_m_ss_ms();

// Function to initialize date-time
void init_datetime(int dd, int mm, int yyyy, int hh, int m, int ss, int ms);

// Function to generate a random number within a given range
int app_random(int range);

// Function to save buffer data to a file
int save_buffer_to_file(const char* filename, const void* buffer, size_t buffer_size);

// Function to take a DDR screenshot
void take_ddr_screenshot(EVE_HalContext* phost, char* name, int ramg_render);

#endif /* HELPERS_H_ */
