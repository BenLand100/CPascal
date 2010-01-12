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

class Type;
class Array;
class Pointer;
class Record;
class Meth;

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
#define TYPE_METH       9

#include <string>
#include <map>
#include <list>
#include "Variable.h"

class Method;

class Type {
public:
    static std::map<std::string,Type*>* alltypes();

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
    static Meth* getMethodType(Method* meth);
    static Type* getType(int name);

    const int type;
    const std::string descr;

    virtual bool instanceOf(Type* type);
    virtual int sizeOf(std::map<int,Type*> &typemap);
protected:
    Type(std::string descr, int type);
private:
    Type(int name); //only for reserved types
};

class Meth : public Type {
    friend class Type;
public:
    const Method* meth;

    bool instanceOf(Type* type);
    int sizeOf(std::map<int,Type*> &typemap);
protected:
    Meth(std::string descr, Method* meth);
};

class RefType : public Type {
    friend class Type;
public:
    const int name;

    bool instanceOf(Type* type);
    int sizeOf(std::map<int,Type*> &typemap);
protected:
    RefType(std::string descr, int name);
};

class Array : public Type {
    friend class Type;
public:
    const bool dynamic;
    Type* element;
    const int from;
    const int to;

    bool instanceOf(Type* type);
    int sizeOf(std::map<int,Type*> &typemap);
protected:
    Array(std::string descr, Type* element, int from, int to);
    Array(std::string descr, Type* element);
};

class Pointer : public Type {
    friend class Type;
public:
    Type* pointsTo;

    bool instanceOf(Type* type);
    int sizeOf(std::map<int,Type*> &typemap);
protected:
    Pointer(std::string descr, Type* pointsTo);
};

class Record : public Type {
    friend class Type;
public:
    std::list<Variable*> fields;

    bool instanceOf(Type* type);
    int sizeOf(std::map<int,Type*> &typemap);
protected:
    int size;
    Record(std::string descr, std::list<Variable*> fields);
};

#endif	/* _TYPE_H */

