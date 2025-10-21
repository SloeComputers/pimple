//-------------------------------------------------------------------------------
// Copyright (c) 2024 John D. Haughton
// SPDX-License-Identifier: MIT
//-------------------------------------------------------------------------------

#include "Sound.h"

namespace AMPLE {

STB::Heap<Player,Sound::MAX_PLAYERS> Sound::player_heap{};
STB::Heap<Voice,Player::MAX_VOICES>  Player::voice_heap{};
STB::Heap<Chan,Voice::MAX_CHANS>     Voice::chan_heap{};

} // namespace AMPLE
