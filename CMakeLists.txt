# CMake project ################################################################
#
# 1. Build for FT4222 example: Use Deveoper Command Promt For VS
#  cd build && cmake  -G "NMake Makefiles" -DEVE_APPS_PLATFORM=EVE_PLATFORM_FT4222 -DEVE_APPS_GRAPHICS=EVE_GRAPHICS_BT820 -DEVE_APPS_DISPLAY=EVE_DISPLAY_WUXGA -DEVE_APPS_SPI=EVE_SPI_QUAD -DCMAKE_BUILD_TYPE=Debug ..
#  nmake SampleApp
#
# 2. Build for MPSSE example: Use Developer Command Promt For VS
#  cd build && cmake  -G "NMake Makefiles" -DEVE_APPS_PLATFORM=EVE_PLATFORM_MPSSE -DEVE_APPS_GRAPHICS=EVE_GRAPHICS_BT820 -DEVE_APPS_DISPLAY=EVE_DISPLAY_WUXGA -DEVE_APPS_SPI=EVE_SPI_QUAD -DCMAKE_BUILD_TYPE=Debug ..
#  nmake SampleApp
#
# 3. Build for RP2040 example:
#  set PICO_SDK_PATH=[path-to-pico-sdk]
#  set PICO_TOOLCHAIN_PATH=[path-to-gnu-arm-toolchain]
#  cd build && cmake  -G "NMake Makefiles" -DEVE_APPS_PLATFORM=MM2040EV -DEVE_APPS_GRAPHICS=VM820B156 -DEVE_APPS_DISPLAY=EVE_DISPLAY_FHD -DEVE_APPS_SPI=EVE_SPI_SINGLE -DCMAKE_BUILD_TYPE=Debug ..
#  nmake SampleApp
#

CMAKE_MINIMUM_REQUIRED(VERSION 3.13 FATAL_ERROR)

#Uncomment to see gcc stdout
#set (CMAKE_VERBOSE_MAKEFILE ON)
IF (POLICY CMP0020)
	CMAKE_POLICY(SET CMP0020 NEW)
ENDIF ()

IF (POLICY CMP0015)
	CMAKE_POLICY(SET CMP0015 NEW)
ENDIF ()

SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
define_property(
  TARGET
  PROPERTY FOLDER
  INHERITED
  BRIEF_DOCS "Set the folder name."
  FULL_DOCS  "Use to organize targets in an IDE."
)

# Ensure that we are not building in our source directories. ###################
SET(Build_Dir_OK "TRUE")
STRING(REGEX MATCH "^${CMAKE_SOURCE_DIR}" In_Sub_Dir ${CMAKE_BINARY_DIR})
IF (In_Sub_Dir)
  STRING(REGEX MATCH "^${CMAKE_SOURCE_DIR}/[Bb]uild" In_Build_Dir ${CMAKE_BINARY_DIR})
  IF (NOT In_Build_Dir)
    SET(Build_Dir_OK "FALSE")
  ENDIF ()
ENDIF ()

IF (NOT Build_Dir_OK)
  MESSAGE(FATAL_ERROR "You must run cmake from a directory that is not in your source tree, or that is in a special subdirectory of the tree whose name begins with 'build'.")
ENDIF ()
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/") 

# Valid option input ###########################################################
set(LIST_GRAPHICS EVE_GRAPHICS_BT820
                  VM820B156
  )
set(LIST_PLATFORM EVE_PLATFORM_FT4222
                  EVE_PLATFORM_MPSSE
                  EVE_PLATFORM_RP2040
                  MM2040EV
  )
set(LIST_DISPLAY  EVE_DISPLAY_WUXGA
                  EVE_DISPLAY_FHD
  )

# Validate input ###############################################################
function(give_sugguest value array global_set)
  FOREACH(item ${${array}})
    STRING(REGEX MATCH ".*${value}.*" search_val "${item}")
    IF(search_val)
      MESSAGE(STATUS "Notice: ${value} is not defined, selected this similar configure: ${search_val}")
      set(${global_set} "${search_val}" CACHE STRING "Graphics Module" FORCE)
      break()
    ENDIF()
  ENDFOREACH()
endfunction()
  
function(validate_input option_name array)
  set(value "${${option_name}}")
  
  list(FIND ${array} "${value}" _index)
  STRING(REPLACE ";"  ", " list_str "${${array}}")
  if (${_index} EQUAL -1)
    # give sugguestion
    give_sugguest(${value} ${array} "${option_name}")
    
    # if give_sugguest works ok, then value of option_name is changed, if no, stop cmake with error message
    if ("${value}" STREQUAL "${${option_name}}")
      MESSAGE(STATUS "\nError: ${option_name} is not set, the valid ${option_name} is: ")
      MESSAGE(STATUS "${list_str}\n")
      MESSAGE(FATAL_ERROR "${option_name} is not set") # stop cmake
    ENDIF()
  ENDIF()
  
  MESSAGE(STATUS "${option_name} is set to ${${option_name}}")
  MESSAGE(STATUS "Other ${option_name}'s are: ${list_str} \n")
endfunction()

function(validate_env_rp2040 env env_name)
    MESSAGE(STATUS "${env_name} is ${env}")
    
    if ("${env}" STREQUAL "")
        MESSAGE(FATAL_ERROR "${env_name} is not set, stop compile!")
    endif()
    MESSAGE(STATUS "${env_name} is ${env}")
endfunction()

validate_input("EVE_APPS_GRAPHICS" LIST_GRAPHICS)
validate_input("EVE_APPS_PLATFORM" LIST_PLATFORM)
validate_input("EVE_APPS_DISPLAY"  LIST_DISPLAY)

# Find host platform name ######################################################
STRING(REGEX MATCH "(EVE_PLATFORM_FT4222|EVE_PLATFORM_MPSSE)" IS_PLATFORM_WINDOW   ${EVE_APPS_PLATFORM})
STRING(REGEX MATCH "EVE_PLATFORM_FT4222"       IS_PLATFORM_FT4222   ${EVE_APPS_PLATFORM})
STRING(REGEX MATCH "EVE_PLATFORM_MPSSE"        IS_PLATFORM_MPSSE    ${EVE_APPS_PLATFORM}) 
STRING(REGEX MATCH "(EVE_PLATFORM_RP2040|MM2040EV)"       IS_PLATFORM_RP2040   ${EVE_APPS_PLATFORM})
SET(IS_PLATFORM_WINDOW   "${IS_PLATFORM_WINDOW}"   CACHE STRING "Platform Target WINDOW  ")
SET(IS_PLATFORM_FT4222   "${IS_PLATFORM_FT4222}"   CACHE STRING "Platform Target FT4222  ")
SET(IS_PLATFORM_MPSSE    "${IS_PLATFORM_MPSSE}"    CACHE STRING "Platform Target MPSSE   ")
SET(IS_PLATFORM_RP2040   "${IS_PLATFORM_RP2040}"   CACHE STRING "Platform Target RP2040  ")

mark_as_advanced(IS_PLATFORM_WINDOW  )
mark_as_advanced(IS_PLATFORM_FT4222  )
mark_as_advanced(IS_PLATFORM_MPSSE   )
mark_as_advanced(IS_PLATFORM_RP2040  )

# Set up the basic build environment ###########################################
IF (CMAKE_BUILD_TYPE STREQUAL "")
  # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This messes up
  # differentiation between debug and release builds.
  SET(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
ENDIF ()

# Platform SDk setting #########################################################
IF(IS_PLATFORM_WINDOW)
  PROJECT("EVE_Apps" C CXX)
  
  # Strip binary in release mode
  IF (CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "-s")
  ENDIF ()
  
  SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  
  IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
    MESSAGE(STATUS "Using 64-bit Windows dependencies")
    LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/common/eve_hal/Bin/MSVC/x64)
  ELSE()
    MESSAGE(STATUS "Using 32-bit Windows dependencies")
    LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/common/eve_hal/Bin/MSVC/x86)
  ENDIF()

ELSEIF(IS_PLATFORM_RP2040)
  #validate_env_rp2040("$ENV{PICO_SDK_PATH}" "PICO_SDK_PATH")
  #validate_env_rp2040("$ENV{PICO_TOOLCHAIN_PATH}" "PICO_TOOLCHAIN_PATH")

  # Pull in Pico SDK (must be before project)
  INCLUDE(pico_sdk_import.cmake)
  PROJECT("EVE_Apps" C CXX ASM)

  # Initialise the Pico SDK
  pico_sdk_init()

  INCLUDE_DIRECTORIES(common/application/fatfs/source)
ENDIF()

# C language version ###########################################################
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# For embedded targets, the platforma always needs to be specified, though, since CMake needs to set the proper build options
add_definitions("-D${EVE_APPS_GRAPHICS}")
add_definitions("-D${EVE_APPS_PLATFORM}")
add_definitions("-D${EVE_APPS_DISPLAY}")
add_definitions("-D${EVE_APPS_SPI}")

# Common setup #################################################################
ADD_SUBDIRECTORY(common)
INCLUDE_DIRECTORIES("common/eve_hal")
INCLUDE_DIRECTORIES("common/eve_hal/Hdr")

# Project setup ################################################################
ADD_SUBDIRECTORY(SampleApp)

################################################################################
