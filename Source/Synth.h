//-------------------------------------------------------------------------------
// Copyright (c) 2024 John D. Haughton
// SPDX-License-Identifier: MIT
//-------------------------------------------------------------------------------

#pragma once

#include "STB/MIDIInstrument.h"

template <unsigned N, typename VOICE, unsigned AMP_N = N>
class Synth : public MIDI::Instrument
{
public:
   Synth(VOICE* voice_)
      : MIDI::Instrument(/* num_coices */ N)
      , voice(voice_)
   {
   }

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
   void voiceOn(unsigned index_, uint8_t note_, uint8_t velocity_) override
   {
      voice[index_].noteOn(note_);
   }

   void voiceOff(unsigned index_, uint8_t velocity_) override
   {
      voice[index_].noteOff();
   }

   void voicePitchBend(unsigned index_, int16_t value_) override
   {
      voice[index_].pitchBend(value_);
   }

   VOICE* voice;
};

