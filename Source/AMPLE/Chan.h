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

#include "Table_phase_delta.h"
#include "Table_log_sine.h"
#include "Table_exp.h"

namespace AMPLE {

class Chan : public STB::List<Chan>::Elem
{
public:
   enum Param
   {
      AMP,
      PITCH,
      OFFSET,
      SHIFT,
      WAVE,
      POS,
      RM,
      SYNC,
      FM,
      INVERT,
      GATE,
      AGATE,
      PGATE,
      AENV,
      PENV
   };

   Chan() = default;

   bool getGate() const { return gate; }

   bool isPlaying(uint8_t midi_note_) const { return gate && (midi_note == midi_note_); }

   void reset()
   {
      amp    = 0;
      pitch  = 0;
      offset = 0;
      shift  = 0;
      wave   = 0;
      pos    = 0;
      fm     = 0;
      rm     = false;
      sync   = false;
      invert = false;
      gate   = false;
      agate  = false;
      pgate  = false;
      aenv   = 0;
      penv   = 0;
   }

   //! Channel control
   bool control(Param param_, Number value_)
   {
      switch(param_)
      {
      case AMP:    amp    = std::clamp<Number>(value_,     0,   128); break;
      case PITCH:  pitch  = std::clamp<Number>(value_, -1024, +1023); break;
      case OFFSET: offset = value_; break;
      case SHIFT:  shift  = std::clamp<Number>(value_, -1024, +1023); break;
      case WAVE:   wave   = std::clamp<Number>(value_,     0,    13); break;
      case POS:    pos    = std::clamp<Number>(value_,    -3,    +3); break;
      case FM:     fm     = value_; break;
      case RM:     rm     = value_ != 0; break;
      case SYNC:   sync   = value_ != 0; break;
      case INVERT: invert = value_ != 0; break;
      case GATE:   gate   = value_ != 0; break;
      case AGATE:  agate  = value_ != 0; break;
      case PGATE:  pgate  = value_ != 0; break;
      case AENV:   aenv   = value_; break;
      case PENV:   penv   = value_; break;
      }

      return true;
   }

   void noteOn(uint8_t midi_note_)
   {
      midi_note = midi_note_;
      gate      = true;

      if (sync)
      {
         phase_0_32 = 0;
      }

      updatePitch();
   }

   void noteOff()
   {
      gate = false;
   }

   void pitchBend(int16_t value_)
   {
      pitch_bend_4_12 = value_ >> 1;
   
      if (not gate)
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

   uint8_t midi_note{};

   uint8_t amp{128};    //!< 0..128
   signed  pitch{0};    //!< -1024..+1023  0 => middle C   one octave is 192
   signed  offset{0};   //!< -32768..+32767                (units of 0.0056Hz)
   signed  shift{0};    //!< -1024..+1023
   uint8_t wave{0};     //!< 0..13
   int8_t  pos{0};      //!< -3..+3
   uint8_t fm{0};
   bool    rm{false};
   bool    sync{false};
   bool    invert{false};
   bool    gate{false};
   bool    agate{false};
   bool    pgate{false};
   uint8_t aenv{};
   uint8_t penv{};

   int8_t         transpose{0};
   int32_t        pitch_bend_4_12{0}; //!< Pitch bend offset (semitone)
   uint32_t       octave_4_12{};
   // const uint8_t* wave_ram{};
   uint32_t       delta_0_32{0};  //!< Oscillator phase inc (x2pi) Q0.32
   uint32_t       phase_0_32{0};  //!< Oscillator phase     (x2pi) Q0.32
};

} // namespace AMPLE
