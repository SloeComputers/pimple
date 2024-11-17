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

#include "STB/Fifo.h"

#include "AMPLE/Types.h"

namespace AMPLE {

class Lex
{
public:
   enum Token
   {
      ERROR,
      STRING,
      NUMBER,
      WORD,
      END
   };

   Lex() = default;

   //! Clear input buffer
   void clear()
   {
      input.clear();
   }

   //! Add char to input buffer
   void push(char ch)
   {
      input.push(ch);
   }

   //! Get the next token from the input stream
   Token next()
   {
      char ch;

      // skip white space
      while(true)
      {
         if (input.empty())
            return Token::END;

         ch = input.back();
         if ((ch != ' ') && (ch != '\n'))
            break;;

         input.pop();
      }

      if (ch == '%')
      {
         // skip comment
         while(true)
         {
            input.pop();
            if (input.empty())
               return Token::END;

            if (ch == '\n')
               break;

            ch = input.back();
         }
      }
      else if (ch == '&')
      {
         return lexHexadecimal(ch);
      }
      else if (isDec(ch) || (ch == '-'))
      {
         return lexDecimal(ch);
      }
      else if (ch == '"')
      {
         return lexString(ch);
      }

      return lexWord(ch);
   }

   //! Get value of NUMBER token
   Number number() const { return tkn_num; }

   //! Get value of STRING token
   const char* string() const { return &tkn_str.back(); }

   //! Get value of WORD token
   const char* word() const { return &tkn_str.back(); }

   //! Get value of ERROR token
   const char* error() const { return tkn_err; }

private:
   static bool isDec(char ch) { return ch >= '0' && ch <= '9'; }

   static bool isHex(char ch) { return isDec(ch) || (ch >= 'A' && ch <= 'F'); }

   Token error(const char* message_)
   {
      tkn_err = message_;
      return Token::ERROR;
   }

   //! Lexical analysis of a decimal literal
   Token lexDecimal(char ch)
   {
      bool negative{false};

      if (ch == '-')
      {
         negative = true;

         input.pop();
         if (input.empty())
            return error("Missing number");

         ch = input.back();
         if (not isDec(ch))
            return error("Number expected");;
      }

      int32_t value = 0;

      while(true)
      {
         value = value * 10 + ch - '0';

         input.pop();
         if (input.empty())
            break;

         ch = input.back();
         if (not isDec(ch))
            break;
      }

      if (negative)
      {
         if (value > 0x8000)
            return error("Number too big");;

         tkn_num = Number(-value);
      }
      else
      {
         if (value > 0x7FFF)
            return error("Number too big");;

         tkn_num = Number(value);
      }

      return Token::NUMBER;
   }

   //! Lexical analysis of a hexadecimal literal
   Token lexHexadecimal(char ch)
   {
      input.pop();
      if (input.empty())
         return error("Missing hex");

      ch = input.back();
      if (not isHex(ch))
         return error("Bad hex");

      uint32_t value = 0;

      while(true)
      {
         if (isDec(ch))
            value = value * 16 + ch - '0';
         else
            value = value * 16 + ch - 'A' + 0xA;

         input.pop();
         if (input.empty())
            break;

         ch = input.back();
         if (not isHex(input.back()))
            break;
      }

      if (value > 0xFFFF)
         return error("Number too big");;

      tkn_num = Number(value);

      return Token::NUMBER;
   }

   //! Lexical analysis of a string literal 
   Token lexString(char ch)
   {
      input.pop();
      tkn_str.clear();

      while(true)
      {
         if (input.empty())
            return error("Unterminated string");;

         ch = input.back();
         if (ch == '"')
            break;

         input.pop();
         tkn_str.push(ch);
      }

      input.pop();
      tkn_str.push('\0');

      return Token::STRING;
   }

   //! Lexical analysis of a keyword
   Token lexWord(char ch)
   {
      tkn_str.clear();

      while(true)
      {
         tkn_str.push(ch);

         input.pop();
         if (input.empty())
            break;

         ch = input.back();
         if ((ch == ' ') || (ch == '\n') || (ch == '%'))
            break;
      }

      tkn_str.push('\0');

      return Token::WORD;
   }

   STB::Fifo<char,8> input{};
   Number            tkn_num{};
   STB::Fifo<char,4> tkn_str{};
   const char*       tkn_err{};
};

} // namespace AMPLE
