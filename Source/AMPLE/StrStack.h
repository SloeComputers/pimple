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

namespace AMPLE {

template <size_t SIZE>
class StrStack
{
public:
   StrStack() = default;

   StrStack(const char* s_)
   {
      push(s_);
   }

   bool empty() const { return top == 0; }

   size_t size() const
   {
      if (empty())
         return 0;

      return buffer[top - 1];
   }

   void clear()
   {
      top = 0;
   }

   void push(const char* start_, const char* end_ = nullptr)
   {
      size_t len = 0;

      for(const char* s = start_; s != end_; ++s)
      {
         if ((*s == '\0') || (len == 255) || (top == (SIZE - 2)))
            break;

         buffer[top++] = *s;
         len++;
      }

      buffer[top++] = '\0';
      buffer[top++] = len;
   }

   void add(const char* text_)
   {
      if (empty())
      {
         push(text_);
         return;
      }

      uint8_t len = buffer[--top];
      --top;

      for(const char* s = text_; *s != '\0'; ++s)
      {
         if ((len == 255) || (top == (SIZE - 2)))
            break;

         buffer[top++] = *s;
         len++;
      }

      buffer[top++] = '\0';
      buffer[top++] = len;
   }

   const char* peek() const
   {
      if (empty())
         return nullptr;

      uint8_t len = buffer[top - 1];

      return (const char*)(buffer + top - len - 2);
   }

   const char* pop()
   {
      const char* s = peek();
      if (s != nullptr)
      {
         top = (uint8_t*)s - buffer;
      }

      return s;
   }

private:
   size_t  top{0};
   uint8_t buffer[SIZE];
};

} // namespace AMPLE
