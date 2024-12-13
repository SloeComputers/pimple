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
      allocVoices(1);
   }

   ~Player()
   {
      allocVoices(0);
   }

   Voice* getVoice()
   {
      return current_voice;
   }

   Chan* getChan()
   {
      if (current_voice == nullptr)
         return nullptr;

      return current_voice->getChan();
   }

   bool allocVoices(size_t n_)
   {
      current_voice = nullptr;

      while(not voices.empty())
      {  
         Voice* voice = voices.front();
         voices.pop_front();
         voice_heap.free(voice);
      }  
      
      for(auto& voice : voices)
      {
         voice_heap.free(&voice);
      }

      if (n_ == 0)
         return true;

      if (voice_heap.avail() < n_)
         return false;

      for(size_t i = 0; i < n_; ++i)
      {
         voices.push_back(voice_heap.alloc());
      }

      setVoice(1);

      return true;
   }

   bool allocChans(size_t n_)
   {
      if (current_voice == nullptr)
         return false;

      return current_voice->allocChans(n_);
   }

   bool setVoice(size_t n_)
   {
      size_t index = n_ - 1;

      if (index >= voices.size())
         return false;

      current_voice = voices[index];

      setChan(1);

      return true;
   }

   bool setChan(size_t n_)
   {
      if (current_voice == nullptr)
         return false;

      return current_voice->setChan(n_);
   }

   static const size_t MAX_VOICES = 8;

private:
   Voice*           current_voice{};
   STB::List<Voice> voices;

   static STB::Heap<Voice,MAX_VOICES> voice_heap;
};

} // namespace AMPLE
