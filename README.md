

# EveApps 2.0

EveApps 2.0 is a collection of applications that use Eve Series chip (BT820) for reference purposes. The collection includes 10 sample applications, all written in ANSI C code, and the complete source code is provided.

The sample applications are intended for instructional purposes, focusing on individual display lists or commands.

All the applications rely on a set of common files located under the "common" folder. Each application has a subfolder named "project," which contains the project file necessary for building the application with the toolchain.

Users are expected to be familiar with the programming guide and data sheet for the Eve Series chips.

It should be noted that certain applications are specifically developed for particular host platforms (refer to "Support platforms").



## Folder introduction
```
📂 EveApps
    ├───common                  
    │   ├───application      | Application's common functions and fatfs library
    │   ├───eve_flash        | Blob binary for flash programming
    │   └───eve_hal          | Hardware abstraction layer to bridge different platforms
    │    
    ├───SampleApp
    │    ├───Animation       | Sample usage of animation 
    │    ├───Bitmap          | Sample usage of bitmap 
    │    ├───Flash           | Sample usage of flash 
    │    ├───Font            | Sample usage of font 
    │    ├───Primitives      | Sample usage of drawing primitives
    |    ├───Sound           | Sample usage of sound functionality    
    │    ├───Touch           | Sample usage of touch functionality
    │    ├───Utility         | Sample usage of helper utilities
    │    ├───Video           | Sample usage of video playback
    │    ├───Widget          | Sample for default widgets
```

Note:
- For Eve registers and commands/instructions definition, user can find it at the file common/eve_hal/EVE_GpuDef.h and common/eve_hal/Eve_CoCmd.h.
- For flash blob file used to access the eve connected flash chip, user can find it at common/eve_flash.  

## Build instructions

### FT4222 and MPSSE
Way 1: Open SampleApp_MSVC.sln, press F5 to build and run

Way 2: Use Cmake: (Need Cmake 3.19)
```sh
#Launch the Developer Command Prompt for VS
cd X:\source\EveApps
mkdir build
cd build
cmake -G "NMake Makefiles" -DEVE_APPS_PLATFORM=EVE_PLATFORM_FT4222 -DEVE_APPS_GRAPHICS=[EVE graphics] ..
nmake [Project name]

# EVE_APPS_PLATFORM can be FT4222 or MPSSE
# [EVE graphics] can be BT820
# [Project name] is the folder name of a subfolder inside SampleApp
```

example: 
```
$ cmake.exe -G "NMake Makefiles" -DEVE_APPS_PLATFORM=EVE_PLATFORM_FT4222 -DEVE_APPS_GRAPHICS=EVE_GRAPHICS_BT820 -DEVE_APPS_DISPLAY=EVE_DISPLAY_WUXGA ..
$ nmake 
```

