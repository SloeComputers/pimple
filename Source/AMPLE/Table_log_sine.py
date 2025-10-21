#-------------------------------------------------------------------------------
# Copyright (c) 2023 John D. Haughton
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------

import math
import table

table.gen('log_sine',
          func      = lambda i,x : int(math.sin(x * 2 * math.pi) * 0x7FFF + 0.5),
          log2_size = 16,
          typename  = "int16_t",
          fmt       = '6d')
