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

class InterpEx;

#ifndef _EXCEPTIONS_H
#define	_EXCEPTIONS_H

#define E_DIV_ZERO      0
#define E_NOT_INTEGER   1
#define E_NOT_REAL      2
#define E_NOT_CHAR      3
#define E_NOT_STRING    4
#define E_NOT_BOOLEAN   5
#define E_NOT_RECORD    6
#define E_NOT_ARRAY     7
#define E_NOT_POINTER   8
#define E_NULL_VAL      9
#define E_NON_NUMERIC   10
#define E_INDEX_BOUNDS  11
#define E_NO_FIELD      12
#define E_UNRESOLVABLE  13
#define E_STATIC_ARRAY  14
#define E_NOT_METHOD    15
#define E_WRONG_NUM_ARG 16
#define E_REF_TYPE      17
#define E_EXIT          18
#define E_BREAK         19

#include <exception>
#include <list>

class InterpEx : public std::exception {
public:
    InterpEx(int cause);
    ~InterpEx() throw ();

    void addTrace(int pos);
    int getCause();
    std::list<int> getTrace();
    const char* what() const throw();
    void printStackTrace();
private:
    std::list<int> trace;
    int cause;
};

#endif	/* _EXCEPTIONS_H */

