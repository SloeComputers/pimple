//-------------------------------------------------------------------------------
// Copyright (c) 2024 John D. Haughton
// SPDX-License-Identifier: MIT
//-------------------------------------------------------------------------------

#pragma once

#include <array>
#include <cstdio>

#include "STB/Heap.h"
#include "STB/List.h"
#include "STB/BitArray.h"

namespace AMPLE {

template <typename TYPE,
          size_t   NUM_STATES,
          size_t   NUM_TRANS = NUM_STATES>
class Dict
{
   template <typename VALUE>
   class DFAState
   {
   public:
      class Trans : public STB::List<Trans>::Elem
      {
      public:
         Trans(char lower_, char upper_, DFAState* state_)
            : lower(lower_)
            , upper(upper_)
            , state(state_)
         {
         }

         DFAState* get() const { return state; }

         bool uniqueMatch(char symbol_) const
         {
            return (symbol_ == lower) && (symbol_ == upper);
         }

         DFAState* lookup(char symbol_) const
         {
            return (symbol_ >= lower) && (symbol_ <= upper) ? state : nullptr;
         }

         template <typename HEAP>
         void print(const HEAP& heap) const
         {
            printf("[%c", lower);

            if (lower != upper)
            {
               printf("-%c]", upper);
            }
            else
            {
               printf("]  ");
            }

            state->printName(heap, " -> ", "\n");
         }

      private:
         const char lower{};
         const char upper{};
         DFAState*  state{};
      };

      DFAState() = default;

      //! State has no exit transitions
      bool is_final() const { return trans_list.empty(); }

      //! State can terminate a sequence
      bool is_terminal() const { return terminal; }

      //! State is the destination of more than one transition
      bool is_shared() const { return shared; }

      //! Value of state when it is terminal
      const VALUE& get_value() const { return value; }

      //! Clear state to initial condition
      template <typename HEAP>
      void clear(HEAP& heap_)
      {
         set_non_terminal();

         while(not trans_list.empty())
         {
            Trans* trans = trans_list.front();
            trans_list.pop_front();
            heap_.free(trans);
         }
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

      //! Add a transition for a range of characters
      template <typename HEAP>
      void link(HEAP& heap_, char lo_, char hi_, DFAState* next_)
      {
         Trans* trans = heap_.alloc(lo_, hi_, next_);
         trans_list.push_front(trans);
      }

      //! Add a transition for a single character
      template <typename HEAP>
      void link(HEAP& heap_, char symbol_, DFAState* next_)
      {
         link(heap_, symbol_, symbol_, next_);
      }

      //! Remove a transition for a single character
      template <typename HEAP>
      void unlink(HEAP& heap_, char symbol_)
      {
         for(auto& trans : trans_list)
         {
            if (trans.uniqueMatch(symbol_))
            {
               trans_list.remove(&trans);
               heap_.free(&trans);
               break;
            }
         }
      }

      //! Get next state for a symbol
      DFAState* follow(char symbol_) const
      {
         for(const auto& trans : trans_list)
         {
            DFAState* state = trans.lookup(symbol_);
            if (state)
               return state;
         }

         return nullptr;
      }

      void clearVisited() const
      {
         if (not visited)
            return;
         visited = false;

         for(const auto& trans : trans_list)
            trans.get()->clearVisited();
      }

      template <typename HEAP>
      void printName(HEAP& heap, const char* prefix_, const char* suffix_) const
      {
         size_t index = heap.index(this);

         printf("%s%c%zu%s", prefix_, terminal ? 'T' : 'S', index, suffix_);
      }

      template <typename HEAP>
      void print(const HEAP& heap) const
      {
         if (visited)
            return;
         visited = true;

         printName(heap, "", ":\n");

         for(const auto& trans : trans_list)
            trans.print(heap);

         for(const auto& trans : trans_list)
            trans.get()->print(heap);
      }

   private:
      bool             terminal{false};
      bool             shared{false};
      mutable bool     visited{false};
      VALUE            value{};
      STB::List<Trans> trans_list{};
   };

   using State = DFAState<TYPE>;
   using Trans = typename State::Trans;

public:
   Dict()
   {
      clear();
   }

   //! Get number allocated
   size_t allocatedS() const { return state_heap.allocated(); }
   size_t allocatedT() const { return trans_heap.allocated(); }

   //! Remove all entries from the dictionary
   void clear()
   {
      state_heap.reset();
      trans_heap.reset();

      root = state_heap.alloc();
   }

   //! Add a token to the dictionary
   bool add(const char* token_, const TYPE& value_)
   {
      return add(root, token_, value_);
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

      if (range.any())
      {
         if (*s_ == '*')
            next_ = state_;
         else
            next_ = state_heap.alloc();

         for(char ch = ' '; ch <= '~'; ++ch)
         {
            if (range[ch])
            {
               char lower = ch;
               while((++ch <= '~') && range[ch]);
               char upper = --ch;

               state_->link(trans_heap, lower, upper, next_);

               if (*s_ == '+')
                  next_->link(trans_heap, lower, upper, next_);
            }
         }
      }
      else
      {
         if (*s_ == '*')
         {
            next_ = state_;
            next_->link(trans_heap, ch, next_);
         }
         else
         {
            next_ = state_->follow(ch);
            if (next_ == nullptr)
            {
               next_ = state_heap.alloc();
               state_->link(trans_heap, ch, next_);
            }

            if (*s_ == '+')
               next_->link(trans_heap, ch, next_);
         }
      }

      if ((*s_ == '+') || (*s_ == '*'))
         s_++;

      return add(next_, s_, value_);
   }

   //! Remove a tolen from the dictionary
   const char* lookup(const char* input_, TYPE& value_)
   {
      State*      state   = root;
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
      return remove(root, token_);
   }

   //! Debug print dictionary structure
   void print() const
   {
      root->clearVisited();
      root->print(state_heap);
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
            state_->unlink(trans_heap, ch);
            state_heap.free(state_);
         }
      }

      return ok;
   }

   State*                      root{};
   STB::Heap<State,NUM_STATES> state_heap{};
   STB::Heap<Trans,NUM_TRANS>  trans_heap{};
};

} // namespace AMPLE
