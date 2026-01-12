/**
 * @file Common.c
 * @brief Common functions
 *
 * @author
 *
 * @date 2024
 * 
 * MIT License
 *
 * Copyright (c) [2024] [Bridgetek Pte Ltd (BRTChip)]
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
#include "FileTransfer.h"
#include "FlashHelper.h"
#include "PatchDefault.h"

static uint32_t a;
static uint32_t b;
static uint32_t c;
static uint32_t d;
static uint32_t e;
static uint32_t f;

/**
 * @brief EVE initialization
 * 
 * @param phost Pointer to Hal context
 */
void Gpu_Init(EVE_HalContext* phost)
{
    EVE_HalParameters params;

    EVE_Hal_initialize();

    EVE_Hal_defaults(&params);

    EVE_Hal_open(phost, &params);

    EVE_Util_bootupConfig(phost);
#if defined(RP2040_PLATFORM)
    EVE_Util_loadSdCard(phost);
#endif
    if (EVE_LoadDefaultPatch(phost) != 0)
        eve_printf_debug("eve_loadpatch failed\n");
    else
        eve_printf_debug("load patch ok\n");
}

/**
 * @brief EVE release
 * 
 * @param phost Pointer to Hal context
 */
void Gpu_Release(EVE_HalContext* phost)
{
    EVE_Hal_close(phost);
    EVE_Hal_release();
}

/**
 * @brief Appends the decimal representation of `value` to the string in `pSrc`.
 * Ensures no buffer overflow by using snprintf.
 *
 * @param pSrc      Destination buffer containing a null-terminated string.
 * @param buf_size  Total size of the destination buffer in bytes.
 * @param value     Value to append as decimal ASCII.
 *
 * @return Number of characters appended (excluding the final '\0'),
 *         or a negative value on error.
 */
int32_t Gpu_Hal_Dec2Ascii(char *pSrc, size_t buf_size, int32_t value)
{
    size_t len;
    int written;

    if (pSrc == NULL || buf_size == 0)
        return -1;

    len = strlen(pSrc);
    if (len >= buf_size)
    {
        // No space left, string already uses whole buffer
        return -1;
    }

    // Append decimal representation at the end of existing string
    written = snprintf(pSrc + len, buf_size - len, "%ld", value);

    // snprintf returns number of characters that would have been written
    // (excluding '\0'), or a negative value on encoding error.
    if (written < 0)
        return -1;

    // If written >= buf_size - len, output was truncated
    if ((size_t)written >= buf_size - len)
        return -1;

    return (int32_t)written;
}

/**
 * @brief Do calibration
 *
 * @param phost Pointer to Hal context
 * @return bool True on successfull or otherwise
 */
bool EVE_Calibrate(EVE_HalContext *phost)
{
    uint32_t result;
    uint32_t transMatrix[6] = { 0 };
#if defined(EVE_SUPPORT_CAPACITIVE)
    EVE_Hal_wr32(phost, REG_CTOUCH_EXTENDED, CTOUCH_MODE_COMPATIBILITY);
#endif

    eve_printf_debug("App_CoPro_Widget_Calibrate: Start\n");

    EVE_CoCmd_watchdog(phost, 72000000); // Countermeasure for watchdog triggering when calibrate
    Display_Start(phost, (uint8_t[]) { 64, 64, 64 }, (uint8_t[]) { 255, 255, 255 }, 0, 4);
    EVE_CoCmd_text(phost, (uint16_t)(phost->Width / 2), (uint16_t)(phost->Height / 2), 31, OPT_CENTER, "Please Tap on the dot");
    result = EVE_CoCmd_calibrate(phost, 0);
    eve_printf("result %lx\n", result);
    EVE_Cmd_waitFlush(phost);

    eve_printf_debug("App_CoPro_Widget_Calibrate: End\n");

    // Print the configured values
    EVE_Hal_rdMem(phost, (uint8_t *)transMatrix, REG_TOUCH_TRANSFORM_A, 4 * 6); // read all the 6 coefficients
    eve_printf_debug("Touch screen transform values are A 0x%lx,B 0x%lx,C 0x%lx,D 0x%lx,E 0x%lx, F 0x%lx\n",
        transMatrix[0], transMatrix[1], transMatrix[2], transMatrix[3], transMatrix[4], transMatrix[5]);

    return result != 0;
}

/**
 * @brief Restore calibration data
 *
 * @param phost Pointer to Hal context
 */
void Calibration_Restore(EVE_HalContext* phost)
{
    EVE_Hal_wr32(phost, REG_TOUCH_TRANSFORM_A, a);
    EVE_Hal_wr32(phost, REG_TOUCH_TRANSFORM_B, b);
    EVE_Hal_wr32(phost, REG_TOUCH_TRANSFORM_C, c);
    EVE_Hal_wr32(phost, REG_TOUCH_TRANSFORM_D, d);
    EVE_Hal_wr32(phost, REG_TOUCH_TRANSFORM_E, e);
    EVE_Hal_wr32(phost, REG_TOUCH_TRANSFORM_F, f);
}

/**
 * @brief Save calibration data
 *
 * @param phost Pointer to Hal context
 */
void Calibration_Save(EVE_HalContext* phost)
{
    a = EVE_Hal_rd32(phost, REG_TOUCH_TRANSFORM_A);
    b = EVE_Hal_rd32(phost, REG_TOUCH_TRANSFORM_B);
    c = EVE_Hal_rd32(phost, REG_TOUCH_TRANSFORM_C);
    d = EVE_Hal_rd32(phost, REG_TOUCH_TRANSFORM_D);
    e = EVE_Hal_rd32(phost, REG_TOUCH_TRANSFORM_E);
    f = EVE_Hal_rd32(phost, REG_TOUCH_TRANSFORM_F);
}

/**
 * @brief Start a display list
 *
 * @param phost Pointer to Hal context
 * @param bgColor Background color
 * @param textColor Text color
 * @param tag Clear value for the tag buffer
 * @param frac Number of fractional bits in X, Y coordinates
 */
void Display_Start(EVE_HalContext* phost, uint8_t *bgColor, uint8_t *textColor, uint32_t tag, uint8_t frac)
{
    EVE_CoCmd_dlStart(phost);
    EVE_CoDl_clearTag(phost, tag);
    EVE_CoDl_clearColorRgb(phost, bgColor[0], bgColor[1], bgColor[2]);
    EVE_CoDl_clear(phost, 1, 1, 1);
    EVE_CoDl_colorRgb(phost, textColor[0], textColor[1], textColor[2]);
    EVE_CoDl_vertexFormat(phost, frac);
}

/**
 * @brief End a display list and show on screen
 *
 * @param phost Pointer to Hal context
 */
void Display_End(EVE_HalContext* phost)
{
    EVE_CoDl_display(phost);
    EVE_CoCmd_swap(phost);
    EVE_Cmd_waitFlush(phost);
}

/**
 * @brief Draw a text in center
 *
 * @param phost Pointer to Hal context
 * @param str Text to show
 * @param bgColor Background color
 * @param textColor Text Color
 */
static void Gpu_Text(EVE_HalContext* phost, const char* str, uint8_t *bgColor, uint8_t *textColor)
{
   uint16_t font = 0;

    if (phost->Width == 480)
    {
        font = 28;
    }
    else if (phost->Width == 800)
    {
        font = 30;
    }
    else if (phost->Width == 1024)
    {
        font = 31;
    }
    else if (phost->Width == 1280)
    {
        font = 31;
    } 
    else if (phost->Width == 1920)
    {
        font = 31;
    } 
    else {
        font = 27;
    }

    Display_Start(phost, bgColor, textColor, 0, 4);
    EVE_CoCmd_fillWidth(phost, phost->Width);
    EVE_CoCmd_text(phost, (uint16_t)(phost->Width / 2),
        (uint16_t)(phost->Height / 2), font, OPT_CENTERX | OPT_FILL, str);
    Display_End(phost);
}

/**
 * @brief Draw a text in center with 2 second delay
 *
 * @param phost Pointer to Hal context
 * @param str Text to show
 * @param bgColor Background color
 * @param textColor Text Color
 */
void Draw_TextColor(EVE_HalContext* phost, const char* str, uint8_t *bgColor, uint8_t *textColor)
{
    Gpu_Text(phost, str, bgColor, textColor);
    EVE_sleep(2000);
}

/**
 * @brief Draw a black text on white screen in center with 2 second delay
 *
 * @param phost Pointer to Hal context
 * @param str Text to show
 */
void Draw_Text(EVE_HalContext* phost, const char* str)
{
    Gpu_Text(phost, str, (uint8_t[]) { 0xff, 0xff, 0xff }, (uint8_t[]) { 0, 0, 0});
    EVE_sleep(2000);
}

/**
 * @brief Play a sound effect
 *
 * @param phost Pointer to Hal context
 * @param sound Sound effect
 * @param vol Volumn
 * @param midi MIDI note
 */
void Play_Sound(EVE_HalContext* phost, uint8_t sound, uint8_t vol, uint8_t midi)
{
    uint16_t val = (uint16_t) ((midi << 8) | sound);
    EVE_Hal_wr32(phost, REG_VOL_SOUND, (uint32_t)vol);
    EVE_Hal_wr32(phost, REG_SOUND, (uint32_t)val);
    EVE_Hal_wr32(phost, REG_PLAY, 1);
}

/**
 * @brief Initialize Flash and program Flash with a file
 * File should be in PC or MCU connected SD card depend on platform
 *
 * @param phost Pointer to Hal context
 * @param file File name, should be full name with path
 * @return uint32_t flash address where image saved
 */
uint32_t Flash_Init(EVE_HalContext* phost, const uint8_t *file)
{
#if defined(_WIN32)
#define _WHERE "PC"
#elif defined(RP2040_PLATFORM)
#define _WHERE "SDcard"
#endif

    /// show a dialog on the screen with two options: Yes or No?
    if (!Show_Diaglog_YesNo(phost, "Flash programming",
            "Program the flash with the file in " _WHERE "?\nIf not, the flash assets may not be found\nbecause the flash image offset was not saved.")) {
        /// If No, proceeds to boot up
        return 0;
    }

    /// If YES, Program the flash with the file
    uint32_t addr_flash = 0;
    uint32_t sent = Write_To_Flash_With_Progressbar(phost, file, &addr_flash, false);

    /// If fail to program flash, reset application
    if (0 >= sent) {
        uint8_t count = 5;
        uint8_t msg[100];
        while (count) {
            snprintf(msg, 100, "Error: Cannot open file: %s, exit in %u s",
                file, count);
            Show_Diaglog_Info(phost, msg);
            EVE_sleep(1000);
            count--;
        }
        exit(0);
    }
    eve_printf_debug("flash image programed at 0x%lx\n", addr_flash);
    return addr_flash;
}

/**
 * @brief Display a diaglog
 *
 * @param phost Pointer to Hal context
 * @param msg Message to show
 */
void Show_Diaglog_Info(EVE_HalContext* phost, const uint8_t* msg)
{
    uint8_t ret = 0;
    const uint32_t tag_y = 1;
    const uint32_t tag_n = 2;

    const uint16_t border = 2; // pixel
    const uint16_t base_w = 800;
    const uint16_t base_h = 600;
    uint16_t w = 400;
    uint16_t h = 200;
    uint16_t x = (base_w - w) / 2;
    uint16_t y = (base_h - h) / 2;
    uint8_t htop = 50;
    uint8_t hbottom = 50;
    uint8_t font = 31;

    uint16_t btn_h = 40;
    uint16_t btn_w = 100;

    uint32_t ratio = base_w * 100 / phost->Width;

    if (ratio != 100) {
        w = w * 100 / ratio;
        h = h * 100 / ratio;
        btn_w = btn_w * 100 / ratio;
        btn_h = btn_h * 100 / ratio;
        x = (phost->Width - w) / 2;
        y = (phost->Height - h) / 2;
        htop = htop * 100 / ratio;
        hbottom = hbottom * 100 / ratio;

        ratio += (100 - ratio) / 2;
    }

    Display_Start(phost, (uint8_t[]) { 0x3F, 0x3F, 0x3F }, (uint8_t[]) { 255, 255, 255 }, 0, 4);

    // diag border
    EVE_CoDl_colorRgb(phost, 0xE1, 0xE1, 0xE1);
    EVE_CoDl_vertex2f_4(phost, x * 16, y * 16);
    EVE_CoDl_vertex2f_4(phost, (x + w) * 16, (y + h) * 16);

    // center part
    EVE_CoDl_colorRgb(phost, 0xFF, 0xFF, 0xFF);
    EVE_CoDl_vertex2f_4(phost, (x + border) * 16, (y + border) * 16);
    EVE_CoDl_vertex2f_4(phost, (x + w - border) * 16, (y + h - border) * 16);

    // top part
    EVE_CoDl_colorRgb(phost, 0xF3, 0xF3, 0xF3);
    EVE_CoDl_vertex2f_4(phost, (x + border) * 16, (y + border) * 16);
    EVE_CoDl_vertex2f_4(phost, (x + w - border) * 16, (y + htop) * 16);

    // bottom part
    EVE_CoDl_colorRgb(phost, 0xF3, 0xF3, 0xF3);
    EVE_CoDl_vertex2f_4(phost, (x + border) * 16, (y + h - hbottom + border) * 16);
    EVE_CoDl_vertex2f_4(phost, (x + w - border) * 16, (y + h - border) * 16);
    EVE_CoDl_end(phost);

    EVE_CoDl_colorRgb(phost, 0x78, 0x78, 0x78);
    EVE_CoCmd_text(phost, x + border + 30, y + h / 2 - 20, font, 0, msg);

    Display_End(phost);

}

/**
 * @brief Display a dialog box with a title and Yes/No buttons
 *
 * @param phost Pointer to Hal context
 * @param title Title
 * @param msg Message to show
 */
uint8_t Show_Diaglog_YesNo(EVE_HalContext* phost, const uint8_t* title,	const uint8_t* msg)
{
    uint8_t ret = 0;
    const uint32_t tag_y = 1;
    const uint32_t tag_n = 2;

    const uint16_t border = 2; // pixel
    const uint16_t base_w = 800;
    const uint16_t base_h = 600;
    uint16_t w = 400;
    uint16_t h = 200;
    uint16_t x = (base_w - w) / 2;
    uint16_t y = (base_h - h) / 2;
    uint8_t htop = 50;
    uint8_t hbottom = 50;
    uint8_t font = 30;

    uint16_t btn_h = 30;
    uint16_t btn_w = 80;

    uint32_t ratio = base_w * 100 / phost->Width;

    if (ratio != 100) {
        w = w * 100 / ratio;
        h = h * 100 / ratio;
        btn_w = btn_w * 100 / ratio;
        btn_h = btn_h * 100 / ratio;
        x = (phost->Width - w) / 2;
        y = (phost->Height - h) / 2;
        htop = htop * 100 / ratio;
        hbottom = hbottom * 100 / ratio;
   }

    do {
        Display_Start(phost, (uint8_t[]) { 0x3F, 0x3F, 0x3F }, (uint8_t[]) { 0, 0, 0 }, 0, 4);

        EVE_CoDl_begin(phost, RECTS);
        // diag border
        EVE_CoDl_colorRgb(phost, 0xE1, 0xE1, 0xE1);
        EVE_CoDl_vertex2f_4(phost, x * 16, y * 16);
        EVE_CoDl_vertex2f_4(phost, (x + w) * 16, (y + h) * 16);

        // center part
        EVE_CoDl_colorRgb(phost, 0xFF, 0xFF, 0xFF);
        EVE_CoDl_vertex2f_4(phost, (x + border) * 16, (y + border) * 16);
        EVE_CoDl_vertex2f_4(phost, (x + w - border) * 16, (y + h - border) * 16);

        // top part
        EVE_CoDl_colorRgb(phost, 0x46, 0x50, 0x52);
        EVE_CoDl_vertex2f_4(phost, (x + border) * 16, (y + border) * 16);
        EVE_CoDl_vertex2f_4(phost, (x + w - border) * 16, (y + htop) * 16);

        // bottom part
        EVE_CoDl_colorRgb(phost, 0xF3, 0xF3, 0xF3);
        EVE_CoDl_vertex2f_4(phost, (x + border) * 16, (y + h - hbottom + border) * 16);
        EVE_CoDl_vertex2f_4(phost, (x + w - border) * 16, (y + h - border) * 16);
        EVE_CoDl_end(phost);

        // diag message
        EVE_CoDl_colorRgb(phost, 0xFF, 0xFF, 0xFF);
        EVE_CoCmd_text(phost, x + border + 10, y + border + 10, font, 0, title);

        EVE_CoDl_colorRgb(phost, 0x78, 0x78, 0x78);
        EVE_CoCmd_fillWidth(phost, w);
        EVE_CoCmd_text(phost, x + border + 30, y + h / 2 - 20, font, OPT_FILL, msg);

        // diag button yes/no
        EVE_CoDl_colorRgb(phost, 0xFF, 0xFF, 0xFF);
        const uint16_t btn_margin = 25;
        EVE_CoDl_tag(phost, tag_y);
        EVE_CoCmd_button(phost, x + w / 2 - btn_w - btn_margin,
                y + h - hbottom + border + (hbottom - btn_h) / 2, btn_w, btn_h,
                font, 0, "Yes");

        EVE_CoDl_tag(phost, tag_n);
        EVE_CoCmd_button(phost, x + w / 2 + btn_margin,
                y + h - hbottom + border + (hbottom - btn_h) / 2, btn_w, btn_h,
                font, 0, "No");
        EVE_CoDl_tag(phost, 0);

        Display_End(phost);
        EVE_sleep(10);

        uint32_t tag = EVE_Hal_rd32(phost, REG_TOUCH_TAG) & 0xFFFFFF;
        if (tag == tag_y)
        {
            return true;
        }
        else if (tag == tag_n)
        {
            return false;
        }
    } while (true);

    return false;
}

/* This image contains cells: 32x32
Cell 0: Home
Cell 1: Star
Cell 2: Down triangle
Cell 3: Up triangle
Cell 4: Right triangle (Play button icon)
Size: 460 bytes */
const unsigned char home_start_icon[] = { 0x78, 0x9C, 0xE5, 0x94, 0xBF, 0x4E, 0xC2, 0x40, 0x1C, 0xC7, 0x7F, 0x2D, 0x04, 0x8B, 0x20, 0x45, 0x76, 0x14, 0x67, 0xA3, 0xF1, 0x0D, 0x64, 0x75, 0xD2, 0xD5, 0x09, 0x27, 0x17, 0x13, 0xE1, 0x0D, 0xE4, 0x0D, 0x78, 0x04, 0x98, 0x5D, 0x30, 0x26, 0x0E, 0x4A, 0xA2, 0x3E, 0x82, 0x0E, 0x8E, 0x82, 0xC1, 0x38, 0x62, 0x51, 0x0C, 0x0A, 0x42, 0x7F, 0xDE, 0xB5, 0x77, 0xB4, 0x77, 0x17, 0x28, 0x21, 0x26, 0x46, 0xFD, 0x26, 0xCD, 0xE5, 0xD3, 0x7C, 0xFB, 0xBB, 0xFB, 0xFD, 0xB9, 0x02, 0xCC, 0xA4, 0xE8, 0x99, 0x80, 0x61, 0xC4, 0x8A, 0x9F, 0xCB, 0x6F, 0x31, 0x3B, 0xE3, 0x61, 0x7A, 0x98, 0x84, 0x7C, 0x37, 0xF6, 0xFC, 0xC8, 0xDD, 0x45, 0x00, 0xDD, 0xBA, 0xC4, 0x77, 0xE6, 0xEE, 0x40, 0xEC, 0x0E, 0xE6, 0x91, 0xF1, 0xD2, 0x00, 0x42, 0x34, 0x5E, 0xCE, 0xE5, 0x08, 0x16, 0xA0, 0x84, 0x68, 0x67, 0xB4, 0x86, 0xC3, 0xD5, 0x26, 0x2C, 0x20, 0x51, 0x17, 0xA2, 0xB8, 0x03, 0xB0, 0xFE, 0x49, 0xDD, 0x54, 0x15, 0xD8, 0xEE, 0x73, 0x37, 0x95, 0x9D, 0xD4, 0x1A, 0xB7, 0xA5, 0x26, 0xC4, 0x91, 0xA9, 0x0B, 0x06, 0xEE, 0x72, 0xB7, 0xFB, 0xC5, 0x16, 0x80, 0xE9, 0xF1, 0x07, 0x8D, 0x3F, 0x15, 0x5F, 0x1C, 0x0B, 0xFC, 0x0A, 0x90, 0xF0, 0xF3, 0x09, 0xA9, 0x90, 0xC4, 0xC6, 0x37, 0xB0, 0x93, 0xBF, 0xE1, 0x71, 0xDB, 0xA9, 0xD7, 0x41, 0xAD, 0x46, 0xEA, 0x19, 0xA9, 0xD5, 0xCE, 0x93, 0xB3, 0x35, 0x73, 0x0A, 0x69, 0x59, 0x91, 0xC3, 0x0F, 0x22, 0x1B, 0x1D, 0x91, 0x13, 0x3D, 0x91, 0x73, 0x43, 0xF1, 0x6C, 0x55, 0xDA, 0x3A, 0x4F, 0xBA, 0x25, 0xCE, 0x4F, 0x04, 0xF1, 0xC5, 0xCF, 0x71, 0xDA, 0x3C, 0xD7, 0xB9, 0xB2, 0x48, 0xB4, 0x89, 0x38, 0x20, 0x4B, 0x2A, 0x95, 0x0C, 0xD5, 0xEF, 0x5B, 0xAD, 0x96, 0x45, 0x8A, 0x41, 0x96, 0x7A, 0x1F, 0x60, 0x0D, 0x7D, 0x22, 0x75, 0x82, 0x2B, 0x0F, 0xFB, 0xCE, 0x51, 0x3D, 0x2E, 0x3A, 0x21, 0xF3, 0x1C, 0xD9, 0x38, 0x86, 0x2C, 0xC6, 0x05, 0xB6, 0x7B, 0x9A, 0x8F, 0x0F, 0x97, 0x1B, 0x72, 0x6F, 0x1C, 0xEB, 0xAE, 0xFF, 0xDA, 0x97, 0x0D, 0xBA, 0x43, 0x32, 0xCA, 0x66, 0x34, 0x3D, 0x54, 0xCB, 0x24, 0x9B, 0x43, 0xF2, 0x70, 0x3E, 0x42, 0xBB, 0xA0, 0x95, 0x11, 0x37, 0x46, 0xE1, 0x4F, 0x49, 0xC5, 0x1B, 0xFC, 0x3C, 0x3A, 0x3E, 0xD1, 0x65, 0x0E, 0x6F, 0x58, 0xF8, 0x9E, 0x5B, 0xDB, 0x55, 0xB6, 0x41, 0x34, 0xCB, 0xBE, 0xDB, 0x87, 0x5F, 0xA9, 0xD1, 0x85, 0x6B, 0xB3, 0x17, 0x9C, 0x61, 0x0C, 0x9B, 0xA2, 0x5D, 0x61, 0x10, 0xED, 0x2A, 0x9B, 0xA2, 0x5D, 0x61, 0x10, 0xED, 0x2A, 0x9B, 0xA2, 0x5D, 0x61, 0x10, 0xED, 0x2A, 0x9B, 0xED, 0xC9, 0xFC, 0xDF, 0x14, 0x54, 0x8F, 0x80, 0x7A, 0x06, 0xF5, 0x23, 0xA0, 0x9F, 0x41, 0xF3, 0x10, 0x30, 0x4F, 0x41, 0xF3, 0x18, 0x30, 0xCF, 0xCA, 0xFC, 0xFF, 0x35, 0xC9, 0x79, 0xC9, 0x89, 0xFA, 0x33, 0xD7, 0x1D, 0xF6, 0x5E, 0x84, 0x5C, 0x56, 0x6E, 0xA7, 0xDA, 0x1E, 0xF9, 0xFA, 0xAB, 0xF5, 0x97, 0xFF, 0x2F, 0xED, 0x89, 0x7E, 0x29, 0x9E, 0xB4, 0x9F, 0x74, 0x1E, 0x69, 0xDA, 0xA4, 0x9F, 0x81, 0x94, 0xEF, 0x4F, 0xF6, 0xF9, 0x0B, 0xF4, 0x65, 0x51, 0x08 };

/**
 * @brief Display a welcome screen
 *
 * @param phost Pointer to Hal context
 * @param info Information to show
 */
void WelcomeScreen(EVE_HalContext *phost, char *info[])
{
    EVE_CoCmd_inflate(phost, 0, 0);
    EVE_Cmd_wrMem(phost,home_start_icon,sizeof(home_start_icon)); //Load from RAM
    EVE_Cmd_waitFlush(phost);

    do
    {
        Display_Start(phost, (uint8_t[]) { 255, 255, 255 }, (uint8_t[]) { 0, 0, 0 }, 0, 4);
        EVE_CoCmd_text(phost, phost->Width / 2, 40, 31, OPT_CENTER, info[0]);
        EVE_CoCmd_text(phost, phost->Width / 2, 80, 31, OPT_CENTER, info[1]);
        EVE_CoCmd_text(phost, phost->Width / 2, 120, 31, OPT_CENTER, info[2]);
        EVE_CoCmd_text(phost, phost->Width / 2, 140, 31, OPT_CENTER, info[3]);
        EVE_CoCmd_text(phost, phost->Width / 2, 200, 31, OPT_CENTER, "Click to play");

        // play button circle boundary
        EVE_CoDl_colorRgb(phost, 100, 100, 100);
        EVE_CoDl_begin(phost, POINTS);
        EVE_CoDl_pointSize(phost, 50 * 16);
        EVE_CoDl_tag(phost, 'P');
        EVE_CoDl_vertex2f_4(phost, (phost->Width / 2) * 16, (phost->Height / 2) * 16);
        EVE_CoDl_colorRgb(phost, 180, 35, 35);

        // play button
        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_bitmapHandle(phost, 0); // handle for background stars
        EVE_CoDl_bitmapSource(phost, 0); // Starting address in ramg
        EVE_CoDl_cell(phost, 4);
        EVE_CoCmd_loadIdentity(phost);
        EVE_CoCmd_scale(phost, 65536 * 3, 65536 * 3);
        EVE_CoCmd_setMatrix(phost);
        EVE_CoDl_bitmapLayout(phost, L4, 16, 32); // format
        EVE_CoDl_bitmapSize(phost, NEAREST, BORDER, BORDER, 32 * 3, 32 * 3);
        EVE_CoDl_vertex2f_4(phost, (phost->Width / 2 - 45) * 16, (phost->Height / 2 - 48) * 16);
        EVE_CoDl_end(phost);
        EVE_CoCmd_loadIdentity(phost);
        EVE_CoCmd_setMatrix(phost);
        Display_End(phost);

    } while (EVE_Hal_rd32(phost, REG_TOUCH_TAG) != 'P');

    Play_Sound(phost, 0x50, 255, 0xc0);
}

/**
 * @brief Fadeout animation
 *
 * @param phost Pointer to Hal context
 */
void fadeout(EVE_HalContext *phost)
{
    for (int8_t i = 100; i >= 0; i -= 3)
    {
        EVE_Hal_wr32(phost, REG_PWM_DUTY, (uint32_t)i);
        EVE_sleep(2); //sleep for 2 ms
    }
}

/**
 * @brief Fadein animation
 * perform display fadein effect by changing the display PWM from 0 till 100 and finally 128
 *
 * @param phost Pointer to Hal context
 */
void fadein(EVE_HalContext *phost)
{
    for (int8_t i = 0; i <= 100; i += 3)
    {
        EVE_Hal_wr32(phost, REG_PWM_DUTY, (uint32_t)i);
        EVE_sleep(2); //sleep for 2 ms
    }
    /* Finally make the PWM 100% */
    uint8_t i = 128;
    EVE_Hal_wr32(phost, REG_PWM_DUTY, (uint32_t)i);
}

/**
 * @brief Scanout from swapchain_0
 *
 * @param phost Pointer to Hal context
 * @param fbformat Scanout format
 * @param w Width
 * @param h Height
 */
void scanout_swapping(EVE_HalContext *phost, uint16_t fbformat, uint16_t w, uint16_t h)
{
    EVE_Hal_wr32(phost, REG_SO_SOURCE, SWAPCHAIN_0);
    EVE_Hal_wr32(phost, REG_SO_FORMAT, fbformat);
    EVE_Hal_wr32(phost, REG_SO_EN, 1);
    EVE_CoCmd_renderTarget(phost, SWAPCHAIN_0, fbformat, w, h);
}

/**
 * @brief Scanout from single address
 *
 * @param phost Pointer to Hal context
 * @param fbformat Scanout format
 * @param w Width
 * @param h Height
 */
void scanout_single(EVE_HalContext *phost, uint16_t fbformat, uint16_t w, uint16_t h)
{
    EVE_Hal_wr32(phost, REG_SO_SOURCE, DDR_FRAMEBUFFER_STARTADDR);
    EVE_Hal_wr32(phost, REG_SO_FORMAT, fbformat);
    EVE_Hal_wr32(phost, REG_SO_EN, 1);
    EVE_CoCmd_renderTarget(phost, DDR_FRAMEBUFFER_STARTADDR, fbformat, w, h);
}

/**
 * @brief Config LVDS
 *
 * @param phost Pointer to Hal context
 * @param format Scanout format
 * @param mode Display mode
 */
void Display_Config(EVE_HalContext *phost, uint16_t format, Display_mode mode)
{
    uint8_t TXPLLDiv = 0;
    uint8_t lvdspll_cps = 0;
    uint16_t lock_delay = 0x180; //384
    uint8_t lvdspll_cks = 0;
    uint8_t lvdspll_ns = 7;
    uint16_t w, h;
    w = phost->Width;
    h = phost->Height;
    TXPLLDiv = 0x03;
    eve_printf_debug("TXPLLDiv %d\n", TXPLLDiv);
    lvdspll_cks = TXPLLDiv > 4 ? 1 : 2;
    EVE_Hal_wr32(phost, REG_LVDSTX_PLLCFG, LVDSTX_PLLCFG(lvdspll_cps, lock_delay, lvdspll_cks, lvdspll_ns, TXPLLDiv));
#ifdef DISPLAY_RESOLUTION_WXGA
    EVE_Hal_wr32(phost, REG_LVDSTX_EN, LVDS_CH0_EN);
#else
    EVE_Hal_wr32(phost, REG_LVDSTX_EN, LVDS_CH1_EN | LVDS_CH0_EN);
#endif
    EVE_sleep(10);
    eve_printf_debug("LVDSTX_EN: %lx \n", EVE_Hal_rd32(phost, REG_LVDSTX_EN));
    eve_printf_debug("LVDSTX_PLLCFG: %lx \n", EVE_Hal_rd32(phost, REG_LVDSTX_PLLCFG));
    eve_printf_debug("LVDSTX_CTRL_CH0: %lx \n", EVE_Hal_rd32(phost, REG_LVDSTX_CTRL_CH0));
    eve_printf_debug("LVDSTX_CTRL_CH1: %lx \n", EVE_Hal_rd32(phost, REG_LVDSTX_CTRL_CH1));

    EVE_Hal_wr32(phost, REG_SC0_SIZE, 2); // set number of buffers to 2
    EVE_Hal_wr32(phost, REG_SC0_PTR0, SC0_PTR0_STARTADDR);
    EVE_Hal_wr32(phost, REG_SC0_PTR1, SC0_PTR1_STARTADDR);

    EVE_Hal_wr32(phost, REG_SC1_SIZE, 2); // set number of buffers to 2
    EVE_Hal_wr32(phost, REG_SC1_PTR0, SC1_PTR0_STARTADDR);
    EVE_Hal_wr32(phost, REG_SC1_PTR1, SC1_PTR1_STARTADDR);

    EVE_Hal_wr32(phost, REG_SC2_SIZE, 2); // set number of buffers to 2
    EVE_Hal_wr32(phost, REG_SC2_PTR0, SC2_PTR0_STARTADDR);
    EVE_Hal_wr32(phost, REG_SC2_PTR1, SC2_PTR1_STARTADDR);
    EVE_Cmd_waitFlush(phost);

#ifdef DISPLAY_RESOLUTION_WXGA
    EVE_Hal_wr32(phost, REG_SO_MODE, TWO_PIXEL_SINGLE_LVDS);
#else
    EVE_Hal_wr32(phost, REG_SO_MODE, FOUR_PIXEL_DUAL_LVDS);
#endif

    if (mode == MODE_PICTURE)
    {
        scanout_single(phost, format, w, h);
    }
    else if (mode == MODE_DIRECTVIDEO)
    {
        EVE_Hal_wr32(phost, REG_SO_FORMAT, format);

        EVE_Hal_wr32(phost, REG_SO_EN, 0); // disable
        EVE_Hal_wr32(phost, REG_SC1_RESET, 1); // write to reset
        EVE_Hal_wr32(phost, REG_SO_SOURCE, SWAPCHAIN_1);
        EVE_Hal_wr32(phost, REG_SO_EN, 1); // enable
    }
    else if (mode == MODE_VIDEO)
    {
        scanout_swapping(phost, format, w, h);
    }
    else if (mode == MODE_LVDSRX)
    {
        // This LVDSRX mode is not using Swapchain 2 to display, so only simple address is required
        EVE_Hal_wr32(phost, REG_LVDSRX_CORE_ENABLE, 1);
        EVE_Hal_wr32(phost, REG_LVDSRX_CORE_CAPTURE, 1);
        EVE_Hal_wr32(phost, REG_LVDSRX_CORE_SETUP, ((LVDSRX_TWO_CHANNEL << 1) | (LVDSRX_ONE_PIXEL_PER_CLK & 0x1)));
        EVE_Hal_wr32(phost, REG_LVDSRX_CORE_DEST, DDR_FRAMEBUFFER_STARTADDR);
        EVE_Hal_wr32(phost, REG_LVDSRX_CORE_FORMAT, RGB8);
        EVE_Hal_wr32(phost, REG_LVDSRX_CORE_DITHER, 0);

        EVE_Hal_wr32(phost, REG_LVDSRX_SETUP, ((LVDSRX_MODE_VESA_24 << 3) | (VS_POL_HIGH << 2) | (LVDSRX_TWO_CHANNEL) << 1 | LVDSRX_ONE_PIXEL_PER_CLK));
        EVE_Hal_wr32(phost, REG_LVDSRX_CTRL, ((8 << 12) | (CHn_CLKSEL_FALLING << 11) | (CHn_FRANGE_10_30 << 9) | (CHn_PWDN_B_ON << 8) | 
                                              (8 << 4) | (CHn_CLKSEL_FALLING << 3) | (CHn_FRANGE_10_30 << 1) | CHn_PWDN_B_ON));

        scanout_swapping(phost, format, w, h);
    }
    else if (mode == MODE_LVDSRX_SC)
    {
        // This LVDSRX mode is using Swapchain 2 to display
        EVE_Hal_wr32(phost, REG_LVDSRX_CORE_ENABLE, 1);
        EVE_Hal_wr32(phost, REG_LVDSRX_CORE_CAPTURE, 1);
        EVE_Hal_wr32(phost, REG_LVDSRX_CORE_SETUP, ((LVDSRX_TWO_CHANNEL << 1) | (LVDSRX_ONE_PIXEL_PER_CLK & 0x1)));
        EVE_Hal_wr32(phost, REG_LVDSRX_CORE_DEST, SWAPCHAIN_2);
        EVE_Hal_wr32(phost, REG_LVDSRX_CORE_FORMAT, RGB8);
        EVE_Hal_wr32(phost, REG_LVDSRX_CORE_DITHER, 0);

        EVE_Hal_wr32(phost, REG_LVDSRX_SETUP, ((LVDSRX_MODE_VESA_24 << 3) | (VS_POL_HIGH << 2) | (LVDSRX_TWO_CHANNEL) << 1 | LVDSRX_ONE_PIXEL_PER_CLK));
        EVE_Hal_wr32(phost, REG_LVDSRX_CTRL, ((8 << 12) | (CHn_CLKSEL_FALLING << 11) | (CHn_FRANGE_10_30 << 9) | (CHn_PWDN_B_ON << 8) | 
                                              (8 << 4) | (CHn_CLKSEL_FALLING << 3) | (CHn_FRANGE_10_30 << 1) | CHn_PWDN_B_ON));

        scanout_swapping(phost, format, w, h);
    }
    else if (mode == MODE_LVDSRX_LOW_RESOLUTION)
    {
        // This LVDSRX mode is using Swapchain 2 to display with lower resolution (800*600)
        EVE_Hal_wr32(phost, REG_LVDSRX_CORE_ENABLE, 1);
        EVE_Hal_wr32(phost, REG_LVDSRX_CORE_CAPTURE, 1);
        EVE_Hal_wr32(phost, REG_LVDSRX_CORE_SETUP, ((LVDSRX_ONE_CHANNEL << 1) | (LVDSRX_ONE_PIXEL_PER_CLK & 0x1)));
        EVE_Hal_wr32(phost, REG_LVDSRX_CORE_DEST, SWAPCHAIN_2);
        EVE_Hal_wr32(phost, REG_LVDSRX_CORE_FORMAT, RGB565);
        EVE_Hal_wr32(phost, REG_LVDSRX_CORE_DITHER, 0);

        EVE_Hal_wr32(phost, REG_LVDSRX_SETUP, ((LVDSRX_MODE_JEIDA_18 << 3) | (VS_POL_HIGH << 2) | (LVDSRX_ONE_CHANNEL) << 1 | LVDSRX_ONE_PIXEL_PER_CLK));
        EVE_Hal_wr32(phost, REG_LVDSRX_CTRL, ((8 << 12) | (CHn_CLKSEL_FALLING << 11) | (CHn_FRANGE_10_30 << 9) | (CHn_PWDN_B_ON << 8) |
                                              (8 << 4) | (CHn_CLKSEL_FALLING << 3) | (CHn_FRANGE_10_30 << 1) | CHn_PWDN_B_ON));

        scanout_swapping(phost, format, w, h);
    }
    else
    {
        scanout_swapping(phost, format, w, h);
    }

    EVE_Cmd_waitFlush(phost);
    eve_printf_debug("LVDSTX_STAT is %lx \n", EVE_Hal_rd32(phost, REG_LVDSTX_STAT));
    eve_printf_debug("LVDSTX_ERR_STAT is %lx \n", EVE_Hal_rd32(phost, REG_LVDSTX_ERR_STAT));
}

#if defined(RP2040_PLATFORM)
void strcat_s(char *dest, size_t dest_size, const char *src)
{
    size_t space_left = dest_size - strlen(dest) - 1; // leave space for null terminator

    if (space_left > 0)
    {
        strncat(dest, src, space_left);
    }
    else
    {
        // Optional: handle error or truncation
        eve_printf_debug("Buffer full. Cannot append string.\n");
    }
}
#endif