/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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
     TOK_NUMBER = 258,
     TOK_USERFUNC = 259,
     TOK_ID = 260,
     TOK_STRINGLIT = 261,
     TOK_IF = 262,
     TOK_ELSE = 263,
     TOK_DO = 264,
     TOK_FOR = 265,
     TOK_WHILE = 266,
     TOK_BREAK = 267,
     TOK_CONTINUE = 268,
     TOK_GOTO = 269,
     TOK_RETURN = 270,
     TOK_FUNCTION = 271,
     TOK_LOCAL = 272,
     TOK_GLOBAL = 273,
     TOK_GLOBAL_8U = 274,
     TOK_GLOBAL_8S = 275,
     TOK_ARRAY_8U = 276,
     TOK_ARRAY_8S = 277,
     TOK_ARRAY_16 = 278,
     TOK_EXTERN = 279,
     TOK_MEMADDR = 280,
     TOK_SECTION = 281,
     TOK_TRAMPOLINE = 282,
     TOK_LINE = 283,
     TOK_VMIP = 284,
     TOK_VMSP = 285,
     TOK_VMSFP = 286,
     TOK_PTR_8U = 287,
     TOK_PTR_8S = 288,
     TOK_PTR_16 = 289,
     TOK_PTR_F = 290,
     TOK_LOR = 291,
     TOK_LAND = 292,
     TOK_NE = 293,
     TOK_EQ = 294,
     TOK_GE = 295,
     TOK_LE = 296,
     TOK_SHR = 297,
     TOK_SHL = 298,
     NEG = 299,
     TOK_DEC = 300,
     TOK_INC = 301,
     TOK_ASSIGN_ADD = 302,
     TOK_ASSIGN_SUB = 303,
     TOK_ASSIGN_MUL = 304,
     TOK_ASSIGN_DIV = 305,
     TOK_ASSIGN_MOD = 306,
     TOK_ASSIGN_SHL = 307,
     TOK_ASSIGN_SHR = 308,
     TOK_ASSIGN_AND = 309,
     TOK_ASSIGN_OR = 310,
     TOK_ASSIGN_XOR = 311
   };
#endif
/* Tokens.  */
#define TOK_NUMBER 258
#define TOK_USERFUNC 259
#define TOK_ID 260
#define TOK_STRINGLIT 261
#define TOK_IF 262
#define TOK_ELSE 263
#define TOK_DO 264
#define TOK_FOR 265
#define TOK_WHILE 266
#define TOK_BREAK 267
#define TOK_CONTINUE 268
#define TOK_GOTO 269
#define TOK_RETURN 270
#define TOK_FUNCTION 271
#define TOK_LOCAL 272
#define TOK_GLOBAL 273
#define TOK_GLOBAL_8U 274
#define TOK_GLOBAL_8S 275
#define TOK_ARRAY_8U 276
#define TOK_ARRAY_8S 277
#define TOK_ARRAY_16 278
#define TOK_EXTERN 279
#define TOK_MEMADDR 280
#define TOK_SECTION 281
#define TOK_TRAMPOLINE 282
#define TOK_LINE 283
#define TOK_VMIP 284
#define TOK_VMSP 285
#define TOK_VMSFP 286
#define TOK_PTR_8U 287
#define TOK_PTR_8S 288
#define TOK_PTR_16 289
#define TOK_PTR_F 290
#define TOK_LOR 291
#define TOK_LAND 292
#define TOK_NE 293
#define TOK_EQ 294
#define TOK_GE 295
#define TOK_LE 296
#define TOK_SHR 297
#define TOK_SHL 298
#define NEG 299
#define TOK_DEC 300
#define TOK_INC 301
#define TOK_ASSIGN_ADD 302
#define TOK_ASSIGN_SUB 303
#define TOK_ASSIGN_MUL 304
#define TOK_ASSIGN_DIV 305
#define TOK_ASSIGN_MOD 306
#define TOK_ASSIGN_SHL 307
#define TOK_ASSIGN_SHR 308
#define TOK_ASSIGN_AND 309
#define TOK_ASSIGN_OR 310
#define TOK_ASSIGN_XOR 311




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 95 "parser.y"
{
	int number;
	char *string;
	struct evm_insn_s *insn;
	struct func_call_args_desc_s *fc;
	struct loopcontext_s *loopctx;
	struct array_init_s ainit;
	void *vp;
}
/* Line 1529 of yacc.c.  */
#line 171 "parser.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

