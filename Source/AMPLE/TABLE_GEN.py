#!/usr/bin/env python3
#------------------------------------------------------------------------------
# Copyright (c) 2023 John D. Haughton
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#------------------------------------------------------------------------------

import math
import sys
import table

def phase_delta(octave, x):
    """ Convert Q4.12 octave value to Q0.32 phase increment """

    SAMPLE_FREQ       = 48000    # Hz
    A4_FREQ           = 440      # Hz
    A4_OCTAVE         = 0xA000   # Q4.12 representation of A4
    OCTAVE_BIN_PLACES = 12
    PHASE_BIN_PLACES  = 32

    frequency = math.pow(2.0, (octave - A4_OCTAVE) / (1<<OCTAVE_BIN_PLACES)) * A4_FREQ

    delta = frequency / SAMPLE_FREQ

    return int(delta * (1<<PHASE_BIN_PLACES) + 0.5)


# octave to phase delta table
table.gen('phase_delta',
          func       = phase_delta,
          typename  = "uint32_t",
          log2_size  = 16,
          fmt        = '08x')

table.gen('log_sine',
          func      = lambda i,x : int(math.sin(x * 2 * math.pi) * 0x7FFF + 0.5),
          log2_size = 16,
          typename  = "int16_t",
          fmt       = '6d')

table.gen('exp',
          func      = lambda i,x : (int(math.pow(2.0, i / 127) * 2048 + 0.5) << (i >> 10)) >> 13,
          typename  = "uint16_t",
          log2_size = 7,
          fmt       = '04x')

