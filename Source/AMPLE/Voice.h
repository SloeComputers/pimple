//-------------------------------------------------------------------------------
// Copyright (c) 2024 John D. Haughton
// SPDX-License-Identifier: MIT
//-------------------------------------------------------------------------------

#pragma once

#include <cstdio>

#include "Chan.h"

#include "STB/List.h"
#include "STB/Heap.h"
#include "STB/Fifo.h"

namespace AMPLE {

struct Voice : public STB::List<Voice>::Elem
{
public:
   Voice(bool dummy_ = false)
      : dummy(dummy_)
   {
      if (chanAlloc(2))
         chanSelect(1);
   }

   ~Voice()
   {
      chanFree();
   }

   //! Reset all the channels (AMPLE SOUND)
   void chanReset()
   {
      for(auto& chan : chans)
         chan.reset();

      if (chans.size() != 0)
      {
         chanSelect(1);
         chanControl(Chan::AMP, 128);
      }
   }

   //! Allocate channels (AMPLS CHANS)
   bool chanAlloc(Number n_)
   {
      if (dummy)
         return false;

      if ((n_ % 2) == 1)
         return false;

      if ((n_ < 0) || (signed(chan_heap.avail()) < n_))
         return false;

      chanFree();

      for(Number i = 0; i < n_; ++i)
         chans.push_back(chan_heap.alloc());

      chanSelect(ON);

      return true;
   }

   //! Select the specified channel(s) (AMPLE CHAN)
   bool chanSelect(Number n_)
   {
      uint16_t all_chans = (1 << chans.size()) - 1;

      switch(n_)
      {
      case ON:   chan_mask = all_chans;          return true;
      case OFF:  chan_mask = 0;                  return true;
      case ODD:  chan_mask = 0x5555 & all_chans; return true;
      case EVEN: chan_mask = 0xAAAA & all_chans; return true;

      default:
         if ((n_ >= 1) && (n_ <- signed(chans.size())))
         {
            chan_mask = 1 << (n_ - 1);
            return true;
         }
         else if (n_ >= 65)
         {
            unsigned pair = (n_ - 64) / 2;

            if ((pair * 2) < chans.size())
            {
               chan_mask = 0b11 << (pair * 2);
               return true;
            }
         }
         break;
      }

      return false;
   }

   //! Control the selected channels
   bool chanControl(Chan::Param param_, Number value_)
   {
      uint16_t mask = 1;
      for(auto& chan : chans)
      {
         if (chan_mask & mask)
         {
            if (not chan.control(param_, value_))
               return false;
         }
         mask <<= 1;
      }
      return true;
   }

   void schedule(uint8_t note_, uint8_t length_)
   {
      queue.push(Event(note_, length_));
   }

   void tick()
   {
      if (queue.empty())
         return;

      Event& event = queue.back();

      if (not playing)
      {
         playing = true;

         if (event.note == 0)
            noteOff();
         else
            noteOn(event.note);
      }

      event.length -= 1;
      if (event.length == 0)
      {
         queue.pop();

         if (queue.empty())
         {
            playing = false;
         }
         else
         {
            noteOff();
            noteOn(queue.back().note);
         }
      }
   }

   static Chan* getChannels()
   {
      return chan_heap.data();
   }

   void debug()
   {
      unsigned i = 1;
      for(const auto& chan : chans)
      {
         (void) chan;
         printf("    Voice::chans[%u] %s\n", i, (chan_mask & (1<<(i-1))) ? "<<<" : "");
         i++;
      }
   }

   static const size_t MAX_CHANS = 16;

private:
   struct Event
   {
      Event() = default;

      Event(uint8_t note_, uint8_t length_)
         : note(note_)
         , length(length_)
      {
      }

      uint8_t note{0};
      uint8_t length{0};
   };

   //! Free all the channels
   void chanFree()
   {
      while(not chans.empty())
      {  
         Chan* chan = chans.front();
         chans.pop_front();
         chan_heap.free(chan);
      }  

      chanSelect(OFF);
   }

   void noteOn(uint8_t note_)
   {
      for(auto& chan : chans)
         chan.noteOn(note_);
   }

   void noteOff()
   {
      for(auto& chan : chans)
         chan.noteOff();
   }

   bool               dummy{false};
   uint16_t           chan_mask{0};
   STB::List<Chan>    chans;
   bool               playing{false};
   STB::Fifo<Event,8> queue;

   static STB::Heap<Chan,MAX_CHANS> chan_heap;
};

} // namespace AMPLE
