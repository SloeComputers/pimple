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

#include <cstdio>

#include "STB/Heap.h"

namespace AMPLE {

template <typename TYPE, size_t NUM_NODES>
class Dict
{
public:
   Dict() = default;

   //! Get number allocated
   size_t allocated() const { return heap.allocated(); }

   //! Remove all entries from the dictionary
   void clear()
   {
      root->child = nullptr;
      heap.reset();
   }

   //! Add a token to the dictionary
   bool add(const char* token_, TYPE value_)
   {
      Node* node = &root;
      Node* state{};

      for(const char* s = token_; *s; ++s)
      {
         Node* parent = node;
         Range range;

         if (*s == '\\')
         {
            // \: Escape speical meaning of next char
            if (*++s == '\0') return false;
            range = *s;
         }
         else if (*s == '+')
         {
            if (s[1] == '\0')
            {
               parent->terminal = true;
               parent->value    = value_;
            }

            node          = heap.alloc();
            node->range   = parent->range;
            node->child   = node;
            parent->child = node;
            continue;
         }
         else if (*s == '*')
         {
            node = parent;
            node->child = node;
            continue;
         }
         else if (*s == '[')
         {
            // [l-u]: Range of chars
            if (*++s == '\0') return false;
            range.lower = *s;
            if (*++s != '-')  return false;
            if (*++s == '\0') return false;
            range.upper = *s;
            if (*++s != ']') return false;
         }
         else if (*s == '.')
         {
            // .: Any printable char
            range.lower = ' ';
            range.upper = '~';
         }
         else
         {
            // Exactly match char
            range = *s;
         }

         state = parent->child;

         for(node = state; node != nullptr; node = node->next)
         {
            if (node->match(*s))
               break;
         }

         if (node == nullptr)
         {
            if (heap.empty())
                return false;

            node          = heap.alloc();
            node->range   = range;
            node->next    = state;
            state         = node;
            parent->child = state;
         }

         if ((s[1] == '*') && (s[2] == '\0'))
         {
            parent->terminal = true;
            parent->value    = value_;
         }
      }

      if (node->terminal)
         return false;

      node->terminal = true;
      node->value    = value_;
      return true;
   }

   //! Remove a token from the dictionary
   bool remove(const char* token_)
   {
      return remove(&root, token_);
   }

   //! Remove a tolen from the dictionary
   const char* lookup(const char* input_, TYPE& value_)
   {
      Node* node  = &root;
      Node* match = nullptr;
      const char* match_s{};
      const char* s;

      for(s = input_; *s; ++s)
      {
         Node* parent = node;

         for(node = parent->child; node != nullptr; node = node->next)
         {
            if (node->match(*s))
            {
               if (node->terminal)
               {
                  match   = node;
                  match_s = s;
               }
               break;
            }
         }

         if (node == nullptr)
         {
            break;
         }
      }

      if (match == nullptr)
      {
         return input_;
      }

      value_ = match->value;

      return match_s + 1;
   }

   //! Debug print dictionary structure
   void print() const
   {
      print(root.child);
   }

private:
   struct Range
   {
      Range() = default;

      char operator=(char ch_) { lower = upper = ch_; return ch_; }

      bool match(char ch_) const { return ch_ >= lower && ch_ <= upper; }

      char lower{'\0'};
      char upper{'\0'};
   };

   struct Node
   {
      Node() = default;

      bool match(char ch_) const { return range.match(ch_); }

      Range   range{};
      bool    terminal{false};
      TYPE    value{};
      Node*   child{nullptr};
      Node*   next{nullptr};
   };

   //! Recursive dictionary debug print
   void print(const Node* state) const
   {
      printf("S%zu\n", heap.index(state));

      for(const Node* n = state; n != nullptr; n = n->next)
      {
         if (n->range.lower == n->range.upper)
         {
            printf("  [%c]   %c",
                   n->range.lower,
                   n->terminal ? 'T' : ' ');
         }
         else
         {
            printf("  [%c-%c] %c",
                   n->range.lower, n->range.upper,
                   n->terminal ? 'T' : ' ');
         }

         if (n->child)
         {
            printf(" => S%zu", heap.index(n->child));
         }

         printf("\n");
      }

      for(const Node* n = state; n != nullptr; n = n->next)
      {
         if (n->child && (n != n->child))
         {
            print(n->child);
         }
      }
   }

   //! Recursive pattern removal
   bool remove(Node* parent, const char* token_)
   {
      Node* prev  = nullptr;
      Node* state = parent->child;

      for(Node* node = state; node != nullptr; node = node->next)
      {
         if (node->match(*token_))
         {
            if (token_[1] == '\0')
            {
               if (node->terminal)
               {
                  // Terminal node found
                  node->terminal = false;

                  // Remove node if not used by other patterns
                  if (node->child == nullptr)
                  {
                     if (prev != nullptr)
                        prev->next = node->next;
                     else
                        parent->child = node->next;
                     heap.free(node);
                  }

                  return true;
               }
            }
            else if (node->child)
            {
               if (remove(node, token_ + 1))
               {
                  // Terminal node found in children

                  // Remove node if not used by other patterns
                  if (not node->terminal && (node->child == nullptr))
                  {
                     if (prev != nullptr)
                        prev->next = node->next;
                     else
                        parent->child = node->next;
                     heap.free(node);
                  }

                  return true;
               }
            }
            else
            {
               // XXX broken dictionary
               // A non-terminal match with no children should not occur
               break;
            }
         }

         prev = node;
      }

      return false;
   }

   STB::Heap<Node,NUM_NODES> heap{};
   Node                      root{};
};

} // namespace AMPLE
