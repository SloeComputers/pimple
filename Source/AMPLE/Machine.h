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
#include "Dict.h"
#include "StrStack.h"

namespace AMPLE {

class Machine
{
public:
   Machine()
   {
      builtin.add("\n",      &Machine::wordNOTHING);
      builtin.add(" +",      &Machine::wordNOTHING);
      builtin.add("\".*\"",  &Machine::wordSTRING);
      builtin.add("#!",      &Machine::wordSTORE);
      builtin.add("#\\*",    &Machine::wordNMUL);
      builtin.add("#\\+",    &Machine::wordNADD);
      builtin.add("#\\+!",   &Machine::wordMADD);
      builtin.add("#-",      &Machine::wordNSUB);
      builtin.add("#/",      &Machine::wordNDIV);
      builtin.add("#11",     &Machine::wordNDUP);
      builtin.add("#12",     &Machine::wordNSWAP);
      builtin.add("#2",      &Machine::wordNDROP);
      builtin.add("#212",    &Machine::wordNDUPPREV);
      builtin.add("#2121",   &Machine::wordNDUP2);
      builtin.add("#213",    &Machine::wordNROT);
      builtin.add("#<",      &Machine::wordNLT);
      builtin.add("#=",      &Machine::wordNEQ);
      builtin.add("#>",      &Machine::wordNGT);
      builtin.add("#?",      &Machine::wordNFETCH);
      builtin.add("%.*\n",   &Machine::wordNOTHING);
      builtin.add("&[A-F]+", &Machine::wordHEXNUMBER);
      builtin.add("[0-9]+",  &Machine::wordNUMBER);
      builtin.add("-[0-9]+", &Machine::wordNUMBER);

      builtin.add("$\\+",    &Machine::wordSADD);
      builtin.add("$\\-",    &Machine::wordSPLIT);
      builtin.add("$12",     &Machine::wordSSWAP);
      builtin.add("$2",      &Machine::wordSDROP);
      builtin.add("%.*\n",   &Machine::wordNOTHING);
      builtin.add("&[A-F]+", &Machine::wordHEXNUMBER);
      builtin.add("'",       &Machine::wordBUILTIN);
      builtin.add("(",       &Machine::wordSTARTGROUP);
      builtin.add(")",       &Machine::wordENDGROUP);
      builtin.add("\\*.*\n", &Machine::wordSYSTEM);
      builtin.add("\\+",     &Machine::wordSHARPEN);
      builtin.add(",",       &Machine::wordLENGTH);
      builtin.add("-",       &Machine::wordFLATTEN);
      builtin.add("\\..*\n",   &Machine::wordBUFFER);
      builtin.add("/",       &Machine::wordLENGTHEN);
      builtin.add(":",       &Machine::wordPITCH);
      builtin.add(";",       &Machine::wordVOICE);
      builtin.add("<",       &Machine::wordOCTDOWN);
      builtin.add("=",       &Machine::wordNATURALISE);
      builtin.add(">",       &Machine::wordUPOCTAVE);
      builtin.add("@",       &Machine::wordFINISH);
      builtin.add("\\[",     &Machine::wordSTARTWORD);
      builtin.add("]",       &Machine::wordENDWORD);
      builtin.add("^",       &Machine::wordREST);
      builtin.add("|",       &Machine::wordENDBAR);

      builtin.add("NOUT",    &Machine::wordNOUT);
      builtin.add("&NOUT",   &Machine::wordHOUT);
      builtin.add("$OUT",    &Machine::wordSOUT);
   }

   void run(const char* input_)
   {
      for(token_start = input_; *token_start != '\0'; token_start = token_end)
      {
         WordFunc func;
         token_end = builtin.lookup(token_start, func);

         if (token_end == token_start)
         {
            printf("ERR: %s\n", token_start);
            break;
         }

         (this->*func)();
      }
   }

private:
   using WordFunc = void (Machine::*)();

   void wordNOTHING()    {}
   void wordSTRING()     { str_stack.push(token_start + 1, token_end - 1); printf(">%s\n", str_stack.peek()); }
   void wordSTORE()      { write(pop(), pop()); }
   void wordNMUL()       { push(pop() * pop()); }
   void wordNADD()       { push(pop() + pop()); }
   void wordMADD()       { Address addr = pop(); write(addr, read(addr) + pop()); }
   void wordNSUB()       { push(pop() - pop()); }
   void wordNDIV()       { push(pop() / pop()); }
   void wordNDUP()       { push(peek()); }
   void wordNSWAP()      { Number one = pop(); Number two = pop(); push(one); push(two); }
   void wordNDROP()      { (void) pop(); }
   void wordNDUPPREV()   { push(peek(-2)); }
   void wordNDUP2()      { push(peek(-2)); push(peek(-2)); }

   void wordNROT()
   {
      Number one   = pop();
      Number two   = pop();
      Number three = pop();
      push(two);
      push(one);
      push(three);
   }

   void wordNLT()        { push(pop() <   pop() ? -1 : 0); }
   void wordNEQ()        { push(pop() ==  pop() ? -1 : 0); }
   void wordNGT()        { push(pop() >   pop() ? -1 : 0); }
   void wordNFETCH()     { push(read(pop())); }

   void wordSADD()
   {
      StrStack<256> one{str_stack.pop()};
      str_stack.add(one.peek());
   }

   void wordSPLIT()
   {
      StrStack<256> one{str_stack.pop()};
      Number        p = pop();
      str_stack.push(one.peek(), one.peek() + p);
      if (p <= one.size())
         str_stack.push(one.peek() + p);
   }

   void wordSSWAP()
   {
      StrStack<256> one{str_stack.pop()};
      StrStack<256> two{str_stack.pop()};
      str_stack.push(one.peek());
      str_stack.push(two.peek());
   }

   void wordSDROP()      { str_stack.pop(); }
   void wordBUILTIN()    {}
   void wordSTARTGROUP() {}
   void wordENDGROUP()   {}
   void wordSYSTEM()     {}
   void wordSHARPEN()    {}
   void wordLENGTH()     {}
   void wordFLATTEN()    {}
   void wordBUFFER()     {}
   void wordLENGTHEN()   {}
   void wordPITCH()      {}
   void wordVOICE()      {}
   void wordOCTDOWN()    {}
   void wordNATURALISE() {}
   void wordUPOCTAVE()   {}
   void wordFINISH()     {}
   void wordSTARTWORD()  {}
   void wordENDWORD()    {}
   void wordREST()       {}
   void wordENDBAR()     {}

   void wordHEXNUMBER()
   {
      Number number = 0;
      for(const char* s = token_start + 1; s < token_end; s++)
         if (*s >= 'A')
            number = number * 16 + *s - '0';
         else
            number = number * 16 + *s - 'A' + 0xA;
      push(number);
   }

   void wordNUMBER()
   {
      const char* s = token_start;
      signed sign   = +1;
      if (*s == '-')
      {
         s++;
         sign = -1;
      }
      Number number = 0;
      for(; s < token_end; s++)
         number = number * 10 + *s - '0';
      push(sign * number);
   }

   void wordNOUT()     { printf("%d\n", pop()); }
   void wordHOUT()     { printf("%X\n", pop() & 0xFFFF); }
   void wordSOUT()     { printf("A%s\n", str_stack.pop()); }

public:
   void word(const char* word)
   {
#if 0
           if (eq(word, "MAX"))   { push(std::max(pop(), pop())); }
      else if (eq(word, "MIN"))   { push(std::min(pop(), pop())); }
      else if (eq(word, "NL"))    { printf("\n"); }
      else if (eq(word, "&NOUT")) { printf("%X\n", pop()); }
      else if (eq(word, "NOT"))   { push(pop() ? 0 : -1); }
      else if (eq(word, "OFF"))   { push(0); }
      else if (eq(word, "ON"))    { push(-1); }
      else if (eq(word, "OR"))    { push(pop() | pop()); }
      else if (eq(word, "OUT"))   { printf("%c\n", pop() & 0x7F); }
      else if (eq(word, "XOR"))   { push(pop() ^ pop()); }
#endif
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
      num_stack.push_back(value_);
   }

   Number pop()
   {
      Number value = num_stack.back();
      num_stack.pop_back();
      return value;
   }

   Number peek(size_t index = -1)
   {
      return num_stack[num_stack.size() - 1];
   }

   void pushStr(const char* string_)
   {
   }

private:
   static const unsigned NUMBER_STACK_SIZE = 16;
   static const Address  MEMORY_SIZE       = 9184;

   using NumStack = STB::Stack<Number,NUMBER_STACK_SIZE>;

   NumStack           num_stack{};
   StrStack<512>      str_stack{};
   Number             memory[MEMORY_SIZE];
   Dict<WordFunc,512> builtin{};
   Dict<unsigned,512> user{};
   const char*        token_start{};
   const char*        token_end{};
};

} // namespace AMPLE
