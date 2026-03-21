#-------------------------------------------------------------------------------
# Copyright (c) 2024 John D. Haughton
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------

add_subdirectory(AMPLE)

add_picoX7_executable(pimple
                         pimple.cpp
                      LIBRARIES
                         AMPLE)
