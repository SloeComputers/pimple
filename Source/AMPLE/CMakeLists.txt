#-------------------------------------------------------------------------------
# Copyright (c) 2024 John D. Haughton
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------

autogen_py(Table_phase_delta)
autogen_py(Table_exp)
autogen_py(Table_log_sine)

add_library(AMPLE STATIC
            Sound.cpp
            Table_phase_delta.cpp
            Table_log_sine.cpp
            Table_exp.cpp
            )

target_include_directories(AMPLE
   PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
   PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
   )

target_link_libraries(AMPLE
   PUBLIC STB
   )
