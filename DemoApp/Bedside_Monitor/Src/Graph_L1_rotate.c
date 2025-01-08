  /// #include "Helpers.h"
  /// #include "Demo_Bedside_Monitor_81x.h"
  /// 
  /// extern EVE_HalContext s_halContext;
  /// extern EVE_HalContext* s_pHalContext;
  /// 
  /// int new_data_heartbeat(int** data, int* data_size);
  /// int new_data_pleth(int** data, int* data_size);
  /// int new_data_co2(int** data, int* data_size);
  /// 
  /// #define GRAPH_BIT_PER_PIXEL   1 // L1 1 bit per pixel, 1 pixel per line, value 0-1
  /// #define GRAPH_BIT_PER_LINE  (GRAPH_W * GRAPH_BIT_PER_PIXEL)
  /// #define GRAPH_BYTE_PER_LINE (ALIGN_TO_8(GRAPH_BIT_PER_LINE / BIT_PER_CHAR))
  /// #define GRAPH_BYTE_PER_BUFFER (GRAPH_H * GRAPH_BYTE_PER_LINE)
  /// #define GRAPH_BUFFER_NUM 3 // display from buffer 1, append to buffer 2, loopback to buffer 0
  /// #define GRAPH_BUFFER_SIZE (GRAPH_BYTE_PER_BUFFER * GRAPH_BUFFER_NUM) 
  /// 
  /// typedef struct {
  /// 	int handler;
  /// 	int bitmap_rp; // bitmap read pointer, use buffer 0 and 1
  /// 	int bitmap_wp; // bitmap write pointer, use buffer 1 and 2
  /// 	int bitmap_wplb; // bitmap write pointer loopback, use buffer 0
  /// 	int buffer0, buffer1, buffer2; // 3 buffer on ramg
  /// 	int buffer2_end; // end addreff of buffer 2
  /// 	int x, y, w, h;
  /// 	int rgba;
  /// 
  /// 	uint32_t accumulator;// To accumulate 4 bytes
  /// 	int byte_count;// Count of bytes collected
  /// 	int addr_tf;//transfer address
  /// 
  /// 	int lastx, lasty;
  /// 	int color_offset;
  /// }app_graph_t;
  /// app_graph_t graph_heartbeat;
  /// app_graph_t graph_pleth;
  /// app_graph_t graph_co2;
  /// 
  /// void set_color(uint8_t* line_buffer, char color) {
  /// 	line_buffer[0] = color;
  /// }
  /// 
  /// void draw_pixel(uint8_t* line_buffer, int x, int y, int color_offset)
  /// {
  /// 	if (y > GRAPH_H * 2)
  /// 		return;
  /// 
  /// 	if (x < 0 || x >= GRAPH_W)
  /// 	{
  /// 		return;
  /// 	}
  /// 	set_color(line_buffer + x, color_offset);
  /// }
  /// 
  /// void bresenham_line(uint8_t *line_buffer, int x1, int y1, int x2, int y2, int color_offset)
  /// {
  /// 	int sx = (x2 > x1) ? 1 : -1;
  /// 	int sy = (y2 > y1) ? 1 : -1;
  /// 	int dx = abs(x2 - x1);
  /// 	int dy = abs(y2 - y1);
  /// 	while (dx-- > -1)
  /// 	{
  /// 		draw_pixel(ramg_buffer, x1, y1, color_offset);
  /// 		x1 += sx;
  /// 	}
  /// }
  /// 
  /// void graph_transfer_queue(app_graph_t* graph, int* lines, int line_count) {
  /// 	for (int i = 0; i < line_count; i++) {
  /// 		uint8_t line_buffer[GRAPH_BYTE_PER_LINE] = { {0} };
  /// 		// Get the current byte (considering `data` as an int array)
  /// 		int byte = lines[i] & 0xFF;
  /// 
  /// 		// get sample (x,y) coordinate
  /// 		int pixel_y = i;
  /// 		int pixel_x = lines[(pixel_y)];
  /// 
  /// 		// Draw graph to buffer
  /// 		bresenham_line(line_buffer, pixel_x, pixel_y, graph->lastx, graph->lasty, color_offset);
  /// 		graph->lastx = pixel_x;
  /// 		graph->lasty = pixel_y;
  /// 	}
  /// 
  /// }
  /// 
  /// void graph_L1(int bitmap_addr, int color_source, int x, int y) {
  /// 	int bformat = L1;
  /// 	int lw = max(GRAPH_H, GRAPH_W);
  /// 	int lh = max(GRAPH_H, GRAPH_W);
  /// #define MAX_ANGLE 360
  /// #define MAX_CIRCLE_UNIT 65536
  /// 	int rotation_angle = -90;
  /// 
  /// 	EVE_CoCmd_setBitmap(s_pHalContext, bitmap_addr, bformat, GRAPH_W, GRAPH_H);
  /// 	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, lw, lh));
  /// 	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H(lw >> 9, lh >> 9));
  /// 	EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());
  /// 	EVE_CoCmd_loadIdentity(s_pHalContext);
  /// 	EVE_CoCmd_translate(s_pHalContext, 0, GRAPH_W * MAX_CIRCLE_UNIT);
  /// 	EVE_CoCmd_rotate(s_pHalContext, rotation_angle * MAX_CIRCLE_UNIT / MAX_ANGLE);
  /// 	EVE_CoCmd_setMatrix(s_pHalContext);
  /// 	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
  /// 	EVE_DRAW_AT(x, y);
  /// 	EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
  /// }
  /// 
  /// void graph_append_and_display(app_graph_t* graph, int* lines, int line_count) {
  /// 	// write data to ramg
  /// 	while (line_count > 0) {
  /// 		// Calculate how much data can be written continuously to the buffer
  /// 		int contiguous_space = graph->buffer2_end - graph->bitmap_wp;
  /// 		int line_to_write = (line_count < contiguous_space) ? line_count : contiguous_space;
  /// 
  /// 		// if data size >= buffer2, write to buffer0, and set rp according
  /// 		if (graph->bitmap_wp + line_to_write >= graph->buffer2_end) {
  /// 			int buffer2_gap = graph->bitmap_wp + line_to_write - graph->buffer2_end;
  /// 			graph->bitmap_wp = graph->bitmap_wplb;
  /// 			graph_transfer_queue(graph, lines, line_to_write);
  /// 
  /// 			graph->bitmap_rp = graph->buffer0 + buffer2_gap;
  /// 			graph->bitmap_wplb = graph->buffer0;
  /// 			continue;
  /// 		}
  /// 
  /// 		graph_transfer_queue(graph, lines, line_to_write);
  /// 
  /// 		if (graph->bitmap_wp + line_to_write >= graph->buffer2) { // loopback
  /// 			EVE_CoCmd_memCpy(s_pHalContext, graph->bitmap_wplb, graph->bitmap_wp, line_to_write);
  /// 			graph->bitmap_wplb += line_to_write;
  /// 		}
  /// 
  /// 		// increase write pointer
  /// 		graph->bitmap_rp += line_to_write;
  /// 		graph->bitmap_wp += line_to_write;
  /// 		lines += line_to_write;
  /// 		line_count -= line_to_write;
  /// 	}
  /// 
  /// 	graph_L1(graph->bitmap_rp, color_source, graph->x, graph->y);
  /// }
  /// 
  /// void graph_bargraph_init(app_box* box_heartbeat, app_box* box_pleth, app_box* box_co2) {
  /// 	//      <-------- read pointer -------->x<- when write pointer reached here, start loopback to buffer 0
  /// 	//      -------------------------------------------------         
  /// 	//      |   buffer 0    |   buffer 1    |   buffer 2    |         
  /// 	//      -------------------------------------------------         
  /// 	//                      <-------- write pointer -------->
  /// 	app_graph_t* graphs[] = { &graph_heartbeat , &graph_pleth , &graph_co2 };
  /// 	app_box* boxs[] = { box_heartbeat , box_pleth , box_co2 };
  /// 	int colors[] = { 0x00ff0000, 0x00ffff00, 0xffff0000 };
  /// 
  /// 	for (int i = 0; i < 3; i++) {
  /// 		app_graph_t* gh = graphs[i];
  /// 		gh->buffer0 = i * GRAPH_BUFFER_SIZE; // loopback buffer
  /// 		gh->buffer1 = gh->buffer0 + GRAPH_BYTE_PER_BUFFER;
  /// 		gh->buffer2 = gh->buffer1 + GRAPH_BYTE_PER_BUFFER;
  /// 		gh->buffer2_end = gh->buffer2 + GRAPH_BYTE_PER_BUFFER;
  /// 
  /// 		gh->bitmap_rp = gh->buffer0;  // display from buffer 0 and continue to buffer 1
  /// 		gh->bitmap_wp = gh->buffer1;  // append to buffer 1 and continue to buffer 2
  /// 		gh->bitmap_wplb = gh->buffer0;  // loopback from buffer 0
  /// 		gh->x = boxs[i]->x + 10;
  /// 		gh->y = boxs[i]->y - 60;
  /// 		gh->w = GRAPH_H;
  /// 		gh->h = GRAPH_W;
  /// 		gh->handler = 1;
  /// 		gh->rgba = colors[i];
  /// 		gh->accumulator = 0;
  /// 		gh->byte_count = 0;
  /// 		gh->addr_tf = gh->bitmap_wp;
  /// 		EVE_CoCmd_memSet(s_pHalContext, gh->bitmap_rp, 255, GRAPH_BUFFER_SIZE);
  /// 	}
  /// }