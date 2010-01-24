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


//Runtime Exceptions
#define E_RUNTIME       0
#define E_DIV_ZERO      E_RUNTIME + 0
#define E_NOT_INTEGER   E_RUNTIME + 1
#define E_NOT_REAL      E_RUNTIME + 2
#define E_NOT_CHAR      E_RUNTIME + 3
#define E_NOT_STRING    E_RUNTIME + 4
#define E_NOT_BOOLEAN   E_RUNTIME + 5
#define E_NOT_RECORD    E_RUNTIME + 6
#define E_NOT_ARRAY     E_RUNTIME + 7
#define E_NOT_POINTER   E_RUNTIME + 8
#define E_NULL_VAL      E_RUNTIME + 9
#define E_NON_NUMERIC   E_RUNTIME + 10
#define E_INDEX_BOUNDS  E_RUNTIME + 11
#define E_NO_FIELD      E_RUNTIME + 12
#define E_UNRESOLVABLE  E_RUNTIME + 13
#define E_STATIC_ARRAY  E_RUNTIME + 14
#define E_NOT_METHOD    E_RUNTIME + 15
#define E_WRONG_NUM_ARG E_RUNTIME + 16
#define E_REF_TYPE      E_RUNTIME + 17

//Non-catchable exceptions
#define E_NOCATCH       100
#define E_EXIT          E_NOCATCH + 0
#define E_BREAK         E_NOCATCH + 1

//Parser exceptions
#define E_PARSER        200

#include <exception>
#include <list>

class InterpEx : public std::exception {
public:
    static int getType(int cause);

    InterpEx(int cause);
    ~InterpEx() throw ();

    void addTrace(int pos);
    int getType();
    int getCause();
    std::list<int> getTrace();
    const char* what() const throw();
    void printStackTrace(char *ppg);
private:
    std::list<int> trace;
    int cause;
};

#endif	/* _EXCEPTIONS_H */

