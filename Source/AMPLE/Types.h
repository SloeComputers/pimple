//-------------------------------------------------------------------------------
// Copyright (c) 2024 John D. Haughton
// SPDX-License-Identifier: MIT
//-------------------------------------------------------------------------------

#pragma once

#include <cstdint>
#include <cstring>

namespace AMPLE {

using Number  = int16_t;
using Address = uint16_t;

// Some AMPLE constants
static const Number OFF     = 0;
static const Number ON      = -1;
static const Number EVEN    = -2;
static const Number ODD     = -3;
static const Number SIMPLEP = 254;
static const Number SIMPLEA = 255;
static const Number SIMPLEW = 0;

}
