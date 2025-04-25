

# EveApps-BT82X-RP2040
EveApps-BT82X is a collection of applications designed as reference resources for Bridgetek's 5th-generation graphics controller, the BT82X.
EveApps-BT82X-RP2040 branch adds RP2040 interfacing with VM820C.

The collection is divided into two categories: **Sample** and **Demo** applications. Both are written in ANSI C, with complete source code provided.

- **Sample applications** focus on instructional use, demonstrating individual display lists or commands. 
- **Demo applications** are more complex, closely resembling real-world projects.  (To be updated) 

All applications rely on shared resources located in the "common" folder.

Each application includes a "project" subfolder containing the necessary files for building the application using the MS Visual Studio 2019.

Users are expected to have familiarity with the BT82X programming guide and datasheet.



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
    │    ├───Flash           | Sample usage of flash 
    │    ├───Font            | Sample usage of font 
    │    ├───Primitives      | Sample usage of drawing primitives
    │    ├───Touch           | Sample usage of touch functionality
    │    ├───Utility         | Sample usage of helper utilities
    │    ├───Video           | Sample usage of video playback
    │    ├───Widget          | Sample for default widgets
```

Note:
- For Eve registers and commands/instructions definition, user can find it at the file common/eve_hal/EVE_GpuDef.h and common/eve_hal/Eve_CoCmd.h.
- For flash blob file used to access the eve connected flash chip, user can find it at common/eve_flash.

## Hardeware dependency 
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
#Launch the Developer Command Prompt for VS
cd [path to EveApps]
mkdir build
cd build
cmake -G "NMake Makefiles" -DEVE_APPS_PLATFORM=EVE_PLATFORM_FT4222 -DEVE_APPS_GRAPHICS=[EVE graphics] -DEVE_APPS_DISPLAY=[EVE display] -DEVE_APPS_SPI=[EVE SPI] -DCMAKE_BUILD_TYPE=Debug ..
nmake SampleApp_[Project Name]

# EVE_APPS_PLATFORM can be one of : EVE_PLATFORM_FT4222 or EVE_PLATFORM_MPSSE
# [EVE graphics] must be : EVE_GRAPHICS_BT820
# [EVE display] must be:  EVE_DISPLAY_WUXGA   (1920x1200 LCD)
# [EVE SPI] can be one of : EVE_SPI_QUAD, EVE_SPI_DUAL, EVE_SPI_SINGLE  (FT4222 can support Quad/Dual/Single, MPSSE supports single only)
# [Project Name] is the folder name of a subfolder inside SampleApp , one example is SampleApp_Widget
# The output binary [Project Name].exe  can be found in build\[Project Name]

One example is :
cmake -G "NMake Makefiles" -DEVE_APPS_PLATFORM=EVE_PLATFORM_FT4222 -DEVE_APPS_GRAPHICS=EVE_GRAPHICS_BT820 -DEVE_APPS_DISPLAY=EVE_DISPLAY_WUXGA -DCMAKE_BUILD_TYPE=Debug ..
```

example: 
![image](https://github.com/user-attachments/assets/d1d4b27c-8634-44d4-a0db-1efed9333bd4)

![image](https://github.com/user-attachments/assets/2f0bfb2d-30c0-4b5f-a19f-4eb864e2a721)

### RP2040
#### Use Cmake: (Need Cmake 3.19 installed)
```sh
#Launch the Developer Command Prompt for VS
cd [path to EveApps]
mkdir build
cd build
cmake  -G "NMake Makefiles" -DEVE_APPS_PLATFORM=EVE_PLATFORM_RP2040 -DEVE_APPS_GRAPHICS=EVE_GRAPHICS_BT820 -DEVE_APPS_DISPLAY=EVE_DISPLAY_WUXGA ..
nmake SampleApp_[Project Name]

# [Project Name] is the folder name of a subfolder inside SampleApp , one example is SampleApp_Widget
# The output binary [Project Name].exe  can be found in build\[Project Name]
```


