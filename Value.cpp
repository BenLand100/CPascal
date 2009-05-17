#include "Value.h"
#include "Exceptions.h"

#include <cstring>
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

Value* Value::fromTypeMem(Type* type, std::map<int,Type*> &typemap, void* mem) {
    std::map<int,Type*>::iterator end = typemap.end();
    debug("fromType " << type->type);
    while (type->type == TYPE_REF && typemap.find(((RefType*)type)->name) != end)
        type = typemap[((RefType*)type)->name];
    if (type->type == TYPE_REF) return 0;
    debug("fromType " << type->type);
    switch (type->type) {
        case TYPE_STRING:
            return new StringValue(mem);
        case TYPE_CHAR:
            return new CharValue(mem);
        case TYPE_INTEGER:
            return new IntegerValue(mem);
        case TYPE_REAL:
            return new RealValue(mem);
        case TYPE_BOOLEAN:
            return new BooleanValue(mem);
        case TYPE_RECORD:
            //return new RecordValue((Record*)type, typemap, mem);
        case TYPE_ARRAY:
            return new ArrayValue((Array*)type, typemap, mem);
        case TYPE_POINTER:
            return new PointerValue((Pointer*)type, typemap, mem);
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
char* Value::asString() throw(int) { throw E_NOT_STRING; }
char Value::asChar() throw(int) { throw E_NOT_CHAR; }
double Value::asReal() throw(int) { throw E_NOT_REAL; }
int Value::asInteger() throw(int) { throw E_NOT_INTEGER; }
bool Value::asBoolean() throw(int) { throw E_NOT_BOOLEAN; }
int Value::argSize() { return -1; }
void Value::valArg(void* mem) { }
void Value::refArg(void* mem) { }
void Value::read_c(void* res) { }
void Value::read_fpc(void* res) { }

//**** BEGIN INTEGERVALUE DEFINITION ***

IntegerValue::IntegerValue(void* mem) : Value(TYPE_INTEGER,Type::getInteger()) {
    refcount = new int;
    *refcount = 2; //ensure never freed
    integer = (int*) mem;
    *integer = 0;
}

IntegerValue::IntegerValue(int i) : Value(TYPE_INTEGER,Type::getInteger()) {
    refcount = new int;
    *refcount = 1;
    integer = new int;
    *integer = i;
}

IntegerValue::IntegerValue(IntegerValue& val) : Value(TYPE_INTEGER,Type::getInteger()) {
    refcount = val.refcount;
    (*refcount)++;
    integer = val.integer;
}

IntegerValue::~IntegerValue() {
    if (!(--*refcount)) {
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

int IntegerValue::argSize() { return 4; }
void IntegerValue::valArg(void* mem) { *(int*)mem = *integer; }
void IntegerValue::refArg(void* mem) { *(int**)mem = integer; }
void IntegerValue::read_c(void* res) { *integer = (int)res; }
void IntegerValue::read_fpc(void* res) { *integer = (int)res; }

//**** BEGIN STRINGVALUE DEFINITION ***

StringValue::StringValue(void* mem_impl) : Value(TYPE_STRING,Type::getString()) {
    refcount = new int;
    *refcount = 2;

    int len = 0;
    mem = (char**) mem_impl;
    *mem = new char[8+len+1];
    objref = new int*;
    *objref = (int*)mem;
    **objref = 1;
    size = new int*;
    *size = (int*)(mem+4);
    **size = len;
    str = new char*;
    *str = (char*)(mem+8);
    **str = 0;
}

StringValue::StringValue(char* str_impl) : Value(TYPE_STRING,Type::getString()) {
    refcount = new int;
    *refcount = 1;

    int len = strlen(str_impl);
    mem = new char*;
    *mem = new char[8+len+1];
    objref = new int*;
    *objref = (int*)(*mem);
    **objref = 1;
    size = new int*;
    *size = (int*)(*mem+4);
    **size = len;
    str = new char*;
    *str = (char*)(*mem+8);
    strcpy(*str,str_impl);
}

StringValue::StringValue(std::string cpp_str) : Value(TYPE_STRING,Type::getString()) {
    refcount = new int;
    *refcount = 1;

    int len = cpp_str.length();
    mem = new char*;
    *mem = new char[8+len+1];
    objref = new int*;
    *objref = (int*)(*mem);
    **objref = 1;
    size = new int*;
    *size = (int*)(*mem+4);
    **size = len;
    str = new char*;
    *str = (char*)(*mem+8);
    strcpy(*str,cpp_str.c_str());
}

StringValue::StringValue(StringValue& val) : Value(TYPE_STRING,Type::getString()) {
    refcount = val.refcount;
    (*refcount)++;
    objref = val.objref;
    size = val.size;
    str = val.str;
    mem = val.mem;
}

StringValue::~StringValue() {
    if (!(--*refcount)) {
        if (!(--**objref)) {
            delete [] *mem;
        }
        delete objref;
        delete mem;
        delete refcount;
        delete size;
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
    if (!(--**objref)) {
        delete [] *mem;
    }
    *objref = *((StringValue*)val)->objref;
    (**objref)++;
    *size = *((StringValue*)val)->size;
    *str = *((StringValue*)val)->str;
    *mem = *((StringValue*)val)->mem;
}

char* StringValue::asString() throw(int) {
    return *str;
}

int StringValue::argSize() { return 4; }
void StringValue::valArg(void* mem) { *(char**)mem = *str; }
void StringValue::refArg(void* mem) { *(char***)mem = str; }
void StringValue::read_c(void* res) { StringValue val((char*)res); set(&val); delete (char*) res; }
void StringValue::read_fpc(void* res) { StringValue val((char*)res); set(&val); } //leaks like hell

//**** BEGIN REALVALUE DEFINITION ***

RealValue::RealValue(void* mem) : Value(TYPE_REAL,Type::getReal()) {
    refcount = new int;
    *refcount = 2;
    real = (double*) mem;
    *real = 0.0;
}

RealValue::RealValue(double real_impl) : Value(TYPE_REAL,Type::getReal()) {
    refcount = new int;
    *refcount = 1;
    real = new double;
    *real = real_impl;
}

RealValue::RealValue(RealValue& val) : Value(TYPE_REAL,Type::getReal()) {
    refcount = val.refcount;
    (*refcount)++;
    real = val.real;
}

RealValue::~RealValue() {
    if (!(--*refcount)) {
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

int RealValue::argSize() { return 8; }
void RealValue::valArg(void* mem) { *(double*)mem = *real; }
void RealValue::refArg(void* mem) { *(double**)mem = real; }

//**** BEGIN CHARVALUE DEFINITION ***

CharValue::CharValue(void* mem) : Value(TYPE_CHAR,Type::getChar()) {
    refcount = new int;
    *refcount = 2;
    chr = (char*)mem;
    *chr = 0;
    str = new char*;
    *str = new char[2];
    (*str)[1] = 0;
}

CharValue::CharValue(char chr_impl) : Value(TYPE_CHAR,Type::getChar()) {
    refcount = new int;
    *refcount = 1;
    chr = new char;
    *chr = chr_impl;
    str = new char*;
    *str = new char[2];
    (*str)[1] = 0;
}

CharValue::CharValue(CharValue& val) : Value(TYPE_CHAR,Type::getChar()) {
    refcount = val.refcount;
    (*refcount)++;
    chr = val.chr;
    str = val.str;
}

CharValue::~CharValue() {
    if (!(--*refcount)) {
        delete refcount;
        delete chr;
        delete *str;
        delete str;
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

char* CharValue::asString() throw(int) {
    **str = *chr;
    return *str;
}


int CharValue::argSize() { return 4; }
void CharValue::valArg(void* mem) { *(char*)mem = *chr; }
void CharValue::refArg(void* mem) { *(char**)mem = chr; }
void CharValue::read_c(void* res) { *chr = (char)(unsigned int)res; }
void CharValue::read_fpc(void* res) { *chr = (char)(unsigned int)res; }

//**** BEGIN BOOLEANVALUE DEFINITION ***

BooleanValue::BooleanValue(void* mem) : Value(TYPE_BOOLEAN,Type::getBoolean()) {
    refcount = new int;
    *refcount = 2;
    boolean = (char*)mem;
    *boolean = false;
}

BooleanValue::BooleanValue(bool boolean_impl) : Value(TYPE_BOOLEAN,Type::getBoolean()) {
    refcount = new int;
    *refcount = 1;
    boolean = new char;
    *boolean = boolean_impl != false ? 1 : 0;
}

BooleanValue::BooleanValue(BooleanValue& val) : Value(TYPE_BOOLEAN,Type::getBoolean()) {
    refcount = val.refcount;
    (*refcount)++;
    boolean = val.boolean;
}

BooleanValue::~BooleanValue() {
    if (!(--*refcount)) {
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
    return *boolean != 0;
}

int BooleanValue::asInteger() throw(int) {
    return *boolean;
}

int BooleanValue::argSize() { return 4; }
void BooleanValue::valArg(void* mem) { *(char*)mem = *boolean; }
void BooleanValue::refArg(void* mem) { *(char**)mem = boolean; }
void BooleanValue::read_c(void* res) { *boolean = (bool)res; }
void BooleanValue::read_fpc(void* res) { *boolean = (char)(unsigned int)res; }

//**** BEGIN ARRAYVALUE DEFINITION ***

ArrayValue::ArrayValue(Array* arr, std::map<int,Type*> &typemap, void* mem_impl) : Value(TYPE_ARRAY,arr) {
    refcount = new int;
    *refcount = 2;
    elemType = new Type*;
    *elemType = (Type*)arr->element;
    dynamic = new bool;
    *dynamic = arr->dynamic;
    start = new int;
    *start = arr->from;

    elemsz = new int;
    *elemsz = arr->sizeOf(typemap);
    int numelems = arr->to - arr->from + 1; //to == -1 and from == 0 for dynamic just for this

    if (arr->dynamic) {
        mem = (char**) mem_impl;
        *mem = new char[8+(*elemsz)*numelems];
    } else {
        mem = new char*;
        *mem = (char*)mem_impl;
    }
    objref = new int*;
    *objref = (int*)*mem;
    **objref = 1;
    asize = new int*;
    *asize = (int*)(*mem+4);
    **asize = numelems;

    array = new Value**;
    *array = new Value*[**asize];
    pas_array = new char*;
    *pas_array = (char*)(*mem+8);

    for (int i = 0; i < numelems; i++) {
        (*array)[i] = Value::fromTypeMem(*elemType,typemap,(void*)(*pas_array+(*elemsz)*i));
    }
}

ArrayValue::ArrayValue(Array* arr, std::map<int,Type*> &typemap) : Value(TYPE_ARRAY,arr) {
    refcount = new int;
    *refcount = 1;
    elemType = new Type*;
    *elemType = (Type*)arr->element;
    dynamic = new bool;
    *dynamic = arr->dynamic;
    start = new int;
    *start = arr->from;

    elemsz = new int;
    *elemsz = arr->sizeOf(typemap);
    int numelems = arr->to - arr->from + 1; //to == -1 and from == 0 for dynamic just for this

    mem = new char*;
    *mem = new char[8+(*elemsz)*numelems];
    objref = new int*;
    *objref = (int*)*mem;
    **objref = 1;
    asize = new int*;
    *asize = (int*)(*mem+4);
    **asize = numelems;

    array = new Value**;
    *array = new Value*[**asize];
    pas_array = new char*;
    *pas_array = (char*)(*mem+8);

    for (int i = 0; i < numelems; i++) {
        (*array)[i] = Value::fromTypeMem(*elemType,typemap,(void*)(*pas_array+(*elemsz)*i));
    }
}

ArrayValue::ArrayValue(Array* arr) : Value(TYPE_ARRAY,arr) {
    refcount = new int;
    (*refcount)++;
    elemType = new Type*;
    dynamic = new bool;
    start = new int;
    elemsz = new int;

    mem = new char*;
    objref = new int*;
    asize = new int*;
    array = new Value**;
    pas_array = new char*;
}

ArrayValue::ArrayValue(ArrayValue& val) : Value(TYPE_ARRAY,(Type*)val.typeObj) {
    refcount = val.refcount;
    (*refcount)++;
    elemType = val.elemType;
    dynamic = val.dynamic;
    start = val.start;
    mem = val.mem;
    objref = val.objref;
    asize = val.asize;
    array = val.array;
    pas_array = val.pas_array;
    elemsz = val.elemsz;
}

ArrayValue::~ArrayValue() {
    if (!(--(*refcount))) {
        if (!(--(**objref))) {
            for (int i = 0; i < **asize; i++) {
                delete (*array)[i];
            }
            delete *array;
            delete *mem;
        }
        delete refcount;
        delete elemType;
        delete dynamic;
        delete start;
        delete mem;
        delete objref;
        delete asize;
        delete array;
        delete pas_array;
        delete elemsz;
    }
}

Value* ArrayValue::duplicate() {
    return new ArrayValue(*this);
}

Value* ArrayValue::clone() {
    ArrayValue* clone = new ArrayValue((Array*)typeObj);
    *clone->elemType = *elemType;
    *clone->dynamic = *dynamic;
    *clone->start = *start;

    *clone->elemsz = *elemsz;
    Array* arr = (Array*)typeObj;
    int numelems = arr->to - arr->from + 1; //to == -1 and from == 0 for dynamic just for this

    *clone->mem = new char[8+(*elemsz)*numelems];
    *clone->objref = (int*)*mem;
    **clone->objref = 1;
    *clone->asize = (int*)(*mem+4);
    **clone->asize = numelems;

    *clone->array = new Value*[**asize];
    *clone->pas_array = (char*)(*mem+8);

    std::map<int,Type*> typemap;
    for (int i = 0; i < numelems; i++) {
        (*clone->array)[i] = Value::fromTypeMem(*elemType,typemap,(void*)(*clone->pas_array+(*elemsz)*i));
        (*clone->array)[i]->set((*array)[i]);
    }
}

void ArrayValue::set(Value* val) throw(int) {
    if (val->type != TYPE_ARRAY) throw E_NOT_ARRAY;
    ArrayValue* arr = (ArrayValue*)val;
    if (!(--(**objref))) {
        for (int i = 0; i < **asize; i++) {
            delete (*array)[i];
        }
        delete *array;
        delete *mem;
    }
    *elemType = *arr->elemType;
    *dynamic = *arr->dynamic;
    *start = *arr->start;

    *elemsz = *arr->elemsz;
    int numelems = **arr->asize; //to == -1 and from == 0 for dynamic just for this

    *mem = new char[8+(*elemsz)*numelems];
    *objref = (int*)*mem;
    **objref = 1;
    *asize = (int*)(*mem+4);
    **asize = numelems;

    *array = new Value*[**asize];
    *pas_array = (char*)(*mem+8);

    std::map<int,Type*> typemap;
    for (int i = 0; i < numelems; i++) {
        (*array)[i] = Value::fromTypeMem(*elemType,typemap,(void*)(*pas_array+(*elemsz)*i));
        (*array)[i]->set((*arr->array)[i]);
    }
}

int ArrayValue::size() throw(int) {
    return **asize;
}

void ArrayValue::resize(int len, std::map<int,Type*>& typemap) throw(int) {
    Value** oldarr = *array;

    char* newmem = new char[8+(*elemsz)*len];
    int* newobjref = (int*)newmem;
    *newobjref = **objref;
    int* newsize = (int*)(newmem+4);
    *newsize = len;

    Value** newarr = new Value*[len];
    char* newpas_array = (char*)(newmem+8);

    if (len <= **asize) {
        int cutoff = len;
        for (int i = 0; i < cutoff; i++) {
            newarr[i] = Value::fromTypeMem(*elemType,typemap,(void*)(newpas_array+(*elemsz)*i));
            newarr[i]->set(oldarr[i]);
            delete oldarr[i];
        }
        for (int i = cutoff; i < **this->asize; i++) {
            delete oldarr[i];
        }
    } else {
        int cutoff = **this->asize;
        for (int i = 0; i < cutoff; i++) {
            newarr[i] = Value::fromTypeMem(*elemType,typemap,(void*)(newpas_array+(*elemsz)*i));
            newarr[i]->set(oldarr[i]);
            delete oldarr[i];
        }
        for (int i = cutoff; i < len; i++) {
            newarr[i] = Value::fromTypeMem(*elemType,typemap,(void*)(newpas_array+(*elemsz)*i));
        }
    }
    delete *array;
    delete *mem;
    *array = newarr;
    *mem = newmem;
    *objref = newobjref;
    *asize = newsize;
    *pas_array = newpas_array;
}

void ArrayValue::setIndex(int index, Value* val) throw(int) {
    index -= *start;
    if (index >= **asize) throw E_INDEX_BOUNDS;
    (*array)[index]->set(val);
}

Value* ArrayValue::getIndex(int index) throw(int) {
    index -= *start;
    if (index >= **asize) throw E_INDEX_BOUNDS;
    return (*array)[index];
}

int ArrayValue::argSize() { return 4; }
void ArrayValue::valArg(void* mem) { *(char**)mem = *pas_array; }
void ArrayValue::refArg(void* mem) { *(char***)mem = pas_array; }

//**** BEGIN POINTERVALUE DEFINITION ***

PointerValue::PointerValue(Pointer* pt, std::map<int,Type*> &typemap, void* mem) {
    refcount = new int;
    *refcount = 2;
    refType = new Type*;
    *refType = (Type*)pt->pointsTo;
    ref = new Value*;
    *ref = new Value();
    pas_ref = (void**) mem;
    (*ref)->refArg(*pas_ref);
}

PointerValue::PointerValue(Pointer* pt, std::map<int,Type*> &typemap) : Value(TYPE_POINTER,pt) {
    refcount = new int;
    *refcount = 1;
    refType = new Type*;
    *refType = (Type*)pt->pointsTo;
    ref = new Value*;
    *ref = new Value();
    pas_ref = new void*;
    (*ref)->refArg(*pas_ref);
}


PointerValue::PointerValue(Pointer* pt) : Value(TYPE_POINTER,pt) {
    refType = new Type*;
    ref = new Value*;
    pas_ref = new void*;
}

PointerValue::PointerValue(PointerValue& val) : Value(TYPE_POINTER,(Type*)val.typeObj) {
    refcount = val.refcount;
    (*refcount)++;
    refType = val.refType;
    ref = val.ref;
    pas_ref = val.pas_ref;
}

PointerValue::~PointerValue() {
    if (!(--*refcount)) {
        delete refcount;
        delete refType;
        delete *ref;
        delete ref;
        delete pas_ref;
    }
}

Value* PointerValue::duplicate() {
    return new PointerValue(*this);
}

Value* PointerValue::clone() {
    PointerValue* pt = new PointerValue((Pointer*)typeObj);
    *(pt->refType) = *refType;
    *(pt->ref) = (*ref)->duplicate();
    (*ref)->refArg(*pt->pas_ref);
}

void PointerValue::set(Value* val) throw(int) {
    if (val->type != TYPE_POINTER) throw E_NOT_POINTER;
    *refType = *((PointerValue*)val)->refType;
    delete *ref;
    *ref = (*((PointerValue*)val)->ref)->duplicate();
    (*ref)->refArg(*pas_ref);
}

Value* PointerValue::getRef() throw(int) {
    return *ref;
}

//FIXME Should check assignment types
void PointerValue::setRef(Value* ref_impl) throw(int) {
    delete *ref;
    *ref = ref_impl->duplicate();
    (*ref)->refArg(*pas_ref);
}

int PointerValue::argSize() { return 4; }
void PointerValue::valArg(void* mem) { *(void**)mem = *pas_ref; }
void PointerValue::refArg(void* mem) { *(void***)mem = pas_ref; }

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
