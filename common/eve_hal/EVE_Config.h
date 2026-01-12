/**
 * @file EVE_Config.h
 * @brief This file processes the input definitions
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

#ifndef EVE_CONFIG__H
#define EVE_CONFIG__H

#include "EVE_IntTypes.h"

/** @name Chip IDs with EVE generation */
///@{
#define EVE_BT820 0x50820
///@}

/** @name EVE generations */
///@{
#define EVE5 5 /**< BT820 */
///@}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
/** @attention

The following values are set based on the input definitions.
Do not set these values manually; instead, use the specific user definitions.

Graphics target:
- EVE_BT820

Platform target:
- FT4222_PLATFORM (set EVE_PLATFORM_FT4222)
- MPSSE_PLATFORM (set EVE_PLATFORM_MPSSE)
- RP2040_PLATFORM (set EVE_PLATFORM_RP2040)

Display resolution:
- DISPLAY_RESOLUTION_WUXGA
- DISPLAY_RESOLUTION_FHD
- DISPLAY_RESOLUTION_WXGA

Additionally, the following support flags are set:
- EVE_SUPPORT_UNICODE
- EVE_SUPPORT_MEDIAFIFO
- EVE_SUPPORT_CAPACITIVE
- EVE_SUPPORT_RESISTIVE

*/

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
/* Validate the configured options. */
#if defined(EVE_GRAPHICS_BT820) || defined(VM820B15A) || defined(VM820B10A)
#define EVE_GRAPHICS_AVAILABLE
#endif

#if defined(EVE_DISPLAY_WUXGA) || defined(EVE_DISPLAY_FHD) || defined(EVE_DISPLAY_WXGA)
#define EVE_DISPLAY_AVAILABLE
#endif

#if defined(EVE_PLATFORM_FT4222) || defined(EVE_PLATFORM_MPSSE) || defined(EVE_PLATFORM_RP2040) \
    || defined(MM2040EV)
#define EVE_PLATFORM_AVAILABLE
#endif


///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

/** @details

The selected graphics module below must set one of the following options.
(Users must select the graphics module definition, *not* one of these enable options.)
- BT820_ENABLE

It may also set platform, display, and flash values if none are configured.

*/

/** graphics module config **/
#if defined(EVE_GRAPHICS_BT820)
#define EVE_CHIPID EVE_BT820
#define EVE_GEN EVE5
#define BT820_ENABLE
#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WUXGA
#endif

#elif defined(VM820B15A)
#define EVE_CHIPID EVE_BT820
#define EVE_GEN EVE5
#define BT820_ENABLE
#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_FHD
#endif

#elif defined(VM820B10A)
#define EVE_CHIPID EVE_BT820
#define EVE_GEN EVE5
#define BT820_ENABLE
#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WXGA
#endif
#endif

/** Display config **/
#if defined(EVE_DISPLAY_WUXGA)
#define DISPLAY_RESOLUTION_WUXGA
#elif defined(EVE_DISPLAY_FHD)
#define DISPLAY_RESOLUTION_FHD
#define TOUCH_PATCH_REQUIRED
#elif defined(EVE_DISPLAY_WXGA)
#define DISPLAY_RESOLUTION_WXGA
#endif

/** Platform config **/
#if defined(MM2040EV) && !defined(EVE_PLATFORM_RP2040)
#define EVE_PLATFORM_RP2040
#endif

/** SPI config **/
#if defined(EVE_SPI_QUAD)
#define ENABLE_SPI_QUAD
#elif defined(EVE_SPI_DUAL)
#define ENABLE_SPI_DUAL
#else
#define ENABLE_SPI_SINGLE
#endif
#if defined(EVE_PLATFORM_RP2040) // RP2040 only support single mode
#ifdef ENABLE_SPI_QUAD
#undef ENABLE_SPI_QUAD
#endif
#ifdef ENABLE_SPI_DUAL
#undef ENABLE_SPI_DUAL
#endif
#endif

/** DDR config **/
#define EVE_DDR_SIZE 0 /**< 128M Bytes */
//#define EVE_DDR_SIZE 2 /**< 512M Bytes */

/** Flash config **/
#define EVE_FLASH_NAND
#define EVE_FLASH_SIZE 128 /**< 128M Bytes */

/* Platform config */
/*

Ultimately, the platform selection must set one of the following internal platform flags.
- FT4222_PLATFORM
- MPSSE_PLATFORM
- RP2040_PLATFORM
These may only be set by one of the platform target definitions, and should not be set manually by the user.

*/
#if defined(EVE_PLATFORM_FT4222)
#define FT4222_PLATFORM
#define MSVC_PLATFORM
#define EVE_HOST EVE_HOST_FT4222

#elif defined(EVE_PLATFORM_MPSSE)
#define MPSSE_PLATFORM
#define MSVC_PLATFORM
#define EVE_HOST EVE_HOST_MPSSE

#elif defined(EVE_PLATFORM_RP2040)
#define RP2040_PLATFORM
#define EVE_HOST EVE_HOST_RP2040
#endif

#define EVE_CONFIG__STR(x) #x
#define EVE_CONFIG_STR(x) EVE_CONFIG__STR(x)

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

/** @name Feature support.
 * Avoid hardcoding specific EVE models throughout the libraries.
 * \n Allows disabling specific features for debugging purposes.
 */
///@{
#define EVE_SUPPORT_UNICODE
#define EVE_SUPPORT_MEDIAFIFO
#define EVE_SUPPORT_CAPACITIVE
//#define EVE_SUPPORT_RESISTIVE
///@}
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

// Other options
#define EVE_CMD_HOOKS 0 /**< Allow adding a callback hook into EVE_CoCmd calls using CoCmdHook in EVE_HalContext */

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
#ifndef _MSC_VER
/* strcpy_s, sprintf_s is not available in GCC */
#define strcpy_s(dst, sz, src) strcpy(dst, src)
#define sprintf_s(str, size, fmt, ...) snprintf(str, size, fmt, ##__VA_ARGS__)
#endif

#ifdef _MSC_VER
#define inline __inline
#endif

#if !defined(NDEBUG) && !defined(_DEBUG)
#define _DEBUG 1
#endif

#if defined(__GNUC__)
#define DO_PRAGMA_(x) _Pragma(#x)
#define eve_pragma_warning(msg) DO_PRAGMA_("GCC warning \"" msg "\"")
#elif defined(_MSC_VER)
#define eve_pragma_warning(msg) __pragma(message(__FILE__ "(" EVE_CONFIG_STR(__LINE__) "): warning EVE_Hal: " msg))
#else
#define eve_pragma_warning(msg)
#endif

#if defined(__GNUC__)
#define eve_pragma_error(msg) _Pragma("GCC error \"" msg "\"")
#elif defined(_MSC_VER)
#define eve_pragma_error(msg) __pragma(message(__FILE__ "(" EVE_CONFIG_STR(__LINE__) "): error EVE_Hal: " msg))
#else
#define eve_pragma_error(msg)
#endif

/* Set implementation options */
#if defined(MPSSE_PLATFORM)
#define EVE_BUFFER_WRITES
#endif

/* Enable FatFS by default on supported platforms */
#if defined(RP2040_PLATFORM)
#ifndef EVE_ENABLE_FATFS
#define EVE_ENABLE_FATFS 1
#endif
#else
#define EVE_ENABLE_FATFS 0
#endif

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

/// Configuration sanity checks
#if !defined(EVE_DISPLAY_AVAILABLE)
#pragma message(__FILE__ "(" EVE_CONFIG_STR(__LINE__) "): error EVE_DISPLAY_AVAILABLE: " \
                                                      "No display model has been selected")
#endif
#if ((defined(FT4222_PLATFORM) ? 1 : 0)   \
    + (defined(MPSSE_PLATFORM) ? 1 : 0)   \
    + (defined(RP2040_PLATFORM) ? 1 : 0)) \
    > 1
#pragma message(__FILE__ "(" EVE_CONFIG_STR(__LINE__) "): warning PLATFORM: " \
                                                      "More than one platform has been selected")
#endif
#if !defined(EVE_PLATFORM_AVAILABLE)
#pragma message(__FILE__ "(" EVE_CONFIG_STR(__LINE__) "): warning PLATFORM: " \
                                                      "No platform was selected")
#endif

#endif /* EVE_CONFIG__H */

/* end of file */
