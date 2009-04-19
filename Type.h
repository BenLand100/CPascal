class Type;
class Array;
class Pointer;
class Record;

#ifndef _TYPE_H
#define	_TYPE_H

#define TYPE_NIL        -1
#define TYPE_STRING     0
#define TYPE_INTEGER    1
#define TYPE_REAL       2
#define TYPE_BOOLEAN    3
#define TYPE_CHAR       4
#define TYPE_REF        5
#define TYPE_RECORD     6
#define TYPE_ARRAY      7
#define TYPE_POINTER    8

#include <string>
#include <map>
#include <list>
#include "Variable.h"

class Type {
public:
    static Type* getNil();
    static Type* getString();
    static Type* getInteger();
    static Type* getReal();
    static Type* getBoolean();
    static Type* getChar();
    static Record* getRecordType(std::map<int,Type*> fields);
    static Array* getBoundArrayType(Type* element, int from, int to);
    static Array* getDynamicArrayType(Type* element);
    static Pointer* getPointerType(Type* type);
    static Type* getType(int name);

    const int type;
    const std::string descr;

    virtual bool instanceOf(Type* type);
protected:
    static Type* nil;
    static Type* string;
    static Type* integer;
    static Type* real;
    static Type* boolean;
    static Type* chr;

    Type(std::string descr, int type);
private:
    static std::map<std::string,Type*> types;

    Type(int name); //only for reserved types
};

class Array : public Type {
    friend class Type;
public:
    const bool dynamic;
    const Type* element;
    const int from;
    const int to;

    bool instanceOf(Type* type);
protected:
    Array(std::string descr, Type* element, int from, int to);
    Array(std::string descr, Type* element);
};

class Pointer : public Type {
    friend class Type;
public:
    const Type* pointsTo;

    bool instanceOf(Type* type);
protected:
    Pointer(std::string descr, Type* pointsTo);
};

class Record : public Type {
    friend class Type;
public:
    std::list<Variable*> fields;

    bool instanceOf(Type* type);
protected:
    Record(std::string descr, std::list<Variable*> fields);
};

#endif	/* _TYPE_H */

