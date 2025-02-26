#include <stdio.h>
#include <stdint.h>

#include "Helpers.h"
#include "common.h"
#include "Bedside_Monitor.h"

#define APP_PRINTF //printf
extern EVE_HalContext* s_pHalContext;
// Day (1-31)
static uint32_t dd_list[] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
    21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
};

// Month (1-12)
static uint32_t mm_list[] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12
};

// Year (1900-2100) -> Adjust as needed
static uint32_t yy_list[] = {
    1900, 1901, 1902, 1903, 1904, 1905, 1906, 1907, 1908, 1909,
    1910, 1911, 1912, 1913, 1914, 1915, 1916, 1917, 1918, 1919,
    1920, 1921, 1922, 1923, 1924, 1925, 1926, 1927, 1928, 1929,
    1930, 1931, 1932, 1933, 1934, 1935, 1936, 1937, 1938, 1939,
    1940, 1941, 1942, 1943, 1944, 1945, 1946, 1947, 1948, 1949,
    1950, 1951, 1952, 1953, 1954, 1955, 1956, 1957, 1958, 1959,
    1960, 1961, 1962, 1963, 1964, 1965, 1966, 1967, 1968, 1969,
    1970, 1971, 1972, 1973, 1974, 1975, 1976, 1977, 1978, 1979,
    1980, 1981, 1982, 1983, 1984, 1985, 1986, 1987, 1988, 1989,
    1990, 1991, 1992, 1993, 1994, 1995, 1996, 1997, 1998, 1999,
    2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009,
    2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
    2020, 2021, 2022, 2023, 2024, 2025, 2026, 2027, 2028, 2029,
    2030, 2031, 2032, 2033, 2034, 2035, 2036, 2037, 2038, 2039,
    2040, 2041, 2042, 2043, 2044, 2045, 2046, 2047, 2048, 2049,
    2050, 2051, 2052, 2053, 2054, 2055, 2056, 2057, 2058, 2059,
    2060, 2061, 2062, 2063, 2064, 2065, 2066, 2067, 2068, 2069,
    2070, 2071, 2072, 2073, 2074, 2075, 2076, 2077, 2078, 2079,
    2080, 2081, 2082, 2083, 2084, 2085, 2086, 2087, 2088, 2089,
    2090, 2091, 2092, 2093, 2094, 2095, 2096, 2097, 2098, 2099, 2100
};

// Hour (0-23)
static uint32_t hh_list[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
    12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23
};

// Minute (0-59)
static uint32_t mt_list[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
    20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
    40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59
};

// Second (0-59)
static uint32_t ss_list[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
    20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
    40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59
};

// Function to find the index of a given number in the array
int find_index(uint32_t arr[], size_t size, uint32_t target) {
    for (size_t i = 0; i < size; i++) {
        if (arr[i] == target) {
            return i;  // Return the index if found
        }
    }
    return -1; // Return -1 if not found
}

// Function to return the number of valid left neighbors
int find_left_neighbors(uint32_t arr[], size_t size, int index, int n) {
    int count = 0;
    for (int i = index - n; i < index; i++) {
        if (i >= 0) {  // Ensure the index is valid
            count++;
        }
    }
    return count; // Return the number of valid left neighbors
}

// Function to return the number of valid right neighbors
int find_right_neighbors(uint32_t arr[], size_t size, int index, int n) {
    int count = 0;
    for (int i = index + 1; i <= index + n; i++) {
        if (i < size) {  // Ensure the index is within array bounds
            count++;
        }
    }
    return count; // Return the number of valid right neighbors
}

int32_t draw_selection(EVE_HalContext* phost, uint32_t* arr, uint32_t arr_count, int32_t y_offset, app_box drawing_frame, int32_t tag_val) {
    int ret = arr[0];

    int32_t x = drawing_frame.x + drawing_frame.w / 2;
    int32_t y = drawing_frame.y + drawing_frame.h / 2 + y_offset;
    int32_t font[] = { 30, 31, FONT_33, 31, 30 };

    int i = 0;
    while (y < drawing_frame.y - drawing_frame.h) {
        i++;
        y += drawing_frame.h;
    }
    //int font_start = 5 - min(5, (arr_count - i +1));
    int font_start = 0;
    font_start += max(0, (y - drawing_frame.y)) / drawing_frame.h;

    EVE_Cmd_wr32(phost, SCISSOR_XY(drawing_frame.x, drawing_frame.y));
    EVE_Cmd_wr32(phost, SCISSOR_SIZE(drawing_frame.w, drawing_frame.h * 5));
    EVE_Cmd_wr32(phost, COLOR_RGB(0, 0, 0));
    for (; i < arr_count; i++) {
        if (font_start == 2) {
            EVE_Cmd_wr32(phost, COLOR_RGB(0, 0, 255));
            ret = arr[i];
        }
        EVE_CoCmd_text(phost, x, y, font[font_start], OPT_FORMAT | OPT_CENTER, "%d", arr[i]);
        if (font_start == 2) {
            EVE_Cmd_wr32(phost, COLOR_RGB(0, 0, 0));
        }
        y += drawing_frame.h;
        if (y >= drawing_frame.y + drawing_frame.h*6) {
            break;
        }
        if (y >= drawing_frame.y + drawing_frame.h && font_start < 4) {
            font_start++;
        }
    }
    
    // transparent drawing_frame
    EVE_Cmd_wr32(phost, COLOR_A(110));
    EVE_Cmd_wr32(phost, COLOR_RGB(0, 0, 0));
    EVE_Cmd_wr32(phost, TAG(tag_val));
    DRAW_RECT(drawing_frame.x, drawing_frame.y, drawing_frame.w, drawing_frame.h * 5);

    return ret;
}

void dateime_adjustment(EVE_HalContext* phost) {
	uint32_t dd= get_dd();
	uint32_t mm= get_mm();
	uint32_t yy= get_yyyy();
	uint32_t hh= get_hh();
	uint32_t mt= get_mt();
	uint32_t ss= get_ss();
	uint32_t ms= get_ms();

#define dd_text "Date"
#define mm_text "Month"
#define yy_text "year"
#define hh_text "Hour"
#define mt_text "Minute"
#define ss_text "Second"

    int dd_count = sizeof(dd_list) / sizeof(uint32_t);
    int mm_count = sizeof(mm_list) / sizeof(uint32_t);
    int yy_count = sizeof(yy_list) / sizeof(uint32_t);
    int hh_count = sizeof(hh_list) / sizeof(uint32_t);
    int mt_count = sizeof(mt_list) / sizeof(uint32_t);
    int ss_count = sizeof(ss_list) / sizeof(uint32_t);

    const int32_t dd_tag = 1;
    const int32_t mm_tag = 2;
    const int32_t yy_tag = 3;
    const int32_t hh_tag = 4;
    const int32_t mt_tag = 5;
    const int32_t ss_tag = 6;
    const int32_t tag_btn_ok = 7;
    const int32_t tag_btn_cancel = 8;

    app_box box_datetime = INIT_APP_BOX(phost->Width/2 - phost->Width * 2 / 6, phost->Height/2 - phost->Height * 2 / 6, phost->Width*2/3, phost->Height * 2/3 );

    int h = 50, w = 100;
    int x = phost->Width/2 - (200 + w*5)/2; // x center screen
    int y = phost->Height/2 - h*5/2; // y center screen
    app_box yy_frame = INIT_APP_BOX(x, y, 200, h);
    app_box mm_frame = INIT_APP_BOX(yy_frame.x_end + 1, y, w, h);
    app_box dd_frame = INIT_APP_BOX(mm_frame.x_end + 1, y, w, h);
    app_box hh_frame = INIT_APP_BOX(dd_frame.x_end + 1, y, w, h);
    app_box mt_frame = INIT_APP_BOX(hh_frame.x_end + 1, y, w, h);
    app_box ss_frame = INIT_APP_BOX(mt_frame.x_end + 1, y, w, h);

    const int32_t velocicty_div = 50;
    int32_t tag_selected = 0;

    int32_t dd_y_offset=0, dd_y_swift = 0, dd_y_scroll = 0, dd_y_fineturn = 1, dd_y_init=0;
    int32_t mm_y_offset=0, mm_y_swift = 0, mm_y_scroll = 0, mm_y_fineturn = 1, mm_y_init=0;
    int32_t yy_y_offset=0, yy_y_swift = 0, yy_y_scroll = 0, yy_y_fineturn = 1, yy_y_init=0;
    int32_t hh_y_offset=0, hh_y_swift = 0, hh_y_scroll = 0, hh_y_fineturn = 1, hh_y_init=0;
    int32_t mt_y_offset=0, mt_y_swift = 0, mt_y_scroll = 0, mt_y_fineturn = 1, mt_y_init=0;
    int32_t ss_y_offset=0, ss_y_swift = 0, ss_y_scroll = 0, ss_y_fineturn = 1, ss_y_init=0;

    int i = 0;
    i = 0; while(dd_list[i+2] != dd){i++; dd_y_offset -=h ;}
    i = 0; while(mm_list[i+2] != mm){i++; mm_y_offset -=h ;}
    i = 0; while(yy_list[i+2] != yy){i++; yy_y_offset -=h ;}
    i = 0; while(hh_list[i+2] != hh){i++; hh_y_offset -=h ;}
    i = 0; while(mt_list[i+2] != mt){i++; mt_y_offset -=h ;}
    i = 0; while(ss_list[i+2] != ss){i++; ss_y_offset -=h ;}

    while (1) {
        Display_Start(phost);
        EVE_Cmd_wr32(s_pHalContext, VERTEX_FORMAT(EVE_VERTEX_FORMAT));

        Gesture_Touch_t* ges = utils_gestureRenew(phost);
        if(ges->tagPressed >= dd_tag && ges->tagPressed <= ss_tag) {
            if (tag_selected == dd_tag) { dd_y_offset -= dd_y_fineturn * (dd_y_offset % dd_frame.h); }
            if (tag_selected == mm_tag) { mm_y_offset -= mm_y_fineturn * (mm_y_offset % mm_frame.h); }
            if (tag_selected == yy_tag) { yy_y_offset -= yy_y_fineturn * (yy_y_offset % yy_frame.h); }
            if (tag_selected == hh_tag) { hh_y_offset -= hh_y_fineturn * (hh_y_offset % hh_frame.h); }
            if (tag_selected == mt_tag) { mt_y_offset -= mt_y_fineturn * (mt_y_offset % mt_frame.h); }
            if (tag_selected == ss_tag) { ss_y_offset -= ss_y_fineturn * (ss_y_offset % ss_frame.h);}
            tag_selected = ges->tagPressed;
        }

        // swift distance
        if (ges->tagPressed == dd_tag) { dd_y_swift = ges->distanceY; }
        if (ges->tagPressed == mm_tag) { mm_y_swift = ges->distanceY; }
        if (ges->tagPressed == yy_tag) { yy_y_swift = ges->distanceY; }
        if (ges->tagPressed == hh_tag) { hh_y_swift = ges->distanceY; }
        if (ges->tagPressed == mt_tag) { mt_y_swift = ges->distanceY; }
        if (ges->tagPressed == ss_tag) { ss_y_swift = ges->distanceY;}
        if (ges->tagReleased == dd_tag) {dd_y_offset += dd_y_swift;dd_y_swift = 0;}
        if (ges->tagReleased == mm_tag) {mm_y_offset += mm_y_swift;mm_y_swift = 0;}
        if (ges->tagReleased == yy_tag) {yy_y_offset += yy_y_swift;yy_y_swift = 0;}
        if (ges->tagReleased == hh_tag) {hh_y_offset += hh_y_swift;hh_y_swift = 0;}
        if (ges->tagReleased == mt_tag) {mt_y_offset += mt_y_swift;mt_y_swift = 0;}
        if (ges->tagReleased == ss_tag) {ss_y_offset += ss_y_swift;ss_y_swift = 0;}

        // scroll distance
        if (ges->velocityY != 0) { // scrolling, also set fineturn=-1 or 1
            if (tag_selected == dd_tag) {dd_y_scroll = -ges->velocityY_total / velocicty_div;dd_y_fineturn = 1 | (ges->velocityY >> 31);}
            if (tag_selected == mm_tag) {mm_y_scroll = -ges->velocityY_total / velocicty_div;mm_y_fineturn = 1 | (ges->velocityY >> 31);}
            if (tag_selected == yy_tag) {yy_y_scroll = -ges->velocityY_total / velocicty_div;yy_y_fineturn = 1 | (ges->velocityY >> 31);}
            if (tag_selected == hh_tag) {hh_y_scroll = -ges->velocityY_total / velocicty_div;hh_y_fineturn = 1 | (ges->velocityY >> 31);}
            if (tag_selected == mt_tag) {mt_y_scroll = -ges->velocityY_total / velocicty_div;mt_y_fineturn = 1 | (ges->velocityY >> 31);}
            if (tag_selected == ss_tag) {ss_y_scroll = -ges->velocityY_total / velocicty_div;ss_y_fineturn = 1 | (ges->velocityY >> 31);}
        }
        else { // scrolling fineturn
            dd_y_offset += dd_y_scroll;dd_y_scroll = 0;int dd_y_remain = dd_y_offset % dd_frame.h;if (dd_y_remain != 0) {dd_y_offset -= dd_y_fineturn;}
            mm_y_offset += mm_y_scroll;mm_y_scroll = 0;int mm_y_remain = mm_y_offset % mm_frame.h;if (mm_y_remain != 0) {mm_y_offset -= mm_y_fineturn;}
            yy_y_offset += yy_y_scroll;yy_y_scroll = 0;int yy_y_remain = yy_y_offset % yy_frame.h;if (yy_y_remain != 0) {yy_y_offset -= yy_y_fineturn;}
            hh_y_offset += hh_y_scroll;hh_y_scroll = 0;int hh_y_remain = hh_y_offset % hh_frame.h;if (hh_y_remain != 0) {hh_y_offset -= hh_y_fineturn;}
            mt_y_offset += mt_y_scroll;mt_y_scroll = 0;int mt_y_remain = mt_y_offset % mt_frame.h;if (mt_y_remain != 0) {mt_y_offset -= mt_y_fineturn;}
            ss_y_offset += ss_y_scroll;ss_y_scroll = 0;int ss_y_remain = ss_y_offset % ss_frame.h;if (ss_y_remain != 0) {ss_y_offset -= ss_y_fineturn;}
        }

        if (ges->tagReleased == tag_btn_ok) {
            init_datetime(dd, mm, yy, hh, mt, ss, 0);
            return;
        }
        else if (ges->tagReleased == tag_btn_cancel) {
            return;
        }

        DRAW_BOX_BORDER(box_datetime, 0x00FFFFFF, 2, 0x00000000 );
        EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
        EVE_CoCmd_text(phost, box_datetime.x + box_datetime.w / 2, box_datetime.y + 30, 31, OPT_CENTERX, "Select the date time");
        EVE_CoCmd_text(phost, dd_frame.x + dd_frame.w / 2, dd_frame.y - 30, 28, OPT_CENTERX, dd_text);
        EVE_CoCmd_text(phost, mm_frame.x + mm_frame.w / 2, mm_frame.y - 30, 28, OPT_CENTERX, mm_text);
        EVE_CoCmd_text(phost, yy_frame.x + yy_frame.w / 2, yy_frame.y - 30, 28, OPT_CENTERX, yy_text);
        EVE_CoCmd_text(phost, hh_frame.x + hh_frame.w / 2, hh_frame.y - 30, 28, OPT_CENTERX, hh_text);
        EVE_CoCmd_text(phost, mt_frame.x + mt_frame.w / 2, mt_frame.y - 30, 28, OPT_CENTERX, mt_text);
        EVE_CoCmd_text(phost, ss_frame.x + ss_frame.w / 2, ss_frame.y - 30, 28, OPT_CENTERX, ss_text);
        
        int btn_w = 100, btn_distance = 20;
        int btn_x = (box_datetime.x + box_datetime.w / 2) - (btn_w * 2 + btn_distance) / 2; // x start of the 2 buttons center screen
        int btn_y = ss_frame.y + ss_frame.h * 5 + 10;
        EVE_CoCmd_fgColor(s_pHalContext, 0x144344);
        EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
        EVE_Cmd_wr32(s_pHalContext, TAG(tag_btn_ok));
        EVE_CoCmd_button(phost, btn_x, btn_y, btn_w, 30, 28, 0, "Ok");
        EVE_Cmd_wr32(s_pHalContext, TAG(tag_btn_cancel));
        EVE_CoCmd_button(phost, btn_x + btn_w + btn_distance, btn_y, btn_w, 30, 28, 0, "Cancel");
        EVE_Cmd_wr32(s_pHalContext, TAG(tag_btn_cancel));
        EVE_CoCmd_button(phost, box_datetime.x + 5, box_datetime.y + 5, 80, 30, 28, 0, "Back");

        // fix boundary
        dd_y_offset = max(min(dd_y_offset + dd_y_swift + dd_y_scroll, dd_frame.h * 2), -dd_frame.h * (dd_count - 3)) - dd_y_swift - dd_y_scroll; // 3 is number of items when scoll to end
        mm_y_offset = max(min(mm_y_offset + mm_y_swift + mm_y_scroll, mm_frame.h * 2), -mm_frame.h * (mm_count - 3)) - mm_y_swift - mm_y_scroll;
        yy_y_offset = max(min(yy_y_offset + yy_y_swift + yy_y_scroll, yy_frame.h * 2), -yy_frame.h * (yy_count - 3)) - yy_y_swift - yy_y_scroll;
        hh_y_offset = max(min(hh_y_offset + hh_y_swift + hh_y_scroll, hh_frame.h * 2), -hh_frame.h * (hh_count - 3)) - hh_y_swift - hh_y_scroll;
        mt_y_offset = max(min(mt_y_offset + mt_y_swift + mt_y_scroll, mt_frame.h * 2), -mt_frame.h * (mt_count - 3)) - mt_y_swift - mt_y_scroll;
        ss_y_offset = max(min(ss_y_offset + ss_y_swift + ss_y_scroll, ss_frame.h * 2), -ss_frame.h * (ss_count - 3)) - ss_y_swift - ss_y_scroll;
        
        dd = draw_selection(phost, dd_list, dd_count, dd_y_offset + dd_y_swift + dd_y_scroll, dd_frame, dd_tag);
        mm = draw_selection(phost, mm_list, mm_count, mm_y_offset + mm_y_swift + mm_y_scroll, mm_frame, mm_tag);
        yy = draw_selection(phost, yy_list, yy_count, yy_y_offset + yy_y_swift + yy_y_scroll, yy_frame, yy_tag);
        hh = draw_selection(phost, hh_list, hh_count, hh_y_offset + hh_y_swift + hh_y_scroll, hh_frame, hh_tag);
        mt = draw_selection(phost, mt_list, mt_count, mt_y_offset + mt_y_swift + mt_y_scroll, mt_frame, mt_tag);
        ss = draw_selection(phost, ss_list, ss_count, ss_y_offset + ss_y_swift + ss_y_scroll, ss_frame, ss_tag);

        // reset scissor
        EVE_Cmd_wr32(phost, SCISSOR_XY(0, 0));
        EVE_Cmd_wr32(phost, SCISSOR_SIZE(2048, 2048));

        Display_End(phost);
    }
}