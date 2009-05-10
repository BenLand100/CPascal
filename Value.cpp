#include "Value.h"
#include "Exceptions.h"

#include <iostream>
//#define debug(x) std::cout << x << '\n'
#define debug(x)

Value* Value::fromType(Type* type, std::map<int,Type*> &typemap) {
    std::map<int,Type*>::iterator end = typemap.end();
    debug("fromType " << type->type);
    while (type->type == TYPE_REF && typemap.find(((RefType*)type)->name) != end)
        type = typemap[((RefType*)type)->name];
    if (type->type == TYPE_REF) return 0;
    debug("fromType " << type->type);
    switch (type->type) {
        case TYPE_STRING:
            return new StringValue((char*)"");
        case TYPE_CHAR:
            return new CharValue('\0');
        case TYPE_INTEGER:
            return new IntegerValue(0);
        case TYPE_REAL:
            return new RealValue(0.0);
        case TYPE_BOOLEAN:
            return new BooleanValue(false);
        case TYPE_RECORD:
            return new RecordValue((Record*)type, typemap);
        case TYPE_ARRAY:
            return new ArrayValue((Array*)type, typemap);
        case TYPE_POINTER:
            return new PointerValue((Pointer*)type, typemap);
        default:
            return 0;
    }
}

Value::Value() : Element(ELEM_VALUE), type(TYPE_NIL), typeObj(Type::getNil()) { }
Value::Value(int impl_type, Type* impl_typeObj) : Element(ELEM_VALUE), type(impl_type), typeObj(impl_typeObj) { }
Value::~Value() { }
void Value::set(Value* val) throw(int) { throw E_NULL_VAL; }
Value* Value::clone() { return new Value(); }
Value* Value::duplicate() { return new Value(); }
Value* Value::getRef() throw(int) { throw E_NOT_POINTER; }
void Value::setRef(Value* ref) throw(int) { throw E_NOT_POINTER; }
void Value::negate() throw(int) { throw E_NON_NUMERIC; }
void Value::incr() throw(int) { throw E_NON_NUMERIC; }
void Value::decr() throw(int) { throw E_NON_NUMERIC; }
Value* Value::getField(int name) throw(int) { throw E_NOT_RECORD; }
void Value::setField(int name, Value* val)  throw(int) { throw E_NOT_RECORD; }
int Value::size() throw(int) { throw E_NOT_ARRAY; }
void Value::resize(int size, std::map<int,Type*>& typemap) throw(int) { throw E_NOT_ARRAY; }
void Value::setIndex(int index, Value* val) throw(int) { throw E_NOT_ARRAY; }
Value* Value::getIndex(int index) throw(int) { throw E_NOT_ARRAY; }
std::string Value::asString() throw(int) { throw E_NOT_STRING; }
char Value::asChar() throw(int) { throw E_NOT_CHAR; }
double Value::asReal() throw(int) { throw E_NOT_REAL; }
int Value::asInteger() throw(int) { throw E_NOT_INTEGER; }
bool Value::asBoolean() throw(int) { throw E_NOT_BOOLEAN; }
int Value::bytes() { return 0; }
void Value::store(void* mem) { }
void Value::read(void* mem) { }

//**** BEGIN INTEGERVALUE DEFINITION ***

IntegerValue::IntegerValue(int i) : Value(TYPE_INTEGER,Type::getInteger()) {
    refcount = new int;
    *refcount = 0;
    integer = new int;
    *integer = i;
}

IntegerValue::IntegerValue(IntegerValue& val) : Value(TYPE_INTEGER,Type::getInteger()) {
    refcount = val.refcount;
    (*refcount)++;
    integer = val.integer;
}

IntegerValue::~IntegerValue() {
    if (*refcount) {
        (*refcount)--;
    } else {
        delete refcount;
        delete integer;
    }
}

Value* IntegerValue::duplicate() {
    return new IntegerValue(*this);
}

Value* IntegerValue::clone() {
    return new IntegerValue(*integer);
}

void IntegerValue::set(Value* val) throw(int) {
    if (val->type != TYPE_INTEGER) throw E_NOT_INTEGER;
    *integer = *((IntegerValue*)val)->integer;
}

void IntegerValue::negate() throw(int) {
    *integer = -(*integer);
}

void IntegerValue::incr() throw(int) {
    ++(*integer);
}

void IntegerValue::decr() throw(int) {
    --(*integer);
}

double IntegerValue::asReal() throw(int) {
    return *integer;
}

int IntegerValue::asInteger() throw(int) {
    return *integer;
}

int IntegerValue::bytes() { return 4; }
void IntegerValue::store(void* mem) { *(int*)mem = *integer; }
void IntegerValue::read(void* mem) { *integer = (int)mem; }

//**** BEGIN STRINGVALUE DEFINITION ***

StringValue::StringValue(char* str_impl) : Value(TYPE_STRING,Type::getString()) {
    refcount = new int;
    *refcount = 0;
    str = new std::string(str_impl);
}

StringValue::StringValue(std::string str_impl) : Value(TYPE_STRING,Type::getString()) {
    refcount = new int;
    *refcount = 0;
    str = new std::string(str_impl);
}

StringValue::StringValue(StringValue& val) : Value(TYPE_STRING,Type::getString()) {
    refcount = val.refcount;
    (*refcount)++;
    str = val.str;
}

StringValue::~StringValue() {
    if (*refcount) {
        (*refcount)--;
    } else {
        delete refcount;
        delete str;
    }
}

Value* StringValue::duplicate() {
    return new StringValue(*this);
}

Value* StringValue::clone() {
    return new StringValue(*str);
}

void StringValue::set(Value* val) throw(int) {
    if (val->type != TYPE_STRING) throw E_NOT_STRING;
    *str = *((StringValue*)val)->str;
}

std::string StringValue::asString() throw(int) {
    return *str;
}

int StringValue::bytes() { return 4; }
void StringValue::store(void* mem) { *(unsigned int*)mem = (unsigned int)str->c_str(); }
void StringValue::read(void* mem) {
    delete str;
    str = new std::string((char*)mem);
    delete (char*)mem;
}

//**** BEGIN REALVALUE DEFINITION ***

RealValue::RealValue(double real_impl) : Value(TYPE_REAL,Type::getReal()) {
    refcount = new int;
    *refcount = 0;
    real = new double;
    *real = real_impl;
}

RealValue::RealValue(RealValue& val) : Value(TYPE_REAL,Type::getReal()) {
    refcount = val.refcount;
    (*refcount)++;
    real = val.real;
}

RealValue::~RealValue() {
    if (*refcount) {
        (*refcount)--;
    } else {
        delete refcount;
        delete real;
    }
}

Value* RealValue::duplicate() {
    return new RealValue(*this);
}

Value* RealValue::clone() {
    return new RealValue(*real);
}

void RealValue::set(Value* val) throw(int) {
    if (val->type != TYPE_REAL) throw E_NOT_REAL;
    *real = *((RealValue*)val)->real;
}

void RealValue::negate() throw(int) {
    *real = -(*real);
}

void RealValue::incr() throw(int) {
    ++(*real);
}

void RealValue::decr() throw(int) {
    --(*real);
}

double RealValue::asReal() throw(int) {
    return *real;
}

int RealValue::bytes() { return 8; }
void RealValue::store(void* mem) { *(double*)mem =  *real; }
void RealValue::read(void* mem) {
    *real = *(double*)mem;
}

//**** BEGIN CHARVALUE DEFINITION ***

CharValue::CharValue(char chr_impl) : Value(TYPE_CHAR,Type::getChar()) {
    refcount = new int;
    *refcount = 0;
    chr = new char[2];
    chr[0] = chr_impl;
    chr[1] = 0;
}

CharValue::CharValue(CharValue& val) : Value(TYPE_CHAR,Type::getChar()) {
    refcount = val.refcount;
    (*refcount)++;
    chr = val.chr;
}

CharValue::~CharValue() {
    if (*refcount) {
        (*refcount)--;
    } else {
        delete refcount;
        delete chr;
    }
}

Value* CharValue::duplicate() {
    return new CharValue(*this);
}

Value* CharValue::clone() {
    return new CharValue(*chr);
}

void CharValue::set(Value* val) throw(int) {
    if (val->type != TYPE_CHAR) throw E_NOT_CHAR;
    *chr = *((CharValue*)val)->chr;
}

void CharValue::incr() throw(int) {
    ++(*chr);
}

void CharValue::decr() throw(int) {
    --(*chr);
}

char CharValue::asChar() throw(int) {
    return *chr;
}

int CharValue::asInteger() throw(int) {
    return *chr;
}

std::string CharValue::asString() throw(int) {
    std::string str(chr);
    return str;
}

int CharValue::bytes() { return 4; }
void CharValue::store(void* mem) { *(unsigned int*)mem = (unsigned int) *chr; }
void CharValue::read(void* mem) {
    *chr = (char) *(unsigned int*)mem;
}

//**** BEGIN BOOLEANVALUE DEFINITION ***

BooleanValue::BooleanValue(bool boolean_impl) : Value(TYPE_BOOLEAN,Type::getBoolean()) {
    refcount = new int;
    *refcount = 0;
    boolean = new bool;
    *boolean = boolean_impl;
}

BooleanValue::BooleanValue(BooleanValue& val) : Value(TYPE_BOOLEAN,Type::getBoolean()) {
    refcount = val.refcount;
    (*refcount)++;
    boolean = val.boolean;
}

BooleanValue::~BooleanValue() {
    if (*refcount) {
        (*refcount)--;
    } else {
        delete refcount;
        delete boolean;
    }
}

Value* BooleanValue::duplicate() {
    return new BooleanValue(*this);
}

Value* BooleanValue::clone() {
    return new BooleanValue(*boolean);
}

void BooleanValue::set(Value* val) throw(int) {
    if (val->type != TYPE_BOOLEAN) throw E_NOT_BOOLEAN;
    *boolean = *((BooleanValue*)val)->boolean;
}

bool BooleanValue::asBoolean() throw(int) {
    return *boolean;
}

int BooleanValue::asInteger() throw(int) {
    return *boolean;
}

int BooleanValue::bytes() { return 4; }
void BooleanValue::store(void* mem) { *(unsigned int*)mem = (unsigned int) *boolean; }
void BooleanValue::read(void* mem) {
    *boolean = (bool) *(unsigned int*)mem;
}

//**** BEGIN ARRAYVALUE DEFINITION ***

ArrayValue::ArrayValue(Array* arr, std::map<int,Type*> &typemap) : Value(TYPE_ARRAY,arr) {
    refcount = new int;
    *refcount = 0;
    elemType = new Type*;
    *elemType = (Type*)arr->element;
    dynamic = new bool;
    *dynamic = arr->dynamic;
    asize = new int;
    *asize = arr->to - arr->from + 1; //to == -1 and from == 0 for dynamic just for this
    start = new int;
    *start = arr->from;
    objrefcount = new int*;
    *objrefcount = new int;
    **objrefcount = 0;
    array = new Value**;
    *array = new Value*[*asize];
    for (int i = 0; i < *asize; i++) {
        (*array)[i] = Value::fromType(*elemType,typemap);
    }
}

ArrayValue::ArrayValue(Array* arr) : Value(TYPE_ARRAY,arr) {
    elemType = new Type*;
    dynamic = new bool;
    asize = new int;
    start = new int;
    objrefcount = new int*;
    array = new Value**;
}

ArrayValue::ArrayValue(ArrayValue& val) : Value(TYPE_ARRAY,(Type*)val.typeObj) {
    refcount = val.refcount;
    (*refcount)++;
    elemType = val.elemType;
    dynamic = val.dynamic;
    asize = val.asize;
    start = val.start;
    objrefcount = val.objrefcount;
    array = val.array;
}

ArrayValue::~ArrayValue() {
    if (*refcount) {
        (*refcount)--;
    } else {
        if (**objrefcount) {
            (**objrefcount)--;
        } else {
            for (int i = 0; i < *asize; i++) {
                delete (*array)[i];
            }
            delete *array;
            delete *objrefcount;
        }
        delete elemType;
        delete refcount;
        delete dynamic;
        delete start;
        delete asize;
        delete array;
        delete objrefcount;
    }
}

Value* ArrayValue::duplicate() {
    return new ArrayValue(*this);
}

Value* ArrayValue::clone() {
    ArrayValue* arr = new ArrayValue((Array*)typeObj);
    *(arr->elemType) = *elemType;
    *(arr->dynamic) = *dynamic;
    *(arr->asize) = *asize;
    *(arr->start) = *start;
    *(arr->objrefcount) = *objrefcount;
    (**objrefcount)++;
    *(arr->array) = *array;
}

void ArrayValue::set(Value* val) throw(int) {
    if (val->type != TYPE_ARRAY) throw E_NOT_ARRAY;
    ArrayValue* arr = (ArrayValue*)val;
    if (**objrefcount) {
        (**objrefcount)--;
    } else {
        for (int i = 0; i < *asize; i++) {
            delete (*array)[i];
        }
        delete *array;
        delete *objrefcount;
    }
    *objrefcount = *arr->objrefcount;
    (**objrefcount)++;
    *array = *arr->array;
    *elemType = *arr->elemType;
    *dynamic = *arr->dynamic;
    *asize = *arr->asize;
    *start = *arr->start;
}

int ArrayValue::size() throw(int) {
    return *asize;
}

void ArrayValue::resize(int size, std::map<int,Type*>& typemap) throw(int) {
    Value** oldarr = *array;
    Value** newarr = new Value*[size];
    if (size <= *asize) {
        int cutoff = size;
        for (int i = 0; i < cutoff; i++) {
            newarr[i] = oldarr[cutoff];
        }
        for (int i = cutoff; i < *asize; i++) {
            delete oldarr[cutoff];
        }
    } else {
        int cutoff = *asize;
        for (int i = 0; i < cutoff; i++) {
            newarr[i] = oldarr[cutoff];
        }
        for (int i = cutoff; i < size; i++) {
            newarr[i] = Value::fromType(*elemType,typemap);
        }
    }
    delete oldarr;
    *array = newarr;
    *asize = size;
}

void ArrayValue::setIndex(int index, Value* val) throw(int) {
    index -= *start;
    if (index >= *asize) throw E_INDEX_BOUNDS;
    (*array)[index]->set(val);
}

Value* ArrayValue::getIndex(int index) throw(int) {
    index -= *start;
    if (index >= *asize) throw E_INDEX_BOUNDS;
    return (*array)[index];
}

//**** BEGIN POINTERVALUE DEFINITION ***

PointerValue::PointerValue(Pointer* pt, std::map<int,Type*> &typemap) : Value(TYPE_POINTER,pt) {
    refcount = new int;
    *refcount = 0;
    refType = new Type*;
    *refType = (Type*)pt->pointsTo;
    ref = new Value*;
    *ref = new Value();
}


PointerValue::PointerValue(Pointer* pt) : Value(TYPE_POINTER,pt) {
    refType = new Type*;
    ref = new Value*;
}

PointerValue::PointerValue(PointerValue& val) : Value(TYPE_POINTER,(Type*)val.typeObj) {
    refcount = val.refcount;
    (*refcount)++;
    refType = val.refType;
    ref = val.ref;
}

PointerValue::~PointerValue() {
    if (*refcount) {
        (*refcount)--;
    } else {
        delete refcount;
        delete refType;
        delete *ref;
        delete ref;
    }
}

Value* PointerValue::duplicate() {
    return new PointerValue(*this);
}

Value* PointerValue::clone() {
    PointerValue* pt = new PointerValue((Pointer*)typeObj);
    *(pt->refType) = *refType;
    *(pt->ref) = (*ref)->duplicate();
}

void PointerValue::set(Value* val) throw(int) {
    if (val->type != TYPE_POINTER) throw E_NOT_POINTER;
    *refType = *((PointerValue*)val)->refType;
    delete *ref;
    *ref = (*((PointerValue*)val)->ref)->duplicate();
}

Value* PointerValue::getRef() throw(int) {
    return *ref;
}

//FIXME Should check assignment types
void PointerValue::setRef(Value* ref_impl) throw(int) {
    delete *ref;
    *ref = ref_impl->duplicate();
}

//**** BEGIN RECORDVALUE DEFINITION ***

RecordValue::RecordValue(Record* rec, std::map<int,Type*> &typemap) : Value(TYPE_RECORD,rec) {
    refcount = new int;
    *refcount = 0;
    fields = new std::map<int,Value*>*;
    *fields = new std::map<int,Value*>;
    types = new std::map<int,Type*>*;
    *types = new std::map<int,Type*>;
    std::list<Variable*>::iterator iter = rec->fields.begin();
    std::list<Variable*>::iterator end = rec->fields.end();
    while (iter != end) {
        Type* ftype = (Type*)(*iter)->type;
        (*types)->insert(std::pair<int,Type*>((*iter)->name,ftype));
        (*fields)->insert(std::pair<int,Value*>((*iter)->name, Value::fromType(ftype,typemap)));
        iter++;
    }
    objrefcount = new int*;
    *objrefcount = new int;
    **objrefcount = 0;
}


RecordValue::RecordValue(Record* rec) : Value(TYPE_RECORD,rec) {
    fields = new std::map<int,Value*>*;
    types = new std::map<int,Type*>*;
    objrefcount = new int*;
}

RecordValue::RecordValue(RecordValue& val) : Value(TYPE_RECORD,(Type*)val.typeObj) {
    refcount = val.refcount;
    (*refcount)++;
    objrefcount = val.objrefcount;
    fields = val.fields;
    types = val.types;
}

RecordValue::~RecordValue() {
    if (*refcount) {
        (*refcount)--;
    } else {
        if (**objrefcount) {
            (**objrefcount)--;
        } else {
            delete *types;
            std::map<int,Value*>::iterator iter = (*fields)->begin();
            std::map<int,Value*>::iterator end = (*fields)->end();
            while (iter != end) {
                delete iter->second;
                iter++;
            }
            delete *fields;
            delete *objrefcount;
        }
        delete types;
        delete fields;
        delete refcount;
        delete objrefcount;
    }
}

Value* RecordValue::duplicate() {
    return new RecordValue(*this);
}

Value* RecordValue::clone() {
    RecordValue* rec = new RecordValue((Record*)typeObj);
    *(rec->objrefcount) = *objrefcount;
    (**objrefcount)++;
    *(rec->types) = *types;
    *(rec->fields) = *fields;
}

//FIXME should check type
void RecordValue::set(Value* val) throw(int) {
    if (val->type != TYPE_RECORD) throw E_NOT_RECORD;
    RecordValue* rec = (RecordValue*)val;
    if (**objrefcount) {
        (**objrefcount)--;
    } else {
        delete *types;
        delete *fields;
        delete *objrefcount;
    }
    *objrefcount = *rec->objrefcount;
    (**objrefcount)++;
    *fields = *rec->fields;
    *types = *rec->types;
}

Value* RecordValue::getField(int name) throw(int) {
    if ((*fields)->find(name) == (*fields)->end()) throw E_NO_FIELD;
    return (**fields)[name];
}

void RecordValue::setField(int name, Value* value) throw(int) {
    if ((*fields)->find(name) == (*fields)->end()) throw E_NO_FIELD;
    (**fields)[name]->set(value);
}
