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

#include "Exceptions.h"
#include <cstdio>

InterpEx::InterpEx(int cause_impl) : cause(cause_impl) {

}

InterpEx::~InterpEx() throw() {

}

void InterpEx::addTrace(int pos) {
    trace.push_back(pos);
}

int InterpEx::getCause() {
    return cause;
}

std::list<int> InterpEx::getTrace() {
    return trace;
}

void InterpEx::printStackTrace() {
    std::list<int>::iterator trace_iter = trace.begin();
    std::list<int>::iterator trace_end = trace.end();
    printf("Exception at position %i: %s\n", *trace_iter, what());
    trace_iter++;
    while (trace_iter != trace_end) {
        printf("\t-%i\n",*trace_iter);
        trace_iter++;
    }
}

const char* InterpEx::what() const throw() {
    switch (cause) {
        case E_DIV_ZERO:
            return "Divide by zero";
        case E_NOT_INTEGER:
            return "Integer expected";
        case E_NOT_REAL:
            return "Real expected";
        case E_NOT_CHAR:
            return "Char expected";
        case E_NOT_STRING:
            return "String expected";
        case E_NOT_BOOLEAN:
            return "Boolean expected";
        case E_NOT_RECORD:
            return "Record expected";
        case E_NOT_ARRAY:
            return "Array expected";
        case E_NOT_POINTER:
            return "Pointer expected";
        case E_NULL_VAL:
            return "Null value ";
        case E_NON_NUMERIC:
            return "Numeric value expected";
        case E_INDEX_BOUNDS:
            return "Index out of bounds";
        case E_NO_FIELD:
            return "No such field";
        case E_UNRESOLVABLE:
            return "Symbol undefined";
        case E_STATIC_ARRAY:
            return "Array is not dynamic";
        case E_NOT_METHOD:
            return "Cannot invoke a non-method";
        case E_WRONG_NUM_ARG:
            return "Wrong number of arguments to method";
        default:
            return "Unknown error!";
    }
}

