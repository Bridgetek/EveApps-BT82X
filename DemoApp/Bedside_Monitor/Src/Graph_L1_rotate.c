#include "Helpers.h"
#include "Bedside_Monitor.h"

extern EVE_HalContext s_halContext;
extern EVE_HalContext* s_pHalContext;

int new_data_heartbeat(int** data, int* data_size);
int new_data_pleth(int** data, int* data_size);
int new_data_co2(int** data, int* data_size);

#define GRAPH_BIT_PER_PIXEL 1 // l1 1 bit per pixel
#define GRAPH_BIT_PER_LINE (GRAPH_BIT_PER_PIXEL * GRAPH_W) // bbp * width
#define GRAPH_BYTE_PER_LINE (GRAPH_BIT_PER_LINE / BIT_PER_CHAR)
#define GRAPH_BYTE_PER_BUFFER (GRAPH_H * (GRAPH_BIT_PER_LINE / BIT_PER_CHAR))
#define GRAPH_BUFFER_NUM 3 // display from buffer 1, append to buffer 2, loopback to buffer 0
#define GRAPH_BUFFER_SIZE (GRAPH_BYTE_PER_BUFFER * GRAPH_BUFFER_NUM)

typedef struct {
	int handler;
	int bitmap_rp; // bitmap read pointer, use buffer 0 and 1
	int bitmap_wp; // bitmap write pointer, use buffer 1 and 2
	int bitmap_wplb; // bitmap write pointer loopback, use buffer 0
	int buffer0, buffer1, buffer2; // 3 buffer on ramg
	int buffer2_end; // end addreff of buffer 2
	int x, y, w, h;
	uint32_t rgba;
	int x_graph_last;
}app_graph_t;

static app_graph_t graph_heartbeat;
static app_graph_t graph_pleth;
static app_graph_t graph_co2;

static void draw_pixel(SIGNALS_DATA_TYPE* buffer, int x, int y, uint32_t rgba)
{
	if (y > GRAPH_H * 2) {
		goto err;
	}
	
	if (x < 0) {
		goto err;
	}
	if (x < 0 || x >= GRAPH_W)
	{
		goto err;
	}

	// set pixel color on/off
	int pixel_nth = x + y * GRAPH_W;
	int byte_index = pixel_nth / 8;    // byte nth
	int bit_index = 7 - pixel_nth % 8;    // bit nth

	if(byte_index >= GRAPH_BYTE_PER_LINE * g_graph_zoom_lv) {
		goto err;
	}
	
	buffer[byte_index] |= (1 << bit_index);

err:
	//printf("Skip a pixel at (%d, %d)\n", x, y);
	return;
}

static void bresenham_linex(char* buffer, int x1, int y1, int x2, int y2, uint32_t rgba)
{
	int sx = (x2 > x1) ? 1 : -1;
	int sy = (y2 > y1) ? 1 : -1;
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	while (dx-- > -1)
	{
		draw_pixel(buffer, x1, y1, rgba);
		x1 += sx;
	}
}

static void bresenham_line(SIGNALS_DATA_TYPE* buffer, int x1, int y1, int x2, int y2, uint32_t rgba)
{
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	int sx = (x2 > x1) ? 1 : -1;
	int sy = (y2 > y1) ? 1 : -1;
	int err = dx - dy;

	while (1)
	{
		// Draw the current pixel
		draw_pixel(buffer, x1, y1, rgba);

		// Check if the endpoint is reached
		if (x1 == x2 && y1 == y2) {
			break;
		}

		int e2 = 2 * err;

		// Adjust error and coordinates
		if (e2 > -dy) {
			err -= dy;
			x1 += sx;
		}
		if (e2 < dx) {
			err += dx;
			y1 += sy;
		}
	}
}


// Function to normalize sensor data to fit within the graph range
static int normalize_to_graph(app_graph_t* graph, int sensor_value, int sensor_min, int sensor_max) {
#define graph_max graph->h
#define graph_min 1

	// Ensure the sensor range is valid to prevent division by zero
	if (sensor_max == sensor_min) {
		return 0; // Default to minimum y-value if range is zero
	}

	/* Normalize the sensor value to the range[graph_min, graph_max]
	   here is the formular:
		 data range:  sensor_min  --> sensor_value ------> sensor_max
		 graph range: graph_min   --> x            ------> graph_max
		 find x: => (graph_max - x) / (graph_max - graph_min) = (sensor_max - sensor_value) / (sensor_max - sensor_min)
		 find x: => (graph_max - x) = (graph_max - graph_min) * (sensor_max - sensor_value) / (sensor_max - sensor_min)
		 find x: => x = graph_max - (graph_max - graph_min) * (sensor_max - sensor_value) / (sensor_max - sensor_min)
	*/
	return graph_max - (graph_max - graph_min) * (sensor_max - sensor_value) / (sensor_max - sensor_min);
}

static void graph_append(app_graph_t* graph, SIGNALS_DATA_TYPE* lines, int line_count) {
	uint8_t buffer_1line[GRAPH_BYTE_PER_LINE * GRAPH_ZOOM_LV_MAX] = { {0} };
	uint32_t addr = graph->bitmap_wp;
	for (int i = 0; i < line_count; i++) {
		memset(buffer_1line, 0, sizeof(buffer_1line));
		SIGNALS_DATA_TYPE x = lines[i] & 0xFF;
		x = normalize_to_graph(graph, x, 0, 255);
		bresenham_line(buffer_1line, x, g_graph_zoom_lv - 1, graph->x_graph_last, 0, graph->rgba);
		graph->x_graph_last = x;
		EVE_Hal_wrMem(s_pHalContext, addr, buffer_1line, GRAPH_BYTE_PER_LINE * g_graph_zoom_lv);
		addr += GRAPH_BYTE_PER_LINE * g_graph_zoom_lv;
	}
}

static void graph_display(app_graph_t* graph) {
	int bformat = L1;
	int lw = max(graph->w, GRAPH_W);
	int lh = max(graph->w, GRAPH_W);
#define MAX_ANGLE 360
#define MAX_CIRCLE_UNIT 65536
	int rotation_angle = -90;

	// display bitmap
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	EVE_CoCmd_setBitmap(s_pHalContext, graph->bitmap_rp, bformat, GRAPH_W, graph->w);
	EVE_Cmd_wr32(s_pHalContext, PALETTE_SOURCE(0));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, lw, lh));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H(lw >> 9, lh >> 9));
	EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());
	EVE_CoCmd_loadIdentity(s_pHalContext);
	EVE_CoCmd_translate(s_pHalContext, 0, GRAPH_W * MAX_CIRCLE_UNIT);
	EVE_CoCmd_rotate(s_pHalContext, rotation_angle * MAX_CIRCLE_UNIT / MAX_ANGLE);
	EVE_CoCmd_setMatrix(s_pHalContext);

	// coloring
	int r = (graph->rgba >> 24) & 0xFF;  // Extract Red (most significant byte)
	int g = (graph->rgba >> 16) & 0xFF;  // Extract Green
	int b = (graph->rgba >> 8) & 0xFF;   // Extract Blue
	int a = graph->rgba & 0xFF;          // Extract Alpha (least significant byte)
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(r, g, b));

	// vertex
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	int x = graph->x;
	int y = graph->y - (GRAPH_W - graph->h);
	EVE_DRAW_AT(x, y);
	EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
}

static void graph_append_and_display(app_graph_t* graph, SIGNALS_DATA_TYPE* lines, int line_count)
{
	// write data to ramg
	while (line_count > 0 && btnStartState == BTN_START_INACTIVE)
	{
		int line_count_graph = line_count * g_graph_zoom_lv;
		int bytes_count_graph = line_count_graph * GRAPH_BYTE_PER_LINE;

		// Calculate how much data can be written continuously to the buffer
		int contiguous_space = graph->buffer2_end - graph->bitmap_wp;
		int bytes_to_write = (bytes_count_graph < contiguous_space) ? bytes_count_graph : contiguous_space;
		int line_to_write = bytes_to_write / GRAPH_BYTE_PER_LINE;   //(line_count_graph < contiguous_space) ? line_count_graph : contiguous_space;

		// if data size >= buffer2, write to buffer0, and set rp with gap
		if (graph->bitmap_wp + bytes_to_write >= graph->buffer2_end)
		{
			int buffer2_gap = graph->bitmap_wp + bytes_to_write - graph->buffer2_end;
			graph->bitmap_wp = graph->bitmap_wplb;
			graph_append(graph, lines, line_to_write / g_graph_zoom_lv);

			graph->bitmap_rp = graph->buffer0 + buffer2_gap;
			graph->bitmap_wplb = graph->buffer0;
			continue;
		}

		graph_append(graph, lines, line_to_write / g_graph_zoom_lv);

		if (graph->bitmap_wp + bytes_to_write >= graph->buffer2)
		{ // loopback
			EVE_CoCmd_memCpy(s_pHalContext, graph->bitmap_wplb, graph->bitmap_wp, bytes_to_write);
			graph->bitmap_wplb += bytes_to_write;
		}

		// increase write pointer
		graph->bitmap_rp += bytes_to_write;
		graph->bitmap_wp += bytes_to_write;
		lines += line_to_write / g_graph_zoom_lv;
		line_count -= line_to_write / g_graph_zoom_lv;
	}

	graph_display(graph);
}

uint32_t graph_l1_rotate_init(app_box* box_heartbeat, app_box* box_pleth, app_box* box_co2) {
	//      <-------- read pointer -------->x<- when write pointer reached here, start loopback to buffer 0
	//      -------------------------------------------------         
	//      |   buffer 0    |   buffer 1    |   buffer 2    |         
	//      -------------------------------------------------         
	//                      <-------- write pointer -------->
	app_graph_t* graphs[] = { &graph_heartbeat , &graph_pleth , &graph_co2 };
	app_box* boxs[] = { box_heartbeat , box_pleth , box_co2 };
	int colors[] = { 0x00ff0000, 0x00ffff00, 0xffff0000 };

	for (int i = 0; i < 3; i++) {
		app_graph_t* gh = graphs[i];
		gh->buffer0 = i * GRAPH_BUFFER_SIZE; // loopback buffer
		gh->buffer1 = gh->buffer0 + GRAPH_BYTE_PER_BUFFER;
		gh->buffer2 = gh->buffer1 + GRAPH_BYTE_PER_BUFFER;
		gh->buffer2_end = gh->buffer2 + GRAPH_BYTE_PER_BUFFER;

		gh->bitmap_rp = gh->buffer0;  // display from buffer 0 and continue to buffer 1
		gh->bitmap_wp = gh->buffer1;  // append to buffer 1 and continue to buffer 2
		gh->bitmap_wplb = gh->buffer0;  // loopback from buffer 0
		gh->x = boxs[i]->x;
		gh->y = boxs[i]->y;
		gh->w = boxs[i]->w;
		gh->h = boxs[i]->h;
		gh->handler = i+1;
		gh->rgba = colors[i];
		EVE_CoCmd_memSet(s_pHalContext, gh->buffer0, 0, GRAPH_BUFFER_SIZE);
	}
	return graph_co2.buffer2_end;
}

void graph_l1_rotate_draw() {
	SIGNALS_DATA_TYPE* data_heartbeat;
	SIGNALS_DATA_TYPE* data_pleth;
	SIGNALS_DATA_TYPE* data_co2;
	int data_heartbeat_size = 0;
	int data_pleth_size = 0;
	int data_co2_size = 0;

	int x1 = new_data_heartbeat(&data_heartbeat, &data_heartbeat_size);
	int x2 = new_data_pleth(&data_pleth, &data_pleth_size);
	int x3 = new_data_co2(&data_co2, &data_co2_size);

	graph_append_and_display(&graph_heartbeat, data_heartbeat, data_heartbeat_size);
	graph_append_and_display(&graph_pleth, data_pleth, data_pleth_size);
	graph_append_and_display(&graph_co2, data_co2, data_co2_size);
}