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
#include <cstdio>

#include "STB/Stack.h"

#include "Types.h"
#include "Dict.h"
#include "StrStack.h"
#include "Score.h"
#include "Sound.h"

namespace AMPLE {

class Machine
{
public:
   Machine()
   {
      addWord("\n",        &Machine::wordNOTHING);
      addWord(" +",        &Machine::wordNOTHING);

      addWord("\"[ !#-~]*\"", &Machine::wordSTRING);

      addWord("#!",        &Machine::wordSTORE);
      addWord("#\\*",      &Machine::wordNMUL);
      addWord("#\\+",      &Machine::wordNADD);
      addWord("#\\+!",     &Machine::wordMADD);
      addWord("#-",        &Machine::wordNSUB);
      addWord("#/",        &Machine::wordNDIV);
      addWord("#11",       &Machine::wordNDUP);
      addWord("#12",       &Machine::wordNSWAP);
      addWord("#2",        &Machine::wordNDROP);
      addWord("#212",      &Machine::wordNDUPPREV);
      addWord("#2121",     &Machine::wordNDUP2);
      addWord("#213",      &Machine::wordNROT);
      addWord("#<",        &Machine::wordNLT);
      addWord("#=",        &Machine::wordNEQ);
      addWord("#>",        &Machine::wordNGT);
      addWord("#?",        &Machine::wordNFETCH);
      addWord("#B!",       &Machine::wordSTOREBYTE);
      addWord("#B12",      &Machine::wordSWAPBYTES);
      addWord("#B?",       &Machine::wordFETCHBYTE);
      addWord("#IN",       &Machine::wordNIN);
      addWord("#OUT",      &Machine::wordNOUT);

      addWord("$\\+",      &Machine::wordSADD);
      addWord("$\\-",      &Machine::wordSPLIT);
      addWord("$12",       &Machine::wordSSWAP);
      addWord("$2",        &Machine::wordSDROP);
      addWord("$CHAR",     &Machine::wordSCHAR);
      addWord("$IN",       &Machine::wordSIN);
      addWord("$OUT",      &Machine::wordSOUT);
      addWord("$PAD",      &Machine::wordSPAD);
      addWord("$REV",      &Machine::wordSREV);
      addWord("$STR",      &Machine::wordSSTR);
      addWord("$STRIP",    &Machine::wordSSTRIP);

      addWord("%.*\n",     &Machine::wordNOTHING);

      addWord("&$STR",     &Machine::wordHSSTR);
      addWord("&[0-9A-F]+",&Machine::wordHEXNUMBER);
      addWord("&[0-9A-F]+",&Machine::wordHEXNUMBER);
      addWord("&NOUT",     &Machine::wordHOUT);
      addWord("&VAL",      &Machine::wordHVAL);

      addWord("'",         &Machine::wordBUILTIN);

      addWord("(",         &Machine::wordSTARTGROUP);

      addWord(")",         &Machine::wordENDGROUP);
      addWord(")ELSE(",    &Machine::wordELSE);
      addWord(")FOR",      &Machine::wordENDFOR);
      addWord(")IF",       &Machine::wordENDIF);
      addWord(")K",        &Machine::wordENDKEY);
      addWord(")NOTE",     &Machine::wordENDNOTEG);
      addWord(")PLAY",     &Machine::wordENDPLAY);
      addWord(")REP",      &Machine::wordENDREP);
      addWord(")REST",     &Machine::wordENDREST);
      addWord(")TIE",      &Machine::wordENDTIE);
      addWord(")UNTIL(",   &Machine::wordUNTIL);

      addWord("\\*.*\n",   &Machine::wordSYSTEM);
      addWord("\\+",       &Machine::wordSHARPEN);
      addWord(",",         &Machine::wordLENGTH);
      addWord("-",         &Machine::wordFLATTEN);
      addWord("\\..*\n",   &Machine::wordBUFFER);
      addWord("/",         &Machine::wordLENGTHEN);
      addWord(":",         &Machine::wordOCTAVE);
      addWord(";",         &Machine::wordVOICE);
      addWord("<",         &Machine::wordOCTDOWN);
      addWord("=",         &Machine::wordNATURALISE);
      addWord(">",         &Machine::wordUPOCTAVE);
      addWord("@",         &Machine::wordFINISH);
      addWord("\\[",       &Machine::wordSTARTWORD);
      addWord("]",         &Machine::wordENDWORD);
      addWord("^",         &Machine::wordREST);
      addWord("|",         &Machine::wordENDBAR);

      addWord("[0-9]+",    &Machine::wordNUMBER);
      addWord("-[0-9]+",   &Machine::wordNUMBER);

#if 0
      addWord("[A-F]",     &Machine::wordNOTE);
      addWord("[a-f]",     &Machine::wordNOTE);
#else
      addWord("a",         &Machine::wordNOTE);
      addWord("b",         &Machine::wordNOTE);
      addWord("c",         &Machine::wordNOTE);
      addWord("d",         &Machine::wordNOTE);
      addWord("e",         &Machine::wordNOTE);
      addWord("f",         &Machine::wordNOTE);
      addWord("g",         &Machine::wordNOTE);
      addWord("A",         &Machine::wordNOTE);
      addWord("B",         &Machine::wordNOTE);
      addWord("C",         &Machine::wordNOTE);
      addWord("D",         &Machine::wordNOTE);
      addWord("E",         &Machine::wordNOTE);
      addWord("F",         &Machine::wordNOTE);
      addWord("G",         &Machine::wordNOTE);
#endif

      addWord("ADSR",      &Machine::wordADSR);
      addWord("AGATE",     &Machine::wordAGATE);
      addWord("AENV",      &Machine::wordAENV);
      addWord("ALIGN",     &Machine::wordALIGN);
      addWord("AMP",       &Machine::wordAMP);
      addWord("AMPLE",     &Machine::wordAMPLE);
      addWord("APPEND",    &Machine::wordAPPEND);
      addWord("AND",       &Machine::wordAND);
      addWord("ASC",       &Machine::wordASC);
      addWord("ATTACK",    &Machine::wordATTACK);

      addWord("BAR",       &Machine::wordBAR);

      addWord("CHAN",      &Machine::wordCHAN);
      addWord("CHANS",     &Machine::wordCHANS);
      addWord("CLEAR",     &Machine::wordCLEAR);
      addWord("CODE",      &Machine::wordCODE);
      addWord("COMPACT",   &Machine::wordCOMPACT);
      addWord("CYCLE",     &Machine::wordCYCLE);

      addWord("DECAY",     &Machine::wordDECAY);
      addWord("DELETE",    &Machine::wordDELETE);
      addWord("DURATION",  &Machine::wordDURATION);

      addWord("EBIG",      &Machine::wordEBIG);
      addWord("EDIT",      &Machine::wordEDIT);
      addWord("EGRAD",     &Machine::wordEGRAD);
      addWord("ELEV",      &Machine::wordELEV);
      addWord("EMOD",      &Machine::wordEMOD);
      addWord("ESECT",     &Machine::wordESECT);
      addWord("EVEN",      &Machine::wordEVEN);

      addWord("FAST",      &Machine::wordFAST);
      addWord("FLUSH",     &Machine::wordFLUSH);
      addWord("FM",        &Machine::wordFM);
      addWord("FOR(",      &Machine::wordFOR);
      addWord("FREEZE",    &Machine::wordFREEZE);

      addWord("GATE",      &Machine::wordGATE);
      addWord("GO",        &Machine::wordGO);
      addWord("GVAR",      &Machine::wordGVAR);

      addWord("IDLE",      &Machine::wordIDLE);
      addWord("IF",        &Machine::wordIF);
      addWord("INDEX",     &Machine::wordINDEX);
      addWord("INVERT",    &Machine::wordINVERT);

      addWord("K(",        &Machine::wordKEY);

      addWord("LEN",       &Machine::wordLEN);
      addWord("LIST",      &Machine::wordLIST);
      addWord("LOAD",      &Machine::wordLOAD);

      addWord("MAX",       &Machine::wordMAX);
      addWord("MIN",       &Machine::wordMIN);
      addWord("MODE",      &Machine::wordMODE);

      addWord("NEW",       &Machine::wordNEW);
      addWord("NL",        &Machine::wordNL);
      addWord("NOT",       &Machine::wordNOT);
      addWord("NOTE(",     &Machine::wordNOTEG);
      addWord("NOUT",      &Machine::wordNOUT);

      addWord("ODD",       &Machine::wordODD);
      addWord("OFF",       &Machine::wordOFF);
      addWord("OFFSET",    &Machine::wordOFFSET);
      addWord("ON",        &Machine::wordON);
      addWord("OR",        &Machine::wordOR);

      addWord("PAGE",      &Machine::wordPAGE);
      addWord("PAIR",      &Machine::wordPAIR);
      addWord("PENV",      &Machine::wordPENV);
      addWord("PGATE",     &Machine::wordPGATE);
      addWord("PITCH",     &Machine::wordPITCH);
      addWord("PLAY(",     &Machine::wordPLAY);
      addWord("PLAYERS",   &Machine::wordPLAYERS);
      addWord("POS",       &Machine::wordPOS);
      addWord("PRINT",     &Machine::wordPRINT);
      addWord("PVAR",      &Machine::wordPVAR);

      addWord("QKEY",      &Machine::wordQKEY);

      addWord("RAND?",     &Machine::wordRAND);
      addWord("RAND!",     &Machine::wordSEED);
      addWord("RELEASE",   &Machine::wordRELEASE);
      addWord("REN",       &Machine::wordREN);
      addWord("REP(",      &Machine::wordREP);
      addWord("REPORT",    &Machine::wordREPORT);
      addWord("REST(",     &Machine::wordREST);
      addWord("RM",        &Machine::wordRM);

      addWord("SAVE",      &Machine::wordSAVE);
      addWord("SCAN",      &Machine::wordSCAN);
      addWord("SCORE",     &Machine::wordSCORE);
      addWord("SHARE",     &Machine::wordSHARE);
      addWord("SHIFT",     &Machine::wordSHIFT);
      addWord("SHOW",      &Machine::wordSHOW);
      addWord("SIGN",      &Machine::wordSIGN);
      addWord("SIMPLEA",   &Machine::wordSIMPLEA);
      addWord("SIMPLEACT", &Machine::wordSIMPLEACT);
      addWord("SIMPLEP",   &Machine::wordSIMPLEP);
      addWord("SIMPLEW",   &Machine::wordSIMPLEW);
      addWord("SOUND",     &Machine::wordSOUND);
      addWord("SP",        &Machine::wordSP);
      addWord("STOP",      &Machine::wordSTOP);
      addWord("SYNC",      &Machine::wordSYNC);
      addWord("SUSTAIN",   &Machine::wordSUSTAIN);

      addWord("TEMPO",     &Machine::wordTEMPO);
      addWord("TIE",       &Machine::wordTIE);
      addWord("TIME",      &Machine::wordTIME);
      addWord("TUNE",      &Machine::wordTUNE);

      addWord("VAL",       &Machine::wordVAL);
      addWord("VERSION",   &Machine::wordVERSION);
      addWord("VOICE",     &Machine::wordVOICE);
      addWord("VOICES",    &Machine::wordVOICES);

      addWord("WAVE",      &Machine::wordWAVE);
      addWord("WG!",       &Machine::wordWGWRITE);
      addWord("WG?",       &Machine::wordWGREAD);
      addWord("WGC",       &Machine::wordWGC);
      addWord("WH!",       &Machine::wordWHWRITE);
      addWord("WHG",       &Machine::wordWHG);
      addWord("WRITE",     &Machine::wordWRITE);
      addWord("WMOD",      &Machine::wordWMOD);
      addWord("WZERO",     &Machine::wordWZERO);

      addWord("XOR",       &Machine::wordXOR);

      reset();
   }

   //! User command line entry point
   void shell(bool echo_)
   {
      printf("%zu\n", sizeof(*this));
      printf("%zu\n", sizeof(builtin));

      static const unsigned MAX_LINE_LENGTH = 255;

      while(true)
      {
         printf(" %%");

         char line[MAX_LINE_LENGTH + 1];

         for(unsigned i = 0; true; ++i)
         {
            char ch = getchar();
            if (ch == -1)
               return;

            if (echo_)
               putchar(ch);

            if (ch == '\n')
            {
               line[i] = '\0';
               break;
            }

            line[i] = ch;
         }

         parseLine(line);
      }
   }

   //! Sequencer tick entry point
   void tick()
   {
   }

private:
   using WordFunc = void (Machine::*)();

   void parseLine(const char* line_)
   {
      for(token_start = line_; *token_start != '\0'; token_start = token_end)
      {
         unsigned code;
         token_end = user.lookup(token_start, code);
         if (token_end != token_start)
         {
         }
         else if (token_end == token_start)
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
   }

   void reset()
   {
      num_stack.clear();
      str_stack.clear();
      score.reset();
      sound.reset();

      printf("AMPLE re-code\n");
   }

   void error(const char* msg_)
   {
      printf("%s\n", msg_);
   }

   void addWord(const char* pattern_, WordFunc func_)
   {
      if (not builtin.add(pattern_, func_))
      {
         error("Dictionary full");
         printf("pattern = %s\n", pattern_);
      }
   }

   // VALUE WORDS

   void wordOFF()     { push(0); }
   void wordON()      { push(-1); }
   void wordEVEN()    { push(-2); }
   void wordODD()     { push(-3); }

   void wordHEXNUMBER()
   {
      Number number = 0;
      for(const char* s = token_start + 1; s < token_end; s++)
         if (*s >= 'A')
            number = number * 16 + *s - 'A' + 0xA;
         else
            number = number * 16 + *s - '0';
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

   void wordAND()        { push(pop() & pop()); }
   void wordOR()         { push(pop() | pop()); }
   void wordXOR()        { push(pop() ^ pop()); }
   void wordNADD()       { push(pop() + pop()); }
   void wordNSUB()       { push(pop() - pop()); }
   void wordNMUL()       { push(pop() * pop()); }
   void wordNDIV()       { push(pop() / pop()); }
   void wordMAX()        { push(std::max(pop(), pop())); }
   void wordMIN()        { push(std::min(pop(), pop())); }
   void wordNOT()        { push(pop() ? 0 : -1); }

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

   // MEMORY WORDS
   void wordSTORE()     { write(pop(), pop()); }
   void wordMADD()      { Address addr = pop(); write(addr, read(addr) + pop()); }
   void wordSTOREBYTE() {}
   void wordSWAPBYTES() {}
   void wordFETCHBYTE() {}

   void wordNLT()        { push(pop() <   pop() ? -1 : 0); }
   void wordNEQ()        { push(pop() ==  pop() ? -1 : 0); }
   void wordNGT()        { push(pop() >   pop() ? -1 : 0); }
   void wordSIGN()       { }
   void wordNFETCH()     { push(read(pop())); }

   // STRING WORDS
   void wordSTRING()  { str_stack.push(token_start + 1, token_end - 1); }

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

   void wordSCHAR()      {}
   void wordSDROP()      { str_stack.pop(); }
   void wordSPAD()       {}
   void wordSSTR()       {}
   void wordSSTRIP()     {}
   void wordSREV()       {}
   void wordHSSTR()      {}

   void wordBUILTIN()    {}
   void wordSTARTGROUP() {}
   void wordENDGROUP()   {}
   void wordSYSTEM()     {}
   void wordBUFFER()     {}
   void wordLENGTHEN()   {}
   void wordOCTDOWN()    {}
   void wordUPOCTAVE()   {}
   void wordFINISH()     {}
   void wordSTARTWORD()  {}
   void wordENDWORD()    {}
   void wordENDREST()    {}
   void wordENDBAR()     {}

   void wordSIMPLEA()    {}
   void wordSIMPLEACT()  {}
   void wordSIMPLEP()    {}
   void wordSIMPLEW()    {}

   void wordWGWRITE()    {}
   void wordWGREAD()     {}
   void wordWGC()        {}
   void wordWHWRITE()    {}
   void wordWHG()        {}
   void wordWMOD()       {}
   void wordWRITE()      {}
   void wordWZERO()      {}
   void wordPLAY()       {}
   void wordENDPLAY()    {}

   void wordQKEY()       {}
   void wordSIN()        {}
   void wordNIN()        {}
   void wordNOUT()       { printf("%d\n", pop()); }
   void wordHOUT()       { printf("%X\n", pop() & 0xFFFF); }
   void wordSOUT()       { printf("%s\n", str_stack.pop()); }
   void wordOUT()        { printf("%c", pop() & 0x7F); }
   void wordNL()         { printf("\n"); }
   void wordSP()         {}

   void wordALIGN()     {}
   void wordAPPEND()    {}
   void wordASC()       {}
   void wordVAL()       {}
   void wordHVAL()      {}

   // SCORE WORDS
   void wordSCORE()      { score.reset(); }
   void wordKEY()        { score.keySig(true); }
   void wordENDKEY()     { score.keySig(false); }
   void wordOCTAVE()     { score.setOctave(pop()); }
   void wordNOTE()       { score.note(*token_start); }
   void wordFLATTEN()    { score.acid(-1); }
   void wordNATURALISE() { score.acid(0);  }
   void wordSHARPEN()    { score.acid(+1); }
   void wordREST()       { score.rest(); }
   void wordLENGTH()     { score.setLength(pop()); }
   void wordTIE()        { score.tie(); }

   void wordBAR()       {}
   void wordDURATION()  {}
   void wordENDTIE()    {}


   void wordFLUSH()     {}
   void wordTEMPO()     {}
   void wordFREEZE()    {}
   void wordGO()        {}
   void wordIDLE()      {}
   void wordMODE()      {}
   void wordLEN()       {}
   void wordFAST()      {}
   void wordNOTEG()     {}
   void wordENDNOTEG()  {}
   void wordSTOP()      {}
   void wordTIME()      {}

   // ENVELOPE WORDS
   void wordADSR()      {}
   void wordAENV()      {}
   void wordAGATE()     {}
   void wordEBIG()      {}
   void wordEGRAD()     {}
   void wordELEV()      {}
   void wordEMOD()      {}
   void wordESECT()     {}
   void wordPENV()      {}
   void wordPGATE()     {}
   void wordATTACK()    {}
   void wordDECAY()     {}
   void wordSUSTAIN()   {}
   void wordRELEASE()   {}

   // SOUND WORDS
   void wordAMP()       { sound.getChan()->setAmp(pop()); }
   void wordCHAN()      { sound.setChan(pop()); }
   void wordCHANS()     { if (not sound.allocChans(pop())) error("Too many channels"); }
   void wordCYCLE()     {}

   void wordFM()        { sound.getChan()->setFM(pop()); }
   void wordGATE()      { sound.getChan()->setGate(pop()); }
   void wordINVERT()    { sound.getChan()->setInvert(pop()); }
   void wordOFFSET()    { sound.getChan()->setOffset(pop()); }
   void wordPITCH()     { sound.getChan()->setPitch(pop()); }
   void wordPOS()       { sound.getChan()->setPos(pop()); }
   void wordRM()        { sound.getChan()->setRM(pop()); }
   void wordSHIFT()     { sound.getChan()->setShift(pop()); }

   void wordPLAYERS()   {}
   void wordSOUND()     { Voice* voice = sound.getVoice(); if (voice != nullptr) voice->reset(); }
   void wordSYNC()      { sound.getChan()->setSync(pop()); }
   void wordVOICE()     { sound.setVoice(pop()); }
   void wordVOICES()    { if (not sound.allocVoices(pop())) error("Too many voices"); }
   void wordSHARE()     {}
   void wordWAVE()      { sound.getChan()->setWave(pop()); }
   void wordTUNE()      {}

   // PROGRAM WORDS
   void wordCLEAR()     {}
   void wordDELETE()    {}
   void wordEDIT()      {}
   void wordNOTHING()   {}
   void wordIF()        {}
   void wordENDIF()     {}
   void wordFOR()       {}
   void wordENDFOR()    {}
   void wordREP()       {}
   void wordENDREP()    {}
   void wordELSE()      {}
   void wordUNTIL()     {}
   void wordLIST()      {}
   void wordLOAD()      {}
   void wordSAVE()      {}
   void wordNEW()       {}
   void wordPAGE()      {}
   void wordPRINT()     {}
   void wordGVAR()      {}
   void wordRAND()      {}
   void wordSEED()      {}
   void wordREPORT()    {}
   void wordREN()       {}
   void wordSCAN()      {}
   void wordSHOW()      {}
   void wordINDEX()     {}
   void wordPAIR()      {}
   void wordPVAR()      {}

   // SYSTEM WORDS
   void wordAMPLE()     {}
   void wordCODE()      {}
   void wordCOMPACT()   {}
   void wordVERSION()   {}

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

   static const unsigned NUM_STACK_SIZE = 16;
   static const unsigned STR_STACK_SIZE = 512;
   static const Address  MEMORY_SIZE    = 9184;

   using NumStack    = STB::Stack<Number,NUM_STACK_SIZE>;
   using StringStack = StrStack<STR_STACK_SIZE>;

   const char*        token_start{};
   const char*        token_end{};
   Dict<WordFunc,512> builtin{};
   Dict<unsigned,512> user{};
   NumStack           num_stack{};
   StringStack        str_stack{};
   Number             memory[MEMORY_SIZE];
   Score              score{};
   Sound              sound{};
};

} // namespace AMPLE
