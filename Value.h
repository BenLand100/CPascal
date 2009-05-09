class Value;
class StringValue;
class IntegerValue;
class RealValue;
class CharValue;
class BooleanValue;
class ArrayValue;
class RecordValue;
class PointerValue;

#ifndef _VALUE_H
#define	_VALUE_H

#include <string>
#include "Element.h"
#include "Type.h"

class Value : public Element {
public:
    static Value* fromType(Type* type, std::map<int,Type*> &typemap);

    const Type* typeObj;
    const int   type;

    Value();
    virtual ~Value();

    virtual void set(Value* val);
    virtual Value* clone(); 
    virtual Value* duplicate();
    virtual Value* getRef();
    virtual void negate();
    virtual void incr();
    virtual void decr();
    virtual Value* getField(int name);
    virtual void setField(int name, Value* val);
    virtual int size();
    virtual void resize(int size, std::map<int,Type*>& typemap);
    virtual void setIndex(int index, Value* val);
    virtual Value* getIndex(int index);
    virtual std::string asString();
    virtual char asChar();
    virtual double asReal();
    virtual int asInteger();
    virtual bool asBoolean();

    virtual int bytes();
    virtual void store(void* mem);
    virtual void read(void* mem);

protected:
    Value(int impl_type, Type* impl_typeObj);
    int* refcount;
private:
    Value(Value &val);
};

class IntegerValue : public Value {
public:
    IntegerValue(int i);
    ~IntegerValue();
    
    Value* duplicate();
    Value* clone();
    void set(Value* val);
    void negate();
    void incr();
    void decr();
    double asReal();
    int asInteger();

    int bytes();
    void store(void* mem);
    void read(void* mem);

private:
    int* integer;
    IntegerValue(IntegerValue &val);
};

class StringValue : public Value {
public:
    StringValue(char* str);
    StringValue(std::string str);
    ~StringValue();

    Value* duplicate();
    Value* clone();
    void set(Value* val);
    std::string asString();

    int bytes();
    void store(void* mem);
    void read(void* mem);

private:
    std::string* str;
    StringValue(StringValue &val);
};

class RealValue : public Value {
public:
    RealValue(double real);
    ~RealValue();
    
    Value* duplicate();
    Value* clone();
    void set(Value* val);
    void negate();
    void incr();
    void decr();
    double asReal();

    int bytes();
    void store(void* mem);
    void read(void* mem);

private:
    double* real;
    RealValue(RealValue &val);
};

class CharValue : public Value {
public:
    CharValue(char c);
    ~CharValue();

    Value* duplicate();
    Value* clone();
    void set(Value* val);
    void incr();
    void decr();
    char asChar();
    int asInteger();
    std::string asString();

    int bytes();
    void store(void* mem);
    void read(void* mem);

private:
    char* chr;
    CharValue(CharValue &val);
};

class BooleanValue : public Value {
public:
    BooleanValue(bool b);
    ~BooleanValue();
    
    Value* duplicate();
    Value* clone();
    void set(Value* val);
    bool asBoolean();
    int asInteger();

    int bytes();
    void store(void* mem);
    void read(void* mem);

private:
    bool* boolean;
    BooleanValue(BooleanValue &val);
};

class ArrayValue : public Value {
public:
    ArrayValue(Array* arr, std::map<int,Type*> &typemap);
    ~ArrayValue();

    Value* duplicate();
    Value* clone();
    void set(Value* val);
    int size();
    void resize(int size, std::map<int,Type*>& typemap);
    Value* getIndex(int index);
    void setIndex(int index, Value* val);

private:
    Type** elemType;
    int** objrefcount;
    Value*** array;
    bool* dynamic;
    int* start;
    int* asize;
    ArrayValue(Array* arr);
    ArrayValue(ArrayValue &val);
};

class PointerValue : public Value {
public:
    PointerValue(Pointer* pt, std::map<int,Type*> &typemap);
    PointerValue(Value* ref);
    ~PointerValue();

    Value* duplicate();
    Value* clone();
    void set(Value* val);
    Value* getRef();

private:
    Value** ref;
    Type** refType;
    PointerValue(PointerValue &val);
    PointerValue(Pointer* pt);
};

class RecordValue : public Value {
public:
    RecordValue(Record* rec, std::map<int,Type*> &typemap);
    ~RecordValue();

    Value* duplicate();
    Value* clone();
    void set(Value* val);
    Value* getField(int name);
    void setField(int name, Value* value);

private:
    int** objrefcount;
    std::map<int,Value*>** fields;
    std::map<int,Type*>** types;
    RecordValue(RecordValue &val);
    RecordValue(Record* rec);
};

#endif	/* _VALUE_H */

