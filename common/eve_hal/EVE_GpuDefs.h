/**
 * @file EVE_GpuDefs.h
 * @brief Defines EVE hardware values
 *
 * This header is separated and included last 
 * in case of conflicts with other libraries.
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

#ifndef EVE_GPU_DEFS__H
#define EVE_GPU_DEFS__H

/*********************
 *      INCLUDES
 *********************/
#include "EVE_Config.h"

/* Definitions used for BT820 coprocessor command buffer */
#define EVE_DL_SIZE                   (16 * 1024UL) /**< 16kB Display List buffer size */
#define EVE_CMD_FIFO_SIZE             ((16) * 1024UL) /**< 16kB coprocessor FIFO size */
#define EVE_CMD_FIFO_COUNT            (EVE_CMD_FIFO_SIZE / 4)
#define EVE_CMD_FIFO_MASK             (EVE_CMD_FIFO_SIZE - 1)
#define EVE_CMD_FIFO_ALIGNMENT_MASK   (EVE_CMD_FIFO_SIZE - ((4) - 1))
#define EVE_CMD_FAULT(rp)             (rp & 0x3)

#define SWAPCHAIN_0              0xFFFF00FF
#define SWAPCHAIN_1              0xFFFF01FF
#define SWAPCHAIN_2              0xFFFF02FF

#define SPI_WIDTH_1bit           0x000000 /**< SINGLE mode */
#define SPI_WIDTH_2bit           0x000100 /**< DUAL mode */
#define SPI_WIDTH_4bit           0X000200 /**< QUAD mode */

#define SYS_CLK_FREQ             72000000UL
#define SYS_PLL_FREQ             576000000UL

/**************
** Addresses **
**************/

/** @name Memory Map */
///@{
#define RAM_G                    0UL
#define BASE                     0x7f000000
#define RAM_CMD                  (BASE + 0x0000)
#define REG_CORE_R2              (BASE + 0x4000)
#define RAM_REPORT               (BASE + 0x4800)
#define REG_CORE_R1              (BASE + 0x6000)
#define RAM_DL                   (BASE + 0x8000)

#define REG_LVDSTX               (BASE + 0x800300)
#define REG_SYS                  (BASE + 0x800400)
#define REG_LVDSRX               (BASE + 0x800500)
#define REG_I2S                  (BASE + 0x800800)
///@}

/** @name Auxiliary Registers */
///@{
#define REG_TRACKER              (REG_CORE_R2 + 0x0000)
#define REG_TRACKER_1            (REG_CORE_R2 + 0x0004)
#define REG_TRACKER_2            (REG_CORE_R2 + 0x0008)
#define REG_TRACKER_3            (REG_CORE_R2 + 0x000C)
#define REG_TRACKER_4            (REG_CORE_R2 + 0x0010)
#define REG_MEDIAFIFO_READ       (REG_CORE_R2 + 0x0014)
#define REG_MEDIAFIFO_WRITE      (REG_CORE_R2 + 0x0018)
#define REG_ANIM_ACTIVE          (REG_CORE_R2 + 0x002C)
#define REG_OBJECT_COMPLETE      (REG_CORE_R2 + 0x0038)
#define REG_EXTENT_X0            (REG_CORE_R2 + 0x003C)
#define REG_EXTENT_Y0            (REG_CORE_R2 + 0x0040)
#define REG_EXTENT_X1            (REG_CORE_R2 + 0x0044)
#define REG_EXTENT_Y1            (REG_CORE_R2 + 0x0048)
#define REG_PLAY_CONTROL         (REG_CORE_R2 + 0x0050)
///@}

/** @name Flash Interface Registers */
///@{
#define REG_FLASH_SIZE           (REG_CORE_R2 + 0x0024)
#define REG_FLASH_STATUS         (REG_CORE_R1 + 0x05D4)
///@}

/** @name Swap Chain Registers */
///@{
#define REG_SC0_RESET            (REG_CORE_R1 + 0x0034)
#define REG_SC0_SIZE             (REG_CORE_R1 + 0x0038)
#define REG_SC0_PTR0             (REG_CORE_R1 + 0x003C)
#define REG_SC0_PTR1             (REG_CORE_R1 + 0x0040)
#define REG_SC0_PTR2             (REG_CORE_R1 + 0x0044)
#define REG_SC0_PTR3             (REG_CORE_R1 + 0x0048)
#define REG_SC1_RESET            (REG_CORE_R1 + 0x004C)
#define REG_SC1_SIZE             (REG_CORE_R1 + 0x0050)
#define REG_SC1_PTR0             (REG_CORE_R1 + 0x0054)
#define REG_SC1_PTR1             (REG_CORE_R1 + 0x0058)
#define REG_SC1_PTR2             (REG_CORE_R1 + 0x005C)
#define REG_SC1_PTR3             (REG_CORE_R1 + 0x0060)
#define REG_SC2_RESET            (REG_CORE_R1 + 0x0064)
#define REG_SC2_SIZE             (REG_CORE_R1 + 0x0068)
#define REG_SC2_PTR0             (REG_CORE_R1 + 0x006C)
#define REG_SC2_PTR1             (REG_CORE_R1 + 0x0070)
#define REG_SC2_PTR2             (REG_CORE_R1 + 0x0074)
#define REG_SC2_PTR3             (REG_CORE_R1 + 0x0078)
#define REG_SC2_STATUS           (REG_CORE_R1 + 0x0780)
#define REG_SC2_ADDR             (REG_CORE_R1 + 0x0784)
///@}

/** @name Miscellaneous Registers */
///@{
#define REG_ID                   (REG_CORE_R1 + 0x0000)
#define REG_FRAMES               (REG_CORE_R1 + 0x0004)
#define REG_CLOCK                (REG_CORE_R1 + 0x0008)
#define REG_FREQUENCY            (REG_CORE_R1 + 0x000C)
#define REG_CPURESET             (REG_CORE_R1 + 0x0088)
#define REG_GPIO_DIR             (REG_CORE_R1 + 0x00DC)
#define REG_GPIO                 (REG_CORE_R1 + 0x00E0)
#define REG_DISP                 (REG_CORE_R1 + 0x00E4)
#define REG_INT_FLAGS            (REG_CORE_R1 + 0x0100)
#define REG_INT_EN               (REG_CORE_R1 + 0x0104)
#define REG_INT_MASK             (REG_CORE_R1 + 0x0108)
#define REG_PWM_HZ               (REG_CORE_R1 + 0x0128)
#define REG_PWM_DUTY             (REG_CORE_R1 + 0x012C)
#define REG_MACRO_0              (REG_CORE_R1 + 0x0130)
#define REG_MACRO_1              (REG_CORE_R1 + 0x0134)
#define REG_BOOT_CFG             (REG_CORE_R1 + 0x0628)
///@}

/** @name Graphics Engine Registers */
///@{
#define REG_RE_DEST              (REG_CORE_R1 + 0x0010)
#define REG_RE_FORMAT            (REG_CORE_R1 + 0x0014)
#define REG_RE_ROTATE            (REG_CORE_R1 + 0x0018)
#define REG_RE_W                 (REG_CORE_R1 + 0x001C)
#define REG_RE_H                 (REG_CORE_R1 + 0x0020)
#define REG_RE_DITHER            (REG_CORE_R1 + 0x0024)
#define REG_RE_ACTIVE            (REG_CORE_R1 + 0x0028)
#define REG_RE_RENDERS           (REG_CORE_R1 + 0x002C)

#define REG_HCYCLE               (REG_CORE_R1 + 0x008C)
#define REG_HOFFSET              (REG_CORE_R1 + 0x0090)
#define REG_HSIZE                (REG_CORE_R1 + 0x0094)
#define REG_HSYNC0               (REG_CORE_R1 + 0x0098)
#define REG_HSYNC1               (REG_CORE_R1 + 0x009C)
#define REG_VCYCLE               (REG_CORE_R1 + 0x00A0)
#define REG_VOFFSET              (REG_CORE_R1 + 0x00A4)
#define REG_VSIZE                (REG_CORE_R1 + 0x00A8)
#define REG_VSYNC0               (REG_CORE_R1 + 0x00AC)
#define REG_VSYNC1               (REG_CORE_R1 + 0x00B0)
#define REG_DLSWAP               (REG_CORE_R1 + 0x00B4)
#define REG_PCLK_POL             (REG_CORE_R1 + 0x00B8)
#define REG_TAG_X                (REG_CORE_R1 + 0x00BC)
#define REG_TAG_Y                (REG_CORE_R1 + 0x00C0)
#define REG_TAG                  (REG_CORE_R1 + 0x00C4)

#define REG_SO_MODE              (REG_CORE_R1 + 0x05F4)
#define REG_SO_SOURCE            (REG_CORE_R1 + 0x05F8)
#define REG_SO_FORMAT            (REG_CORE_R1 + 0x05FC)
#define REG_SO_EN                (REG_CORE_R1 + 0x0600)
///@}

/** @name Audio Engine Registers */
///@{
#define REG_VOL_L_PB             (REG_CORE_R1 + 0x00C8)
#define REG_VOL_R_PB             (REG_CORE_R1 + 0x00CC)
#define REG_VOL_SOUND            (REG_CORE_R1 + 0x00D0)
#define REG_SOUND                (REG_CORE_R1 + 0x00D4)
#define REG_PLAY                 (REG_CORE_R1 + 0x00D8)

#define REG_PLAYBACK_START       (REG_CORE_R1 + 0x010C)
#define REG_PLAYBACK_LENGTH      (REG_CORE_R1 + 0x0110)
#define REG_PLAYBACK_READPTR     (REG_CORE_R1 + 0x0114)
#define REG_PLAYBACK_FREQ        (REG_CORE_R1 + 0x0118)
#define REG_PLAYBACK_FORMAT      (REG_CORE_R1 + 0x011C)
#define REG_PLAYBACK_LOOP        (REG_CORE_R1 + 0x0120)
#define REG_PLAYBACK_PLAY        (REG_CORE_R1 + 0x0124)
#define REG_PLAYBACK_PAUSE       (REG_CORE_R1 + 0x05D0)

#define REG_AUD_PWM              (REG_CORE_R1 + 0x013C)

#define REG_I2S_EN               (REG_CORE_R1 + 0x0714)
#define REG_I2S_FREQ             (REG_CORE_R1 + 0x0718)
///@}

/** @name coprocessor engine Registers */
///@{
#define REG_CMD_READ             (REG_CORE_R1 + 0x014C)
#define REG_CMD_WRITE            (REG_CORE_R1 + 0x0150)
#define REG_CMD_DL               (REG_CORE_R1 + 0x0154)
#define REG_CMDB_SPACE           (REG_CORE_R1 + 0x0594)
#define REG_CMDB_WRITE           (BASE + 0x10000)
#define CMDBUF_SIZE              0x4000
///@}

/** @name Touch Screen Engine Registers */
///@{
#define REG_CTOUCH_EXTENDED      (REG_CORE_R1 + 0x015C)
#define REG_CTOUCH_TOUCH0_XY     (REG_CORE_R1 + 0x0160)
#define REG_TOUCH_SCREEN_XY      (REG_CORE_R1 + 0x0160)
#define REG_CTOUCH_TOUCHA_XY     (REG_CORE_R1 + 0x0164)
#define REG_TOUCH_RAW_XY         (REG_CORE_R1 + 0x0164)
#define REG_CTOUCH_TOUCHB_XY     (REG_CORE_R1 + 0x0168)
#define REG_CTOUCH_TOUCHC_XY     (REG_CORE_R1 + 0x016C)
#define REG_CTOUCH_TOUCH4_XY     (REG_CORE_R1 + 0x0170)
#define REG_TOUCH_TAG_XY         (REG_CORE_R1 + 0x0174)
#define REG_TOUCH_TAG            (REG_CORE_R1 + 0x0178)
#define REG_TOUCH_TAG1_XY        (REG_CORE_R1 + 0x017C)
#define REG_TOUCH_TAG1           (REG_CORE_R1 + 0x0180)
#define REG_TOUCH_TAG2_XY        (REG_CORE_R1 + 0x0184)
#define REG_TOUCH_TAG2           (REG_CORE_R1 + 0x0188)
#define REG_TOUCH_TAG3_XY        (REG_CORE_R1 + 0x018C)
#define REG_TOUCH_TAG3           (REG_CORE_R1 + 0x0190)
#define REG_TOUCH_TAG4_XY        (REG_CORE_R1 + 0x0194)
#define REG_TOUCH_TAG4           (REG_CORE_R1 + 0x0198)
#define REG_TOUCH_TRANSFORM_A    (REG_CORE_R1 + 0x019C)
#define REG_TOUCH_TRANSFORM_B    (REG_CORE_R1 + 0x01A0)
#define REG_TOUCH_TRANSFORM_C    (REG_CORE_R1 + 0x01A4)
#define REG_TOUCH_TRANSFORM_D    (REG_CORE_R1 + 0x01A8)
#define REG_TOUCH_TRANSFORM_E    (REG_CORE_R1 + 0x01AC)
#define REG_TOUCH_TRANSFORM_F    (REG_CORE_R1 + 0x01B0)
#define REG_TOUCH_CONFIG         (REG_CORE_R1 + 0x01B4)
///@}

/** @name LVDSRX Core registers */
///@{
#define REG_LVDSRX_CORE_ENABLE   (REG_CORE_R1 + 0x0670)
#define REG_LVDSRX_CORE_CAPTURE  (REG_CORE_R1 + 0x0674)
#define REG_LVDSRX_CORE_SETUP    (REG_CORE_R1 + 0x0678)
#define REG_LVDSRX_CORE_DEST     (REG_CORE_R1 + 0x067C)
#define REG_LVDSRX_CORE_FORMAT   (REG_CORE_R1 + 0x0680)
#define REG_LVDSRX_CORE_DITHER   (REG_CORE_R1 + 0x0684)
#define REG_LVDSRX_CORE_FRAMES   (REG_CORE_R1 + 0x0698)
///@}

/** @name LVDSRX System Register */
///@{
#define REG_LVDSRX_SETUP         (REG_LVDSRX + 0x0000)
#define REG_LVDSRX_CTRL          (REG_LVDSRX + 0x0004)
#define REG_LVDSRX_STAT          (REG_LVDSRX + 0x0008)
///@}

/** @name LVDSTX system registers */
///@{
#define REG_LVDSTX_EN            (REG_LVDSTX + 0x0000)
#define REG_LVDSTX_PLLCFG        (REG_LVDSTX + 0x0004)
#define REG_LVDSTX_CTRL_CH0      (REG_LVDSTX + 0x0014)
#define REG_LVDSTX_CTRL_CH1      (REG_LVDSTX + 0x0018)
#define REG_LVDSTX_STAT          (REG_LVDSTX + 0x001C)
#define REG_LVDSTX_ERR_STAT      (REG_LVDSTX + 0x0020)
///@}

/** @name System Registers */
///@{
#define REG_PIN_DRV_0            (REG_SYS + 0x0008)
#define REG_PIN_DRV_1            (REG_SYS + 0x000C)
#define REG_PIN_SLEW_0           (REG_SYS + 0x0010)
#define REG_PIN_TYPE_0           (REG_SYS + 0x0014)
#define REG_PIN_TYPE_1           (REG_SYS + 0x0018)
#define REG_SYS_CFG              (REG_SYS + 0x0020)
#define REG_SYS_STAT             (REG_SYS + 0x0024)
#define REG_CHIP_ID              (REG_SYS + 0x0048)
#define REG_BOOT_STATUS          (REG_SYS + 0x004C) /**< to confirm whether it is normal running after start up */
#define REG_DDR_TYPE             (REG_SYS + 0x0054)
#define REG_PIN_DRV_2            (REG_SYS + 0x0064)
#define REG_PIN_SLEW_1           (REG_SYS + 0x0068)
#define REG_PIN_TYPE_2           (REG_SYS + 0x006C)
///@}

/** @name I2S Registers */
///@{
#define REG_I2S_CFG              (REG_I2S + 0x0000)
#define REG_I2S_CTL              (REG_I2S + 0x0004)
#define REG_I2S_STAT             (REG_I2S + 0x0010)
#define REG_I2S_PAD_CFG          (REG_I2S + 0x0014)
///@}

/*************
** Commands **
*************/

// clang-format off


/** @name Commands for BT820 */
///@{
#define CMD_ANIMDRAW         4294967119UL /**< 0xFFFFFF4FUL */
#define CMD_ANIMFRAME        4294967134UL /**< 0xFFFFFF5EUL */
#define CMD_ANIMSTART        4294967135UL /**< 0xFFFFFF5FUL */
#define CMD_ANIMSTOP         4294967117UL /**< 0xFFFFFF4DUL */
#define CMD_ANIMXY           4294967118UL /**< 0xFFFFFF4EUL */
#define CMD_APPEND           4294967068UL /**< 0xFFFFFF1CUL */
#define CMD_APPENDF          4294967122UL /**< 0xFFFFFF52UL */
#define CMD_ARC              4294967175UL /**< 0xFFFFFF87UL */
#define CMD_BGCOLOR          4294967047UL /**< 0xFFFFFF07UL */
#define CMD_BITMAP_TRANSFORM 4294967071UL /**< 0xFFFFFF1FUL */
#define CMD_BUTTON           4294967051UL /**< 0xFFFFFF0BUL */
#define CMD_CALIBRATE        4294967059UL /**< 0xFFFFFF13UL */
#define CMD_CALIBRATESUB     4294967126UL /**< 0xFFFFFF56UL */
#define CMD_CALLLIST         4294967131UL /**< 0xFFFFFF5BUL */
#define CMD_CGRADIENT        4294967178UL /**< 0xFFFFFF8AUL */
#define CMD_CLOCK            4294967058UL /**< 0xFFFFFF12UL */
#define CMD_COLDSTART        4294967086UL /**< 0xFFFFFF2EUL */
#define CMD_COPYLIST         4294967176UL /**< 0xFFFFFF88UL */
#define CMD_DDRSHUTDOWN      4294967141UL /**< 0xFFFFFF65UL */
#define CMD_DDRSTARTUP       4294967142UL /**< 0xFFFFFF66UL */
#define CMD_DIAL             4294967081UL /**< 0xFFFFFF29UL */
#define CMD_DLSTART          4294967040UL /**< 0xFFFFFF00UL */
#define CMD_ENABLEREGION     4294967166UL /**< 0xFFFFFF7EUL */
#define CMD_ENDLIST          4294967133UL /**< 0xFFFFFF5DUL */
#define CMD_FENCE            4294967144UL /**< 0xFFFFFF68UL */
#define CMD_FGCOLOR          4294967048UL /**< 0xFFFFFF08UL */
#define CMD_FILLWIDTH        4294967121UL /**< 0xFFFFFF51UL */
#define CMD_FLASHATTACH      4294967107UL /**< 0xFFFFFF43UL */
#define CMD_FLASHDETACH      4294967106UL /**< 0xFFFFFF42UL */
#define CMD_FLASHERASE       4294967102UL /**< 0xFFFFFF3EUL */
#define CMD_FLASHFAST        4294967108UL /**< 0xFFFFFF44UL */
#define CMD_FLASHPROGRAM     4294967140UL /**< 0xFFFFFF64UL */
#define CMD_FLASHREAD        4294967104UL /**< 0xFFFFFF40UL */
#define CMD_FLASHSOURCE      4294967112UL /**< 0xFFFFFF48UL */
#define CMD_FLASHSPIDESEL    4294967109UL /**< 0xFFFFFF45UL */
#define CMD_FLASHSPIRX       4294967111UL /**< 0xFFFFFF47UL */
#define CMD_FLASHSPITX       4294967110UL /**< 0xFFFFFF46UL */
#define CMD_FLASHUPDATE      4294967105UL /**< 0xFFFFFF41UL */
#define CMD_FLASHWRITE       4294967103UL /**< 0xFFFFFF3FUL */
#define CMD_FSDIR            4294967182UL /**< 0xFFFFFF8EUL */
#define CMD_FSOPTION         4294967149UL /**< 0xFFFFFF6DUL */
#define CMD_FSREAD           4294967153UL /**< 0xFFFFFF71UL */
#define CMD_FSSIZE           4294967168UL /**< 0xFFFFFF80UL */
#define CMD_FSSOURCE         4294967167UL /**< 0xFFFFFF7FUL */
#define CMD_GAUGE            4294967057UL /**< 0xFFFFFF11UL */
#define CMD_GETIMAGE         4294967128UL /**< 0xFFFFFF58UL */
#define CMD_GETMATRIX        4294967087UL /**< 0xFFFFFF2FUL */
#define CMD_GETPROPS         4294967074UL /**< 0xFFFFFF22UL */
#define CMD_GETPTR           4294967072UL /**< 0xFFFFFF20UL */
#define CMD_GLOW             4294967179UL /**< 0xFFFFFF8BUL */
#define CMD_GRADCOLOR        4294967088UL /**< 0xFFFFFF30UL */
#define CMD_GRADIENT         4294967049UL /**< 0xFFFFFF09UL */
#define CMD_GRADIENTA        4294967120UL /**< 0xFFFFFF50UL */
#define CMD_GRAPHICSFINISH   4294967147UL /**< 0xFFFFFF6BUL */
#define CMD_I2SSTARTUP       4294967145UL /**< 0xFFFFFF69UL */
#define CMD_INFLATE          4294967114UL /**< 0xFFFFFF4AUL */
#define CMD_INTERRUPT        4294967042UL /**< 0xFFFFFF02UL */
#define CMD_KEYS             4294967052UL /**< 0xFFFFFF0CUL */
#define CMD_LOADASSET        4294967169UL /**< 0xFFFFFF81UL */
#define CMD_LOADIDENTITY     4294967075UL /**< 0xFFFFFF23UL */
#define CMD_LOADIMAGE        4294967073UL /**< 0xFFFFFF21UL */
#define CMD_LOADPATCH        4294967170UL /**< 0xFFFFFF82UL */
#define CMD_LOADWAV          4294967173UL /**< 0xFFFFFF85UL */
#define CMD_LOGO             4294967085UL /**< 0xFFFFFF2DUL */
#define CMD_MEDIAFIFO        4294967092UL /**< 0xFFFFFF34UL */
#define CMD_MEMCPY           4294967067UL /**< 0xFFFFFF1BUL */
#define CMD_MEMCRC           4294967062UL /**< 0xFFFFFF16UL */
#define CMD_MEMSET           4294967065UL /**< 0xFFFFFF19UL */
#define CMD_MEMWRITE         4294967064UL /**< 0xFFFFFF18UL */
#define CMD_MEMZERO          4294967066UL /**< 0xFFFFFF1AUL */
#define CMD_NEWLIST          4294967132UL /**< 0xFFFFFF5CUL */
#define CMD_NOP              4294967123UL /**< 0xFFFFFF53UL */
#define CMD_NUMBER           4294967082UL /**< 0xFFFFFF2AUL */
#define CMD_PLAYVIDEO        4294967093UL /**< 0xFFFFFF35UL */
#define CMD_PLAYWAV          4294967161UL /**< 0xFFFFFF79UL */
#define CMD_PROGRESS         4294967053UL /**< 0xFFFFFF0DUL */
#define CMD_REGREAD          4294967063UL /**< 0xFFFFFF17UL */
#define CMD_REGWRITE         4294967174UL /**< 0xFFFFFF86UL */
#define CMD_RENDERTARGET     4294967181UL /**< 0xFFFFFF8DUL */
#define CMD_RESETFONTS       4294967116UL /**< 0xFFFFFF4CUL */
#define CMD_RESTORECONTEXT   4294967165UL /**< 0xFFFFFF7DUL */
#define CMD_RESULT           4294967177UL /**< 0xFFFFFF89UL */
#define CMD_RETURN           4294967130UL /**< 0xFFFFFF5AUL */
#define CMD_ROMFONT          4294967097UL /**< 0xFFFFFF39UL */
#define CMD_ROTATE           4294967078UL /**< 0xFFFFFF26UL */
#define CMD_ROTATEAROUND     4294967115UL /**< 0xFFFFFF4BUL */
#define CMD_RUNANIM          4294967136UL /**< 0xFFFFFF60UL */
#define CMD_SAVECONTEXT      4294967164UL /**< 0xFFFFFF7CUL */
#define CMD_SCALE            4294967077UL /**< 0xFFFFFF25UL */
#define CMD_SCREENSAVER      4294967083UL /**< 0xFFFFFF2BUL */
#define CMD_SCROLLBAR        4294967055UL /**< 0xFFFFFF0FUL */
#define CMD_SDATTACH         4294967150UL /**< 0xFFFFFF6EUL */
#define CMD_SDBLOCKREAD      4294967151UL /**< 0xFFFFFF6FUL */
#define CMD_SETBASE          4294967091UL /**< 0xFFFFFF33UL */
#define CMD_SETBITMAP        4294967101UL /**< 0xFFFFFF3DUL */
#define CMD_SETFONT          4294967094UL /**< 0xFFFFFF36UL */
#define CMD_SETMATRIX        4294967079UL /**< 0xFFFFFF27UL */
#define CMD_SETROTATE        4294967089UL /**< 0xFFFFFF31UL */
#define CMD_SETSCRATCH       4294967095UL /**< 0xFFFFFF37UL */
#define CMD_SKETCH           4294967084UL /**< 0xFFFFFF2CUL */
#define CMD_SKIPCOND         4294967180UL /**< 0xFFFFFF8CUL */
#define CMD_SLIDER           4294967054UL /**< 0xFFFFFF0EUL */
#define CMD_SNAPSHOT         4294967069UL /**< 0xFFFFFF1DUL */
#define CMD_SPINNER          4294967060UL /**< 0xFFFFFF14UL */
#define CMD_STOP             4294967061UL /**< 0xFFFFFF15UL */
#define CMD_SWAP             4294967041UL /**< 0xFFFFFF01UL */
#define CMD_SYNC             4294967100UL /**< 0xFFFFFF3CUL */
#define CMD_TESTCARD         4294967127UL /**< 0xFFFFFF57UL */
#define CMD_TEXT             4294967050UL /**< 0xFFFFFF0AUL */
#define CMD_TEXTDIM          4294967172UL /**< 0xFFFFFF84UL */
#define CMD_TOGGLE           4294967056UL /**< 0xFFFFFF10UL */
#define CMD_TRACK            4294967080UL /**< 0xFFFFFF28UL */
#define CMD_TRANSLATE        4294967076UL /**< 0xFFFFFF24UL */
#define CMD_VIDEOFRAME       4294967099UL /**< 0xFFFFFF3BUL */
#define CMD_VIDEOSTART       4294967098UL /**< 0xFFFFFF3AUL */
#define CMD_WAIT             4294967129UL /**< 0xFFFFFF59UL */
#define CMD_WAITCHANGE       4294967143UL /**< 0xFFFFFF67UL */
#define CMD_WAITCOND         4294967160UL /**< 0xFFFFFF78UL */
#define CMD_WATCHDOG         4294967171UL /**< 0xFFFFFF83UL */
///@}

/*****************
** Display List **
*****************/

/** @name Setting Graphics State */
///@{
#define ALPHA_FUNC(func, ref)       ((0x09 << 24) | (((func) & 0x7) << 8) | (((ref) & 0xFF) << 0))
#define BEGIN(prim)                 ((0x1F << 24) | ((prim) & 0xF))
#define BITMAP_EXT_FORMAT(format)   ((0x2E << 24) | ((format) & 0xFFFF))
#define BITMAP_HANDLE(handle)       ((0x05 << 24) | ((handle) & 0x3F))
#define BITMAP_LAYOUT(format, linestride, height)  ((0x07 << 24) | (((format) & 0x1F) << 19) | (((linestride) & 0x3FF) << 9) | ((height) & 0x1FF))
#define BITMAP_LAYOUT_H(linestride, height)        ((0x28 << 24) | (((linestride) & 0x3) << 2) | ((height) & 0x3))
#define BITMAP_SIZE(filter, wrapx, wrapy, width, height)   ((0x08 << 24) | (((filter) & 0x1) << 20) | (((wrapx) & 0x1) << 19) | (((wrapy) & 0x1) << 18) | (((width) & 0x1FF) << 9) | ((height) & 0x1FF))
#define BITMAP_SIZE_H(width, height)               ((0x29 << 24) | (((width) & 0x3) << 2) | ((height) & 0x3))
#define BITMAP_SOURCE(addr)         ((0x01 << 24) | ((addr) & 0xFFFFFF))
#define BITMAP_SOURCEH(addr)        ((0x31 << 24) | ((addr) & 0xFF))
#define BITMAP_SWIZZLE(r, g, b, a)  ((0x2F << 24) | (((r) & 0x7) << 9) | (((g) & 0x7) << 6) | (((b) & 0x7) << 3) | ((a) & 0x7))
#define BITMAP_TRANSFORM_A(p, v)    ((0x15 << 24) | (((p) & 0x1) << 17) | ((v) & 0x1FFFF))
#define BITMAP_TRANSFORM_B(p, v)    ((0x16 << 24) | (((p) & 0x1) << 17) | ((v) & 0x1FFFF))
#define BITMAP_TRANSFORM_C(c)       ((0x17 << 24) | ((c) & 0xFFFFFF))
#define BITMAP_TRANSFORM_D(p, v)    ((0x18 << 24) | (((p) & 0x1) << 17) | ((v) & 0x1FFFF))
#define BITMAP_TRANSFORM_E(p, v)    ((0x19 << 24) | (((p) & 0x1) << 17) | ((v) & 0x1FFFF))
#define BITMAP_TRANSFORM_F(f)       ((0x1A << 24) | ((f) & 0xFFFFFF))
#define BITMAP_ZORDER(o)            ((0x33 << 24) | ((o) & 0xFF))
#define BLEND_FUNC(src, dst)        ((0x0B << 24) | (((src) & 0x7) << 3) | ((dst) & 0x7))
#define CELL(cell)                  ((0x06 << 24) | ((cell) & 0x7F))
#define CLEAR_COLOR_A(alpha)        ((0x0F << 24) | ((alpha) & 0xFF))
#define CLEAR_COLOR_RGB(red, green, blue)         ((0x02 << 24) | (((red) & 0xFF) << 16) | (((green) & 0xFF) << 8) | ((blue) & 0xFF))
#define CLEAR_STENCIL(s)            ((0x11 << 24) | ((s) & 0xFF))
#define CLEAR_TAG(s)                ((0x12 << 24) | ((s) & 0xFFFFFF))
#define COLOR_A(alpha)              ((0x10 << 24) | ((alpha) & 0xFF))
#define COLOR_MASK(r, g, b, a)      ((0x20 << 24) | (((r) & 0x1) << 3) | (((g) & 0x1) << 2) | (((b) & 0x1) << 1) | ((a) & 0x1))
#define COLOR_RGB(red, green, blue) ((0x04 << 24) | (((red) & 0xFF) << 16) | (((green) & 0xFF) << 8) | ((blue) & 0xFF))
#define END()                       ((0x21 << 24))
#define LINE_WIDTH(width)           ((0x0E << 24) | ((width) & 0xFFF))
#define PALETTE_SOURCE(addr)        ((0x2A << 24) | ((addr) & 0xFFFFFF))
#define PALETTE_SOURCEH(addr)       ((0x32 << 24) | ((addr) & 0xFF))
#define POINT_SIZE(size)            ((0x0D << 24) | ((size) & 0x1FFF))
#define RESTORE_CONTEXT()           ((0x23 << 24))
#define SAVE_CONTEXT()              ((0x22 << 24))
#define SCISSOR_SIZE(width, height) ((0x1C << 24) | (((width) & 0xFFF) << 12) | ((height) & 0xFFF))
#define SCISSOR_XY(x, y)            ((0x1B << 24) | (((x) & 0x7FF) << 11) | ((y) & 0x7FF))
#define STENCIL_FUNC(func, ref, mask)             ((0x0A << 24) | (((func) & 0xF) << 16) | (((ref) & 0xFF) << 8) | ((mask) & 0xFF))
#define STENCIL_MASK(mask)          ((0x13 << 24) | ((mask) & 0xFF))
#define STENCIL_OP(sfail, spass)    ((0x0C << 24) | (((sfail) & 0x7) << 3) | ((spass) & 0x7))
#define TAG(s)                      ((0x03 << 24) | ((s) & 0xFFFFFF))
#define TAG_MASK(mask)              ((0x14 << 24) | ((mask) & 0x1))
#define VERTEX_FORMAT(frac)         ((0x27 << 24) | ((frac) & 0x7))
#define VERTEX_TRANSLATE_X(x)       ((0x2B << 24) | ((x) & 0x1FFFF))
#define VERTEX_TRANSLATE_Y(y)       ((0x2C << 24) | ((y) & 0x1FFFF))
///@}

/** @name Drawing Actions */
///@{
#define CLEAR(c, s, t)                ((0x26 << 24) | (((c) & 0x1) << 2) | (((s) & 0x1) << 1) | ((t) & 0x1))
#define VERTEX2F(x, y)                ((0x01 << 30) | (((x) & 0x7FFF) << 15) | ((y) & 0x7FFF))
#define VERTEX2II(x, y, handle, cell) ((0x02 << 30) | (((x) & 0x1FF) << 21) | (((y) & 0x1FF) << 12) | (((handle) & 0x1F) << 7) | ((cell) & 0x7F))
///@}

/** @name Execution Control */
///@{
#define CALL(dest)                    ((0x1D << 24) | ((dest) & 0xFFFF))
#define DISPLAY()                     ((0x0 << 24))
#define JUMP(dest)                    ((0x1E << 24) | ((dest) & 0xFFFF))
#define MACRO(m)                      ((0x25 << 24) | ((m) & 0x1))
#define NOP()                         ((0x2D << 24))
#define REGION(y, h, dest)            ((0x34 << 24) | (((y) & 0x3F) << 18) | (((h) & 0x3F) << 12) | ((dest) & 0xFFF))
#define RETURN()                      ((0x24 << 24))
///@}

/***************
** Parameters **
****************/

/** @name for REG_DLSWAP */
///@{
#define DLSWAP_DONE       0UL
#define DLSWAP_FRAME      2UL
///@}

/** @name Options */
///@{
#define OPT_3D            0UL /**< 0x0 */
#define OPT_RGB565        0UL /**< 0x0 */
#define OPT_1BIT          0UL /**< 0x0 */
#define OPT_FULLSPEED     0UL /**< 0x0 */
#define OPT_MONO          1UL /**< 0x1 */
#define OPT_SFNLOWER      1UL /**< 0x1 */
#define OPT_NODL          2UL /**< 0x2 */
#define OPT_4BIT          2UL /**< 0x2 */
#define OPT_CASESENSITIVE 2UL /**< 0x2 */
#define OPT_DIRSEP_WIN    4UL /**< 0x4 */
#define OPT_HALFSPEED     4UL /**< 0x4 */
#define OPT_QUARTERSPEED  8UL /**< 0x8 */
#define OPT_FULLSCREEN    8UL /**< 0x8 */
#define OPT_DIRSEP_UNIX   8UL /**< 0x8 */
#define OPT_MEDIAFIFO     16UL /**< 0x10 */
#define OPT_IS_MMC        16UL /**< 0x10 */
#define OPT_SOUND         32UL /**< 0x20 */
#define OPT_IS_SD         32UL /**< 0x20 */
#define OPT_FLASH         64UL /**< 0x40 */
#define OPT_OVERLAY       128UL /**< 0x80 */
#define OPT_FLAT          256UL /**< 0x100 */
#define OPT_SIGNED        256UL /**< 0x100 */
#define OPT_DITHER        256UL /**< 0x100 */
#define OPT_TRUECOLOR     512UL /**< 0x200 */
#define OPT_CENTERX       512UL /**< 0x200 */
#define OPT_CENTERY       1024UL /**< 0x400 */
#define OPT_YCBCR         1024UL /**< 0x400 */
#define OPT_CENTER        1536UL /**< 0x600 */
#define OPT_RIGHTX        2048UL /**< 0x800 */
#define OPT_DIRECT        2048UL /**< 0x800 */
#define OPT_COMPLETEREG   4096UL /**< 0x1000 */
#define OPT_FORMAT        4096UL /**< 0x1000 */
#define OPT_NOBACK        4096UL /**< 0x1000 */
#define OPT_FILL          8192UL /**< 0x2000 */
#define OPT_NOTICKS       8192UL /**< 0x2000 */
#define OPT_FS            8192UL /**< 0x2000 */
#define OPT_NOHM          16384UL /**< 0x4000 */
#define OPT_NOPOINTER     16384UL /**< 0x4000 */
#define OPT_BASELINE      32768UL /**< 0x8000 */
#define OPT_NOSECS        32768UL /**< 0x8000 */
#define OPT_NOHANDS       49152UL /**< 0xC000 */
///@}

/** @name for CMD_ANIMSTART */
///@{
#define ANIM_ONCE         0UL
#define ANIM_LOOP         1UL
#define ANIM_HOLD         2UL
///@}

/** @name for STENCIL_OP */
///@{
#define ZERO              0UL
#define KEEP              1UL
#define REPLACE           2UL
#define INCR              3UL
#define DECR              4UL
#define INVERT            5UL
///@}

/** @name for BLEND_FUNC */
///@{
//#define ZERO            0UL
#define ONE               1UL
#define SRC_ALPHA         2UL
#define DST_ALPHA         3UL
#define ONE_MINUS_SRC_ALPHA 4UL
#define ONE_MINUS_DST_ALPHA 5UL
///@}

/** @name for BITMAP_SWIZZLE */
///@{
//#define ZERO            0UL
//#define ONE             1UL
#define RED               2UL
#define GREEN             3UL
#define BLUE              4UL
#define ALPHA             5UL
///@}

/** @name for ALPHA_FUNC, CMD_SKIPCOND, CMD_WAITCOND */
///@{
#define NEVER             0UL
#define LESS              1UL
#define LEQUAL            2UL
#define GREATER           3UL
#define GEQUAL            4UL
#define EQUAL             5UL
#define NOTEQUAL          6UL
#define ALWAYS            7UL
///@}

/** @name format of the bitmap for BITMAP_EXT_FORMAT */
///@{
#define ARGB1555          0UL
#define L1                1UL
#define L4                2UL
#define L8                3UL
#define RGB332            4UL
#define ARGB2             5UL
#define ARGB4             6UL
#define RGB565            7UL
#define BARGRAPH          11UL
#define L2                17UL
#define RGB8              19UL
#define ARGB8             20UL
#define PALETTEDARGB8     21UL
#define RGB6              22UL
#define ARGB6             23UL
#define LA1               24UL
#define LA2               25UL
#define LA4               26UL
#define LA8               27UL
#define YCBCR             28UL
#define COMPRESSED_RGBA_ASTC_4x4_KHR   37808UL
#define COMPRESSED_RGBA_ASTC_5x4_KHR   37809UL
#define COMPRESSED_RGBA_ASTC_5x5_KHR   37810UL
#define COMPRESSED_RGBA_ASTC_6x5_KHR   37811UL
#define COMPRESSED_RGBA_ASTC_6x6_KHR   37812UL
#define COMPRESSED_RGBA_ASTC_8x5_KHR   37813UL
#define COMPRESSED_RGBA_ASTC_8x6_KHR   37814UL
#define COMPRESSED_RGBA_ASTC_8x8_KHR   37815UL
#define COMPRESSED_RGBA_ASTC_10x5_KHR  37816UL
#define COMPRESSED_RGBA_ASTC_10x6_KHR  37817UL
#define COMPRESSED_RGBA_ASTC_10x8_KHR  37818UL
#define COMPRESSED_RGBA_ASTC_10x10_KHR 37819UL
#define COMPRESSED_RGBA_ASTC_12x10_KHR 37820UL
#define COMPRESSED_RGBA_ASTC_12x12_KHR 37821UL
///@}

/** @name for REG_FLASH_STATUS */
///@{
#define FLASH_STATUS_INIT          0UL
#define FLASH_STATUS_DETACHED      1UL
#define FLASH_STATUS_BASIC         2UL
#define FLASH_STATUS_FULL          3UL
///@}

/** @name for BITMAP_SIZE */
///@{
//filter
#define NEAREST                    0UL
#define BILINEAR                   1UL
//wrapx/wrapy
#define BORDER                     0UL
#define REPEAT                     1UL
///@}

/** @name for REG_PLAYBACK_FORMAT */
///@{
#define LINEAR_SAMPLES             0UL
#define ULAW_SAMPLES               1UL
#define ADPCM_SAMPLES              2UL
#define S16_SAMPLES                3UL
#define S16S_SAMPLES               4UL
///@}

#ifdef POINTS
#undef POINTS
#endif
/** @name for BEGIN */
///@{
#define BITMAPS                    1UL
#define POINTS                     2UL
#define LINES                      3UL
#define LINE_STRIP                 4UL
#define EDGE_STRIP_R               5UL
#define EDGE_STRIP_L               6UL
#define EDGE_STRIP_A               7UL
#define EDGE_STRIP_B               8UL
#define RECTS                      9UL
///@}

/** @name for REG_CTOUCH_EXTENDED */
///@{
#define CTOUCH_MODE_COMPATIBILITY  1UL /*< single-touch */
#define CTOUCH_MODE_EXTENDED       0UL /*< multi-touch */
///@}

/** @name for CMD_CGRADIENT */
///@{
#define CORNER_ZERO                0UL
#define EDGE_ZERO                  1UL
///@}

/** @name for REG_LVDSTX_EN */
///@{
#define LVDS_CH1_EN                4UL
#define LVDS_CH0_EN                2UL
///@}

/** @name for REG_I2S_CTL */
///@{
#define I2S_AUDIO_DISABLE          8UL
#define I2S_TX_EN                  2UL
#define I2S_SRST                   1UL
///@}

/** @name for REG_I2S_CFG FORMAT */
///@{
#define I2S_FORMAT                 0UL
#define LEFT_FORMAT                1UL
#define RIGHT_FORMAT               2UL
///@}

/** @name for REG_I2S_EN */
///@{
#define I2S_ENABLE                 1UL
#define I2S_DISABLE                0UL
///@}

/** @name for REG_SO_MODE */
///@{
#define ONE_PIXEL_SINGLE_LVDS      0UL
#define TWO_PIXEL_SINGLE_LVDS      1UL
#define TWO_PIXEL_DUAL_LVDS        2UL
#define FOUR_PIXEL_DUAL_LVDS       3UL
///@}

/** @name for REG_LVDSTX_PLLCFG */
///@{
#define LVDSTX_PLLCFG(cps, lock, cks, ns, clkdiv)   ((((cps)&0x7) << 25) | (((lock)&0xFFF) << 13) | (((cks)&0x3) << 11) | (((ns)&0x7F) << 4) | ((clkdiv)&0xF))
///@}

/** @name for REG_LVDSRX_CORE_SETUP */
///@{
#define LVDSRX_ONE_CHANNEL         0UL
#define LVDSRX_TWO_CHANNEL         1UL
#define LVDSRX_ONE_PIXEL_PER_CLK   0UL
#define LVDSRX_TWO_PIXEL_PER_CLK   1UL
///@}

/** @name for REG_LVDSRX_SETUP */
///@{
#define LVDS_MODE_JEIDA_18         0UL
#define LVDS_MODE_JEIDA_24         1UL
#define LVDS_MODE_VESA_24          2UL
#define VS_POL_LOW                 0UL
#define VS_POL_HIGH                1UL
///@}

/** @name for REG_LVDSRX_CTRL */
///@{
#define CHn_CLKSEL_RISING          0UL
#define CHn_CLKSEL_FALLING         1UL
#define CHn_FRANGE_10_30           0UL
#define CHn_FRANGE_30_60           1UL
#define CHn_FRANGE_60_100          2UL
#define CHn_FRANGE_100_180         3UL
#define CHn_PWDN_B_OFF             0UL
#define CHn_PWDN_B_ON              1UL
///@}

/** @name for REG_LVDSRX_STAT */
///@{
#define SIG_VALUE                  15UL /**< 0xF */
#define CHn_DLL_LOCKED             1UL
///@}

/** @name for REG_INT_FLAGS and REG_INT_MASK */
///@{
#define INT_SWAP                   0UL
#define INT_TOUCH                  1UL
#define INT_TAG                    2UL
#define INT_SOUND                  3UL
#define INT_PLAYBACK               4UL
#define INT_CMDEMPTY               5UL
#define INT_CMDFLAG                6UL
#define INT_CONV_COMPLETE          7UL
///@}

// clang-format on

#endif /* #ifndef EVE_GPU_DEFS__H */

/* end of file */
