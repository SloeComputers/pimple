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

#include "STB/Fifo.h"
#include "AMPLE/Types.h"
#include "AMPLE/Machine.h"

#define DBG if (0) printf

namespace AMPLE {

class Shell
{
public:
   Shell() = default;

   void run(bool echo_)
   {
      printf("AMPLE re-code\n");

      while(true)
      {
         char line[256];

         printf(" %%");

         unsigned i = 0;

         while(true)
         {
            char ch = getchar();
            if (ch == -1)
               return;

            if (echo_)
               putchar(ch);

            if (ch == '\n')
               break;

            line[i++] = ch;
         }

         line[i] = '\0';

         mc.run(line);
      }
   }

   Machine mc{};
};

} // namespace AMPLE
