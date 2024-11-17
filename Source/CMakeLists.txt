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

set(python_path ${CMAKE_CURRENT_SOURCE_DIR}/../Platform/python)

add_custom_command(
   COMMENT
      "Auto-generate function tables"
   COMMAND
      ${CMAKE_COMMAND}
         -E env PYTHONPATH=${python_path}
         ${CMAKE_CURRENT_SOURCE_DIR}/TABLE_GEN.py
   DEPENDS
      ${CMAKE_CURRENT_SOURCE_DIR}/TABLE_GEN.py
      ${python_path}/Table.py
   OUTPUT
      ${CMAKE_CURRENT_BINARY_DIR}/Table_phase_delta.h
      ${CMAKE_CURRENT_BINARY_DIR}/Table_phase_delta.cpp
      ${CMAKE_CURRENT_BINARY_DIR}/Table_exp.h
      ${CMAKE_CURRENT_BINARY_DIR}/Table_exp.cpp
      ${CMAKE_CURRENT_BINARY_DIR}/Table_log_sine.h
      ${CMAKE_CURRENT_BINARY_DIR}/Table_log_sine.cpp
   )

add_subdirectory(hw)

foreach(hw_config ${hw_config_list})

   add_executable(pimple_${hw_config}
                  pimple.cpp
                  Table_phase_delta.cpp
                  Table_log_sine.cpp
                  Table_exp.cpp)

   target_compile_definitions(pimple_${hw_config} PRIVATE HW_${hw_config})
   target_include_directories(pimple_${hw_config}
                              PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
                              PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
   target_link_libraries(     pimple_${hw_config} PRIVATE ${hw_target_libs})

endforeach()
