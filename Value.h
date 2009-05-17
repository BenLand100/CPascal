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
    static Value* fromTypeMem(Type* type, std::map<int,Type*> &typemap, void* mem);

    const Type* typeObj;
    const int   type;

    Value();
    virtual ~Value();

    virtual void set(Value* val) throw(int);
    virtual Value* clone();
    virtual Value* duplicate();
    virtual Value* getRef() throw(int);
    virtual void setRef(Value* ref) throw(int);
    virtual void negate() throw(int);
    virtual void incr() throw(int);
    virtual void decr() throw(int);
    virtual Value* getField(int name) throw(int);
    virtual void setField(int name, Value* val) throw(int);
    virtual int size() throw(int);
    virtual void resize(int size, std::map<int,Type*>& typemap) throw(int);
    virtual void setIndex(int index, Value* val) throw(int);
    virtual Value* getIndex(int index) throw(int);
    virtual char* asString() throw(int);
    virtual char asChar() throw(int);
    virtual double asReal() throw(int);
    virtual int asInteger() throw(int);
    virtual bool asBoolean() throw(int);

    virtual int valSize();
    virtual int argSize();
    virtual void refArg(void* mem);
    virtual void valArg(void* mem);
    virtual void read_c(void* res);
    virtual void read_fpc(void* res);

protected:
    Value(int impl_type, Type* impl_typeObj);
    int* refcount;
private:
    Value(Value &val);
};

class IntegerValue : public Value {
public:
    IntegerValue(void* mem);
    IntegerValue(int i);
    ~IntegerValue();
    
    Value* duplicate();
    Value* clone();
    void set(Value* val) throw(int);
    void negate() throw(int);
    void incr() throw(int);
    void decr() throw(int);
    double asReal() throw(int);
    int asInteger() throw(int);

    int valSize();
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
    void set(Value* val) throw(int);
    char* asString() throw(int);

    int valSize();
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
    void set(Value* val) throw(int);
    void negate() throw(int);
    void incr() throw(int);
    void decr() throw(int);
    double asReal() throw(int);

    int valSize();
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
    void set(Value* val) throw(int);
    void incr() throw(int);
    void decr() throw(int);
    char asChar() throw(int);
    int asInteger() throw(int);
    char* asString() throw(int);

    int valSize();
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
    void set(Value* val) throw(int);
    bool asBoolean() throw(int);
    int asInteger() throw(int);

    int valSize();
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
    ArrayValue(Array* arr, std::map<int,Type*> &typemap);
    ~ArrayValue();

    Value* duplicate();
    Value* clone();
    void set(Value* val) throw(int);
    int size() throw(int);
    void resize(int size, std::map<int,Type*>& typemap) throw(int);
    Value* getIndex(int index) throw(int);
    void setIndex(int index, Value* val) throw(int);

private:
    Type** elemType;
    bool* dynamic;
    int* start;

    Value** temp;
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
    PointerValue(Pointer* pt, std::map<int,Type*> &typemap);
    PointerValue(Value* ref);
    ~PointerValue();

    Value* duplicate();
    Value* clone();
    void set(Value* val) throw(int);
    Value* getRef() throw(int);
    void setRef(Value* ref) throw(int);

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
    void set(Value* val) throw(int);
    Value* getField(int name) throw(int);
    void setField(int name, Value* value) throw(int);

private:
    int** objrefcount;
    std::map<int,Value*>** fields;
    std::map<int,Type*>** types;
    RecordValue(RecordValue &val);
    RecordValue(Record* rec);
};

#endif	/* _VALUE_H */

