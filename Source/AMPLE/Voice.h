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

#include "Chan.h"

#include "STB/List.h"
#include "STB/Heap.h"

namespace AMPLE {

struct Voice : public STB::List<Voice>::Elem
{
public:
   Voice()
   {
      allocChans(2);
   }

   ~Voice()
   {
      allocChans(0);
   }

   Chan* getChan()
   {
      return current_chan;
   }

   void reset()
   {
      if (chans.size() == 0)
         return;

      for(auto& chan : chans)
      {
         chan.reset();
      }

      setChan(1);

      getChan()->setAmp(128);
   }

   bool allocChans(size_t n_)
   {
      current_chan = nullptr;

      while(not chans.empty())
      {  
         Chan* chan = chans.front();
         chans.pop_front();
         chan_heap.free(chan);
      }  

      if (n_ == 0)
         return true;

      if (chan_heap.avail() < n_)
         return false;

      for(size_t i = 0; i < n_; ++i)
      {
         chans.push_back(chan_heap.alloc());
      }

      setChan(1);

      return true;
   }

   bool setChan(size_t n_)
   {
      size_t index = n_ - 1;

      if (index >= chans.size())
         return false;

      current_chan = chans[index];

      return true;
   }

   static const size_t MAX_CHANS = 16;

private:
   Chan*           current_chan{};
   STB::List<Chan> chans;

   static STB::Heap<Chan,MAX_CHANS> chan_heap;
};

} // namespace AMPLE
