#-------------------------------------------------------------------------------
# Copyright (c) 2023 John D. Haughton
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------

import math
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
