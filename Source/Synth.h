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

#include "STB/MIDIInstrument.h"

template <unsigned N, typename VOICE, unsigned AMP_N = N>
class Synth : public MIDI::Instrument
{
public:
   Synth(VOICE* voice_)
      : MIDI::Instrument(N)
      , voice(voice_)
   {
   }

   bool isAnyVoiceOn() const { return active != 0; }

   //! Get next sample
   int32_t getSample(unsigned first_voice_= 0,
                     unsigned last_voice_ = N)
   {
      int32_t mix {0};

      for(unsigned i = first_voice_; i < last_voice_; ++i)
      {
         if (voice[i].getGate())
            mix += voice[i].sample();
      }

      return mix / AMP_N;
   }

   //! Control tick
   void tick(unsigned first_voice_ = 0,
             unsigned last_voice_  = N)
   {
      for(unsigned i = first_voice_; i < last_voice_; ++i)
      {
         if (voice[i].getGate())
            voice[i].tick();
      }
   }

private:
   signed allocVoice() const override
   {
      for(unsigned i = 0; i < N; ++i)
      {
         if (not voice[i].getGate()) return i;
      }
      return 0;
   }

   signed findVoice(uint8_t note_) const override
   {
      for(unsigned i = 0; i < N; ++i)
      {
         if (voice[i].isPlaying(note_)) return i;
      }
      return -1;
   }

   void voiceOn(unsigned index_, uint8_t note_, uint8_t velocity_) override
   {
      voice[index_].noteOn(note_);

      ++active;
   }

   void voiceOff(unsigned index_, uint8_t velocity_) override
   {
      voice[index_].noteOff();

      --active;
   }

   void voicePitchBend(unsigned index_, int16_t value_) override
   {
      voice[index_].pitchBend(value_);
   }

   VOICE*   voice;
   unsigned active{0};
};
