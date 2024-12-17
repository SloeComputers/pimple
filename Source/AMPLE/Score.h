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

#include "STB/Fifo.h"

#include "Types.h"

namespace AMPLE {

struct Score
{
public:
   Score()
   {
      reset();
   }

   void reset()
   {
      key_sig     = false;
      accidental  = false;
      last_letter = '\0';
      adjust      = 0;
      octave      = 0;
      length      = 48;

      for(unsigned i = 0; i < 7; ++i)
         signature[i] = 0;
   }

   void setOctave(Number value_)
   {
      octave      = value_;
      last_letter = '\0';
   }

   void setLength(Number value_)
   {
      length = value_;
   }

   void note(char letter_)
   {
      bool     up;
      unsigned index = getIndex(letter_, up);

      if (key_sig)
      {
         signature[index] = adjust;
         return;
      }

      if (last_letter != '\0')
      {
         bool     last_up;
         unsigned last_index = getIndex(last_letter, last_up);

         if (up)
         {
            if ((index < last_index) ||
                ((index == last_index) && not last_up))
            {
               ++octave;
            }
         }
         else
         {
            if ((index > last_index) ||
                ((index == last_index) && last_up))
            {
               --octave;
            }
         }
      }

      last_letter = letter_;

      // Convert letter to MIDI note
      const unsigned octave_notes[7] = {0, 2, 4, 5, 7, 9, 11};
      midi_note = 60 + octave * 12 + octave_notes[index];

      // Apply accidenetal or key signature
      if (accidental)
      {
         accidental = false;
      }
      else
      {
         adjust = signature[index];
      }
      midi_note += adjust;
      adjust = 0;

      schedule(midi_note, length);
   }

   void rest()
   {
      schedule(midi_note, length);
   }

   void tie()
   {
      schedule(midi_note, length);
   }

   //! Enter/leave key signature definition
   void keySig(bool start_) { key_sig = start_; }
   
   //! Apply an accidental to the next note
   void acid(signed direction_)
   {
      switch(direction_)
      {
      case -1:
         if (adjust > -4)
            --adjust;
         break;

      case +1:
         if (adjust < +4)
            ++adjust;
         break;

      case 0:
         adjust = 0;
         break;
      }

      accidental = true;
   }

private:
   //! Convert note letter A..G and a..g to a direction and note index 0-7 (C is 0)
   static unsigned getIndex(char ch_, bool& up_)
   {
      if (ch_ >= 'a')
      {
         up_ = false;
         ch_ = ch_ - 'a' + 'A';
      }
      else
      {
         up_ = true;
      }

      if (ch_ >= 'C')
      {
         return ch_ - 'C';
      }

      return 5 + ch_ - 'A';
   }

   void schedule(uint8_t note_, unsigned length_)
   {
      printf("Note=%u length=%u\n", note_, length_);
   }

   bool     key_sig;
   signed   signature[7];
   bool     accidental;
   char     last_letter;
   signed   octave;
   unsigned length;
   signed   adjust;
   uint8_t  midi_note;
};

} // namespace AMPLE
