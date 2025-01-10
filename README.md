

# EveApps-BT82X
EveApps-BT82X is a collection of applications designed as reference resources for Bridgetek's 5th-generation graphics controller, the BT82X.

The collection is divided into two categories: **Sample** and **Demo** applications. Both are written in ANSI C, with complete source code provided.

- **Sample applications** focus on instructional use, demonstrating individual display lists or commands. 
- **Demo applications** are more complex, closely resembling real-world projects.  (To be updated) 

All applications rely on shared resources located in the "common" folder.

Each application includes a "project" subfolder containing the necessary files for building the application using the MS Visual Studio 2022.

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

![setting](https://github.com/user-attachments/assets/9ea058a0-e7a5-497c-a7da-cb71f9b23cb0)

press F5 to build and run

#### Way 2: Use Cmake: (Need Cmake 3.19 installed)
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
![image](https://github.com/user-attachments/assets/93515ee2-33c7-4b45-9c88-42c2b2949860)

![image](https://github.com/user-attachments/assets/a7d0cc56-bd20-4baa-93d7-84a02bcceb1f)
