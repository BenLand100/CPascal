#include "Exceptions.h"

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
    }
}

