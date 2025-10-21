#-------------------------------------------------------------------------------
# Copyright (c) 2024 John D. Haughton
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------

add_subdirectory(AMPLE)

foreach(hw_config ${hw_picoX7_config_list})

   add_executable(pimple_${hw_config}
                  pimple.cpp)

   target_compile_definitions(pimple_${hw_config} PRIVATE HW_${hw_config})

   target_include_directories(pimple_${hw_config}
                              PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
                              PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

   target_link_libraries(pimple_${hw_config} PRIVATE AMPLE HW)

   set_target_properties(pimple_${hw_config} PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

   if(${hw_config} STREQUAL "NATIVE")
      add_subdirectory(AMPLE/Test)
   endif()

endforeach()
