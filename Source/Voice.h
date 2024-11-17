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

#include "Table_phase_delta.h"
#include "Table_log_sine.h"
#include "Table_exp.h"

template <unsigned SAMPLE_RATE>
class Voice
{
public:
   Voice() = default;

   bool isActive() const { return active; }

   bool isPlaying(uint8_t midi_note_) const { return active && (midi_note == midi_note_); }

   void noteOn(uint8_t midi_note_)
   {
      midi_note  = midi_note_;
      active     = true;
      phase_0_32 = 0;

      updatePitch();
   }

   void noteOff()
   {
      active = false;
   }

   void pitchBend(int16_t value_)
   {
      pitch_bend_4_12 = value_ >> 1;

      if (not active)
         return;

      updateFreq();
   }

   //! Return next sample for this voice
   int32_t sample()
   {
      phase_0_32 += delta_0_32;

      int16_t sample16 = table_log_sine[phase_0_32 >> 16];

      return sample16;
   }

   void tick()
   {
   }

private:
   //! Update voice pitch from current MIDI note value
   void updatePitch()
   {
      // Convert MIDI note to fixed-point (Q4.12) octave value
      // where A4 is 0xA.000 
      const unsigned A4_MIDI   = 69;
      const unsigned A4_OCTAVE = 0xA000;

      // A3 => -12       A4 => 0         A5 => +12
      signed midi_rel_A4 = midi_note - A4_MIDI + transpose;

      // A3 => 0x9.000   A4 => 0xA.000   A5 => 0xB.000    (Q4.12)
      octave_4_12 = midi_rel_A4 * 0x1000 / 12 + A4_OCTAVE;

      //printf("octave = 0x%x.%03x\n", octave_4_12 >> 12, octave_4_12 & 0xFFF);

      updateFreq();
   }

   //! Update voice frequency from current octave value
   void updateFreq()
   {
      // Convert fixed-point (Q4.12) octave value to fixed-point
      // phase increment (Q0.32)

      delta_0_32 = table_phase_delta[octave_4_12 + pitch_bend_4_12];

      //printf("delta  = 0x.%08x\n", delta_0_32);
   }

   bool     active{false};
   uint8_t  midi_note{};        //!< Current note MIDI value (semitone)
   int8_t   transpose{0};       //!< Transpose offset (semitone)
   int32_t  pitch_bend_4_12{0}; //!< Pitch bend offset (semitone)
   uint32_t octave_4_12{};      //!< Current note octave value (Q4.12 A4 is 0xA.000)

   const uint8_t*  wave_ram{};
   uint32_t        delta_0_32{0};  //!< Oscillator phase inc (x2pi) Q0.32
   uint32_t        phase_0_32{0};  //!< Oscillator phase     (x2pi) Q0.32
   uint8_t         wave4{0};
   uint8_t         ampl7{};
};

