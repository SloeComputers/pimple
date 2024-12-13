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
#include "Ensemble.h"

namespace AMPLE {

class Machine
{
public:
   Machine()
   {
      builtin.add("\n",        &Machine::wordNOTHING);
      builtin.add(" +",        &Machine::wordNOTHING);

      builtin.add("\".*\"",    &Machine::wordSTRING);

      builtin.add("#!",        &Machine::wordSTORE);
      builtin.add("#\\*",      &Machine::wordNMUL);
      builtin.add("#\\+",      &Machine::wordNADD);
      builtin.add("#\\+!",     &Machine::wordMADD);
      builtin.add("#-",        &Machine::wordNSUB);
      builtin.add("#/",        &Machine::wordNDIV);
      builtin.add("#11",       &Machine::wordNDUP);
      builtin.add("#12",       &Machine::wordNSWAP);
      builtin.add("#2",        &Machine::wordNDROP);
      builtin.add("#212",      &Machine::wordNDUPPREV);
      builtin.add("#2121",     &Machine::wordNDUP2);
      builtin.add("#213",      &Machine::wordNROT);
      builtin.add("#<",        &Machine::wordNLT);
      builtin.add("#=",        &Machine::wordNEQ);
      builtin.add("#>",        &Machine::wordNGT);
      builtin.add("#?",        &Machine::wordNFETCH);
      builtin.add("#B!",       &Machine::wordSTOREBYTE);
      builtin.add("#B12",      &Machine::wordSWAPBYTES);
      builtin.add("#B?",       &Machine::wordFETCHBYTE);
      builtin.add("#IN",       &Machine::wordNIN);
      builtin.add("#OUT",      &Machine::wordNOUT);

      builtin.add("$\\+",      &Machine::wordSADD);
      builtin.add("$\\-",      &Machine::wordSPLIT);
      builtin.add("$12",       &Machine::wordSSWAP);
      builtin.add("$2",        &Machine::wordSDROP);
      builtin.add("$CHAR",     &Machine::wordSCHAR);
      builtin.add("$IN",       &Machine::wordSIN);
      builtin.add("$OUT",      &Machine::wordSOUT);
      builtin.add("$PAD",      &Machine::wordSPAD);
      builtin.add("$REV",      &Machine::wordSREV);
      builtin.add("$STR",      &Machine::wordSSTR);
      builtin.add("$STRIP",    &Machine::wordSSTRIP);

      builtin.add("%.*\n",     &Machine::wordNOTHING);

      builtin.add("&$STR",     &Machine::wordHSSTR);
      builtin.add("&[A-F]+",   &Machine::wordHEXNUMBER);
      builtin.add("&[A-F]+",   &Machine::wordHEXNUMBER);
      builtin.add("&NOUT",     &Machine::wordHOUT);
      builtin.add("&VAL",      &Machine::wordHVAL);

      builtin.add("'",         &Machine::wordBUILTIN);

      builtin.add("(",         &Machine::wordSTARTGROUP);

      builtin.add(")",         &Machine::wordENDGROUP);
      builtin.add(")ELSE(",    &Machine::wordELSE);
      builtin.add(")FOR",      &Machine::wordENDFOR);
      builtin.add(")IF",       &Machine::wordENDIF);
      builtin.add(")K",        &Machine::wordENDKEY);
      builtin.add(")NOTE",     &Machine::wordENDNOTEG);
      builtin.add(")PLAY",     &Machine::wordENDPLAY);
      builtin.add(")REP",      &Machine::wordENDREP);
      builtin.add(")REST",     &Machine::wordENDREST);
      builtin.add(")TIE",      &Machine::wordENDTIE);
      builtin.add(")UNTIL(",   &Machine::wordUNTIL);

      builtin.add("\\*.*\n",   &Machine::wordSYSTEM);
      builtin.add("\\+",       &Machine::wordSHARPEN);
      builtin.add(",",         &Machine::wordLENGTH);
      builtin.add("-",         &Machine::wordFLATTEN);
      builtin.add("\\..*\n",   &Machine::wordBUFFER);
      builtin.add("/",         &Machine::wordLENGTHEN);
      builtin.add(":",         &Machine::wordPITCH);
      builtin.add(";",         &Machine::wordVOICE);
      builtin.add("<",         &Machine::wordOCTDOWN);
      builtin.add("=",         &Machine::wordNATURALISE);
      builtin.add(">",         &Machine::wordUPOCTAVE);
      builtin.add("@",         &Machine::wordFINISH);
      builtin.add("\\[",       &Machine::wordSTARTWORD);
      builtin.add("]",         &Machine::wordENDWORD);
      builtin.add("^",         &Machine::wordREST);
      builtin.add("|",         &Machine::wordENDBAR);

      builtin.add("[0-9]+",    &Machine::wordNUMBER);
      builtin.add("-[0-9]+",   &Machine::wordNUMBER);

      builtin.add("[A-F]",     &Machine::wordNOTE);
      builtin.add("[a-f]",     &Machine::wordNOTE);

      builtin.add("ADSR",      &Machine::wordADSR);
      builtin.add("AGATE",     &Machine::wordAGATE);
      builtin.add("AENV",      &Machine::wordAENV);
      builtin.add("ALIGN",     &Machine::wordALIGN);
      builtin.add("AMP",       &Machine::wordAMP);
      builtin.add("AMPLE",     &Machine::wordAMPLE);
      builtin.add("APPEND",    &Machine::wordAPPEND);
      builtin.add("AND",       &Machine::wordAND);
      builtin.add("ASC",       &Machine::wordASC);
      builtin.add("ATTACK",    &Machine::wordATTACK);

      builtin.add("BAR",       &Machine::wordBAR);

      builtin.add("CHAN",      &Machine::wordCHAN);
      builtin.add("CHANS",     &Machine::wordCHANS);
      builtin.add("CLEAR",     &Machine::wordCLEAR);
      builtin.add("CODE",      &Machine::wordCODE);
      builtin.add("COMPACT",   &Machine::wordCOMPACT);
      builtin.add("CYCLE",     &Machine::wordCYCLE);

      builtin.add("DECAY",     &Machine::wordDECAY);
      builtin.add("DELETE",    &Machine::wordDELETE);
      builtin.add("DURATION",  &Machine::wordDURATION);

      builtin.add("EBIG",      &Machine::wordEBIG);
      builtin.add("EDIT",      &Machine::wordEDIT);
      builtin.add("EGRAD",     &Machine::wordEGRAD);
      builtin.add("ELEV",      &Machine::wordELEV);
      builtin.add("EMOD",      &Machine::wordEMOD);
      builtin.add("ESECT",     &Machine::wordESECT);
      builtin.add("EVEN",      &Machine::wordEVEN);

      builtin.add("FAST",      &Machine::wordFAST);
      builtin.add("FLUSH",     &Machine::wordFLUSH);
      builtin.add("FM",        &Machine::wordFM);
      builtin.add("FOR(",      &Machine::wordFOR);
      builtin.add("FREEZE",    &Machine::wordFREEZE);

      builtin.add("GATE",      &Machine::wordGATE);
      builtin.add("GO",        &Machine::wordGO);
      builtin.add("GVAR",      &Machine::wordGVAR);

      builtin.add("IDLE",      &Machine::wordIDLE);
      builtin.add("IF",        &Machine::wordIF);
      builtin.add("INDEX",     &Machine::wordINDEX);
      builtin.add("INVERT",    &Machine::wordINVERT);

      builtin.add("K(",        &Machine::wordKEY);

      builtin.add("LEN",       &Machine::wordLEN);
      builtin.add("LIST",      &Machine::wordLIST);
      builtin.add("LOAD",      &Machine::wordLOAD);

      builtin.add("MAX",       &Machine::wordMAX);
      builtin.add("MIN",       &Machine::wordMIN);
      builtin.add("MODE",      &Machine::wordMODE);

      builtin.add("NEW",       &Machine::wordNEW);
      builtin.add("NL",        &Machine::wordNL);
      builtin.add("NOT",       &Machine::wordNOT);
      builtin.add("NOTE(",     &Machine::wordNOTEG);
      builtin.add("NOUT",      &Machine::wordNOUT);

      builtin.add("ODD",       &Machine::wordODD);
      builtin.add("OFF",       &Machine::wordOFF);
      builtin.add("OFFSET",    &Machine::wordOFFSET);
      builtin.add("ON",        &Machine::wordON);
      builtin.add("OR",        &Machine::wordOR);

      builtin.add("PAGE",      &Machine::wordPAGE);
      builtin.add("PAIR",      &Machine::wordPAIR);
      builtin.add("PENV",      &Machine::wordPENV);
      builtin.add("PGATE",     &Machine::wordPGATE);
      builtin.add("PITCH",     &Machine::wordPITCH);
      builtin.add("PLAY(",     &Machine::wordPLAY);
      builtin.add("PLAYERS",   &Machine::wordPLAYERS);
      builtin.add("POS",       &Machine::wordPOS);
      builtin.add("PRINT",     &Machine::wordPRINT);
      builtin.add("PVAR",      &Machine::wordPVAR);

      builtin.add("QKEY",      &Machine::wordQKEY);

      builtin.add("RAND?",     &Machine::wordRAND);
      builtin.add("RAND!",     &Machine::wordSEED);
      builtin.add("RELEASE",   &Machine::wordRELEASE);
      builtin.add("REN",       &Machine::wordREN);
      builtin.add("REP(",      &Machine::wordREP);
      builtin.add("REPORT",    &Machine::wordREPORT);
      builtin.add("REST(",     &Machine::wordREST);
      builtin.add("RM",        &Machine::wordRM);

      builtin.add("SAVE",      &Machine::wordSAVE);
      builtin.add("SCAN",      &Machine::wordSCAN);
      builtin.add("SCORE",     &Machine::wordSCORE);
      builtin.add("SHARE",     &Machine::wordSHARE);
      builtin.add("SHIFT",     &Machine::wordSHIFT);
      builtin.add("SHOW",      &Machine::wordSHOW);
      builtin.add("SIGN",      &Machine::wordSIGN);
      builtin.add("SIMPLEA",   &Machine::wordSIMPLEA);
      builtin.add("SIMPLEACT", &Machine::wordSIMPLEACT);
      builtin.add("SIMPLEP",   &Machine::wordSIMPLEP);
      builtin.add("SIMPLEW",   &Machine::wordSIMPLEW);
      builtin.add("SOUND",     &Machine::wordSOUND);
      builtin.add("SP",        &Machine::wordSP);
      builtin.add("STOP",      &Machine::wordSTOP);
      builtin.add("SYNC",      &Machine::wordSYNC);
      builtin.add("SUSTAIN",   &Machine::wordSUSTAIN);

      builtin.add("TEMPO",     &Machine::wordTEMPO);
      builtin.add("TIE",       &Machine::wordTIE);
      builtin.add("TIME",      &Machine::wordTIME);
      builtin.add("TUNE",      &Machine::wordTUNE);

      builtin.add("VAL",       &Machine::wordVAL);
      builtin.add("VERSION",   &Machine::wordVERSION);
      builtin.add("VOICE",     &Machine::wordVOICE);
      builtin.add("VOICES",    &Machine::wordVOICES);

      builtin.add("WAVE",      &Machine::wordWAVE);
      builtin.add("WG!",       &Machine::wordWGWRITE);
      builtin.add("WG?",       &Machine::wordWGREAD);
      builtin.add("WGC",       &Machine::wordWGC);
      builtin.add("WH!",       &Machine::wordWHWRITE);
      builtin.add("WHG",       &Machine::wordWHG);
      builtin.add("WRITE",     &Machine::wordWRITE);
      builtin.add("WMOD",      &Machine::wordWMOD);
      builtin.add("WZERO",     &Machine::wordWZERO);

      builtin.add("XOR",       &Machine::wordXOR);

      reset();
   }

   void reset()
   {
      num_stack.clear();
      str_stack.clear();
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
   void error(const char* msg_)
   {
      printf("%s\n", msg_);
   }

   using WordFunc = void (Machine::*)();

   // VALKUE WORDS

   void wordOFF()     { push(0); }
   void wordON()      { push(-1); }
   void wordEVEN()    { push(-2); }
   void wordODD()     { push(-3); }

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
   void wordSHARPEN()    {}
   void wordLENGTH()     {}
   void wordFLATTEN()    {}
   void wordBUFFER()     {}
   void wordLENGTHEN()   {}
   void wordOCTDOWN()    {}
   void wordNATURALISE() {}
   void wordUPOCTAVE()   {}
   void wordFINISH()     {}
   void wordSTARTWORD()  {}
   void wordENDWORD()    {}
   void wordREST()       {}
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

   void wordNOTE()      {}
   void wordALIGN()     {}
   void wordAPPEND()    {}
   void wordASC()       {}
   void wordVAL()       {}
   void wordHVAL()      {}

   // MUSIC WORDS
   void wordBAR()       {}
   void wordSCORE()     {}
   void wordDURATION()  {}
   void wordTIE()       {}
   void wordENDTIE()    {}
   void wordKEY()       {}
   void wordENDKEY()    {}
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
   void wordAMP()       { ensemble.getChan()->setAmp(pop()); }
   void wordCHAN()      { ensemble.setChan(pop()); }
   void wordCHANS()     { if (not ensemble.allocChans(pop())) error("Too many channels"); }
   void wordCYCLE()     {}

   void wordFM()        { ensemble.getChan()->setFM(pop()); }
   void wordGATE()      { ensemble.getChan()->setGate(pop()); }
   void wordINVERT()    { ensemble.getChan()->setInvert(pop()); }
   void wordOFFSET()    { ensemble.getChan()->setOffset(pop()); }
   void wordPITCH()     { ensemble.getChan()->setPitch(pop()); }
   void wordPOS()       { ensemble.getChan()->setPos(pop()); }
   void wordRM()        { ensemble.getChan()->setRM(pop()); }
   void wordSHIFT()     { ensemble.getChan()->setShift(pop()); }

   void wordPLAYERS()   {}
   void wordSOUND()     { Voice* voice = ensemble.getVoice(); if (voice != nullptr) voice->reset(); }
   void wordSYNC()      { ensemble.getChan()->setSync(pop()); }
   void wordVOICE()     { ensemble.setVoice(pop()); }
   void wordVOICES()    { if (not ensemble.allocVoices(pop())) error("Too many voices"); }
   void wordSHARE()     {}
   void wordWAVE()      { ensemble.getChan()->setWave(pop()); }
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

public:
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

   Dict<WordFunc,512> builtin{};
   Dict<unsigned,512> user{};
   const char*        token_start{};
   const char*        token_end{};

   using NumStack    = STB::Stack<Number,NUM_STACK_SIZE>;
   using StringStack = StrStack<STR_STACK_SIZE>;

   NumStack    num_stack{};
   StringStack str_stack{};
   Number      memory[MEMORY_SIZE];
   Ensemble    ensemble;
};

} // namespace AMPLE
