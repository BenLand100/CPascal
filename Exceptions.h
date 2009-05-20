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

