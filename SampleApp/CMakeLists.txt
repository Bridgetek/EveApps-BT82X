LINK_LIBRARIES(eve_hal)

set_directory_properties(PROPERTIES FOLDER SampleApp)

INCLUDE_DIRECTORIES("../common/application")

# Iterate through all sub folder and add into ADD_SUBDIRECTORY
macro(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
        SET(dirlist ${dirlist} ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
endmacro()

SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

FOREACH(subdir ${SUBDIRS})
  IF(NOT ${subdir} STREQUAL ".vs")
    ADD_SUBDIRECTORY(${subdir})
  ENDIF()
ENDFOREACH()
