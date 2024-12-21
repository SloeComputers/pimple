#-------------------------------------------------------------------------------
#  Copyright (c) 2024 John D. Haughton
#
#  Permission is hereby granted, free of charge, to any person obtaining a copy
#  of this software and associated documentation files (the "Software"), to deal
#  in the Software without restriction, including without limitation the rights
#  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#  copies of the Software, and to permit persons to whom the Software is
#  furnished to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be included in
#  all copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#  SOFTWARE.
#-------------------------------------------------------------------------------

add_subdirectory(AMPLE)
add_subdirectory(hw)

foreach(hw_config ${hw_config_list})

   add_executable(pimple_${hw_config}
                  pimple.cpp)

   target_compile_definitions(pimple_${hw_config} PRIVATE HW_${hw_config})
   target_include_directories(pimple_${hw_config}
                              PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
                              PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
   target_link_libraries(     pimple_${hw_config} PRIVATE AMPLE ${hw_target_libs})

   if(${hw_config} STREQUAL "NATIVE")
      add_subdirectory(AMPLE/Test)
   endif()

endforeach()
