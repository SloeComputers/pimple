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

#include <algorithm>

#include "STB/Stack.h"

#include "Types.h"

namespace AMPLE {

class Machine
{
public:
   Machine() = default;

   void word(const char* word)
   {
      if (word[0] == '#')
      {
         ++word;

         Number  one, two, three;
         Address addr;

              if (eq(word, "!"))    { write(pop(), pop()); }
         else if (eq(word, "*"))    { push(pop() * pop()); }
         else if (eq(word, "+"))    { push(pop() + pop()); }
         else if (eq(word, "+!"))   { addr = pop(); write(addr, read(addr) + pop()); }
         else if (eq(word, "-"))    { push(pop() - pop()); }
         else if (eq(word, "/"))    { push(pop() / pop()); }
         else if (eq(word, "11"))   { push(peek()); }
         else if (eq(word, "12"))   { one = pop(); two = pop(); push(one); push(two); }
         else if (eq(word, "2"))    { (void) pop(); }
         else if (eq(word, "212"))  { push(peek(-2)); }
         else if (eq(word, "2121")) { push(peek(-2)); push(peek(-2)); }
         else if (eq(word, "213"))  { one = pop(); two = pop(); three = pop(); push(two); push(one); push(three); }
         else if (eq(word, "<"))    { push(pop() <  pop() ? -1 : 0); }
         else if (eq(word, "="))    { push(pop() == pop() ? -1 : 0); }
         else if (eq(word, ">"))    { push(pop() >  pop() ? -1 : 0); }
         else if (eq(word, "?"))    { push(read(pop())); }
         else if (eq(word, "IN"))   { push(getchar()); }
      }
      else if (word[0] == '$')
      {
         ++word;

              if (eq(word, "+"))    {}
         else if (eq(word, "-"))    {}
         else if (eq(word, "12"))   {}
         else if (eq(word, "2"))    {}
         else if (eq(word, "IN"))   {}
      }
      else if (eq(word, "MAX"))   { push(std::max(pop(), pop())); }
      else if (eq(word, "MAX"))   { push(std::min(pop(), pop())); }
      else if (eq(word, "NL"))    { printf("\n"); }
      else if (eq(word, "NOUT"))  { printf("%d\n", pop()); }
      else if (eq(word, "&NOUT")) { printf("%X\n", pop()); }
      else if (eq(word, "NOT"))   { push(pop() ? 0 : -1); }
      else if (eq(word, "OFF"))   { push(0); }
      else if (eq(word, "ON"))    { push(-1); }
      else if (eq(word, "OR"))    { push(pop() | pop()); }
      else if (eq(word, "OUT"))   { printf("%c\n", pop()); }
      else if (eq(word, "XOR"))   { push(pop() ^ pop()); }
   }

   void write(Address address_, Number value_)
   {
      if (address_ >= MEMORY_SIZE)
         return;

      memory[address_] = value_;
   }

   Number read(Address address_)
   {
      if (address_ >= MEMORY_SIZE)
         return 0;

      return memory[address_];
   }

   void push(Number value_)
   {
      stk_num.push_back(value_);
   }

   Number pop()
   {
      Number value = stk_num.back();
      stk_num.pop_back();
      return value;
   }

   Number peek(size_t index = -1)
   {
      return stk_num[stk_num.size() - 1];
   }

   void pushStr(const char* string_)
   {
   }

private:
   static const unsigned NUMBER_STACK_SIZE = 16;
   static const Address  MEMORY_SIZE       = 9184;

   STB::Stack<Number,NUMBER_STACK_SIZE> stk_num{};
   Number                               memory[MEMORY_SIZE];
};

} // namespace AMPLE
