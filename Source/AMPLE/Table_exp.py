#-------------------------------------------------------------------------------
# Copyright (c) 2023 John D. Haughton
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------

import math
import table

table.gen('exp',
          func      = lambda i,x : (int(math.pow(2.0, i / 127) * 2048 + 0.5) << (i >> 10)) >> 13,
          typename  = "uint16_t",
          log2_size = 7,
          fmt       = '04x')

