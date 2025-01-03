# CMake project ################################################################
#
# 1. Build for FT4222 example: Use Deveoper Command Promt For VS
#  cd build && cmake  -G "NMake Makefiles" -DEVE_APPS_PLATFORM=EVE_PLATFORM_FT4222 -DEVE_APPS_GRAPHICS=EVE_GRAPHICS_BT820 -DEVE_APPS_DISPLAY=EVE_DISPLAY_WUXGA -DCMAKE_BUILD_TYPE=Debug ..
#  nmake SampleApp
#
# 2. Build for MPSSE example: Use Developer Command Promt For VS
#  cd build && cmake  -G "NMake Makefiles" -DEVE_APPS_PLATFORM=EVE_PLATFORM_MPSSE -DEVE_APPS_GRAPHICS=EVE_GRAPHICS_BT820 -DEVE_APPS_DISPLAY=EVE_DISPLAY_WUXGA -DCMAKE_BUILD_TYPE=Debug ..
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
set(LIST_GRAPHICS DEFAULT
                  EVE_GRAPHICS_BT820
  )
set(LIST_PLATFORM DEFAULT
                  EVE_PLATFORM_FT4222
                  EVE_PLATFORM_MPSSE
  )
set(LIST_DISPLAY  DEFAULT
                  EVE_DISPLAY_WUXGA
  )

# Set cache ####################################################################
SET(EVE_APPS_GRAPHICS "DEFAULT" CACHE STRING "Graphics Module")
SET_PROPERTY(CACHE EVE_APPS_GRAPHICS PROPERTY STRINGS ${LIST_GRAPHICS})

# TODO: Set a better default here automatically when CMake is run with an embedded platform
SET(EVE_APPS_PLATFORM "DEFAULT" CACHE STRING "Platform Target")
SET_PROPERTY(CACHE EVE_APPS_PLATFORM PROPERTY STRINGS ${LIST_PLATFORM})

SET(EVE_APPS_DISPLAY "DEFAULT" CACHE STRING "Display Resolution")
SET_PROPERTY(CACHE EVE_APPS_DISPLAY PROPERTY STRINGS ${LIST_DISPLAY})

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

validate_input("EVE_APPS_GRAPHICS" LIST_GRAPHICS)
validate_input("EVE_APPS_PLATFORM" LIST_PLATFORM)
validate_input("EVE_APPS_DISPLAY"  LIST_DISPLAY)

# Find host platform name ######################################################
STRING(REGEX MATCH "(EVE_PLATFORM_FT4222|EVE_PLATFORM_MPSSE|EVE_PLATFORM_BT8XXEMU)" IS_PLATFORM_WINDOW   ${EVE_APPS_PLATFORM})
STRING(REGEX MATCH "EVE_PLATFORM_FT4222"       IS_PLATFORM_FT4222   ${EVE_APPS_PLATFORM})
STRING(REGEX MATCH "EVE_PLATFORM_MPSSE"        IS_PLATFORM_MPSSE    ${EVE_APPS_PLATFORM}) 
SET(IS_PLATFORM_WINDOW   "${IS_PLATFORM_WINDOW}"   CACHE STRING "Platform Target WINDOW  ")
SET(IS_PLATFORM_FT4222   "${IS_PLATFORM_FT4222}"   CACHE STRING "Platform Target FT4222  ")
SET(IS_PLATFORM_MPSSE    "${IS_PLATFORM_MPSSE}"    CACHE STRING "Platform Target MPSSE   ")

mark_as_advanced(IS_PLATFORM_WINDOW  )
mark_as_advanced(IS_PLATFORM_FT4222  )
mark_as_advanced(IS_PLATFORM_MPSSE   )

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

ENDIF()

# C language version ###########################################################
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Most graphics boards have a default display and platform specified in the configuration header
# MULTI and DEFAULT behave the same, it depends on the selected graphics board
# For embedded targets, the platforma always needs to be specified, though, since CMake needs to set the proper build options
if(NOT EVE_APPS_GRAPHICS STREQUAL "DEFAULT")
  add_definitions("-D${EVE_APPS_GRAPHICS}")
endif()
if(NOT EVE_APPS_PLATFORM STREQUAL "DEFAULT")
  add_definitions("-D${EVE_APPS_PLATFORM}")
endif()
if(NOT EVE_APPS_DISPLAY STREQUAL "DEFAULT")
  add_definitions("-D${EVE_APPS_DISPLAY}")
endif()

# Common setup #################################################################
ADD_SUBDIRECTORY(common)
INCLUDE_DIRECTORIES("common/eve_hal")
INCLUDE_DIRECTORIES("common/eve_hal/Hdr")

# Project setup ################################################################
IF(NOT EVE_APPS_GRAPHICS STREQUAL "MULTI")
  ADD_SUBDIRECTORY(SampleApp)
ENDIF()

################################################################################
