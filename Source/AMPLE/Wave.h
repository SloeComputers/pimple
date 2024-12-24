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

#include <cstdio>
#include <cstring>

#include "Table_log_sine.h"

#include "Types.h"

namespace AMPLE {

struct Wave
{
public:
   Wave()
   {
      reset();
   }

   void reset()
   {
   }
 
   void waveMod(unsigned index_)
   {
      wmod = index_;
   }

   //! Zero harmonic and gemotric waveform
   void clear()
   {
      for(unsigned i = 0; i < NUM_HARMONICS; ++i)
      {
         harm[i] = 0;
      }

      for(unsigned i = 0; i < SAMPLES_PER_TABLE; ++i)
      {
         geom[i] = 0;
      }
   }

   //! Write harmonic
   void harmWrite(unsigned index_, unsigned amp_)
   {
      if (index_ >= NUM_HARMONICS)
         return;

      harm[index_] = amp_;
   }

   //! Convert harmonic waveform to geometric
   void harmConvertToGeom()
   {
      for(unsigned h = 1; h <= NUM_HARMONICS; ++h)
      {
         for(unsigned j = 0; j < SAMPLES_PER_TABLE; ++j)
         {
            geom[j] += table_log_sine[(j * h) & SAMPLE_MASK];
         }
      }
   }

   //! Read geometric waveform
   int8_t geomRead(unsigned index) const { return geom[index] >> 8; }

   //! Write geometric waveform
   void geomWrite(unsigned index, int8_t value)
   {
      geom[index] = value << 8;
   }

   //! Copy geometric waveform to 
   void geomCopy()
   {
      memcpy(&table[wmod], &geom, sizeof(Table));
   }

private:
   static const unsigned SAMPLES_PER_TABLE = 128;
   static const unsigned SAMPLE_MASK       = SAMPLES_PER_TABLE - 1;
   static const unsigned NUM_WAVES         = 14;
   static const unsigned NUM_HARMONICS     = 16;

   using Table= int16_t[SAMPLES_PER_TABLE];

   unsigned harm[NUM_HARMONICS];
   unsigned wmod{0};
   Table    geom;
   Table    table[14];
};

} // namespace AMPLE
