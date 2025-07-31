/**
 * @file EVE_CoDl.h
 * @brief EVE's co-processor command
 * 
 * The purpose of this header is to provide a simplified interface to display list instructions.
 * The functions do not match 1:1 with the display list instructions. Some instructions are combined 
 * to simplify compatibility between platforms. (For example, BITMAP_SIZE and BITMAP_SIZE_H.)
 * All functions write to the display list through EVE_CoCmd_dl.
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

#ifndef EVE_CO_DL__H
#define EVE_CO_DL__H

/*********************
 *      INCLUDES
 *********************/
#include "EVE_CoCmd.h"

/**********************
 *      MACROS
 **********************/
#define EVE_VERTEX2F_MIN -16384L
#define EVE_VERTEX2F_MAX 16383L
#define EVE_VERTEX2II_MIN 0UL
#define EVE_VERTEX2II_MAX 511UL

/**********************
 *   INLINE FUNCTIONS
 **********************/
/**
 * @brief End the display list. All the commands following this command will be ignored.
 * 
 * @param phost Pointer to Hal context
 */
static inline void EVE_CoDl_display(EVE_HalContext *phost)
{
	EVE_CoCmd_dl(phost, DISPLAY());
}

/** 
 * @brief Start the operation of graphics primitives at the specified screen coordinate, in the pixel precision defined by VERTEX_FORMAT
 * 
 * @param phost Pointer to Hal context
 * @param x Signed x-coordinate in units of pixel precision defined in command VERTEX_FORMAT, which by default is 1/16 pixel precision
 * @param y Signed y-coordinate in units of pixel precision defined in command VERTEX_FORMAT, which by default is 1/16 pixel precision
 */
static inline void EVE_CoDl_vertex2f(EVE_HalContext *phost, int16_t x, int16_t y)
{
	EVE_CoCmd_dl(phost, VERTEX2F(x, y));
}

/**
 * @brief Start the operation of graphics primitive at the specified coordinates in pixel precision
 * 
 * @param phost Pointer to Hal context
 * @param x X-coordinate in pixels
 * @param y Y-coordinate in pixels
 * @param handle Bitmap handle
 * @param cell Cell number
 */
inline static void EVE_CoDl_vertex2ii(EVE_HalContext *phost, uint16_t x, uint16_t y, uint8_t handle, uint8_t cell)
{
	EVE_CoCmd_dl(phost, VERTEX2II(x, y, handle, cell));
}

/**
 * @brief Specify the source address of bitmap data in RAM_G
 * 
 * @param phost Pointer to Hal context
 * @param addr Bitmap address in RAM_G
 */
inline static void EVE_CoDl_bitmapSource(EVE_HalContext *phost, uint32_t addr)
{
	EVE_CoCmd_dl(phost, BITMAP_SOURCEH(addr >> 24));
	EVE_CoCmd_dl(phost, BITMAP_SOURCE(addr));
}

/**
 * @brief Specify clear color RGB
 * 
 * @param phost Pointer to Hal context
 * @param c clear color
 */
inline static void EVE_CoDl_clearColorRgb_ex(EVE_HalContext *phost, uint32_t c)
{
	EVE_CoCmd_dl(phost, CLEAR_COLOR_RGB(0, 0, 0) | (c & 0xFFFFFF));
}

/** 
 * @brief Specify clear values for red, green and blue channels
 * 
 * @param phost Pointer to Hal context
 * @param r Red value used when the color buffer is cleared
 * @param g Green value used when the color buffer is cleared
 * @param b Blue value used when the color buffer is cleared
 */
inline static void EVE_CoDl_clearColorRgb(EVE_HalContext *phost, uint8_t r, uint8_t g, uint8_t b)
{
	EVE_CoCmd_dl(phost, CLEAR_COLOR_RGB(r, g, b));
}

/** 
 * @brief Specify clear value for the alpha channel
 * 
 * @param phost Pointer to Hal context
 * @param alpha Alpha value used when the color buffer is cleared
 */
inline static void EVE_CoDl_clearColorA(EVE_HalContext *phost, uint8_t alpha)
{
	EVE_CoCmd_dl(phost, CLEAR_COLOR_A(alpha));
}

/** 
 * @brief Specify clear color: Alpha (bits 31:24) + RGB (bits 23:0)
 * 
 * @param phost Pointer to Hal context
 * @param c clear color
 */
inline static void EVE_CoDl_clearColorArgb_ex(EVE_HalContext *phost, uint32_t c)
{
	EVE_CoDl_clearColorRgb_ex(phost, c);
	EVE_CoDl_clearColorA(phost, c >> 24);
}

/** 
 * @brief Attach the tag value for the following graphics objects drawn on the screen
 * 
 * @warning: Must be returned to 255 after usage, to ensure next widgets don't draw with invalid tag
 * @param phost Pointer to Hal context
 * @param s Tag value
 */
inline static void EVE_CoDl_tag(EVE_HalContext *phost, uint32_t s)
{
	EVE_CoCmd_dl(phost, TAG(s));
}

/**
 * @brief Specify color RGB 
 * 
 * @param phost Pointer to Hal context
 * @param c RGB value for the current color
 */
inline static void EVE_CoDl_colorRgb_ex(EVE_HalContext *phost, uint32_t c)
{
	EVE_CoCmd_dl(phost, COLOR_RGB(0, 0, 0) | (c & 0xFFFFFF));
}

/** 
 * @brief Set the current color red, green and blue
 * 
 * @param phost Pointer to Hal context
 * @param r Red value for the current color
 * @param g Green value for the current color
 * @param b Blue value for the current color
 */
inline static void EVE_CoDl_colorRgb(EVE_HalContext *phost, uint8_t r, uint8_t g, uint8_t b)
{
	uint32_t rgb = ((uint32_t)r << 16) | ((uint32_t)g << 8) | ((uint32_t)b);
	EVE_CoDl_colorRgb_ex(phost, rgb);
}

/**
 * @brief Set the current color alpha
 * 
 * @param phost Pointer to Hal context
 * @param alpha Alpha for the current color
 */
inline static void EVE_CoDl_colorA(EVE_HalContext *phost, uint8_t alpha)
{
	EVE_CoCmd_dl(phost, COLOR_A(alpha));
}

/** 
 * @brief Specify color: Alpha (bits 31:24) + RGB (bits 23:0)
 * 
 * @param phost Pointer to Hal context
 * @param c
 */
inline static void EVE_CoDl_colorArgb_ex(EVE_HalContext *phost, uint32_t c)
{
	EVE_CoDl_colorRgb_ex(phost, c);
	EVE_CoDl_colorA(phost, c >> 24);
}

/**
 * @brief Specify bitmap handle
 * 
 * @param phost Pointer to Hal context
 * @param handle Bitmap handle
 */
inline static void EVE_CoDl_bitmapHandle(EVE_HalContext *phost, uint8_t handle)
{
	EVE_CoCmd_dl(phost, BITMAP_HANDLE(handle));
}

/**
 * @brief Specify the bitmap cell number for the VERTEX2F command
 * 
 * @param phost Pointer to Hal context
 * @param cell Bitmap cell number
 */
inline static void EVE_CoDl_cell(EVE_HalContext *phost, uint8_t cell)
{
	EVE_CoCmd_dl(phost, CELL(cell));
}

/**
 * @brief Specify the source bitmap memory format and layout for the current handle
 * 
 * @param phost Pointer to Hal context
 * @param format Bitmap pixel format
 * @param linestride Bitmap line strides, in bytes
 * @param height Bitmap height, in lines
 */
static inline void EVE_CoDl_bitmapLayout(EVE_HalContext *phost, uint8_t format, uint16_t linestride, uint16_t height)
{
	EVE_CoCmd_dl(phost, BITMAP_LAYOUT_H(linestride >> 10, height >> 9));
	EVE_CoCmd_dl(phost, BITMAP_LAYOUT(format, linestride, height));
}

/**
 * @brief Specify the screen drawing of bitmaps for the current handle
 * 
 * @param phost Pointer to Hal context
 * @param filter Bitmap filtering mode
 * @param wrapx Bitmap x wrap mode
 * @param wrapy Bitmap y wrap mode
 * @param width Draw bitmap width
 * @param height Draw bitmap height
 */
static inline void EVE_CoDl_bitmapSize(EVE_HalContext *phost, uint8_t filter, uint8_t wrapx, uint8_t wrapy, uint16_t width, uint16_t height)
{
	EVE_CoCmd_dl(phost, BITMAP_SIZE_H(width >> 9, height >> 9));
	EVE_CoCmd_dl(phost, BITMAP_SIZE(filter, wrapx, wrapy, width, height));
}

/**
 * @brief Specify the alpha test function
 *
 * @param phost Pointer to Hal context
 * @param func The test function
 * @param ref The reference value for the alpha test
 */
static inline void EVE_CoDl_alphaFunc(EVE_HalContext *phost, uint8_t func, uint8_t ref)
{
	EVE_CoCmd_dl(phost, ALPHA_FUNC(func, ref));
}

/**
 * @brief Set function and reference value for stencil testing
 * 
 * @param phost Pointer to Hal context
 * @param func Specifies the test function
 * @param ref Specifies the reference value for the stencil test
 * @param mask Specifies a mask that is ANDed with the reference value and the stored stencil value
 */
static inline void EVE_CoDl_stencilFunc(EVE_HalContext *phost, uint8_t func, uint8_t ref, uint8_t mask)
{
	EVE_CoCmd_dl(phost, STENCIL_FUNC(func, ref, mask));
}

/**
 * @brief Specify pixel arithmetic
 * 
 * @param phost Pointer to Hal context
 * @param src Specifies how the source blending factor is computed
 * @param dst Specifies how the destination blending factor is computed
 */
static inline void EVE_CoDl_blendFunc(EVE_HalContext *phost, uint8_t src, uint8_t dst)
{
	EVE_CoCmd_dl(phost, BLEND_FUNC(src, dst));
}

/**
 * @brief Specify pixel arithmetic back to default value
 * 
 * @param phost Pointer to Hal context
 */
static inline void EVE_CoDl_blendFunc_default(EVE_HalContext *phost)
{
	EVE_CoDl_blendFunc(phost, SRC_ALPHA, ONE_MINUS_SRC_ALPHA); // TODO: Add default calls for all display list state instructions
}

/**
 * @brief Set stencil test actions
 * 
 * @param phost Pointer to Hal context
 * @param sfail Specifies the action to take when the stencil test fails
 * @param spass Specifies the action to take when the stencil test passes
 */
static inline void EVE_CoDl_stencilOp(EVE_HalContext *phost, uint8_t sfail, uint8_t spass)
{
	EVE_CoCmd_dl(phost, STENCIL_OP(sfail, spass));
}

/**
 * @brief Specify the radius of points
 * 
 * @param phost Pointer to Hal context
 * @param size Point radius in 1/16 pixel precision
 */
inline static void EVE_CoDl_pointSize(EVE_HalContext *phost, int16_t size)
{
	EVE_CoCmd_dl(phost, POINT_SIZE(size));
}

/**
 * @brief Specify the width of lines to be drawn with primitive LINES in 1/16 pixel precision
 * 
 * @param phost Pointer to Hal context
 * @param width Line width is 1/16 pixel precision
 */
inline static void EVE_CoDl_lineWidth(EVE_HalContext *phost, int16_t width)
{
	EVE_CoCmd_dl(phost, LINE_WIDTH(width));
}

/**
 * @brief Specify clear value for the stencil buffer
 * 
 * @param phost Pointer to Hal context
 * @param s Value used when the stencil buffer is cleared
 */
static inline void EVE_CoDl_clearStencil(EVE_HalContext *phost, uint8_t s)
{
	EVE_CoCmd_dl(phost, CLEAR_STENCIL(s));
}

/**
 * @brief Specify clear value for the tag buffer
 * 
 * @param phost Pointer to Hal context
 * @param s Value used when the tag buffer is cleared
 */
static inline void EVE_CoDl_clearTag(EVE_HalContext *phost, uint32_t s)
{
	EVE_CoCmd_dl(phost, CLEAR_TAG(s));
}

/**
 * @brief Control the writing of individual bits in the stencil planes
 * 
 * @param phost Pointer to Hal context
 * @param mask The mask used to enable writing stencil bits
 */
static inline void EVE_CoDl_stencilMask(EVE_HalContext *phost, uint8_t mask)
{
	EVE_CoCmd_dl(phost, STENCIL_MASK(mask));
}

/**
 * @brief Control the writing of the tag buffer
 * 
 * @param phost Pointer to Hal context
 * @param mask Allow updates to the tag buffer
 */
static inline void EVE_CoDl_tagMask(EVE_HalContext *phost, bool mask)
{
	EVE_CoCmd_dl(phost, TAG_MASK(mask));
}

/**
 * @brief Specify the A coefficient of the bitmap transform matrix
 * 
 * @param phost Pointer to Hal context
 * @param p Precision control
 * @param v Component of the bitmap transform matrix
 */
static inline void EVE_CoDl_bitmapTransformA(EVE_HalContext *phost, bool p, uint16_t v)
{
	EVE_CoCmd_dl(phost, BITMAP_TRANSFORM_A(p, v));
}

/**
 * @brief Specify the B coefficient of the bitmap transform matrix
 * 
 * @param phost Pointer to Hal context
 * @param p Precision control
 * @param v Component of the bitmap transform matrix
 */
static inline void EVE_CoDl_bitmapTransformB(EVE_HalContext *phost, bool p, uint16_t v)
{
	EVE_CoCmd_dl(phost, BITMAP_TRANSFORM_B(p, v));
}

/**
 * @brief Specify the C coefficient of the bitmap transform matrix
 * 
 * @param phost Pointer to Hal context
 * @param c Component of the bitmap transform matrix
 */
static inline void EVE_CoDl_bitmapTransformC(EVE_HalContext *phost, uint32_t c)
{
	EVE_CoCmd_dl(phost, BITMAP_TRANSFORM_C(c));
}

/**
 * @brief Specify the D coefficient of the bitmap transform matrix
 * 
 * @param phost Pointer to Hal context
 * @param p Precision control
 * @param v Component of the bitmap transform matrix
 */
static inline void EVE_CoDl_bitmapTransformD(EVE_HalContext *phost, bool p, uint16_t v)
{
	EVE_CoCmd_dl(phost, BITMAP_TRANSFORM_D(p, v));
}

/**
 * @brief Specify the E coefficient of the bitmap transform matrix
 * 
 * @param phost Pointer to Hal context
 * @param p Precision control
 * @param v Component of the bitmap transform matrix
 */
static inline void EVE_CoDl_bitmapTransformE(EVE_HalContext *phost, bool p, uint16_t v)
{
	EVE_CoCmd_dl(phost, BITMAP_TRANSFORM_E(p, v));
}

/**
 * @brief Specify the F coefficient of the bitmap transform matrix
 * 
 * @param phost Pointer to Hal context
 * @param f Component of the bitmap transform matrix
 */
static inline void EVE_CoDl_bitmapTransformF(EVE_HalContext *phost, uint32_t f)
{
	EVE_CoCmd_dl(phost, BITMAP_TRANSFORM_F(f));
}

/**
 * @brief Specify the coefficient of the bitmap transform matrix
 * 
 * @param phost Pointer to Hal context
 * @param p Precision control
 * @param a Component of the bitmap transform matrix
 * @param b Component of the bitmap transform matrix
 * @param c Component of the bitmap transform matrix
 * @param d Component of the bitmap transform matrix
 * @param e Component of the bitmap transform matrix
 * @param f Component of the bitmap transform matrix
 */
static inline void EVE_CoDl_bitmapTransform(EVE_HalContext *phost, bool p, uint16_t a, uint16_t b, uint32_t c, uint16_t d, uint16_t e, uint32_t f)
{
	EVE_CoDl_bitmapTransformA(phost, p, a);
	EVE_CoDl_bitmapTransformB(phost, p, b);
	EVE_CoDl_bitmapTransformC(phost, c);
	EVE_CoDl_bitmapTransformD(phost, p, d);
	EVE_CoDl_bitmapTransformE(phost, p, e);
	EVE_CoDl_bitmapTransformF(phost, f);
}

/**
 * @brief Specify the coefficient of the bitmap transform matrix to a identify value
 * 
 * @param phost Pointer to Hal context
 */
static inline void EVE_CoDl_bitmapTransform_identity(EVE_HalContext *phost)
{
	EVE_CoDl_bitmapTransformA(phost, 0, 256);
	EVE_CoDl_bitmapTransformB(phost, 0, 0);
	EVE_CoDl_bitmapTransformC(phost, 0);
	EVE_CoDl_bitmapTransformD(phost, 0, 0);
	EVE_CoDl_bitmapTransformE(phost, 0, 256);
	EVE_CoDl_bitmapTransformF(phost, 0);
}

/**
 * @brief Specify the top left corner of the scissor clip rectangle
 * 
 * @param phost Pointer to Hal context
 * @param x The unsigned x coordinate of the scissor clip rectangle, in pixels
 * @param y The unsigned y coordinates of the scissor clip rectangle, in pixels
 */
static inline void EVE_CoDl_scissorXY(EVE_HalContext *phost, uint16_t x, uint16_t y)
{
	EVE_CoCmd_dl(phost, SCISSOR_XY(x, y));
}

/**
 * @brief Specify the size of the scissor clip rectangle
 * 
 * @param phost Pointer to Hal context
 * @param width The width of the scissor clip rectangle, in pixels
 * @param height The height of the scissor clip rectangle, in pixels
 */
static inline void EVE_CoDl_scissorSize(EVE_HalContext *phost, uint16_t width, uint16_t height)
{
	EVE_CoCmd_dl(phost, SCISSOR_SIZE(width, height));
}

/**
 * @brief Execute a sequence of commands at another location in the display list
 * 
 * @param phost Pointer to Hal context
 * @param dest The offset of the destination address from RAM_DL which the display command is to be switched
 */
static inline void EVE_CoDl_call(EVE_HalContext *phost, uint16_t dest)
{
	EVE_CoCmd_dl(phost, CALL(dest));
}

/**
 * @brief Execute commands at another location in the display list
 * 
 * @param phost Pointer to Hal context
 * @param dest Display list address (offset from RAM_DL) to be jumped
 */
static inline void EVE_CoDl_jump(EVE_HalContext *phost, uint16_t dest)
{
	EVE_CoCmd_dl(phost, JUMP(dest));
}

/**
 * @brief Begin drawing a graphics primitive.
 * 
 * @param phost Pointer to Hal context
 * @param prim The graphics primitive to be executed
 */
static inline void EVE_CoDl_begin(EVE_HalContext *phost, uint8_t prim)
{
	EVE_CoCmd_dl(phost, BEGIN(prim));
}

/**
 * @brief Enable or disable writing of color components
 * 
 * @param phost Pointer to Hal context
 * @param r Enable or disable the red channel update of the color buffer
 * @param g Enable or disable the green channel update of the color buffer
 * @param b Enable or disable the blue channel update of the color buffer
 * @param a Enable or disable the alpha channel update of the color buffer
 */
static inline void EVE_CoDl_colorMask(EVE_HalContext *phost, bool r, bool g, bool b, bool a)
{
	EVE_CoCmd_dl(phost, COLOR_MASK(r, g, b, a));
}

/**
 * @brief End drawing a graphics primitive
 * 
 * @param phost Pointer to Hal context
 */
inline static void EVE_CoDl_end(EVE_HalContext *phost)
{
	EVE_CoCmd_dl(phost, END());
}

/** 
 * @brief Push the current graphics context on the context stack
 * 
 * @param phost Pointer to Hal context
 */
inline static void EVE_CoDl_saveContext(EVE_HalContext *phost)
{
	EVE_CoCmd_dl(phost, SAVE_CONTEXT());
}

/**
 * @brief Restore the current graphics context from the context stack
 * 
 * @param phost Pointer to Hal context
 */
inline static void EVE_CoDl_restoreContext(EVE_HalContext *phost)
{
	EVE_CoCmd_dl(phost, RESTORE_CONTEXT());
}

/**
 * @brief Return from a previous CALL command
 * 
 * @param phost Pointer to Hal context
 */
static inline void EVE_CoDl_return(EVE_HalContext *phost)
{
	EVE_CoCmd_dl(phost, RETURN());
}

/**
 * @brief Execute a single command from a macro register
 * 
 * @param phost Pointer to Hal context
 * @param m Macro registers to read
 */
static inline void EVE_CoDl_macro(EVE_HalContext *phost, uint16_t m)
{
	EVE_CoCmd_dl(phost, MACRO(m));
}

/**
 * @brief Clear buffers to preset values
 * 
 * @param phost Pointer to Hal context
 * @param c clear color buffer
 * @param s clear stencil buffer
 * @param t clear tag buffer
 */
static inline void EVE_CoDl_clear(EVE_HalContext *phost, bool c, bool s, bool t)
{
	EVE_CoCmd_dl(phost, CLEAR(c, s, t));
}

/**
 * @brief Set the precision of VERTEX2F coordinates
 * 
 * @param phost Pointer to Hal context
 * @param frac Number of fractional bits in X, Y coordinates
 */
static inline void EVE_CoDl_vertexFormat(EVE_HalContext *phost, uint8_t frac)
{
	EVE_CoCmd_dl(phost, VERTEX_FORMAT(frac));
}

/**
 * @brief Specify the base address of the palette
 * 
 * @param phost Pointer to Hal context
 * @param addr Address of palette in RAM_G, 2-byte alignment is required
 */
inline static void EVE_CoDl_paletteSource(EVE_HalContext *phost, uint32_t addr)
{
	EVE_CoCmd_dl(phost, PALETTE_SOURCE(addr));
	EVE_CoCmd_dl(phost, PALETTE_SOURCEH(addr >> 24));
}

/**
 * @brief Specify the vertex transformation's X translation component
 * 
 * @param phost Pointer to Hal context
 * @param x Signed x-coordinate in 1/16 pixel
 */
static inline void EVE_CoDl_vertexTranslateX(EVE_HalContext *phost, int16_t x)
{
	EVE_CoCmd_dl(phost, VERTEX_TRANSLATE_X(x));
}

/**
 * @brief Specify the vertex transformation's Y translation component
 * 
 * @param phost Pointer to Hal context
 * @param y Signed y-coordinate in 1/16 pixel
 */
static inline void EVE_CoDl_vertexTranslateY(EVE_HalContext *phost, int16_t y)
{
	EVE_CoCmd_dl(phost, VERTEX_TRANSLATE_Y(y));
}

/**
 * @brief No operation
 * 
 * @param phost Pointer to Hal context
 */
static inline void EVE_CoDl_nop(EVE_HalContext *phost)
{
	EVE_CoCmd_dl(phost, NOP());
}

/**
 * @brief Specify the extended format of the bitmap
 * 
 * @param phost Pointer to Hal context
 * @param format Bitmap pixel format
 */
static inline void EVE_CoDl_bitmapExtFormat(EVE_HalContext *phost, uint16_t format)
{
	EVE_CoCmd_dl(phost, BITMAP_EXT_FORMAT(format));
}

/**
 * @brief Set the source for the red, green, blue and alpha channels of a bitmap
 * 
 * @param phost Pointer to Hal context
 * @param r Red component source channel
 * @param g Green component source channel
 * @param b Blue component source channel
 * @param a Alpha component source channel
 */
static inline void EVE_CoDl_bitmapSwizzle(EVE_HalContext *phost, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	EVE_CoCmd_dl(phost, BITMAP_SWIZZLE(r, g, b, a));
}

/**
 * @brief Fixed point vertex using 4 bits subprecision
 * 
 * @param phost Pointer to Hal context
 * @param x
 * @param y
 */
inline static void EVE_CoDl_vertex2f_4(EVE_HalContext *phost, int16_t x, int16_t y)
{
	if (x > EVE_VERTEX2F_MAX || y > EVE_VERTEX2F_MAX) /* Support display up to 2048 px */
	{
		EVE_CoDl_vertexFormat(phost, 3);
		EVE_CoDl_vertex2f(phost, x >> 1, y >> 1);
	}
	else
	{
		EVE_CoDl_vertexFormat(phost, 4);
		EVE_CoDl_vertex2f(phost, x, y);
	}
}

/**
 * @brief Fixed point vertex using 2 bits subprecision
 * 
 * @param phost Pointer to Hal context
 * @param x
 * @param y
 */
inline static void EVE_CoDl_vertex2f_2(EVE_HalContext *phost, int16_t x, int16_t y)
{
	EVE_CoDl_vertexFormat(phost, 2);
	EVE_CoDl_vertex2f(phost, x, y);
}

/**
 * @brief Fixed point vertex using 0 bits subprecision, or integer point vertex
 * 
 * @param phost Pointer to Hal context
 * @param x
 * @param y
 */
inline static void EVE_CoDl_vertex2f_0(EVE_HalContext *phost, int16_t x, int16_t y)
{
	EVE_CoDl_vertexFormat(phost, 0);
	EVE_CoDl_vertex2f(phost, x, y);
}

/**
 * @brief Branch if Y is outside region
 * 
 * @param phost Pointer to Hal context
 * @param y Region Y coordinate start
 * @param h Region height in Region Height Unit
 * @param dest The index of the instruction in the display list that execution control will jump to when the current drawing line is beyond the defined region
 */
inline static void EVE_CoDl_region(EVE_HalContext *phost, uint16_t y, uint16_t h, uint16_t dest)
{
	EVE_CoCmd_dl(phost, REGION(y, h, dest));
}

/**
 * @brief Set the z-order address pattern for a bitmap
 * 
 * @param phost Pointer to Hal context
 * @param o z-order code
 */
inline static void EVE_CoDl_zorder(EVE_HalContext *phost, uint8_t o)
{
	EVE_CoCmd_dl(phost, BITMAP_ZORDER(o));
}
#endif /* EVE_CO_DL__H */

/* end of file */
