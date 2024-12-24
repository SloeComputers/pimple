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

#include "Voice.h"

#include "STB/List.h"
#include "STB/Heap.h"

namespace AMPLE {

struct Player : public STB::List<Player>::Elem
{
public:
   Player()
   {
      reset();
   }

   ~Player()
   {
      voiceFree();
   }

   void reset()
   {
      if (voiceAlloc(1))
         voiceSelect(1);
   }

   //! Allocate voices to this player (AMPLE VOICES)
   bool voiceAlloc(Number n_)
   {
      if ((n_ < 0) || (voice_heap.avail() < n_))
         return false;

      voiceFree();
      
      for(size_t i = 0; i < n_; ++i)
         voices.push_back(voice_heap.alloc());

      return true;
   }

   //! Select voice (AMPLE VOICE)
   bool voiceSelect(Number n_)
   {
      if ((n_ < 0) || (n_ > voices.size()))
         return false;

      current_voice = n_ == 0 ? &dummy_voice
                              : voices[n_ - 1];
      chanSelect(1);

      return true;
   }


   //! Allocate channels in the current voice (AMPLE CHANS)
   bool chanAlloc(Number n_)
   {
      return current_voice->chanAlloc(n_);
   }

   //! Reset all the channels in the current voice (AMPLE SOUND)
   void chanReset()
   {
      current_voice->chanReset();
   }

   //! Select channels in the current voice (AMPLE CHAN)
   bool chanSelect(Number n_)
   {
      return current_voice->chanSelect(n_);
   }

   //! Control channels in the current voice
   bool chanControl(Chan::Param param_, Number value_)
   {
      return current_voice->chanControl(param_, value_);
   }


   void tick()
   {
      for(auto& voice : voices)
         voice.tick();
   }

   void schedule(unsigned n_, uint8_t note_, uint8_t length_)
   {
      if (n_ >= voices.size())
         return;

      voices[n_]->schedule(note_, length_);
   }

   void debug()
   {
      unsigned i = 1;
      for(auto& voice : voices)
      {
         printf("  Player::voices[%u] %s\n", i++, current_voice == &voice ? "<<<" : "");
         voice.debug();
      }
   }

   static const size_t MAX_VOICES = 8;

private:
   //! De-allocate dynamic voices
   void voiceFree()
   {
      while(not voices.empty())
      {  
         Voice* voice = voices.front();
         voices.pop_front();
         voice_heap.free(voice);
      }

      voiceSelect(0);
   }

   Voice            dummy_voice{/* dummy */ true};
   STB::List<Voice> voices;
   Voice*           current_voice{&dummy_voice};

   static STB::Heap<Voice,MAX_VOICES> voice_heap;
};

} // AMPLE
