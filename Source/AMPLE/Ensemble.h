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

#include "Player.h"

#include "STB/List.h"

namespace AMPLE {

struct Ensemble
{
public:
   Ensemble()
   {
      allocPlayers(0);
   }

   ~Ensemble()
   {
      allocPlayers(0);
   }

   Chan* getChan()
   {
      return current_player->getChan();
   }

   Voice* getVoice()
   {
      return current_player->getVoice();
   }

   bool allocPlayers(size_t n_)
   {
      current_player = &static_player;

      while(not players.empty())
      {
         Player* player = players.front();
         players.pop_front();
         player_heap.free(player);
      }

      if (n_ == 0)
         return true;

      if (player_heap.avail() < n_)
         return false;

      for(size_t i = 0; i < n_; ++i)
      {
         players.push_back(player_heap.alloc());
      }

      return true;
   }

   bool allocVoices(size_t n_)
   {
      return current_player->allocVoices(n_);
   }

   bool allocChans(size_t n_)
   {
      return current_player->allocChans(n_);
   }

   bool setPlayer(size_t n_)
   {
      size_t index = n_ - 1;

      if (index >= players.size())
         return false;

      current_player = players[index];

      setPlayer(0);

      return true;
   }

   bool setVoice(size_t n_)
   {
      return current_player->setVoice(n_);
   }

   bool setChan(size_t n_)
   {
      return current_player->setChan(n_);
   }

   static const size_t MAX_PLAYERS = 8;

private:
   Player*           current_player{};
   Player            static_player{};
   STB::List<Player> players{};

   static STB::Heap<Player,MAX_PLAYERS> player_heap;
};

} // namespace AMPLE
