#-------------------------------------------------------------------------------
# Copyright (c) 2024 John D. Haughton
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.20)

project(PIMPLE
        LANGUAGES CXX C ASM
        VERSION 0.1)

include(CTest)

set(HARDWARE picoX7)

add_subdirectory(Platform)
add_subdirectory(Hardware)
add_subdirectory(Source)
