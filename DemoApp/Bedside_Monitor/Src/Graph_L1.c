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

	uint32_t accumulator;// To accumulate 4 bytes
	int byte_count;// Count of bytes collected
	int addr_tf;//transfer address

	int x_graph_last;
}app_graph_t;

static app_graph_t graph_heartbeat;
static app_graph_t graph_pleth;
static app_graph_t graph_co2;

void set_pixel(uint8_t* buffer, int w, int h, int x, int y) {
	int pixel_nth = x + y * w;
	int byte_index = pixel_nth / 8;    // byte nth
	int bit_index = 7 - pixel_nth % 8; // bit nth 
	buffer[byte_index] |= (1 << bit_index);
}

void clear_pixel(uint8_t* buffer, int w, int h, int x, int y) {
	int pixel_nth = x + y * w;
	int byte_index = pixel_nth / 8;    // byte nth
	int bit_index = 7 - pixel_nth % 8; // bit nth 
	buffer[byte_index] |= ~(1 << bit_index);
}

static void draw_pixel(char* buffer, int x, int y, uint32_t rgba)
{
	if (y > GRAPH_H * 2)
		return;
	if (x < 0)
		//printf("Skip a pixel at (%d, %d)\n", x, y);


	if (x < 0 || x >= GRAPH_W)
	{
		return;
	}

	// set pixel color on/off
	set_pixel(buffer, GRAPH_H, GRAPH_W, x, y);
}

static void bresenham_line(char* buffer, int x1, int y1, int x2, int y2, uint32_t rgba)
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

// Function to normalize sensor data to fit within the graph range
static int normalize_to_graph(int sensor_value, int sensor_min, int sensor_max) {
#define graph_max GRAPH_W
#define graph_min 5

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

static void graph_append(app_graph_t* graph, int* lines, int line_to_write) {
	uint8_t buffer_1line[GRAPH_BYTE_PER_LINE] = { {0} };
	uint32_t addr = graph->bitmap_wp;
	for (int i = 0; i < line_to_write; i++) {
		memset(buffer_1line, 0, sizeof(buffer_1line));
		int x = lines[i] & 0xFF;

		x = normalize_to_graph(x, 0, 255);
		bresenham_line(buffer_1line, x, 0, graph->x_graph_last, 1, graph->rgba);
		graph->x_graph_last = x;
		// EVE_Hal_wrMem(s_pHalContext, addr, buffer_1line, sizeof(buffer_1line));
		for (int j = 0; j < sizeof(buffer_1line); j++) {
			if (buffer_1line[j] == 0) {
				continue;
			}


		}
		addr += GRAPH_BYTE_PER_LINE;
	}
}

static void graph_display(app_graph_t* graph) {
	int bformat = L1;
#ifdef BT82X_ENABLE
	bformat = PALETTEDARGB8;
#endif
	int lw = max(GRAPH_H, GRAPH_W);
	int lh = max(GRAPH_H, GRAPH_W);

	// display bitmap
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	EVE_CoCmd_setBitmap(s_pHalContext, graph->bitmap_rp, bformat, GRAPH_W, GRAPH_H);
	EVE_Cmd_wr32(s_pHalContext, PALETTE_SOURCE(0));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, lw, lh));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H(lw >> 9, lh >> 9));

	// coloring
	int r = (graph->rgba >> 24) & 0xFF;  // Extract Red (most significant byte)
	int g = (graph->rgba >> 16) & 0xFF;  // Extract Green
	int b = (graph->rgba >> 8) & 0xFF;   // Extract Blue
	int a = graph->rgba & 0xFF;          // Extract Alpha (least significant byte)
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(r, g, b));

	// vertex
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_DRAW_AT(graph->x, graph->y);
}

static void graph_append_and_display(app_graph_t* graph, int* lines, int line_count)
{
	// write data to ramg
	while (line_count > 0)
	{
		// Calculate how much data can be written continuously to the buffer
		int contiguous_space = graph->buffer2_end - graph->bitmap_wp;
		int line_to_write = (line_count < contiguous_space) ? line_count : contiguous_space;

		// if data size >= buffer2, write to buffer0, and set rp with gap
		if (graph->bitmap_wp + line_to_write >= graph->buffer2_end)
		{
			int buffer2_gap = graph->bitmap_wp + line_to_write - graph->buffer2_end;
			graph->bitmap_wp = graph->bitmap_wplb;
			graph_append(graph, lines, line_to_write);

			graph->bitmap_rp = graph->buffer0 + buffer2_gap;
			graph->bitmap_wplb = graph->buffer0;
			continue;
		}

		graph_append(graph, lines, line_to_write);

		if (graph->bitmap_wp + line_to_write >= graph->buffer2)
		{ // loopback
			EVE_CoCmd_memCpy(s_pHalContext, graph->bitmap_wplb, graph->bitmap_wp, line_to_write * GRAPH_BYTE_PER_LINE);
			graph->bitmap_wplb += line_to_write * GRAPH_BYTE_PER_LINE;
		}

		// increase write pointer
		graph->bitmap_rp += line_to_write * GRAPH_BYTE_PER_LINE;
		graph->bitmap_wp += line_to_write * GRAPH_BYTE_PER_LINE;
		lines += line_to_write;
		line_count -= line_to_write;
	}

	graph_display(graph);
}

void graph_l1_init(app_box* box_heartbeat, app_box* box_pleth, app_box* box_co2) {
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
		gh->x = boxs[i]->x + 10;
		gh->y = boxs[i]->y - 60;
		gh->w = GRAPH_W;
		gh->h = GRAPH_H;
		gh->handler = i+1;
		gh->rgba = colors[i];
		gh->accumulator = 0;
		gh->byte_count = 0;
		gh->addr_tf = gh->bitmap_wp;
		EVE_CoCmd_memSet(s_pHalContext, gh->buffer0, 0, GRAPH_BUFFER_SIZE);
		EVE_CoCmd_memSet(s_pHalContext, gh->buffer1, 0, GRAPH_BUFFER_SIZE);
		EVE_CoCmd_memSet(s_pHalContext, gh->buffer2, 0, GRAPH_BUFFER_SIZE);
	}
}

void graph_l1_draw() {
	int* data_heartbeat;
	int* data_pleth;
	int* data_co2;
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