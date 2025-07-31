/**
 * @file Font.c
 * @brief Sample usage of font
 *
 * @author Bridgetek
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

#include "Font.h"
#include "Common.h"
#include "FileTransfer.h"
#include "FlashHelper.h"

#define SAMAPP_DELAY           EVE_sleep(2000);

static EVE_HalContext s_halContext;
static EVE_HalContext* s_pHalContext;
void SAMAPP_Font();

int main(int argc, char* argv[])
{
    s_pHalContext = &s_halContext;
    Gpu_Init(s_pHalContext);
    LVDS_Config(s_pHalContext, YCBCR, MODE_PICTURE);

    // read and store calibration setting
#if GET_CALIBRATION == 1
    EVE_Calibrate(s_pHalContext);
    Calibration_Save(s_pHalContext);
#endif

    // Write flash binary
    Flash_Init(s_pHalContext, TEST_DIR "/Flash/flash.bin", "flash.bin");
    EVE_Util_clearScreen(s_pHalContext);

    char *info[] =
    {  "EVE Sample Application",
        "This sample demonstrate the using of font", 
        "",
        ""
    }; 

    while (TRUE) {
        WelcomeScreen(s_pHalContext, info);

        SAMAPP_Font();

        EVE_Util_clearScreen(s_pHalContext);

        Gpu_Release(s_pHalContext);

        /* Init HW Hal for next loop*/
        Gpu_Init(s_pHalContext);
        LVDS_Config(s_pHalContext, YCBCR, MODE_PICTURE);
#if GET_CALIBRATION == 1
        Calibration_Restore(s_pHalContext);
#endif
    }

    return 0;
}

/**
* @brief Inbuilt font example for proportional text
* only font handle 16 - 25 can be printed by cell
* This function is solely for displaying fonts in a legacy format and is not intended for regular use
*/
void SAMAPP_Font_romFonts()
{
    int32_t j;
    int32_t hoffset;
    int32_t voffset;
    int32_t stringlen1;
    int32_t stringlen2;
    int32_t stringlen3;
    uint32_t FontTableAddressBase = s_pHalContext->DDR_RamSize - 256; // Pseudo-ROM font table
    uint32_t FontTableAddress;
    const uint8_t Display_string1[] = "!\"#$%&'()*+,-./0123456789:;<=>?";
    const uint8_t Display_string2[] = "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_";
    const uint8_t Display_string3[] = "`abcdefghijklmnopqrstuvwxyz{|}  ";
    EVE_Gpu_Fonts fontstruct;

    Draw_Text(s_pHalContext, "Example for: ROM fonts 16 to 25");

    stringlen1 = sizeof(Display_string1) - 1;
    stringlen2 = sizeof(Display_string2) - 1;
    stringlen3 = sizeof(Display_string3) - 1;

    for (int romFont = 16; romFont < 26; romFont++)
    {
        if (romFont == 17 || romFont == 19)
            continue;
        /* Read the font address from 0xFFFFC location */
        FontTableAddress = EVE_Hal_rd32(s_pHalContext, FontTableAddressBase + 4 * romFont);
        eve_printf_debug("ROM Font address read from 0x%lx is %x \n", FontTableAddressBase + 4 * romFont, FontTableAddress);
        /* Read the font table from hardware */
        EVE_Hal_rdMem(s_pHalContext, (uint8_t *)&fontstruct, FontTableAddress, EVE_GPU_FONT_TABLE_SIZE);
        eve_printf_debug("Inbuilt font structure idx %d fmt 0x%x flags 0x%x hight 0x%x line stride 0x%x width 0x%x Fontaddr 0x%x \n", romFont,
            fontstruct.FontBitmapFormat, fontstruct.FontFlags, fontstruct.FontHeightInPixels,
            fontstruct.FontLineStride, fontstruct.FontWidthInPixels,
            fontstruct.PointerToFontGraphicsData);

        Display_StartColor(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 });
        hoffset = 50;
        voffset = 100;
        char str[12];
        snprintf(str, 12, "ROM FONT %d", romFont);
        EVE_CoCmd_text(s_pHalContext, hoffset, voffset, romFont, 0, str);
        /* Display string */
        EVE_CoDl_begin(s_pHalContext, BITMAPS);
        voffset += 100;

        EVE_CoDl_bitmapHandle(s_pHalContext, romFont);
        for (j = 0; j < stringlen1; j++)
        {
            EVE_CoDl_cell(s_pHalContext, Display_string1[j]);
            EVE_CoDl_vertex2f_4(s_pHalContext, hoffset * 16, voffset * 16);
            hoffset += fontstruct.FontWidth[Display_string1[j]];
        }
        hoffset = 50;
        voffset += fontstruct.FontHeightInPixels + 20;
        for (j = 0; j < stringlen2; j++)
        {
            EVE_CoDl_cell(s_pHalContext, Display_string2[j]);
            EVE_CoDl_vertex2f_4(s_pHalContext, hoffset * 16, voffset * 16);
            hoffset += fontstruct.FontWidth[Display_string2[j]];
        }
        hoffset = 50;
        voffset += fontstruct.FontHeightInPixels + 20;
        for (j = 0; j < stringlen3; j++)
        {
            EVE_CoDl_cell(s_pHalContext, Display_string3[j]);
            EVE_CoDl_vertex2f_4(s_pHalContext, hoffset * 16, voffset * 16);
            hoffset += fontstruct.FontWidth[Display_string3[j]];
        }

        EVE_CoDl_end(s_pHalContext);

        Display_End(s_pHalContext);
        SAMAPP_DELAY;
    }
}

/**
* @brief API to demonstrate custom font display from a JPEG file
*/
void SAMAPP_Font_fromJPEG()
{
    EVE_Gpu_Fonts fontstruct;
    Draw_Text(s_pHalContext, "Example for: Custom font from a JPEG file");

    /*************************************************************************/
    /* Below code demonstrates the usage of setfont. Setfont function draws  */
    /* custom configured fonts on screen. Download the font table and raw    */
    /* font data in L1/L4/L8 format and display text                          */
    /*************************************************************************/
    memset(fontstruct.FontWidth, 16, 128);
    fontstruct.FontBitmapFormat = L8;
    fontstruct.FontFlags = 0;
    fontstruct.FontLineStride = 16;
    fontstruct.FontWidthInPixels = 16;
    fontstruct.FontHeightInPixels = 16;
    fontstruct.PointerToFontGraphicsData = 1024; //data address - starts at location 1024
    EVE_Hal_wrMem(s_pHalContext, RAM_G, (int8_t *)&fontstruct, EVE_GPU_FONT_TABLE_SIZE);

    /* Characters from 32 to 128 are present and each character is 16*16 dimention */
    /******************* Decode jpg output into location 1024 and output color format as MONO *********************/
    EVE_Util_loadImageFile(s_pHalContext, 1024, TEST_DIR "\\font16.jpg", NULL, OPT_MONO);

    Display_StartColor(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 });
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 20, 30, OPT_CENTER,
        "SetFont - format L8");
    EVE_CoCmd_setFont(s_pHalContext, 7, RAM_G, 32);

    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 80, 7, OPT_CENTER,
        "The quick brown fox jumps");
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 120, 7, OPT_CENTER,
        "over the lazy dog.");
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 160, 7, OPT_CENTER,
        "1234567890");
    EVE_CoCmd_resetFonts(s_pHalContext);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief API to demonstrate custom font display from a converted font file
*/
void SAMAPP_Font_fromConvertedTTF()
{
    EVE_Gpu_Fonts fontstruct;

    Draw_Text(s_pHalContext, "Example for: Custom font from a converted font file");
    EVE_Util_loadRawFile(s_pHalContext, RAM_G, TEST_DIR "\\Roboto-BoldCondensed_30_L4.raw");

    EVE_Hal_rdMem(s_pHalContext, (uint8_t *)&fontstruct, RAM_G, EVE_GPU_FONT_TABLE_SIZE);
    eve_printf_debug("font structure fmt 0x%x flags 0x%x hight 0x%x line stride 0x%x width 0x%x Fontaddr 0x%x \n",
        fontstruct.FontBitmapFormat, fontstruct.FontFlags, fontstruct.FontHeightInPixels,
        fontstruct.FontLineStride, fontstruct.FontWidthInPixels,
        fontstruct.PointerToFontGraphicsData);

    Display_StartColor(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 });
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 20, 30, OPT_CENTER,
        "Legacy");
    EVE_CoCmd_setFont(s_pHalContext, 6, RAM_G, 32);
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 80, 6, OPT_CENTER,
        "The quick brown fox jumps");
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 120, 6, OPT_CENTER,
        "over the lazy dog.");
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 160, 6, OPT_CENTER,
        "1234567890");
    EVE_CoCmd_resetFonts(s_pHalContext);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief API to demonstrate custom font display from a converted font file in flash
*/
void SAMAPP_Font_fromConvertedTTF_flash()
{
    EVE_Gpu_Fonts fontstruct;
    if (!FlashHelper_SwitchFullMode(s_pHalContext))
    {
        eve_printf("Cannot switch flash full mode");
        return;
    }
    Draw_Text(s_pHalContext, "Example for: Custom font from a converted font file in flash");
    EVE_CoCmd_flashSource(s_pHalContext, 4096);
    EVE_CoCmd_loadAsset(s_pHalContext, RAM_G, OPT_FLASH);

    EVE_Hal_rdMem(s_pHalContext, (uint8_t *)&fontstruct, RAM_G, EVE_GPU_FONT_TABLE_SIZE);
    eve_printf_debug("font structure fmt 0x%x flags 0x%x hight 0x%x line stride 0x%x width 0x%x Fontaddr 0x%x \n",
        fontstruct.FontBitmapFormat, fontstruct.FontFlags, fontstruct.FontHeightInPixels,
        fontstruct.FontLineStride, fontstruct.FontWidthInPixels,
        fontstruct.PointerToFontGraphicsData);

    Display_StartColor(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 });
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 20, 30, OPT_CENTER,
        "Legacy - from Flash");
    EVE_CoCmd_setFont(s_pHalContext, 6, RAM_G, 32);
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 80, 6, OPT_CENTER,
        "The quick brown fox jumps");
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 120, 6, OPT_CENTER,
        "over the lazy dog.");
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 160, 6, OPT_CENTER,
        "1234567890");
    EVE_CoCmd_resetFonts(s_pHalContext);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief API to demonstrate custom font display from a converted font file in SD card
*/
void SAMAPP_Font_fromConvertedTTF_SD()
{
    EVE_Gpu_Fonts fontstruct;
    uint32_t result = 0;

    Draw_Text(s_pHalContext, "Example for: Custom font from a converted font file in SD card");
	result = EVE_CoCmd_sdattach(s_pHalContext, OPT_4BIT | OPT_IS_SD, result);
	eve_printf_debug("SD attach status 0x%x \n", result);
	if (result != 0)
	{
		eve_printf_debug("SD attach failed\n");
		return;
	}

	result = EVE_CoCmd_fssource(s_pHalContext, "Roboto-BoldCondensed_30_L4.reloc", 0);
	eve_printf_debug("file read status 0x%x \n", result);
	if (result != 0)
	{
		eve_printf_debug("SD read failed\n");
		return;
	}

    EVE_CoCmd_loadAsset(s_pHalContext, RAM_G, OPT_FS);

    EVE_Hal_rdMem(s_pHalContext, (uint8_t *)&fontstruct, RAM_G, EVE_GPU_FONT_TABLE_SIZE);
    eve_printf_debug("font structure fmt 0x%x flags 0x%x hight 0x%x line stride 0x%x width 0x%x Fontaddr 0x%x \n",
        fontstruct.FontBitmapFormat, fontstruct.FontFlags, fontstruct.FontHeightInPixels,
        fontstruct.FontLineStride, fontstruct.FontWidthInPixels,
        fontstruct.PointerToFontGraphicsData);

    Display_StartColor(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 });
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 20, 30, OPT_CENTER,
        "Legacy - from SD");
    EVE_CoCmd_setFont(s_pHalContext, 6, RAM_G, 32);
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 80, 6, OPT_CENTER,
        "The quick brown fox jumps");
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 120, 6, OPT_CENTER,
        "over the lazy dog.");
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 160, 6, OPT_CENTER,
        "1234567890");
    EVE_CoCmd_resetFonts(s_pHalContext);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief API to demonstrate custom font display from a converted font file from CMDB
*/
void SAMAPP_Font_fromConvertedTTF_CMDB()
{
    EVE_Gpu_Fonts fontstruct;

    Draw_Text(s_pHalContext, "Example for: Custom font from a converted font file from CMDB");
    EVE_CoCmd_loadAsset(s_pHalContext, RAM_G, 0);
    EVE_Util_loadCmdFile(s_pHalContext, TEST_DIR "\\Roboto-BoldCondensed_30_L4.reloc", NULL);

    EVE_Hal_rdMem(s_pHalContext, (uint8_t*)&fontstruct, RAM_G, EVE_GPU_FONT_TABLE_SIZE);
    eve_printf_debug("font structure fmt 0x%x flags 0x%x hight 0x%x line stride 0x%x width 0x%x Fontaddr 0x%x \n",
        fontstruct.FontBitmapFormat, fontstruct.FontFlags, fontstruct.FontHeightInPixels,
        fontstruct.FontLineStride, fontstruct.FontWidthInPixels,
        fontstruct.PointerToFontGraphicsData);

    Display_StartColor(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 });
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 20, 30, OPT_CENTER,
        "Legacy - from CMDB");
    EVE_CoCmd_setFont(s_pHalContext, 6, RAM_G, 32);
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 80, 6, OPT_CENTER,
        "The quick brown fox jumps");
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 120, 6, OPT_CENTER,
        "over the lazy dog.");
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 160, 6, OPT_CENTER,
        "1234567890");
    EVE_CoCmd_resetFonts(s_pHalContext);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief API to demonstrate custom extened font display from a converted font file
*/
void SAMAPP_Font_extendedFormat()
{
    EVE_Gpu_FontsExt fontstruct;

    Draw_Text(s_pHalContext, "Example for: extended font (extended format 1)");
    EVE_Util_loadRawFile(s_pHalContext, RAM_G, TEST_DIR "\\Roboto-BoldCondensed_30_L1.raw");

    EVE_Hal_rdMem(s_pHalContext, (uint8_t *)&fontstruct, RAM_G, sizeof(EVE_Gpu_FontsExt));
    eve_printf_debug("signature 0x%lx size %ld fmt %d flags 0x%x layout_width %ld layout_height %ld pixel_width %ld pixel_height %ld num_of_chars %ld\n",
        fontstruct.signature, fontstruct.size, fontstruct.format, fontstruct.flags,
        fontstruct.layout_width, fontstruct.layout_height, fontstruct.pixel_width, fontstruct.pixel_height,
        fontstruct.number_of_characters);

    Display_StartColor(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 });
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 20, 30, OPT_CENTER,
        "SetFont - extended format 1");
    EVE_CoCmd_setFont(s_pHalContext, 6, RAM_G, 32);
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 80, 6, OPT_CENTER,
        "The quick brown fox jumps");
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 120, 6, OPT_CENTER,
        "over the lazy dog.");
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 160, 6, OPT_CENTER,
        "1234567890");
    EVE_CoCmd_resetFonts(s_pHalContext);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief Load xfont to RAM_G
*
* @param isReindex
*/
void helperLoadXfont(uint8_t isReindex)
{
    uint8_t *str_noindex = u8"面朝大海，春暖花开";
    uint8_t *str_reindex = u8"\x49c3\x197e\xb88\x1fd8\x528c\x1886\x18f7\x3512\x1161";
    uint8_t *str = str_noindex;
    uint32_t addr;

    if (isReindex)
    {
        str = str_reindex;
        addr = 3104064;
    }
    else
    {
        str = str_noindex;
        addr = 9984;
    }

    //Load xfont file into graphics RAM
    if (!FlashHelper_SwitchFullMode(s_pHalContext))
    {
        eve_printf("Cannot switch flash full mode");
        return;
    }
    EVE_CoCmd_flashSource(s_pHalContext, addr);
    EVE_CoCmd_loadAsset(s_pHalContext, RAM_G, OPT_FLASH);
    EVE_CoCmd_setFont(s_pHalContext, 6, RAM_G, 0);
    if (isReindex)
    {
        EVE_CoCmd_text(s_pHalContext, 10, 10, 30, 0,
            "Display unicode string use re-index font data");
    }
    else
    {
        EVE_CoCmd_text(s_pHalContext, 10, 10, 30, 0,
            "Display unicode string use font's index");
    }
    EVE_CoCmd_text(s_pHalContext, 10, 100, 6, 0, str);
    EVE_CoCmd_resetFonts(s_pHalContext);
}

/**
* @brief An example for code point ordinal/UTF-8
*/
void SAMAPP_Font_indexer()
{
    Draw_Text(s_pHalContext, "Example for: Code point ordinal/UTF-8");

    Display_StartColor(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 });
    helperLoadXfont(0);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;

    // clear screen
    EVE_Util_clearScreen(s_pHalContext);

    Display_StartColor(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 });
    helperLoadXfont(1);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

EVE_FontsExt2_chblk *get_FontsExt2_glyph_addr(uint32_t xf, uint32_t cp)
{
    uint32_t *page = &((uint32_t *)(XF2_OPTR(xf)))[cp / 128];
    uint32_t *cd = (uint32_t *)(EVE_Hal_rd32(s_pHalContext, (uint32_t)page) + 4 * (cp % 128));
    return((EVE_FontsExt2_chblk *)EVE_Hal_rd32(s_pHalContext, (uint32_t)cd));
}

/**
* @brief Inbuilt font example for proportional text (continue)
* font handle 26 - 34 can be printed by bitmap
* This function is solely for displaying fonts in a extended format 2 and is not intended for regular use
*/
void SAMAPP_Font_romFontsExt2()
{
    int32_t j;
    int32_t hoffset;
    int32_t voffset;
    int32_t stringlen1;
    int32_t stringlen2;
    int32_t stringlen3;
    uint32_t FontTableAddressBase = s_pHalContext->DDR_RamSize - 256; // Pseudo-ROM font table
    uint32_t FontTableAddress;
    const uint8_t Display_string1[] = "!\"#$%&'()*+,-./0123456789:;<=>?";
    const uint8_t Display_string2[] = "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_";
    const uint8_t Display_string3[] = "`abcdefghijklmnopqrstuvwxyz{|}  ";
    EVE_Gpu_FontsExt2 fontstruct;

    Draw_Text(s_pHalContext, "Example for: ROM fonts 26 to 34");
    /* only font handle 16 - 26 can be printed by cell */

    stringlen1 = sizeof(Display_string1) - 1;
    stringlen2 = sizeof(Display_string2) - 1;
    stringlen3 = sizeof(Display_string3) - 1;

    for (int romFont = 26; romFont <= 34; romFont++)
    {
        FontTableAddress = EVE_Hal_rd32(s_pHalContext, FontTableAddressBase + 4 * romFont);
        eve_printf_debug("ROM Font address read from 0x%lx is %x \n", FontTableAddressBase + 4 * romFont, FontTableAddress);
        /* Read the font table from hardware */
        EVE_Hal_rdMem(s_pHalContext, (uint8_t *)&fontstruct, FontTableAddress, sizeof(EVE_Gpu_FontsExt2));
        eve_printf_debug("Inbuilt font %d signature 0x%lx size %ld fmt %d flags 0x%x layout_width %ld layout_height %ld pixel_width %ld pixel_height %ld num_of_chars %ld\n", romFont,
            fontstruct.signature, fontstruct.size, fontstruct.format, fontstruct.flags,
            fontstruct.layout_width, fontstruct.layout_height, fontstruct.pixel_width, fontstruct.pixel_height,
            fontstruct.number_of_characters);

        Display_StartColor(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 });
        hoffset = 50;
        voffset = 100;
        char str[12];
        snprintf(str, 12, "ROM FONT %d", romFont);
        EVE_CoCmd_text(s_pHalContext, hoffset, voffset, romFont, 0, str);
        /* Display string */
        EVE_CoDl_begin(s_pHalContext, BITMAPS);
        voffset += 100;

        //EVE_CoDl_bitmapHandle(s_pHalContext, romFont);
        for (j = 0; j < stringlen1; j++)
        {
            EVE_FontsExt2_chblk *pChblk = get_FontsExt2_glyph_addr(FontTableAddress, Display_string1[j]);
            EVE_FontsExt2_chblk chblk;
            EVE_Hal_rdMem(s_pHalContext, (uint8_t *)&chblk, (uint32_t)pChblk, sizeof(EVE_FontsExt2_chblk));
            EVE_CoCmd_setBitmap(s_pHalContext, chblk.glyph, fontstruct.format, fontstruct.pixel_width, fontstruct.pixel_height);
            EVE_CoDl_vertex2f_4(s_pHalContext, hoffset * 16, voffset * 16);
            hoffset += chblk.width;
        }
        hoffset = 50;
        voffset += fontstruct.pixel_height + 20;
        for (j = 0; j < stringlen2; j++)
        {
            EVE_FontsExt2_chblk *pChblk = get_FontsExt2_glyph_addr(FontTableAddress, Display_string2[j]); 
            EVE_FontsExt2_chblk chblk;
            EVE_Hal_rdMem(s_pHalContext, (uint8_t *)&chblk, (uint32_t)pChblk, sizeof(EVE_FontsExt2_chblk));
            EVE_CoCmd_setBitmap(s_pHalContext, chblk.glyph, fontstruct.format, fontstruct.pixel_width, fontstruct.pixel_height);
            EVE_CoDl_vertex2f_4(s_pHalContext, hoffset * 16, voffset * 16);
            hoffset += chblk.width;
        }
        hoffset = 50;
        voffset += fontstruct.pixel_height + 20;
        for (j = 0; j < stringlen3; j++)
        {
            EVE_FontsExt2_chblk *pChblk = get_FontsExt2_glyph_addr(FontTableAddress, Display_string3[j]);
            EVE_FontsExt2_chblk chblk;
            EVE_Hal_rdMem(s_pHalContext, (uint8_t *)&chblk, (uint32_t)pChblk, sizeof(EVE_FontsExt2_chblk));
            EVE_CoCmd_setBitmap(s_pHalContext, chblk.glyph, fontstruct.format, fontstruct.pixel_width, fontstruct.pixel_height);
            EVE_CoDl_vertex2f_4(s_pHalContext, hoffset * 16, voffset * 16);
            hoffset += chblk.width;
        }

        EVE_CoDl_end(s_pHalContext);

        Display_End(s_pHalContext);
        SAMAPP_DELAY;
    }
}

/**
* @brief API to demonstrate custom extened font display from a converted font file
*/
void SAMAPP_Font_extendedFormat2()
{
    EVE_Gpu_FontsExt2 fontstruct;

    Draw_Text(s_pHalContext, "Example for: extened font (extended format 2)");
    EVE_Util_loadRawFile(s_pHalContext, RAM_G, TEST_DIR "\\Roboto-BoldCondensed_30_L2.raw");

    EVE_Hal_rdMem(s_pHalContext, (uint8_t *)&fontstruct, RAM_G, sizeof(EVE_Gpu_FontsExt2));
    eve_printf_debug("signature 0x%lx size %ld fmt %d flags 0x%x layout_width %ld layout_height %ld pixel_width %ld pixel_height %ld num_of_chars %ld\n",
        fontstruct.signature, fontstruct.size, fontstruct.format, fontstruct.flags,
        fontstruct.layout_width, fontstruct.layout_height, fontstruct.pixel_width, fontstruct.pixel_height,
        fontstruct.number_of_characters);

    Display_StartColor(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 });
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 20, 30, OPT_CENTER,
        "SetFont - extended format 2");
    EVE_CoCmd_setFont(s_pHalContext, 6, RAM_G, 32);
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 80, 6, OPT_CENTER,
        "The quick brown fox jumps");
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 120, 6, OPT_CENTER,
        "over the lazy dog.");
    EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 160, 6, OPT_CENTER,
        "1234567890");
    EVE_CoCmd_resetFonts(s_pHalContext);
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief API to demonstrate unicode font from flash
*/
void SAMAPP_Font_extendedFormat2_flash()
{
    if (!FlashHelper_SwitchFullMode(s_pHalContext))
    {
        eve_printf("Cannot switch flash full mode");
        return;
    }
    Draw_Text(s_pHalContext, "Example for: extened font from flash");

    // Copy font to ramg
    EVE_CoCmd_flashSource(s_pHalContext, 9984);
    EVE_CoCmd_loadAsset(s_pHalContext, RAM_G, OPT_FLASH);

    Display_StartColor(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 });
    EVE_CoCmd_setFont(s_pHalContext, 30, RAM_G, 0);
    EVE_CoCmd_text(s_pHalContext, 0, 0, 30, 0, u8"碧洁特的全球公司总部位于亚太地区的中心新加坡。");
    EVE_CoCmd_text(s_pHalContext, 0, 50, 30, 0, u8"公司在格拉斯哥、新加坡、台北（台湾）和胡志明（越南）设有研发机构，");
    EVE_CoCmd_text(s_pHalContext, 0, 100, 30, 0, u8"并在格拉斯哥、台北、蒂加德（美国俄勒冈州）和上海（中国）设有区域销售和技术支持机构。");
    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief API to demonstrate CMD_RESETFONTS
*/
void SAMAPP_Font_resetFont()
{
    if (!FlashHelper_SwitchFullMode(s_pHalContext))
    {
        eve_printf("Cannot switch flash full mode");
        return;
    }
    Draw_Text(s_pHalContext, "Example for: Font reset");
    EVE_CoCmd_resetFonts(s_pHalContext);

    EVE_CoCmd_flashSource(s_pHalContext, 4096);
    EVE_CoCmd_loadAsset(s_pHalContext, RAM_G, OPT_FLASH);

    Display_StartColor(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 });
    uint32_t x = s_pHalContext->Width / 2;
    uint32_t y = s_pHalContext->Height / 2;
    EVE_CoCmd_setFont(s_pHalContext, 30, RAM_G, 32);
    EVE_CoCmd_text(s_pHalContext, x, y, 30, OPT_CENTER,
        "Use custom font with bitmap handle 30 from SAMAPP_Font_fromConvertedTTF_flash():");
    EVE_CoCmd_text(s_pHalContext, x, y + 50, 30, OPT_CENTER,
        "The quick brown fox jumps over the lazy dog");
    Display_End(s_pHalContext);
    SAMAPP_DELAY;

    Display_StartColor(s_pHalContext, (uint8_t[]) { 0, 0, 0 }, (uint8_t[]) { 255, 255, 255 });
    EVE_CoCmd_resetFonts(s_pHalContext);
    EVE_CoCmd_text(s_pHalContext, x, y, 30, OPT_CENTER, "Call CMD_RESETFONTS and use font 30:");
    EVE_CoCmd_text(s_pHalContext, x, y + 50, 30, OPT_CENTER,
        "The quick brown fox jumps over the lazy dog");
    Display_End(s_pHalContext);

    SAMAPP_DELAY;
}

void SAMAPP_Font() {
    SAMAPP_Font_romFonts();
    SAMAPP_Font_fromJPEG();
    SAMAPP_Font_fromConvertedTTF();
    SAMAPP_Font_fromConvertedTTF_flash();
    SAMAPP_Font_fromConvertedTTF_SD();
    SAMAPP_Font_fromConvertedTTF_CMDB();
    SAMAPP_Font_extendedFormat();
    SAMAPP_Font_indexer();
    SAMAPP_Font_romFontsExt2();
    SAMAPP_Font_extendedFormat2();
    SAMAPP_Font_extendedFormat2_flash();
    SAMAPP_Font_resetFont();
}


