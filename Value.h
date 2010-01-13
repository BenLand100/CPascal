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

class Value;
class StringValue;
class IntegerValue;
class RealValue;
class CharValue;
class BooleanValue;
class ArrayValue;
class RecordValue;
class PointerValue;

#include <string>
#include <stack>
#include "Element.h"
#include "Type.h"
#include "Exceptions.h"
#include "Interpreter.h"

#ifndef _VALUE_H
#define	_VALUE_H

class Value : public Element {
public:
    static Value* fromType(Type* type, std::map<int,Type*> &typemap);
    static Value* fromTypeMem(Type* type, std::map<int,Type*> &typemap, void* mem);

    const Type* typeObj;
    const int   type;

    Value();
    virtual ~Value();

    virtual void set(Value* val) throw(int,InterpEx*);
    virtual Value* clone();
    virtual Value* duplicate();
    virtual Value* getRef() throw(int,InterpEx*);
    virtual void setRef(Value* ref) throw(int,InterpEx*);
    virtual void negate() throw(int,InterpEx*);
    virtual void incr() throw(int,InterpEx*);
    virtual void decr() throw(int,InterpEx*);
    virtual Value* getField(int name) throw(int,InterpEx*);
    virtual void setField(int name, Value* val) throw(int,InterpEx*);
    virtual int size() throw(int,InterpEx*);
    virtual void resize(int size, std::map<int,Type*>& typemap) throw(int,InterpEx*);
    virtual void setIndex(int index, Value* val) throw(int,InterpEx*);
    virtual Value* getIndex(int index) throw(int,InterpEx*);
    virtual char* asString() throw(int,InterpEx*);
    virtual char asChar() throw(int,InterpEx*);
    virtual double asReal() throw(int,InterpEx*);
    virtual int asInteger() throw(int,InterpEx*);
    virtual bool asBoolean() throw(int,InterpEx*);
    virtual Value* invoke(Value** args, int numArgs, Frame* frame) throw (int,InterpEx*);

    virtual int argSize();
    virtual void refArg(void* mem);
    virtual void valArg(void* mem);
    virtual void read_c(void* res);
    virtual void read_fpc(void* res);

protected:
    Value(int impl_type, Type* impl_typeObj);
    int* refcount;
    bool* owns_mem;
private:
    Value(Value &val);
};

class MethodValue : public Value {
    friend class PointerValue;
public:
    MethodValue(void* mem, Meth* type);
    MethodValue(Method* meth);
    ~MethodValue();

    Value* duplicate();
    Value* clone();
    void set(Value* val) throw(int,InterpEx*);
    Value* invoke(Value** args, int numArgs, Frame* frame) throw (int,InterpEx*);

protected:
    Method** meth;

private:
    MethodValue(MethodValue &val);
};

class IntegerValue : public Value {
public:
    IntegerValue(void* mem);
    IntegerValue(int i);
    ~IntegerValue();
    
    Value* duplicate();
    Value* clone();
    void set(Value* val) throw(int,InterpEx*);
    void negate() throw(int,InterpEx*);
    void incr() throw(int,InterpEx*);
    void decr() throw(int,InterpEx*);
    double asReal() throw(int,InterpEx*);
    int asInteger() throw(int,InterpEx*);

    int argSize();
    void refArg(void* mem);
    void valArg(void* mem);
    void read_c(void* res);
    void read_fpc(void* res);

private:
    int* integer;
    IntegerValue(IntegerValue &val);
};

class StringValue : public Value {
public:
    StringValue(void* mem);
    StringValue(char* str);
    StringValue(std::string str);
    ~StringValue();

    Value* duplicate();
    Value* clone();
    void set(Value* val) throw(int,InterpEx*);
    char* asString() throw(int,InterpEx*);
    Value* getIndex(int index) throw(int,InterpEx*);
    void setIndex(int index, Value* val) throw(int,InterpEx*);

    int argSize();
    void refArg(void* mem);
    void valArg(void* mem);
    void read_c(void* res);
    void read_fpc(void* res);

private:
    char** mem;
    int** objref;
    int** size;
    char** str;
    StringValue(StringValue &val);
};

class RealValue : public Value {
public:
    RealValue(void* mem);
    RealValue(double real);
    ~RealValue();

    Value* duplicate();
    Value* clone();
    void set(Value* val) throw(int,InterpEx*);
    void negate() throw(int,InterpEx*);
    void incr() throw(int,InterpEx*);
    void decr() throw(int,InterpEx*);
    double asReal() throw(int,InterpEx*);

    int argSize();
    void refArg(void* mem);
    void valArg(void* mem);

private:
    double* real;
    RealValue(RealValue &val);
};

class CharValue : public Value {
public:
    CharValue(void* mem);
    CharValue(char c);
    ~CharValue();

    Value* duplicate();
    Value* clone();
    void set(Value* val) throw(int,InterpEx*);
    void incr() throw(int,InterpEx*);
    void decr() throw(int,InterpEx*);
    char asChar() throw(int,InterpEx*);
    int asInteger() throw(int,InterpEx*);
    char* asString() throw(int,InterpEx*);

    int argSize();
    void refArg(void* mem);
    void valArg(void* mem);
    void read_c(void* res);
    void read_fpc(void* res);

private:
    char* chr;
    char** str; //temp
    CharValue(CharValue &val);
};

class BooleanValue : public Value {
public:
    BooleanValue(void* mem);
    BooleanValue(bool b);
    ~BooleanValue();

    Value* duplicate();
    Value* clone();
    void set(Value* val) throw(int,InterpEx*);
    bool asBoolean() throw(int,InterpEx*);
    int asInteger() throw(int,InterpEx*);

    int argSize();
    void refArg(void* mem);
    void valArg(void* mem);
    void read_c(void* res);
    void read_fpc(void* res);

private:
    char* boolean;
    BooleanValue(BooleanValue &val);
};

class ArrayValue : public Value {
public:
    ArrayValue(Array* arr, std::map<int,Type*> &typemap, void* mem);
    ArrayValue(Array* arr, std::map<int,Type*> &typemap);
    ~ArrayValue();

    Value* duplicate();
    Value* clone();
    void set(Value* val) throw(int,InterpEx*);
    int size() throw(int,InterpEx*);
    void resize(int size, std::map<int,Type*>& typemap) throw(int,InterpEx*);
    Value* getIndex(int index) throw(int,InterpEx*);
    void setIndex(int index, Value* val) throw(int,InterpEx*);

    int argSize();
    void refArg(void* mem);
    void valArg(void* mem);

private:
    Type** elemType;
    bool* dynamic;
    int* start;
    int* elemsz;

    char** mem;
    int** objref;
    int** asize;
    Value*** array;
    char** pas_array;
    ArrayValue(Array* arr);
    ArrayValue(ArrayValue &val);
};

class PointerValue : public Value {
public:
    PointerValue(Pointer* pt, std::map<int,Type*> &typemap, void* mem);
    PointerValue(Pointer* pt, std::map<int,Type*> &typemap);
    PointerValue(Value* ref);
    ~PointerValue();

    Value* duplicate();
    Value* clone();
    void set(Value* val) throw(int,InterpEx*);
    Value* getRef() throw(int,InterpEx*);
    void setRef(Value* ref) throw(int,InterpEx*);
    Value* invoke(Value** args, int numArgs, Frame* frame) throw (int,InterpEx*);

    int argSize();
    void refArg(void* mem);
    void valArg(void* mem);

private:
    Value** ref;
    void** pas_ref;
    Type** refType;
    PointerValue(PointerValue &val);
    PointerValue(Pointer* pt);
};

class RecordValue : public Value {
public:
    RecordValue(Record* rec, std::map<int,Type*> &typemap, void* mem);
    RecordValue(Record* rec, std::map<int,Type*> &typemap);
    ~RecordValue();

    Value* duplicate();
    Value* clone();
    void set(Value* val) throw(int,InterpEx*);
    Value* getField(int name) throw(int,InterpEx*);
    void setField(int name, Value* value) throw(int,InterpEx*);

    int argSize();
    void refArg(void* mem);
    void valArg(void* mem);

private:
    int** objrefcount;
    int* memsize;
    int** indexes;
    char** mem;
    std::map<int,Value*>* fields;
    RecordValue(RecordValue &val);
    RecordValue(Record* rec);
};

#endif	/* _VALUE_H */

