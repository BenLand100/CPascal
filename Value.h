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

typedef void* (*MALLOC)(int size) __attribute__((stdcall));
typedef void* (*REALLOC)(void* mem, int size) __attribute__((stdcall));
typedef void (*FREE)(void* mem) __attribute__((stdcall));

typedef struct {
    int refcount;
    char data[0];
} fpc_memory;

typedef struct {
    int refcount;
    int size;
    char data[0];
} fpc_array;

class Value : public Element {
public:
    static MALLOC malloc;
    static REALLOC realloc;
    static FREE free;

    static Value* decref(Value *val) throw(int,InterpEx*);
    static Value* incref(Value *val) throw(int,InterpEx*);
    static Value* fromType(Type* type) throw(int,InterpEx*);
    static Value* fromTypeMem(Type* type, void* mem) throw(int,InterpEx*);

    const Type* typeObj;
    const int   type;

    Value();

    virtual void set(Value* val) throw(int,InterpEx*);
    virtual Value* clone();
    virtual Value* getRef() throw(int,InterpEx*);
    virtual void setRef(Value* ref) throw(int,InterpEx*);
    virtual void negate() throw(int,InterpEx*);
    virtual void incr() throw(int,InterpEx*);
    virtual void decr() throw(int,InterpEx*);
    virtual Value* getField(int name) throw(int,InterpEx*);
    virtual void setField(int name, Value* val) throw(int,InterpEx*);
    virtual int size() throw(int,InterpEx*);
    virtual void resize(int size) throw(int,InterpEx*);
    virtual void setIndex(int index, Value* val) throw(int,InterpEx*);
    virtual Value* getIndex(int index) throw(int,InterpEx*);
    virtual char* asString() throw(int,InterpEx*);
    virtual char asChar() throw(int,InterpEx*);
    virtual double asReal() throw(int,InterpEx*);
    virtual int asInteger() throw(int,InterpEx*);
    virtual bool asBoolean() throw(int,InterpEx*);
    virtual Value* invoke(Value** args, unsigned int numArgs, Frame* frame) throw (int,InterpEx*);

    virtual int argSize();
    virtual void refArg(void* mem) throw(int,InterpEx*);
    virtual void valArg(void* mem) throw(int,InterpEx*);
    virtual void read_c(void* res) throw(int,InterpEx*);
    virtual void read_fpc(void* res) throw(int,InterpEx*);

protected:
    Value(int impl_type, Type* impl_typeObj, bool ownsmem);
    virtual ~Value();
    unsigned int refcount;
    bool owns_mem;

private:
    Value(Value &val);
};

class MethodValue : public Value {
    friend class PointerValue;
public:
    MethodValue(void* mem, Meth* type);
    MethodValue(Method* meth);

    Value* clone();
    void set(Value* val) throw(int,InterpEx*);
    Value* invoke(Value** args, unsigned int numArgs, Frame* frame) throw (int,InterpEx*);

protected:
    ~MethodValue();
    Method** meth;
};

class IntegerValue : public Value {
public:
    IntegerValue(void* mem);
    IntegerValue(int i);
    
    Value* clone();
    void set(Value* val) throw(int,InterpEx*);
    void negate() throw(int,InterpEx*);
    void incr() throw(int,InterpEx*);
    void decr() throw(int,InterpEx*);
    double asReal() throw(int,InterpEx*);
    int asInteger() throw(int,InterpEx*);

    int argSize();
    void refArg(void* mem) throw(int,InterpEx*);
    void valArg(void* mem) throw(int,InterpEx*);
    void read_c(void* res) throw(int,InterpEx*);
    void read_fpc(void* res) throw(int,InterpEx*);

protected:
    ~IntegerValue();

private:
    int* integer;
};

class StringValue : public Value {
public:
    StringValue(void* mem);
    StringValue(char* str);
    StringValue(std::string str);

    Value* clone();
    void set(Value* val) throw(int,InterpEx*);
    char* asString() throw(int,InterpEx*);
    Value* getIndex(int index) throw(int,InterpEx*);
    void setIndex(int index, Value* val) throw(int,InterpEx*);
    void resize(int size) throw(int,InterpEx*);
    int size() throw(int,InterpEx*);

    int argSize();
    void refArg(void* mem) throw(int,InterpEx*);
    void valArg(void* mem) throw(int,InterpEx*);
    void read_c(void* res) throw(int,InterpEx*);
    void read_fpc(void* res) throw(int,InterpEx*);

protected:
    ~StringValue();

private:
    char** data;
};

class RealValue : public Value {
public:
    RealValue(void* mem);
    RealValue(double real);

    Value* clone();
    void set(Value* val) throw(int,InterpEx*);
    void negate() throw(int,InterpEx*);
    void incr() throw(int,InterpEx*);
    void decr() throw(int,InterpEx*);
    double asReal() throw(int,InterpEx*);

    int argSize();
    void refArg(void* mem) throw(int,InterpEx*);
    void valArg(void* mem) throw(int,InterpEx*);

protected:
    ~RealValue();

private:
    double* real;
};

class CharValue : public Value {
public:
    CharValue(void* mem);
    CharValue(char c);

    Value* clone();
    void set(Value* val) throw(int,InterpEx*);
    void incr() throw(int,InterpEx*);
    void decr() throw(int,InterpEx*);
    char asChar() throw(int,InterpEx*);
    int asInteger() throw(int,InterpEx*);
    char* asString() throw(int,InterpEx*);

    int argSize();
    void refArg(void* mem) throw(int,InterpEx*);
    void valArg(void* mem) throw(int,InterpEx*);
    void read_c(void* res) throw(int,InterpEx*);
    void read_fpc(void* res) throw(int,InterpEx*);

protected:
    ~CharValue();

private:
    char* chr;
    char** str; //temp
};

class BooleanValue : public Value {
public:
    BooleanValue(void* mem);
    BooleanValue(bool b);

    Value* clone();
    void set(Value* val) throw(int,InterpEx*);
    bool asBoolean() throw(int,InterpEx*);
    int asInteger() throw(int,InterpEx*);

    int argSize();
    void refArg(void* mem) throw(int,InterpEx*);
    void valArg(void* mem) throw(int,InterpEx*);
    void read_c(void* res) throw(int,InterpEx*);
    void read_fpc(void* res) throw(int,InterpEx*);

protected:
    ~BooleanValue();

private:
    char* boolean;
};

class ArrayValue : public Value {
public:
    ArrayValue(Array* arr, void* mem);
    ArrayValue(Array* arr);

    Value* clone();
    void set(Value* val) throw(int,InterpEx*);
    int size() throw(int,InterpEx*);
    void resize(int size) throw(int,InterpEx*);
    Value* getIndex(int index) throw(int,InterpEx*);
    void setIndex(int index, Value* val) throw(int,InterpEx*);

    int argSize();
    void refArg(void* mem) throw(int,InterpEx*);
    void valArg(void* mem) throw(int,InterpEx*);

protected:
    ~ArrayValue();

private:
    Type *elemType;
    bool dynamic;
    int start;
    int elemsz;

    char** data;
    Value** array;
    ArrayValue(Array* arr, bool internal);
};

class PointerValue : public Value {
public:
    PointerValue(Pointer* pt, void* mem);
    PointerValue(Pointer* pt);
    PointerValue(Value* ref);

    Value* clone();
    void set(Value* val) throw(int,InterpEx*);
    Value* getRef() throw(int,InterpEx*);
    void setRef(Value* ref) throw(int,InterpEx*);
    Value* invoke(Value** args, int numArgs, Frame* frame) throw (int,InterpEx*);

    int argSize();
    void refArg(void* mem) throw(int,InterpEx*);
    void valArg(void* mem) throw(int,InterpEx*);

protected:
    ~PointerValue();

private:
    Value* ref;
    void** pas_ref;
    Type* refType;
    PointerValue(Pointer* pt, bool internal);
};

class RecordValue : public Value {
public:
    RecordValue(Record* rec, void* mem);
    RecordValue(Record* rec);

    Value* clone();
    void set(Value* val) throw(int,InterpEx*);
    Value* getField(int slot) throw(int,InterpEx*);
    void setField(int slot, Value* value) throw(int,InterpEx*);

    int argSize();
    void refArg(void* mem) throw(int,InterpEx*);
    void valArg(void* mem) throw(int,InterpEx*);

protected:
    ~RecordValue();

private:
    int memsize;
    char* mem;
    int len;
    int* indexes;
    Value** slots;
    RecordValue(Record* rec, bool internal);
};

#endif	/* _VALUE_H */

