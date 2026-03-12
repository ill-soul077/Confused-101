
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     IDENT = 258,
     STRING_LIT = 259,
     INT_LIT = 260,
     TRUE_LIT = 261,
     FALSE_LIT = 262,
     FLOAT_LIT = 263,
     CHAR_LIT = 264,
     MFUN = 265,
     IN = 266,
     FLT = 267,
     CH = 268,
     BOOL = 269,
     RETURN = 270,
     SHOW = 271,
     ASK = 272,
     IFF = 273,
     ORIFF = 274,
     OR = 275,
     FLP = 276,
     WLP = 277,
     INCIDENT = 278,
     CHECK = 279,
     NOP = 280,
     STK = 281,
     LINE = 282,
     TAKEPART = 283,
     MY_ARRAY = 284,
     EXPONENTIAL = 285,
     INC = 286,
     NAINAI = 287,
     LASTADD = 288,
     LASTDRAG = 289,
     STRJOIN = 290,
     STRLENF = 291,
     ADD = 292,
     SUB = 293,
     MUL = 294,
     DIV = 295,
     MOD = 296,
     AND = 297,
     BOR = 298,
     XOR = 299,
     BNOT = 300,
     GRT = 301,
     LRT = 302,
     EQL = 303,
     EQGRT = 304,
     EQSML = 305,
     NEQL = 306,
     AT = 307,
     COMPOUNDADD = 308,
     HASH = 309,
     DOLLAR = 310,
     COLON = 311
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 1203 "confused.y"

    char        *sval;
    int          ival;
    float        fval;
    char         cval;
    struct Node *nval;



/* Line 1676 of yacc.c  */
#line 118 "confused.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


