//#include "Helpers.h"
//
//static void graph_ecg() {}
//static void graph_ple() {}
//static void graph_co2() {}
//
//
//void set_color(char* a, int n, char color) {
//#if USEBITMAP == USE_BITMAP_L1
//	int byte_index = n / 8;    // byte nth
//	int bit_index = 7 - n % 8;    // bit nth 
//	a[byte_index] |= (1 << bit_index);
//
//#elif  USEBITMAP == USE_BITMAP_PALETTED
//	a[n] = color;
//
//#elif  USEBITMAP == USE_BITMAP_BARGRAPH
//	a[n] = color;
//
//#elif  USEBITMAP == USE_BITMAP_L1_NO_ROTATE
//	a[n] = color;
//
//#elif  USEBITMAP == USE_BITMAP_LINESTRIP
//	a[n] = color;
//
//#endif
//}
//
//void draw_pixel(char* buffer, int x, int y, int color_offset)
//{
//	if (y > GRAPH_H * 2)
//		return;
//	if (x < 0)
//		printf("Error: Drawing pixel at (%d, %d)\n", x, y);
//
//
//	if (x < 0 || x >= GRAPH_W)
//	{
//		return;
//	}
//	set_color(buffer, x + y * GRAPH_W, color_offset);
//}
//
//void bresenham_line(char* buffer, int x1, int y1, int x2, int y2, int color_offset)
//{
//	int sx = (x2 > x1) ? 1 : -1;
//	int sy = (y2 > y1) ? 1 : -1;
//	int dx = abs(x2 - x1);
//	int dy = abs(y2 - y1);
//	while (dx-- > -1)
//	{
//		draw_pixel(buffer, x1, y1, color_offset);
//		x1 += sx;
//	}
//}
//
//int read_time_simulate(int sample_rate, int time_start_ms)
//{
//	int now_ms = EVE_millis();
//	int duration_ms = now_ms - time_start_ms;
//
//	int num_samples = duration_ms * sample_rate / 1000;
//	//return 1;
//	return num_samples;
//}
//
//// put a realtime data into ramg
//// return the paletted bitmap address
//int heartbeat(int ramg_paletted, char* buffer_paletted, int color_offset)
//{
//	// Simulated sinus rhythm data (adjusted to resemble the waveform)
//	const int ecg_data[] = {
//		2, 2, 2, 2, 2, 2, 2, 2, 2, 2,															  // P-wave (small bump)
//		0, 2, 4, 6, 8, 6, 4, 2, 0,																  // PR segment (flat)
//		0, 0, 0, 0, 0,																			  // QRS complex (sharp spike)
//		2, 4, 8, 20, 40, 70, 30, 10, 5, 2,														  // ST segment (flat)
//		2, 2, 2, 2, 2, 2, 2,																	  // T-wave (broad bump)
//		2, 4, 6, 10, 14, 16, 14, 10, 6, 4, 2,													  // Baseline (flat)
//		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // end
//	};
//
//	int* data = ecg_data;
//	const int sample_total = sizeof(ecg_data) / sizeof(int);
//	int data_duration = 1; // unit: second
//	int dividor = 2;
//	int multipler = 3;
//
//	const int sample_per_second = sample_total / data_duration;
//	static int sample_offset = 0;
//
//	static int lastx = 0;
//	static int lasty = 0;
//
//	static int bitmap_rp_offet = 0; // bitmap read pointer, start from buffer 0
//	static int bitmap_wp_offet = GRAPH_BYTE; // bitmap write pointer, start from buffer 1
//
//	int buffer0 = ramg_paletted; // image pointer (bitmap_rp) start from this buffer and move until reach next buffer
//	int buffer1 = ramg_paletted + GRAPH_BYTE; // data append (bitmap_wp) to this buffer until reach next buffer
//	int buffer2 = ramg_paletted + GRAPH_BYTE * 2; // this buffer contain new data appended, image pointer never reach this buffer
//
//	int bitmap_rp = ramg_paletted + bitmap_rp_offet;
//	int bitmap_wp = ramg_paletted + bitmap_wp_offet;
//
//	static uint32_t time_start_ms = 0;
//	if (time_start_ms == 0)
//	{
//		time_start_ms = EVE_millis();
//	}
//	int num_samples = read_time_simulate(sample_per_second, time_start_ms);
//	if (num_samples == 0) {
//		return bitmap_rp;
//	}
//	else {
//		time_start_ms = EVE_millis();
//		num_samples = min(num_samples, sample_total);
//	}
//
//	int copy_bytes = 0;
//	// one sample take 1 line
//	for (int i = 0; i < num_samples; i++)
//	{
//		// reset pixel color to background
//		for (int j = 0; j < GRAPH_W_BYTE; j++)
//		{
//			buffer_paletted[i * GRAPH_W_BYTE + j] = PALETTED_BG;
//		}
//
//		// get sample (x,y) coordinate
//		int pixel_y = i;
//		int pixel_x = data[(sample_offset + pixel_y) % sample_total] * multipler / dividor;
//
//		// Draw graph to buffer
//		bresenham_line(buffer_paletted, pixel_x, pixel_y, lastx, lasty, color_offset);
//		lastx = pixel_x;
//		lasty = pixel_y;
//
//		copy_bytes += GRAPH_W_BYTE;
//
//		if ((bitmap_wp + copy_bytes) >= buffer2 + GRAPH_BYTE) // if buffer 2nd full, start over
//		{
//			bitmap_rp = buffer0;	// reset write pointer
//			bitmap_wp = buffer1;	// reset write pointer
//			break;
//		}
//	}
//	sample_offset = (sample_offset + num_samples) % sample_total;
//
//	// copy to display buffer
//	EVE_Hal_wrMem(s_pHalContext, bitmap_wp, buffer_paletted, copy_bytes);
//
//	// duplicate to buffer 1st for next cycle
//	if (bitmap_wp >= buffer2) {
//		int duplicate_to = buffer0 + (bitmap_wp - buffer2);
//		int duplicate_from = bitmap_wp;
//		EVE_CoCmd_memCpy(s_pHalContext, duplicate_to, duplicate_from, copy_bytes);
//	}
//
//	bitmap_rp += copy_bytes;
//	bitmap_wp += copy_bytes;
//
//	bitmap_rp_offet = bitmap_rp - ramg_paletted;
//	bitmap_wp_offet = bitmap_wp - ramg_paletted;
//
//	return bitmap_rp;
//}
//
//int pleth(int ramg_paletted, char* buffer_paletted, int color_offset)
//{
//	// Simulated sinus rhythm data (adjusted to resemble the waveform)
//	const int pleth_data[] = {
//		0, 6, 12, 18, 30, 42, 60, 78, 102, 126,			  // Cycle 1 - Inhalation
//		150, 174, 192, 210, 228, 240, 252, 264, 270, 276, // Cycle 1 - Continued Inhalation
//		282, 288, 285, 282, 288, 285, 276, 270, 264, 252, // Cycle 1 - Peak
//		240, 228, 210, 192, 174, 160, 140, 120, 130, 110, // Cycle 1 - Exhalation with fluctuations
//		90, 100, 85, 75, 95, 65, 50, 42, 30, 35,		  // Cycle 1 - Continued Exhalation
//		20, 15, 12, 10, 8, 6, 9, 12, 18, 30,			  // Cycle 1 - Trough with fluctuations
//		42, 70, 100, 120, 140, 160, 180, 200, 210, 220,	  // Cycle 2 - Inhalation
//		225, 230, 235, 240, 238, 236, 240, 235, 225, 210, // Cycle 2 - Peak and decline
//		195, 180, 165, 150, 130, 110, 90, 70, 50, 30,	  // Cycle 2 - Faster Exhalation
//		20, 10, 5, 0, 0, 0, 0, 0, 0, 0					  // Cycle 2 - End
//	};
//	int* data = pleth_data;
//	const int sample_total = sizeof(pleth_data) / sizeof(int);
//	int data_duration = 2; // unit: second
//	int dividor = 2;
//	int multipler = 1;
//
//	const int sample_per_second = sample_total / data_duration;
//	static int sample_offset = 0;
//
//	static int lastx = 0;
//	static int lasty = 0;
//
//	static int bitmap_rp_offet = 0; // bitmap read pointer, start from buffer 0
//	static int bitmap_wp_offet = GRAPH_BYTE; // bitmap write pointer, start from buffer 1
//
//	int buffer0 = ramg_paletted; // image pointer (bitmap_rp) start from this buffer and move until reach next buffer
//	int buffer1 = ramg_paletted + GRAPH_BYTE; // data append (bitmap_wp) to this buffer until reach next buffer
//	int buffer2 = ramg_paletted + GRAPH_BYTE * 2; // this buffer contain new data appended, image pointer never reach this buffer
//
//	int bitmap_rp = ramg_paletted + bitmap_rp_offet;
//	int bitmap_wp = ramg_paletted + bitmap_wp_offet;
//
//	static uint32_t time_start_ms = 0;
//	if (time_start_ms == 0)
//	{
//		time_start_ms = EVE_millis();
//	}
//	int num_samples = read_time_simulate(sample_per_second, time_start_ms);
//	if (num_samples == 0) {
//		return bitmap_rp;
//	}
//	else {
//		time_start_ms = EVE_millis();
//		num_samples = min(num_samples, sample_total);
//	}
//
//	int copy_bytes = 0;
//	// one sample take 1 line
//	for (int i = 0; i < num_samples; i++)
//	{
//		// reset pixel color to background
//		for (int j = 0; j < GRAPH_W_BYTE; j++)
//		{
//			buffer_paletted[i * GRAPH_W_BYTE + j] = PALETTED_BG;
//		}
//
//		// get sample (x,y) coordinate
//		int pixel_y = i;
//		int pixel_x = data[(sample_offset + pixel_y) % sample_total] * multipler / dividor;
//
//		// Draw graph to buffer
//		bresenham_line(buffer_paletted, pixel_x, pixel_y, lastx, lasty, color_offset);
//		lastx = pixel_x;
//		lasty = pixel_y;
//
//		copy_bytes += GRAPH_W_BYTE;
//
//		if ((bitmap_wp + copy_bytes) >= buffer2 + GRAPH_BYTE) // if buffer 2nd full, start over
//		{
//			bitmap_rp = buffer0;	// reset write pointer
//			bitmap_wp = buffer1;	// reset write pointer
//			break;
//		}
//	}
//	sample_offset = (sample_offset + num_samples) % sample_total;
//
//	// copy to display buffer
//	EVE_Hal_wrMem(s_pHalContext, bitmap_wp, buffer_paletted, copy_bytes);
//
//	// duplicate to buffer 1st for next cycle
//	if (bitmap_wp >= buffer2) {
//		int duplicate_to = buffer0 + (bitmap_wp - buffer2);
//		int duplicate_from = bitmap_wp;
//		EVE_CoCmd_memCpy(s_pHalContext, duplicate_to, duplicate_from, copy_bytes);
//	}
//
//	bitmap_rp += copy_bytes;
//	bitmap_wp += copy_bytes;
//
//	bitmap_rp_offet = bitmap_rp - ramg_paletted;
//	bitmap_wp_offet = bitmap_wp - ramg_paletted;
//
//	return bitmap_rp;
//}
//
//int co2(int ramg_paletted, char* buffer_paletted, int color_offset)
//{
//	// Simulated sinus rhythm data (adjusted to resemble the waveform)
//	const int co2_data[] = {												  // 1 second sample
//							30, 45, 60, 75, 90, 105, 120, 135, 150, 165,	  //
//							180, 195, 210, 225, 240, 255, 255, 255, 255, 255, //
//							255, 255, 255, 255, 255, 240, 225, 210, 195, 180, //
//							165, 150, 135, 120, 105, 90, 75, 60, 45, 30,	  //
//							15, 10, 5, 0, 0, 0, 0, 0, 0, 0,					  //
//							0, 0, 0, 0, 0, 5, 10, 15, 15 };
//
//	int* data = co2_data;
//	const int sample_total = sizeof(co2_data) / sizeof(int);
//	int data_duration = 1; // unit: second
//	int dividor = 2;
//	int multipler = 1;
//
//	const int sample_per_second = sample_total / data_duration;
//	static int sample_offset = 0;
//
//	static int lastx = 0;
//	static int lasty = 0;
//
//	static int bitmap_rp_offet = 0; // bitmap read pointer, start from buffer 0
//	static int bitmap_wp_offet = GRAPH_BYTE; // bitmap write pointer, start from buffer 1
//
//	int buffer0 = ramg_paletted; // image pointer (bitmap_rp) start from this buffer and move until reach next buffer
//	int buffer1 = ramg_paletted + GRAPH_BYTE; // data append (bitmap_wp) to this buffer until reach next buffer
//	int buffer2 = ramg_paletted + GRAPH_BYTE * 2; // this buffer contain new data appended, image pointer never reach this buffer
//
//	int bitmap_rp = ramg_paletted + bitmap_rp_offet;
//	int bitmap_wp = ramg_paletted + bitmap_wp_offet;
//
//	static uint32_t time_start_ms = 0;
//	if (time_start_ms == 0)
//	{
//		time_start_ms = EVE_millis();
//	}
//	int num_samples = read_time_simulate(sample_per_second, time_start_ms);
//	if (num_samples == 0) {
//		return bitmap_rp;
//	}
//	else {
//		time_start_ms = EVE_millis();
//		num_samples = min(num_samples, sample_total);
//	}
//
//	int copy_bytes = 0;
//	// one sample take 1 line
//	for (int i = 0; i < num_samples; i++)
//	{
//		// reset pixel color to background
//		for (int j = 0; j < GRAPH_W_BYTE; j++)
//		{
//			buffer_paletted[i * GRAPH_W_BYTE + j] = PALETTED_BG;
//		}
//
//		// get sample (x,y) coordinate
//		int pixel_y = i;
//		int pixel_x = data[(sample_offset + pixel_y) % sample_total] * multipler / dividor;
//
//		// Draw graph to buffer
//		bresenham_line(buffer_paletted, pixel_x, pixel_y, lastx, lasty, color_offset);
//		lastx = pixel_x;
//		lasty = pixel_y;
//
//		copy_bytes += GRAPH_W_BYTE;
//
//		if ((bitmap_wp + copy_bytes) >= buffer2 + GRAPH_BYTE) // if buffer 2nd full, start over
//		{
//			bitmap_rp = buffer0;	// reset write pointer
//			bitmap_wp = buffer1;	// reset write pointer
//			break;
//		}
//	}
//	sample_offset = (sample_offset + num_samples) % sample_total;
//
//	// copy to display buffer
//	EVE_Hal_wrMem(s_pHalContext, bitmap_wp, buffer_paletted, copy_bytes);
//
//	// duplicate to buffer 1st for next cycle
//	if (bitmap_wp >= buffer2) {
//		int duplicate_to = buffer0 + (bitmap_wp - buffer2);
//		int duplicate_from = bitmap_wp;
//		EVE_CoCmd_memCpy(s_pHalContext, duplicate_to, duplicate_from, copy_bytes);
//	}
//
//	bitmap_rp += copy_bytes;
//	bitmap_wp += copy_bytes;
//
//	bitmap_rp_offet = bitmap_rp - ramg_paletted;
//	bitmap_wp_offet = bitmap_wp - ramg_paletted;
//
//	return bitmap_rp;
//}
//
//#define CHAR_BIT 8
//#define BITS_IN_TYPE(type) (sizeof(type) * CHAR_BIT)
//#define BIT_PER_CHAR (BITS_IN_TYPE(char))
//#define GRAPH_BIT_PER_PIXEL 1 // L1 bbp value
//
//#define GRAPH_W 160
//#define GRAPH_H 1000
//#define GRAPH_SIZE (GRAPH_W *GRAPH_H)
//#define GRAPH_BYTE (GRAPH_SIZE * GRAPH_BIT_PER_PIXEL / BIT_PER_CHAR)
//#define GRAPH_W_BYTE (GRAPH_W * GRAPH_BIT_PER_PIXEL / BIT_PER_CHAR)
//#define BUFFER_PER_GRAPH 3
//
//#define PALETTED_BG 0
//
//
//// graph palleted buffer
//char buffer_paletted1_x2[GRAPH_BYTE] = { {0} };
//char buffer_paletted2_x2[GRAPH_BYTE] = { {0} };
//char buffer_paletted3_x2[GRAPH_BYTE] = { {0} };
//
//
//void graph_L1(int bitmap_addr, int color_source, int x, int y) {
//	int bformat = L1;
//	int lw = max(GRAPH_H, GRAPH_W);
//	int lh = max(GRAPH_H, GRAPH_W);
//#define MAX_ANGLE 360
//#define MAX_CIRCLE_UNIT 65536
//	int rotation_angle = -90;
//
//	EVE_CoCmd_setBitmap(s_pHalContext, bitmap_addr, bformat, GRAPH_W, GRAPH_H);
//	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, lw, lh));
//	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H(lw >> 9, lh >> 9));
//	EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());
//	EVE_CoCmd_loadIdentity(s_pHalContext);
//	EVE_CoCmd_translate(s_pHalContext, 0, GRAPH_W * MAX_CIRCLE_UNIT);
//	EVE_CoCmd_rotate(s_pHalContext, rotation_angle * MAX_CIRCLE_UNIT / MAX_ANGLE);
//	EVE_CoCmd_setMatrix(s_pHalContext);
//	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
//	EVE_DRAW_AT(x, y);
//	EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
//}
//
//void graph_l1_rotate() {
//	int buffer_offset = paletted_adr;// heartbeat(paletted_adr, buffer_paletted1_x2, ARGB_GREEN);
//	int buffer_offset2 = paletted_adr2;// pleth(paletted_adr2, buffer_paletted2_x2, ARGB_CYAN);
//	int buffer_offset3 = paletted_adr3;//co2(paletted_adr3, buffer_paletted3_x2, ARGB_YELLOW);
//
//			// Graph ecg
//	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 255, 0)); // green
//	graph_paletted8(buffer_offset, color_table_addr, graph_start + 10, box_graph_ecg.y + 30);
//
//	// Graph pleth
//	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 255, 255)); // cyan
//	graph_paletted8(buffer_offset2, color_table_addr, graph_start + 10, box_graph_pleth.y + 30);
//
//	// Graph co2
//	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 0)); // yellow
//	graph_paletted8(buffer_offset3, color_table_addr, graph_start + 10, box_graph_co2.y + 30);
//}