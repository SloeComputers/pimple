//-------------------------------------------------------------------------------
// Copyright (c) 2024 John D. Haughton
// SPDX-License-Identifier: MIT
//-------------------------------------------------------------------------------

#pragma once

#include "Player.h"
#include "Score.h"
#include "Wave.h"

#include "STB/List.h"
#include "STB/Fifo.h"

namespace AMPLE {

struct Sound
{
public:
   Sound(Score& score_)
      : score(&score_)
   {
      reset();
   }

   void reset()
   {
      static_player.reset();

      // No dynamic players
      playerFree();
   }

   //! Allocate dynamic players
   bool playerAlloc(Number n_)
   {
      if ((n_ < 0) || (signed(player_heap.avail()) < n_))
         return false;

      playerFree();

      for(Number i = 0; i < n_; ++i)
         players.push_back(player_heap.alloc());

      return true;
   }

   //! Select current player
   bool playerSelect(Number n_)
   {
      if ((n_ < 0) || (n_ > signed(players.size())))
         return false;

      current_player = n_ == 0 ? &static_player
                               : players[n_ - 1];

      score->setPlayer(current_player);

      voiceSelect(1);

      return true;
   }

   //! Allocate voices in the current player
   bool voiceAlloc(Number n_)
   {
      return current_player->voiceAlloc(n_);
   }

   //! Select voice in the current player
   bool voiceSelect(Number n_)
   {
      return current_player->voiceSelect(n_);
   }

   //! Allocate channels in the current voice
   bool chanAlloc(Number n_)
   {
      return current_player->chanAlloc(n_);
   }

   //! Reset all the channels in the current voice
   void chanReset()
   {
      current_player->chanReset();
   }

   //! Select channels in the current voice
   bool chanSelect(Number n_)
   {
      return current_player->chanSelect(n_);
   }

   //! Control channels in the current voice
   bool chanControl(Chan::Param param_, Number value_)
   {
      return current_player->chanControl(param_, value_);
   }


   void tick()
   {
      static_player.tick();

      for(auto& player : players)
         player.tick();
   }

   void debug()
   {
      printf("Sound::static_player %s\n", &static_player == current_player ? "<<<" : "");
      static_player.debug();

      unsigned i = 1;
      for(auto& player : players)
      {
         printf("Sound::players[%u] %s\n", i++, &player == current_player ? "<<<" : "");
         player.debug();
      }
   }

   static const size_t MAX_PLAYERS = 8;

   Wave wave{};

private:
   void playerFree()
   {
      while(not players.empty())
      {
         Player* player = players.front();
         players.pop_front();
         player_heap.free(player);
      }

      playerSelect(0);
   }

   Score*            score{};
   Player*           current_player{};
   Player            static_player{};
   STB::List<Player> players{};

   static STB::Heap<Player,MAX_PLAYERS> player_heap;
};

} // namespace AMPLE
