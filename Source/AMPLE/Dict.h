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

#include <array>
#include <cstdio>

#include "STB/Heap.h"
#include "STB/BitArray.h"

namespace AMPLE {

template <typename TYPE, size_t NUM_STATES>
class Dict
{
   template <typename VALUE, typename SYMBOL, SYMBOL LOWEST, SYMBOL HIGHEST>
   class DFAState : public STB::List<DFAState<VALUE,SYMBOL,LOWEST,HIGHEST>>::Elem
   {
   public:
      DFAState() = default;

      DFAState(size_t index_)
         : index(index_)
      {
      }

      //! State has no exit transitions
      bool is_final() const { return num_trans == 0; }

      //! State can terminate a sequence
      bool is_terminal() const { return terminal; }

      //! State is the destination of more than one transition
      bool is_shared() const { return shared; }

      //! Value of state when it is terminal
      const VALUE& get_value() const { return value; }

      //! Clear state to initial condition
      void clear()
      {
         set_non_terminal();
         link(LOWEST, HIGHEST, nullptr);
      }

      //! Set the state as a terminal state with a value
      void set(const VALUE& value_)
      {
         terminal = true;
         value    = value_;
      }

      void set_non_terminal()
      {
         terminal = false;
      }

      //! Add a tranition for a range of characters
      void link(SYMBOL lo_, SYMBOL hi_, DFAState* next_)
      {
         for(SYMBOL symbol = lo_; symbol <= hi_; symbol++)
         {
            link(symbol, next_);
         }
      }

      //! Add a tranition for a single character
      void link(SYMBOL symbol_, DFAState* next_)
      {
         if (valid(symbol_))
         {
            size_t index = symbol_ - LOWEST;

            if (trans[index] != nullptr)
               --num_trans;

            trans[index] = next_;

            if (next_ != nullptr)
               ++num_trans;
         }
      }

      //! Get next state for a symbol
      DFAState* follow(SYMBOL symbol_)
      {
         if (not valid(symbol_))
            return nullptr;

         return trans[symbol_ - LOWEST];
      }

      void print_prep() const
      {
         if (not visited)
            return;

         visited = false;

         for(size_t i = 0; i < NUM_CHARS; ++i)
         {
            if (trans[i] != nullptr)
            {
               trans[i]->print_prep();
            }
         }
      }

      void print() const
      {
         visited = true;

         printf("%c%zu\n", terminal ? 'T' : 'S', index);

         for(size_t i = 0; i < NUM_CHARS; ++i)
         {
            if (trans[i] != nullptr)
            {
               printf("'%c' -> %c%zu\n",
                      SYMBOL(LOWEST + i),
                      trans[i]->terminal ? 'T' : 'S',
                      trans[i]->index);
            }
         }
         printf("\n");

         for(size_t i = 0; i < NUM_CHARS; ++i)
         {
            if ((trans[i] != nullptr) && (not trans[i]->visited))
            {
               trans[i]->print();
            }
         }
      }

   private:
      static bool valid(uint8_t ch_) { return (ch_ >= LOWEST) && (ch_ <= HIGHEST); }

      static const size_t NUM_CHARS = HIGHEST - LOWEST + 1;

      bool         terminal{false};
      bool         shared{false};
      mutable bool visited{false};
      size_t       index{0};
      size_t       num_trans{0};
      VALUE        value{};
      DFAState*    trans[NUM_CHARS] = {};
   };

   using State = DFAState<TYPE,char,' ','~'>;

public:
   Dict() = default;

   //! Get number allocated
   size_t allocated() const { return heap.allocated(); }

   //! Remove all entries from the dictionary
   void clear()
   {
      index = 0;
      root.clear();
      heap.reset();
   }

   //! Add a token to the dictionary
   bool add(const char* token_, const TYPE& value_)
   {
      return add(&root, token_, value_);
   }

   //! Recursive add of a token to the dictionary
   bool add(State* state_, const char* s_, const TYPE& value_)
   {
      State*             next_{nullptr};
      STB::BitArray<128> range{};
      char               ch = *s_++;

      switch(ch)
      {
      case '\0':
         state_->set(value_);
         return true;

      case '[':
         while(*s_ != ']')
         {
            if (*s_ == '\0') return false;
            char lower = *s_++;
            if (*s_ == '-')
            {
               s_++;
               char upper = *s_++;
               if (upper == '\0') return false;
               range.set(lower, upper);
            }
            else
            {
               range.set(lower);
            }
         }
         s_++;
         break;

      case '.':
         range.set(' ', '~');
         break;

      case '+':
         return false;

      case '*':
         return false;

      case '\\':
         ch = *s_++;
         if (ch == '\0') return false;
         break;

      default:
         break;
      }

      if (*s_ == '*')
      {
         next_ = state_;
         s_++;
      }

      if (next_ == nullptr)
      {
         next_ = state_->follow(ch);
         if (next_ == nullptr)
         {
            next_ = heap.alloc(++index);
         }
      }

      if (range.any())
      {
         for(char ch = ' '; ch <= '~'; ++ch)
         {
            if (range[ch])
            {
               state_->link(ch, next_);
               if (*s_ == '+')
                  next_->link(ch, next_);
            }
         }
      }
      else
      {
         state_->link(ch, next_);
         if (*s_ == '+')
            next_->link(ch, next_);
      }

      if (*s_ == '+')
         s_++;

      return add(next_, s_, value_);
   }

   //! Remove a tolen from the dictionary
   const char* lookup(const char* input_, TYPE& value_)
   {
      State*      state   = &root;
      State*      match   = nullptr;
      const char* match_s = input_;

      for(const char* s = input_; *s; ++s)
      {
         State* next = state->follow(*s);

         if (next == nullptr)
         {
            break;
         }
         else if (next->is_terminal())
         {
            match   = next;
            match_s = s + 1;
         }

         state = next;
      }

      if (match != nullptr)
         value_ = match->get_value();

      return match_s;
   }

   //! Remove a token from the dictionary
   bool remove(const char* token_)
   {
      return remove(&root, token_);
   }

   //! Debug print dictionary structure
   void print() const
   {
      root.print_prep();
      root.print();
   }

private:
   //! Recursive fixed pattern removal
   bool remove(State* state_, const char* s_)
   {
      char ch = *s_++;

      // Fail for empty string
      if (ch == '\0') return false;

      State* next = state_->follow(ch);

      // Fail for no match
      if (next == nullptr) return false;

      bool ok{false};

      if (*s_ == '\0')
      {
         // Fail for not a terminal state
         if (next->is_terminal())
         {
            next->set_non_terminal();
            ok = true;
         }
      }
      else
      {
         ok = remove(next, s_);
      }

      if (ok)
      {
         if (next->is_final())
         {
            assert(not state_->is_shared());
            state_->link(ch, nullptr);
            heap.free(state_);
         }
      }

      return ok;
   }

   size_t                      index{0};
   State                       root{0};
   STB::Heap<State,NUM_STATES> heap{};
};

} // namespace AMPLE
