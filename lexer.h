/**
 *  Copyright 2010 by Benjamin J. Land (a.k.a. BenLand100)
 *
 *  This file is part of CPascal.
 *
 *  CPascal is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  CPascal is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CPascal. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _LEXER_H
#define	_LEXER_H

#define PBOOLEAN        1
#define POPERATOR       2
#define PSPECIAL        3
#define PCHAR           4
#define PSTRING         5
#define PREAL           6
#define PINTEGER        7
#define PNAME           8

#define RES_PROGRAM     0
#define RES_BEGIN       1
#define RES_END         2
#define RES_VAR         3
#define RES_PROCEDURE   4
#define RES_FUNCTION    5
#define RES_IF          6
#define RES_THEN        7
#define RES_ELSE        8
#define RES_FOR         9
#define RES_TO          10
#define RES_DOWNTO      11
#define RES_DO          12
#define RES_WHILE       13
#define RES_REPEAT      14
#define RES_UNTIL       15
#define RES_CASE        16
#define RES_OF          17
#define RES_CONST       18
#define RES_TRY         19
#define RES_EXCEPT      20
#define RES_FINALLY     21
#define RES_TYPE        22
#define RES_ARRAY       23
#define RES_RECORD      24
#define RES_CHAR        25
#define RES_STRING      26
#define RES_INTEGER     27
#define RES_REAL        28
#define RES_BOOLEAN     29
#define RES_SIZE        30
#define RES_RESIZE      31
#define RES_NIL         32
#define RES_RESULT      33
#define RES_BREAK       34
#define RES_EXIT        35

#define MAX_PROTECTED   23
#define MAX_RES         50

#define OP_IDIV         0
#define OP_FDIV         1
#define OP_MOD          2
#define OP_MUL          3
#define OP_ADD          4
#define OP_SUB          5
#define OP_NOT          6
#define OP_OR           7
#define OP_AND          8
#define OP_EQU          9
#define OP_NEQ          10
#define OP_LESS         11
#define OP_LESSEQ       12
#define OP_GREAT        13
#define OP_GREATEQ      14
#define OP_ASGN         15
#define OP_FIELDGET     16
#define OP_FIELDSET     17
#define OP_DEREFGET     18
#define OP_DEREFSET     19
#define OP_ARRAYGET     20
#define OP_ARRAYSET     21
#define OP_ADDR         22
#define OP_NEG          23
#define OP_SIZE         24 //faux-function
#define OP_RESIZE       25 //faux-function
#define OP_SYMBOL       26 //names
#define OP_ARRAYDEF     27 //faux-function
#define OP_THROW        28 //faux-function

#define SPC_SEMICOLON   0
#define SPC_COLON       1
#define SPC_LPAREN      2
#define SPC_RPAREN      3
#define SPC_COMMA       4
#define SPC_LBRACE      5
#define SPC_RBRACE      6
#define SPC_THRU        7

#define BOOL_FALSE      0
#define BOOL_TRUE       1

#include <map>
#include <string>
#include "Exceptions.h"

typedef bool(*PreCompiler_Callback)(char* name, char* args) __attribute__((stdcall));

/**
 * PBOOLEAN is followed by its BOOL_ definition
 * POPERATOR is followed by its OP_ definition
 * POPERATOR is followed by its SPC_ definition
 * PCHAR is followed by its char
 * PSTRING  is followed by a null terminated char*
 * PREAL is followed by a float value
 * PINTEGER is followed by a long value
 * PNAME is followed by a unique integer (first few are reserved)
 */
char* lex(char* ppg, std::map<std::string,int> &names, PreCompiler_Callback precomp) throw (InterpEx*);

void freetoks(char* tokens);

#endif

