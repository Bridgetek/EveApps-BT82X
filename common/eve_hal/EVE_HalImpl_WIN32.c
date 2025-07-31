/**
 * @file EVE_HalImpl_WIN32.c
 * @brief Eve_Hal framework APIs for WIN32 host platform
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

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 28159) // Use GetTickCount64 instead of GetTickCount
#endif

#include "EVE_HalImpl.h"
#if defined(_WIN32)

/*********
** INIT **
*********/

/** @name INIT */
///@{

/**
 * @brief Init host MCU
 * 
 */
void EVE_Mcu_initialize()
{
	/* no-op */
}

/**
 * @brief Release host MCU
 * 
 */
void EVE_Mcu_release()
{
	/* no-op */
}
///@}

/*********
** MISC **
*********/

static DWORD s_Millis_Start;
static ULONGLONG s_Millis64_Start;
/** @name MISC */
///@{

/**
 * @brief Init timer
 * 
 */
void EVE_Millis_initialize()
{
	s_Millis_Start = GetTickCount();
	s_Millis64_Start = GetTickCount64();
}

/**
 * @brief Release timer
 * 
 */
void EVE_Millis_release()
{
	/* no-op */
}

/**
 * @brief Get clock in miliseond
 * 
 * global counter to loopback after ~49.71 days
 * 
 * @return uint32_t Clock number
 */
uint32_t EVE_millis()
{
	return GetTickCount() - s_Millis_Start;
}

/**
* @brief Get clock in miliseond
* 
* @return uint64_t Clock number
*/
uint64_t EVE_millis64()
{
	return GetTickCount64() - s_Millis64_Start;
}

/**
 * @brief Sleep in milisecond
 * 
 * @param ms Milisecond
 */
void EVE_sleep(uint32_t ms)
{
	Sleep(ms);
}
///@}

#endif

#ifdef _MSC_VER
#pragma warning(pop)
#endif

/* end of file */
