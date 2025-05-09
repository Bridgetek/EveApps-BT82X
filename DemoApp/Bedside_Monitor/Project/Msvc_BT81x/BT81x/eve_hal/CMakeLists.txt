
# CMake configuration for multi-target EVE HAL build

FILE(GLOB SRCS *.c)
FILE(GLOB HDRS *.h)
SOURCE_GROUP("" FILES ${SRCS} ${HDRS})

FILE(GLOB HDRS_EXTERNAL Hdr/*.h)
SOURCE_GROUP("Hdr" FILES ${HDRS_EXTERNAL})

INCLUDE_DIRECTORIES(Hdr)

ADD_LIBRARY(eve_hal
  ${SRCS}
  ${HDRS}
  ${HDRS_EXTERNAL}
)

IF (WIN32)
  TARGET_LINK_LIBRARIES(eve_hal
    libMPSSE
    LibFT4222
    ftd2xx
    bt8xxemu
  )
ELSEIF(UNIX)
  TARGET_LINK_LIBRARIES(eve_hal
    # libMPSSE
    ft4222
    # ftd2xx
    # bt8xxemu
  )
ELSEIF(IS_PLATFORM_RP2040)
  # Add the standard library to the build
  TARGET_LINK_LIBRARIES(eve_hal pico_stdlib)
  
  # Add any user requested libraries
  TARGET_LINK_LIBRARIES(eve_hal
    hardware_spi
    hardware_timer
  )
  TARGET_LINK_LIBRARIES(eve_hal fatfs)
ELSEIF(IS_PLATFORM_FT900)
  TARGET_LINK_LIBRARIES(eve_hal fatfs)
  TARGET_LINK_LIBRARIES(eve_hal "ft900;m;c;stub")
ELSEIF(IS_PLATFORM_FT930)
  TARGET_LINK_LIBRARIES(eve_hal fatfs)
  TARGET_LINK_LIBRARIES(eve_hal "ft930;m;c;stub")
ENDIF()

IF (WIN32)
  IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
    ADD_CUSTOM_COMMAND(TARGET eve_hal POST_BUILD 
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_CURRENT_SOURCE_DIR}/Bin/MSVC/x64/libMPSSE.dll
      ${CMAKE_BINARY_DIR}/bin/$<CONFIG>/libMPSSE.dll)
    ADD_CUSTOM_COMMAND(TARGET eve_hal POST_BUILD 
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_CURRENT_SOURCE_DIR}/Bin/MSVC/x64/LibFT4222-64.dll
      ${CMAKE_BINARY_DIR}/bin/$<CONFIG>/LibFT4222-64.dll)
    ADD_CUSTOM_COMMAND(TARGET eve_hal POST_BUILD 
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_CURRENT_SOURCE_DIR}/Bin/MSVC/x64/ftd2xx.dll
      ${CMAKE_BINARY_DIR}/bin/$<CONFIG>/ftd2xx.dll)
    ADD_CUSTOM_COMMAND(TARGET eve_hal POST_BUILD 
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_CURRENT_SOURCE_DIR}/Bin/Simulation/x64/bt8xxemu.dll
      ${CMAKE_BINARY_DIR}/bin/$<CONFIG>/bt8xxemu.dll)
    ADD_CUSTOM_COMMAND(TARGET eve_hal POST_BUILD 
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_CURRENT_SOURCE_DIR}/Bin/Simulation/x64/mx25lemu.dll
      ${CMAKE_BINARY_DIR}/bin/$<CONFIG>/mx25lemu.dll)
  ELSE()
    ADD_CUSTOM_COMMAND(TARGET eve_hal POST_BUILD 
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_CURRENT_SOURCE_DIR}/Bin/MSVC/x86/libMPSSE.dll
      ${CMAKE_BINARY_DIR}/bin/$<CONFIG>/libMPSSE.dll)
    ADD_CUSTOM_COMMAND(TARGET eve_hal POST_BUILD 
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_CURRENT_SOURCE_DIR}/Bin/MSVC/x86/LibFT4222.dll
      ${CMAKE_BINARY_DIR}/bin/$<CONFIG>/LibFT4222.dll)
    ADD_CUSTOM_COMMAND(TARGET eve_hal POST_BUILD 
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_CURRENT_SOURCE_DIR}/Bin/MSVC/x86/ftd2xx.dll
      ${CMAKE_BINARY_DIR}/bin/$<CONFIG>/ftd2xx.dll)
    ADD_CUSTOM_COMMAND(TARGET eve_hal POST_BUILD 
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_CURRENT_SOURCE_DIR}/Bin/Simulation/x86/bt8xxemu.dll
      ${CMAKE_BINARY_DIR}/bin/$<CONFIG>/bt8xxemu.dll)
    ADD_CUSTOM_COMMAND(TARGET eve_hal POST_BUILD 
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
     ${CMAKE_CURRENT_SOURCE_DIR}/Bin/Simulation/x86/mx25lemu.dll
      ${CMAKE_BINARY_DIR}/bin/$<CONFIG>/mx25lemu.dll)
  ENDIF()
ENDIF()
