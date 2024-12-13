//------------------------------------------------------------------------------
// Copyright (c) 2024 John D. Haughton
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//------------------------------------------------------------------------------

#pragma once

#include <algorithm>
#include <cstdint>

#include "STB/List.h"

#include "Types.h"

namespace AMPLE {

class Chan : public STB::List<Chan>::Elem
{
public:
   Chan() = default;

   void reset()
   {
      pitch  = 0;
      offset = 0;
      shift  = 0;
      pos    = 0;
      wave   = 0;
      invert = false;
      rm     = false;
      sync   = false;
      fm     = false;
      gate   = false;
   }

   void setPitch(Number value_)  { pitch  = std::clamp<Number>(value_, -1024, +1023); }
   void setOffset(Number value_) { offset = std::clamp<Number>(value_, -1024, +1023); }
   void setShift(Number value_)  { shift  = std::clamp<Number>(value_, -1024, +1023); }
   void setWave(Number value_)   { wave   = std::clamp<Number>(value_,     0,    13); }
   void setPos(Number value_)    { pos    = std::clamp<Number>(value_,    -3,    +3); }
   void setAmp(Number value_)    { amp    = std::clamp<Number>(value_,     0,   128); }

   void setRM(Number value_)     { rm     = value_ != 0; }
   void setSync(Number value_)   { sync   = value_ != 0; }
   void setFM(Number value_)     { fm     = value_ != 0; }
   void setInvert(Number value_) { invert = value_ != 0; }
   void setGate(Number value_)   { gate   = value_ != 0; }

private:
   signed  pitch{0};    //!< -1024..+1023  0 => middle C   one octave is 192
   signed  offset{0};   //!< -1024..+1023                  one octave is 192
   signed  shift{0};    //!< -1024..+1023
   uint8_t wave{0};     //!< 0..13
   int8_t  pos{0};      //!< -3..+3
   uint8_t amp{128};    //!< 0..128
   bool    rm{false};
   bool    sync{false};
   bool    fm{false};
   bool    invert{false};
   bool    gate{false};
};

} // namespace AMPLE
