

# EveApps-BT82X
**EveApps-BT82X** is a collection of sample applications created as reference materials for Bridgetek’s 5th-generation graphics controller, the **BT82X**.

The samples are implemented in ANSI C with full source code provided. They are primarily instructional, focusing on demonstrating individual display lists and commands.

All applications share common resources located in the *“common”* folder.

Each sample includes a *“project”* subfolder that contains the required files for building the application with **Microsoft Visual Studio 2019**.

Users should be familiar with the BT82X programming guide and datasheet before using these examples.


## Folder introduction
```
📂 EveApps
    ├───common                  
    │   ├───application      | Application's common functions
    │   ├───eve_flash        | Blob binary for flash programming
    │   └───eve_hal          | Hardware abstraction layer to bridge different platforms
    │    
    ├───SampleApp
    │    ├───Animation       | Sample usage of animation 
    |    ├───Audio           | Sample usage of audio functionality    
    │    ├───Bitmap          | Sample usage of bitmap 
    |    ├───Extension       | Sample usage of extension
    │    ├───Flash           | Sample usage of flash 
    │    ├───Font            | Sample usage of font
    |    ├───Lvds            | Sample usage of LVDSRX
    │    ├───Primitives      | Sample usage of drawing primitives
    │    ├───Touch           | Sample usage of touch functionality
    │    ├───Utility         | Sample usage of helper utilities
    │    ├───Video           | Sample usage of video playback
    │    ├───Widget          | Sample for default widgets
```

Note:
- Definitions for Eve registers, commands, and instructions can be found in common/eve_hal/EVE_GpuDef.h and common/eve_hal/EVE_CoCmd.h.
- The flash blob used for accessing the EVE-connected NOR flash chip is located in common/eve_flash.
- By default, NAND flash settings are defined in common/eve_hal/EVE_Config.h.

## Hardware dependency 
Unless specified otherwise, the default hardware platform is the VM820C.  You may refer to the details at www.brtchip.com

## Build instructions

### FT4222 and MPSSE
#### Way 1: Open SampleApp_MSVC.sln

Go to property

![property](https://github.com/user-attachments/assets/953dd3b1-3a9b-4747-ae0d-1daeda844b94)

Change settings (EVE_PLATFORM_FT4222 / EVE_PLATFORM_MPSSE)

![image](https://github.com/user-attachments/assets/9dd29dea-abd3-4a09-a5f6-31b4f318d41a)

press F5 to build and run

#### Way 2: Use Cmake: (Need Cmake 3.19 installed)
```sh
# Launch the appropriate Developer Command Prompt for VS
# For 64-bit build, open:
# x64 Native Tools Command Prompt for VS
call "[path to VS]\VC\Auxiliary\Build\vcvars64.bat"   # for 64-bit
# For 32-bit build, open:
# x86 Native Tools Command Prompt for VS
call "[path to VS]\VC\Auxiliary\Build\vcvars32.bat"   # for 32-bit

cd [path to EveApps]
mkdir build
cd build
cmake -G "NMake Makefiles" -DEVE_APPS_PLATFORM=EVE_PLATFORM_FT4222 -DEVE_APPS_GRAPHICS=[EVE graphics] -DEVE_APPS_DISPLAY=[EVE display] -DEVE_APPS_SPI=[EVE SPI] -DCMAKE_BUILD_TYPE=Debug ..
nmake SampleApp_[Project Name]

# EVE_APPS_PLATFORM can be one of : EVE_PLATFORM_FT4222 or EVE_PLATFORM_MPSSE
# [EVE graphics] must be : EVE_GRAPHICS_BT820
# [EVE display] can be one of:  EVE_DISPLAY_WUXGA (1920x1200 LCD), EVE_DISPLAY_FHD (1920x1080 LCD)
# [EVE SPI] can be one of : EVE_SPI_QUAD, EVE_SPI_DUAL, EVE_SPI_SINGLE  (FT4222 can support Quad/Dual/Single, MPSSE supports single only)
# [Project Name] is the folder name of a subfolder inside SampleApp , one example is SampleApp_Widget
# The output binary [Project Name].exe can be found in build\[Project Name]

One example is :
cmake -G "NMake Makefiles" -DEVE_APPS_PLATFORM=EVE_PLATFORM_FT4222 -DEVE_APPS_GRAPHICS=EVE_GRAPHICS_BT820 -DEVE_APPS_DISPLAY=EVE_DISPLAY_WUXGA -DCMAKE_BUILD_TYPE=Debug ..
```

example: 
![image](https://github.com/user-attachments/assets/d1d4b27c-8634-44d4-a0db-1efed9333bd4)

![image](https://github.com/user-attachments/assets/2f0bfb2d-30c0-4b5f-a19f-4eb864e2a721)

### Raspberry Pi Pico

Requires the Pico toolchain https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads to be installed.

Pico-SDK version 2.2.0 is required
```sh
$ git clone https://github.com/raspberrypi/pico-sdk.git --branch master
$ git submodule update --init
```


The following steps will build for Raspberry Pi Pico.

 1. Install cmake 3.19.x, Visual Studio 2019 community (must select C++), GNU Arm Embedded Toolchain for window.
 2. Launch the *Developer Command Prompt for VS*
```sh
set PICO_SDK_PATH=[path to pico-sdk]
set PICO_TOOLCHAIN_PATH=[path to GNU Arm Embedded Toolchain\\bin]
cd EveApps
mkdir build
cd build
cmake -G "NMake Makefiles" -DEVE_APPS_PLATFORM=EVE_PLATFORM_RP2040 -DEVE_APPS_GRAPHICS=[EVE graphics] -DEVE_APPS_DISPLAY=[EVE display] -DEVE_APPS_SPI=[EVE SPI] -DCMAKE_BUILD_TYPE=Debug ..
nmake [Project name]

# [EVE graphics] must be : EVE_GRAPHICS_BT820
# [EVE display] can be one of:  EVE_DISPLAY_WUXGA (1920x1200 LCD), EVE_DISPLAY_FHD (1920x1080 LCD)
# [EVE SPI] must : EVE_SPI_SINGLE (PR2040 supports single only)
# [Project Name] is the folder name of a subfolder inside SampleApp , one example is SampleApp_Widget
# The output binary [Project Name].uf2 can be found in build\SampleApp\[Project]
```

Example: 
```
$ cmake.exe -G "NMake Makefiles"  -DEVE_APPS_GRAPHICS=EVE_GRAPHICS_BT820 -DEVE_APPS_PLATFORM=MM2040EV -DEVE_APPS_DISPLAY=EVE_DISPLAY_WUXGA -DEVE_APPS_SPI=EVE_SPI_SINGLE -DCMAKE_BUILD_TYPE=Debug ..
$ nmake 
```

#### Connections

| RP2040 | EVE | UART | SD |
| --- | --- | --- | --- |
| GP0 (TX) | | RX (debug) | |
| GP1 (RX) | | TX (optional) | |
| GP2 (SPI0 SCK) | SCK | | |
| GP3 (SPI0 MOSI) | MOSI | | |
| GP4 (SPI0 MISO) | MISO | | |
| GP5 (GPIO) | CS | | |
| GP6 (GPIO) | INT | | |
| GP7 (GPIO) | PWD | | |
| 5V | 5V | | |
| GND | GND | | |

## Version
This version is v1.2.0_RC1

## Release log
```
v1.2.0_RC1
    - Refactored sample application code
    - Added default patch loading support
    - Introduced new functions for NAND flash
    - Updated RP2040 SDK support to version 2.2.0
    - Removed Display_StartColor
    - Added new sample applications:
      Video playback from SD card with pause/resume functionality
      CMD_COPYLIST example
    - Fixed font indexer issue on RP2040
    - Corrected configuration errors in Release mode
    - Removed unused code
    - Updated asset naming to align with EAB 4.0

v1.1.0_RC1
    - Added support for Raspberry Pi Pico RP2040
    - Added support for the VM820B15A module
    - Updated README to clarify 64-bit and 32-bit build support
    - Enhanced LVDSRX sample applications and added a showcase demonstrating the use of swapchain with LVDSRX 
    - Added a new sample application demonstrating extension usage, including patch loading and the use of newly introduced commands from the patch
      Commands: CMD_REGION, CMD_ENDREGION, CMD_TOUCHOFFSET, CMD_ENDTOUCHOFFSET
    - Added a Flash sample application demonstrating how to program flash from an EVE-connected SD card
    - Performed code cleanup

v1.0.0_RC1
    - Refactored and organized codebase for Doxygen integration
    - Added Doxygen documentation support for EveApps APIs
    - Enhanced the QSPI read/write process
    - Removed emulator support; emulation is no longer supported starting from BT820

v1.0.0_Beta
    - Introduced a new sample application demonstrating the use of LVDSRX
    - Added sample code to showcase new commands and options introduced in BT820
      Commands: CMD_SKIPCOND, CMD_WAITCOND
      Options: OPT_COMPLETEREG
    - Added interrupt driven rendering example
    - Code cleanup and synced register/command names with BRT_AN_086 BT82X Series Programming Guide v1.1

pre_release v0.1.0
    - Initial release
    - Added ten sample applications with FT4222 and MPSSE support for BT820
```
