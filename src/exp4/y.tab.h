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
     NAME = 258,
     STRING = 259,
     NUMBER = 260,
     COMPARISION = 261,
     AND = 262,
     SELECT = 263,
     FROM = 264,
     WHERE = 265,
     ORDER = 266,
     BY = 267,
     ASC = 268,
     DESC = 269,
     ALL = 270,
     UNIQUE = 271,
     DISTINCT = 272,
     CREATE = 273,
     TABLE = 274,
     DROP = 275,
     INDEX = 276,
     INSERT = 277,
     INTO = 278,
     VALUES = 279,
     DELETE = 280,
     CHARACTER = 281,
     INTEGER = 282,
     DATE = 283,
     SHOW = 284,
     TABLES = 285,
     EXIT = 286
   };
#endif
/* Tokens.  */
#define NAME 258
#define STRING 259
#define NUMBER 260
#define COMPARISION 261
#define AND 262
#define SELECT 263
#define FROM 264
#define WHERE 265
#define ORDER 266
#define BY 267
#define ASC 268
#define DESC 269
#define ALL 270
#define UNIQUE 271
#define DISTINCT 272
#define CREATE 273
#define TABLE 274
#define DROP 275
#define INDEX 276
#define INSERT 277
#define INTO 278
#define VALUES 279
#define DELETE 280
#define CHARACTER 281
#define INTEGER 282
#define DATE 283
#define SHOW 284
#define TABLES 285
#define EXIT 286




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 44 "sql.y"
{
	int intval;
	char *strval;
}
/* Line 1529 of yacc.c.  */
#line 116 "y.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

