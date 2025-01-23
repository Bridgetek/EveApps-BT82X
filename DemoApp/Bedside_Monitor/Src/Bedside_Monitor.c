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
#include "Helpers.h"
#include "Gesture.h"

EVE_HalContext s_halContext;
EVE_HalContext *s_pHalContext;
void SAMAPP_Bedside_Monitor();
#define SAMAPP_DELAY EVE_sleep(2000);
#define SCANOUT_FORMAT RGB8

int main(int argc, char *argv[])
{
	s_pHalContext = &s_halContext;
	Gpu_Init(s_pHalContext);
#if defined(BT82X_ENABLE)
	LVDS_Config(s_pHalContext, SCANOUT_FORMAT, TESTCASE_PICTURE);
#endif

	// read and store calibration setting
#if !defined(BT8XXEMU_PLATFORM) && !defined(BT82X_ENABLE) && GET_CALIBRATION == 1
	Calibration_New(s_pHalContext);
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
#if !defined(BT8XXEMU_PLATFORM) && !defined(BT82X_ENABLE) && GET_CALIBRATION == 1
		Calibration_Restore(s_pHalContext);
#endif
	}

	return 0;
}

/// The demo area //////////////////////////////////////////////////////////////////////////////////////
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

#define ENABLE_SCREENSHOT_CAPTURE 0

#define USE_BITMAP_L1                  0
#define USE_BITMAP_L1_ROTATE           1
#define USE_BITMAP_PALETTED_ROTATE     2
#define USE_BITMAP_BARGRAPH            3
#define USE_BITMAP_LINESTRIP           4

#define USEBITMAP USE_BITMAP_L1_ROTATE

#if  USEBITMAP == USE_BITMAP_L1
#define GRAPH_INIT graph_l1_init
#define GRAPH_DRAW graph_l1_draw

#elif  USEBITMAP == USE_BITMAP_L1_ROTATE
#define GRAPH_INIT graph_l1_rotate_init
#define GRAPH_DRAW graph_l1_rotate_draw

#elif  USEBITMAP == USE_BITMAP_PALETTED_ROTATE
#define GRAPH_INIT graph_p8_rotate_init
#define GRAPH_DRAW graph_p8_rotate_draw

#elif  USEBITMAP == USE_BITMAP_BARGRAPH
#define GRAPH_INIT graph_bargraph_init
#define GRAPH_DRAW graph_bargraph_draw

#elif  USEBITMAP == USE_BITMAP_LINESTRIP
#define GRAPH_INIT graph_linestrip_init
#define GRAPH_DRAW graph_linestrip_draw

#endif

void GRAPH_INIT(app_box* box_heartbeat, app_box* box_pleth, app_box* box_co2);
void GRAPH_DRAW();

#define WINDOW_W 1280
#define WINDOW_H 800
#define GRAPH_W 160
#define GRAPH_H 1000

int g_graph_zoom_lv = 3;

app_box box_menu_top;
app_box box_graph_ecg;
app_box box_graph_pleth;
app_box box_graph_co2;
app_box box_menu_bottom;

void process_event() {
#if defined(BT82X_ENABLE)
	return; // disable touch on bt820
#endif

#if  USEBITMAP == USE_BITMAP_BARGRAPH
	return; // disable touch on bargraph
#endif

	Gesture_Touch_t *ges = Gesture_Renew(s_pHalContext);
	if(ges->tagReleased == TAG_ZOOM_DOWN) {
		g_graph_zoom_lv--;
		g_graph_zoom_lv = max(g_graph_zoom_lv, 1);
		//GRAPH_INIT(&box_graph_ecg, &box_graph_pleth, &box_graph_co2);
	}

	else if (ges->tagReleased == TAG_ZOOM_UP) {
		g_graph_zoom_lv++;
		g_graph_zoom_lv = min(g_graph_zoom_lv, GRAPH_ZOOM_LV_MAX);
		//GRAPH_INIT(&box_graph_ecg, &box_graph_pleth, &box_graph_co2);
	}

}

void SAMAPP_Bedside_Monitor()
{
#if  USEBITMAP == USE_BITMAP_BARGRAPH
	g_graph_zoom_lv = 1; // disable touch on bargraph
#endif
	app_box lcd_size = {0, 0, s_pHalContext->Width, s_pHalContext->Height, s_pHalContext->Width, s_pHalContext->Height };
	app_box app_window = INIT_APP_BOX((s_pHalContext->Width - WINDOW_W) / 2, (s_pHalContext->Height - WINDOW_H) / 2, WINDOW_W, WINDOW_H);
	int graph_start = app_window.x + 35;

	box_menu_top    = INIT_APP_BOX(app_window.x, app_window.y, GRAPH_H, app_window.h * 8 / 100);
	box_graph_ecg   = INIT_APP_BOX(graph_start, box_menu_top.y_end, box_menu_top.w, app_window.h * 28 / 100);
	box_graph_pleth = INIT_APP_BOX(graph_start, box_graph_ecg.y_end, box_menu_top.w, box_graph_ecg.h);
	box_graph_co2   = INIT_APP_BOX(graph_start, box_graph_pleth.y_end, box_menu_top.w, box_graph_ecg.h);
	box_menu_bottom = INIT_APP_BOX(app_window.x, box_graph_co2.y_end, box_menu_top.w, box_menu_top.h);

	int x = box_menu_top.x_end;
	int w = app_window.w - box_menu_top.w;
	int h = (app_window.h - box_menu_top.h - box_menu_bottom.h) / 4;
	int y = box_menu_top.y_end;
	int border = 2;

	app_box box_right1 = INIT_APP_BOX(x, y + h * 0, w, h);
	app_box box_right2 = INIT_APP_BOX(x, y + h * 1, w, h);
	app_box box_right3 = INIT_APP_BOX(x, y + h * 2, w, h);
	app_box box_right4 = INIT_APP_BOX(x, y + h * 3, w, h);

	init_datetime(11, 12, 2024, 9, 11, 0, 0);

	int time_start_ms = 0;
	int val_hr = 66;
	int val_spo2 = 96;
	int val_co2 = 22;
	int val_sys = 156;
	int val_dias = 93;

#define FONT_32 2 // note: BT81x maximum handler is 31
#define FONT_33 3
#define FONT_34 4
	static int screenshot_counter = 0;

	GRAPH_INIT(&box_graph_ecg, &box_graph_pleth, &box_graph_co2);

	while (1)
	{
		process_event();
		Display_Start(s_pHalContext);
#if defined(BT82X_ENABLE)
		EVE_CoCmd_sync(s_pHalContext);
#endif
		EVE_Cmd_wr32(s_pHalContext, VERTEX_FORMAT(0));

		draw_app_window(app_window);

		GRAPH_DRAW();

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
		EVE_CoCmd_romFont(s_pHalContext, FONT_32, 32);
		EVE_CoCmd_romFont(s_pHalContext, FONT_33, 33);
		EVE_CoCmd_romFont(s_pHalContext, FONT_34, 34);

		int x = box_menu_top.x;
		int y = box_menu_top.y_mid;
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_CoCmd_text(s_pHalContext, x + 5, y, FONT_32, OPT_CENTERY, "BED");
		EVE_CoCmd_text(s_pHalContext, x + 155, y, 31, OPT_CENTERY, "no 5");
		EVE_CoCmd_text(s_pHalContext, box_menu_top.x_end, y, 31, OPT_CENTERY | OPT_RIGHTX, hh_mm());
		EVE_CoCmd_text(s_pHalContext, box_menu_top.x_mid, y, 31, OPT_CENTER, dd_mm_yyyy());

		// zoom level control
		y = box_menu_top.y;
		x = box_menu_top.x_end + 30;
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
		EVE_CoCmd_text(s_pHalContext, x, y , 25, OPT_FORMAT, "Zoom %d", g_graph_zoom_lv);
		
		int r = 20;
		y = box_menu_top.y_end - r;;
		x = app_window.x_end - r -50 - 5;
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
		DRAW_CIRCLE_WITH_TEXT_TAG(x, y, r, "+", 31, 0, 0x00FFFFFF, TAG_ZOOM_UP);
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
		DRAW_CIRCLE_WITH_TEXT_TAG(x + 50, y, r, "-", 31, 0, 0x00FFFFFF, TAG_ZOOM_DOWN);

		// Graph title text information
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_CoCmd_text(s_pHalContext, box_graph_ecg.x + 15, box_graph_ecg.y + 10, 30, 0, "ECG");
		EVE_CoCmd_text(s_pHalContext, box_graph_pleth.x + 15, box_graph_pleth.y + 10, 30, 0, "PLETH");
		EVE_CoCmd_text(s_pHalContext, box_graph_co2.x + 15, box_graph_co2.y + 10, 30, 0, "CO2");

		// create random data change
		int time_end_ms = EVE_millis();
		int duration = time_end_ms - time_start_ms;
		if (duration > (200 + app_random(100) - 50))
		{
			if (app_random(10) % 9 == 0)
				val_hr = 66 + app_random(8) - 5;
			if (app_random(10) % 5 == 0)
				val_spo2 = 90 + app_random(12) - 6;
			if (app_random(10) % 5 == 0)
				val_co2 = 22 + app_random(4) - 2;
			if (app_random(10) % 2 == 0)
				val_sys = 156 + app_random(8) - 4;
			if (app_random(10) % 3 == 0)
				val_dias = 93 + app_random(8) - 4;

			time_start_ms = time_end_ms;
		}

		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 255, 0));
		// Heart rate
		EVE_CoCmd_text(s_pHalContext, box_right1.x + 5, box_right1.y + 5, 29, 0, "HR");
		EVE_CoCmd_number(s_pHalContext, box_right1.x_mid, box_right1.y_mid, FONT_34, OPT_CENTER, val_hr);
		EVE_CoCmd_text(s_pHalContext, box_right1.x_mid + 90, box_right1.y_mid, 29, OPT_CENTER, "bpm");

		// SPO2
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 255, 255));
		EVE_CoCmd_text(s_pHalContext, box_right2.x + 5, box_right2.y + 5, 29, 0, "spO2");
		EVE_CoCmd_number(s_pHalContext, box_right2.x_mid, box_right2.y_mid, FONT_34, OPT_CENTER, val_spo2);
		EVE_CoCmd_text(s_pHalContext, box_right2.x_mid + 90, box_right2.y_mid, 29, OPT_CENTER, "%");

		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 0));
		// ETCO2
		EVE_CoCmd_text(s_pHalContext, box_right3.x + 5, box_right3.y + 5, 29, 0, "etCO2");
		EVE_CoCmd_number(s_pHalContext, box_right3.x_mid, box_right3.y_mid, FONT_34, OPT_CENTER, val_co2);
		EVE_CoCmd_text(s_pHalContext, box_right3.x_mid + 90, box_right3.y_mid, 29, OPT_CENTER, "mmHg");

		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		// NIBP
		EVE_CoCmd_text(s_pHalContext, box_right4.x + 5, box_right4.y + 5, 29, 0, "NIBP");
		EVE_CoCmd_number(s_pHalContext, box_right4.x + 2, box_right4.y + 50, FONT_33, 0, val_sys);
		EVE_CoCmd_text(s_pHalContext, box_right4.x_mid + 20, box_right4.y_mid + 10, 28, OPT_CENTERX, "mmHg");
		EVE_CoCmd_number(s_pHalContext, box_right4.x_mid + 50, box_right4.y + 50, FONT_33, 0, val_dias);
		EVE_CoCmd_text(s_pHalContext, box_right4.x + 35, box_right4.y_end - 40, 29, 0, "sys");
		EVE_CoCmd_text(s_pHalContext, box_right4.x_end - 70, box_right4.y_end - 40, 29, 0, "dias");

		Display_End(s_pHalContext);

#if ENABLE_SCREENSHOT_CAPTURE
		screenshot_counter++;
		if (screenshot_counter > 1000) {
			take_ddr_screenshot();
		}
#endif

	}
	return 0;
};
#endif