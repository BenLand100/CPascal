#include "Value.h"
#include "Exceptions.h"
#include "Container.h"
#include "lexer.h"
#include "Interpreter.h"

#include <cstring>
#include <iostream>
//#define debug(x) std::cout << x << '\n'
#define debug(x)

Value* Value::fromType(Type* type, std::map<int, Type*> &typemap) {
    std::map<int, Type*>::iterator end = typemap.end();
    debug("fromType " << type->type);
    while (type->type == TYPE_REF && typemap.find(((RefType*) type)->name) != end)
        type = typemap[((RefType*) type)->name];
    if (type->type == TYPE_REF) return 0;
    debug("fromType " << type->type);
    switch (type->type) {
        case TYPE_STRING:
            return new StringValue((char*) "");
        case TYPE_CHAR:
            return new CharValue('\0');
        case TYPE_INTEGER:
            return new IntegerValue(0);
        case TYPE_REAL:
            return new RealValue(0.0);
        case TYPE_BOOLEAN:
            return new BooleanValue(false);
        case TYPE_RECORD:
            return new RecordValue((Record*) type, typemap);
        case TYPE_ARRAY:
            return new ArrayValue((Array*) type, typemap);
        case TYPE_POINTER:
            return new PointerValue((Pointer*) type, typemap);
        default:
            return 0;
    }
}

Value* Value::fromTypeMem(Type* type, std::map<int, Type*> &typemap, void* mem) {
    std::map<int, Type*>::iterator end = typemap.end();
    debug("fromType " << type->type);
    while (type->type == TYPE_REF && typemap.find(((RefType*) type)->name) != end)
        type = typemap[((RefType*) type)->name];
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
            return new RecordValue((Record*) type, typemap, mem);
        case TYPE_ARRAY:
            return new ArrayValue((Array*) type, typemap, mem);
        case TYPE_POINTER:
            return new PointerValue((Pointer*) type, typemap, mem);
        default:
            return 0;
    }
}

Value::Value() : Element(ELEM_VALUE), type(TYPE_NIL), typeObj(Type::getNil()) {
}

Value::Value(int impl_type, Type* impl_typeObj) : Element(ELEM_VALUE), type(impl_type), typeObj(impl_typeObj) {
}

Value::~Value() {
}

void Value::set(Value* val) throw (int, InterpEx*) {
    throw E_NULL_VAL;
}

Value* Value::clone() {
    return new Value();
}

Value* Value::duplicate() {
    return new Value();
}

Value* Value::getRef() throw (int, InterpEx*) {
    throw E_NOT_POINTER;
}

void Value::setRef(Value* ref) throw (int, InterpEx*) {
    throw E_NOT_POINTER;
}

void Value::negate() throw (int, InterpEx*) {
    throw E_NON_NUMERIC;
}

void Value::incr() throw (int, InterpEx*) {
    throw E_NON_NUMERIC;
}

void Value::decr() throw (int, InterpEx*) {
    throw E_NON_NUMERIC;
}

Value* Value::getField(int name) throw (int, InterpEx*) {
    throw E_NOT_RECORD;
}

void Value::setField(int name, Value* val) throw (int, InterpEx*) {
    throw E_NOT_RECORD;
}

int Value::size() throw (int, InterpEx*) {
    throw E_NOT_ARRAY;
}

void Value::resize(int size, std::map<int, Type*>& typemap) throw (int, InterpEx*) {
    throw E_NOT_ARRAY;
}

void Value::setIndex(int index, Value* val) throw (int, InterpEx*) {
    throw E_NOT_ARRAY;
}

Value* Value::getIndex(int index) throw (int, InterpEx*) {
    throw E_NOT_ARRAY;
}

char* Value::asString() throw (int, InterpEx*) {
    throw E_NOT_STRING;
}

char Value::asChar() throw (int, InterpEx*) {
    throw E_NOT_CHAR;
}

double Value::asReal() throw (int, InterpEx*) {
    throw E_NOT_REAL;
}

int Value::asInteger() throw (int, InterpEx*) {
    throw E_NOT_INTEGER;
}

bool Value::asBoolean() throw (int, InterpEx*) {
    throw E_NOT_BOOLEAN;
}

Value* Value::invoke(Value** args, int numArgs, Frame* frame) throw (int, InterpEx*) {
    throw E_NOT_METHOD;
}

int Value::argSize() {
    return -1;
}

void Value::valArg(void* mem) {
}

void Value::refArg(void* mem) {
}

void Value::read_c(void* res) {
}

void Value::read_fpc(void* res) {
}

//**** BEGIN METHODVALUE DEFINITION ***

MethodValue::MethodValue(void* mem, Meth* type) : Value(TYPE_METH, type) {
    refcount = new int;
    *refcount = 1;
    owns_mem = new bool;
    *owns_mem = false;
    meth = (Method**) mem;
    *meth = 0;
}

MethodValue::MethodValue(Method* meth_impl) : Value(TYPE_METH, meth_impl->val_type) {
    refcount = new int;
    *refcount = 1;
    owns_mem = new bool;
    *owns_mem = true;
    meth = new Method*;
    *meth = meth_impl;
}

MethodValue::MethodValue(MethodValue& val) : Value(TYPE_METH, (Type*)val.typeObj) {
    refcount = val.refcount;
    (*refcount)++;
    owns_mem = val.owns_mem;
    meth = val.meth;
}

MethodValue::~MethodValue() {
    if (!(--*refcount)) {
        delete refcount;
        if (*owns_mem) delete meth;
        delete owns_mem;
    }
}

Value* MethodValue::duplicate() {
    return new MethodValue(*this);
}

Value* MethodValue::clone() {
    return new MethodValue(*meth);
}

void MethodValue::set(Value* val) throw (int, InterpEx*) {
    if (val->type != TYPE_METH) throw E_NOT_METHOD;
    *meth = *((MethodValue*) val)->meth;
}

Value* MethodValue::invoke(Value** args, int numArgs, Frame* cur) throw (int, InterpEx*) {
    Method* meth = *this->meth;
    if (numArgs != meth->arguments.size()) return 0;
    if (meth->address) {
        int argsz = 0;
        for (int i = 0; i < numArgs; i++)
            argsz += meth->arguments[i]->byRef ? 4 : args[i]->argSize();
        char* cargs = new char[argsz];
        char* stack = cargs;
        for (int i = 0; i < numArgs; i++) {
            if (meth->arguments[i]->byRef) {
                args[i]->refArg((void*) stack);
                stack += 4;
            } else {
                args[i]->valArg((void*) stack);
                stack += args[i]->argSize();
}
        }
        stack -= 4;
        if (meth->type) {
            if (meth->type->type == TYPE_REAL) {
                double real;
                asm volatile (
                            "pushl %%ecx \n"
                            "cmpl  $0, %%edx \n"
                            "jz mkcall_real \n"
                            "start_real: pushl (%%eax) \n"
                            "cmpl %%eax, %%ecx \n"
                            "jz mkcall_real \n"
                            "subl $4, %%eax \n"
                            "jmp start_real \n"
                            "mkcall_real: call *%%ebx \n"
                            "popl %%ecx \n"
                            : "=t"(real)
                            : "a"(stack), "b"(meth->address), "c"(cargs), "d"(argsz)
                            : "memory"
                            );
                delete[] cargs;
                return new RealValue(real);
            } else {
                void* eax;
                asm volatile (
                            "pushl %%ecx \n"
                            "cmpl  $0, %%edx \n"
                            "jz mkcall_all \n"
                            "start_all: pushl (%%eax) \n"
                            "cmpl %%eax, %%ecx \n"
                            "jz mkcall_all \n"
                            "subl $4, %%eax \n"
                            "jmp start_all \n"
                            "mkcall_all: call *%%ebx \n"
                            "popl %%ecx \n"
                            : "=a" (eax)
                            : "a"(stack), "b"(meth->address), "c"(cargs), "d"(argsz)
                            : "memory"
                            );
                delete[] cargs;
                Value* val = Value::fromType(meth->type, cur->typemap);
                switch (meth->mtype) {
                    case CONV_C_STDCALL:
                        val->read_c(eax);
                        break;
                    case CONV_FPC_STDCALL:
                        val->read_fpc(eax);
                        break;
                }
                return val;
            }
        } else {
            asm volatile (
                        "pushl %%ecx \n"
                        "cmpl  $0, %%edx \n"
                        "jz mkcall_void \n"
                        "start_void: pushl (%%eax) \n"
                        "cmpl %%eax, %%ecx \n"
                        "jz mkcall_void \n"
                        "subl $4, %%eax \n"
                        "jmp start_void \n"
                        "mkcall_void: call *%%ebx \n"
                        "popl %%ecx \n"
                        :
                        : "a"(stack), "b"(meth->address), "c"(cargs), "d"(argsz)
                        : "memory"
                        );
            delete[] cargs;
            return new Value();
        }
    } else {
        //FIXME must check method types
        Frame* frame = new Frame(cur, meth);
        debug("resolve_args");
        for (int i = 0; i < numArgs; i++) {
            Variable* var = meth->arguments[i];
            frame->slots[var->name] = var->byRef ? args[i]->duplicate() : args[i]->clone();
        }
        if (meth->type) frame->slots[RES_RESULT] = Value::fromType(meth->type, cur->typemap);
        debug("resolve_method");
        evalBlock(&meth->block, frame);
        debug("resolve_return");
        for (int i = 0; i < numArgs; i++) {
            delete frame->slots[meth->arguments[i]->name];;
        }
        if (meth->type) {
            Value* res = frame->slots[RES_RESULT];
            delete frame;
            return res;
        } else {
            delete frame;
            return new Value();
        }
    }
}

//**** BEGIN INTEGERVALUE DEFINITION ***

IntegerValue::IntegerValue(void* mem) : Value(TYPE_INTEGER, Type::getInteger()) {
    refcount = new int;
    *refcount = 1;
    owns_mem = new bool;
    *owns_mem = false;
    integer = (int*) mem;
    *integer = 0;
}

IntegerValue::IntegerValue(int i) : Value(TYPE_INTEGER, Type::getInteger()) {
    refcount = new int;
    *refcount = 1;
    owns_mem = new bool;
    *owns_mem = true;
    integer = new int;
    *integer = i;
}

IntegerValue::IntegerValue(IntegerValue& val) : Value(TYPE_INTEGER, Type::getInteger()) {
    refcount = val.refcount;
    (*refcount)++;
    owns_mem = val.owns_mem;
    integer = val.integer;
}

IntegerValue::~IntegerValue() {
    if (!(--*refcount)) {
        delete refcount;
        if (*owns_mem) delete integer;
        delete owns_mem;
    }
}

Value* IntegerValue::duplicate() {
    return new IntegerValue(*this);
}

Value* IntegerValue::clone() {
    return new IntegerValue(*integer);
}

void IntegerValue::set(Value* val) throw (int, InterpEx*) {
    if (val->type != TYPE_INTEGER) throw E_NOT_INTEGER;
    *integer = *((IntegerValue*) val)->integer;
}

void IntegerValue::negate() throw (int, InterpEx*) {
    *integer = -(*integer);
}

void IntegerValue::incr() throw (int, InterpEx*) {
    ++(*integer);
}

void IntegerValue::decr() throw (int, InterpEx*) {
    --(*integer);
}

double IntegerValue::asReal() throw (int, InterpEx*) {
    return *integer;
}

int IntegerValue::asInteger() throw (int, InterpEx*) {
    return *integer;
}

int IntegerValue::argSize() {
    return 4;
}

void IntegerValue::valArg(void* mem) {
    *(int*) mem = *integer;
}

void IntegerValue::refArg(void* mem) {
    *(int**) mem = integer;
}

void IntegerValue::read_c(void* res) {
    *integer = (int) res;
}

void IntegerValue::read_fpc(void* res) {
    *integer = (int) res;
}

//**** BEGIN STRINGVALUE DEFINITION ***

StringValue::StringValue(void* mem_impl) : Value(TYPE_STRING, Type::getString()) {
    refcount = new int;
    *refcount = 1;
    owns_mem = new bool;
    *owns_mem = false;

    int len = 0;
    mem = (char**) mem_impl;
    *mem = new char[8 + len + 1];
    objref = new int*;
    *objref = (int*) * mem;
    **objref = 1;
    size = new int*;
    *size = (int*) (*mem + 4);
    **size = len;
    str = new char*;
    *str = (char*) (*mem + 8);
    **str = 0;
}

StringValue::StringValue(char* str_impl) : Value(TYPE_STRING, Type::getString()) {
    refcount = new int;
    *refcount = 1;
    owns_mem = new bool;
    *owns_mem = true;

    int len = strlen(str_impl);
    mem = new char*;
    *mem = new char[8 + len + 1];
    objref = new int*;
    *objref = (int*) (*mem);
    **objref = 1;
    size = new int*;
    *size = (int*) (*mem + 4);
    **size = len;
    str = new char*;
    *str = (char*) (*mem + 8);
    strcpy(*str, str_impl);
}

StringValue::StringValue(std::string cpp_str) : Value(TYPE_STRING, Type::getString()) {
    refcount = new int;
    *refcount = 1;
    owns_mem = new bool;
    *owns_mem = true;

    int len = cpp_str.length();
    mem = new char*;
    *mem = new char[8 + len + 1];
    objref = new int*;
    *objref = (int*) (*mem);
    **objref = 1;
    size = new int*;
    *size = (int*) (*mem + 4);
    **size = len;
    str = new char*;
    *str = (char*) (*mem + 8);
    strcpy(*str, cpp_str.c_str());
}

StringValue::StringValue(StringValue& val) : Value(TYPE_STRING, Type::getString()) {
    refcount = val.refcount;
    (*refcount)++;
    owns_mem = val.owns_mem;
    objref = val.objref;
    size = val.size;
    str = val.str;
    mem = val.mem;
}

StringValue::~StringValue() {
    if (!(--(*refcount))) {
        delete [] * mem;
        delete objref;
        if (*owns_mem) delete mem;
        delete owns_mem;
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

void StringValue::set(Value* val) throw (int, InterpEx*) {
    if (val->type != TYPE_STRING) throw E_NOT_STRING;
    delete [] * mem;
    StringValue* sval = (StringValue*) val;
    int len = **sval->size;
    *mem = new char[8 + len + 1];
    *objref = (int*) (*mem);
    **objref = 1;
    *size = (int*) (*mem + 4);
    **size = len;
    *str = (char*) (*mem + 8);
    strcpy(*str, *sval->str);
}

char* StringValue::asString() throw (int, InterpEx*) {
    return *str;
}

int StringValue::argSize() {
    return 4;
}

void StringValue::valArg(void* mem) {
    *(char**) mem = *str;
}

void StringValue::refArg(void* mem) {
    *(char***) mem = str;
}

void StringValue::read_c(void* res) {
    StringValue val((char*) res);
    set(&val);
    delete [] (char*) res;
}

void StringValue::read_fpc(void* res) {
    StringValue val((char*) res);
    set(&val);
} //FIXME leaks like hell

//**** BEGIN REALVALUE DEFINITION ***

RealValue::RealValue(void* mem) : Value(TYPE_REAL, Type::getReal()) {
    refcount = new int;
    *refcount = 1;
    owns_mem = new bool;
    *owns_mem = false;
    real = (double*) mem;
    *real = 0.0;
}

RealValue::RealValue(double real_impl) : Value(TYPE_REAL, Type::getReal()) {
    refcount = new int;
    *refcount = 1;
    owns_mem = new bool;
    *owns_mem = true;
    real = new double;
    *real = real_impl;
}

RealValue::RealValue(RealValue& val) : Value(TYPE_REAL, Type::getReal()) {
    refcount = val.refcount;
    (*refcount)++;
    owns_mem = val.owns_mem;
    real = val.real;
}

RealValue::~RealValue() {
    if (!(--*refcount)) {
        delete refcount;
        if (*owns_mem) delete real;
        delete owns_mem;
    }
}

Value* RealValue::duplicate() {
    return new RealValue(*this);
}

Value* RealValue::clone() {
    return new RealValue(*real);
}

void RealValue::set(Value* val) throw (int, InterpEx*) {
    *real = val->asReal();
}

void RealValue::negate() throw (int, InterpEx*) {
    *real = -(*real);
}

void RealValue::incr() throw (int, InterpEx*) {
    ++(*real);
}

void RealValue::decr() throw (int, InterpEx*) {
    --(*real);
}

double RealValue::asReal() throw (int, InterpEx*) {
    return *real;
}

int RealValue::argSize() {
    return 8;
}

void RealValue::valArg(void* mem) {
    *(double*) mem = *real;
}

void RealValue::refArg(void* mem) {
    *(double**) mem = real;
}

//**** BEGIN CHARVALUE DEFINITION ***

CharValue::CharValue(void* mem) : Value(TYPE_CHAR, Type::getChar()) {
    refcount = new int;
    *refcount = 1;
    owns_mem = new bool;
    *owns_mem = false;
    chr = (char*) mem;
    *chr = 0;
    str = new char*;
    *str = new char[2];
    (*str)[1] = 0;
}

CharValue::CharValue(char chr_impl) : Value(TYPE_CHAR, Type::getChar()) {
    refcount = new int;
    *refcount = 1;
    owns_mem = new bool;
    *owns_mem = true;
    chr = new char;
    *chr = chr_impl;
    str = new char*;
    *str = new char[2];
    (*str)[1] = 0;
}

CharValue::CharValue(CharValue& val) : Value(TYPE_CHAR, Type::getChar()) {
    refcount = val.refcount;
    (*refcount)++;
    owns_mem = val.owns_mem;
    chr = val.chr;
    str = val.str;
}

CharValue::~CharValue() {
    if (!(--*refcount)) {
        delete refcount;
        if (*owns_mem) delete chr;
        delete owns_mem;
        delete [] * str;
        delete str;
    }
}

Value* CharValue::duplicate() {
    return new CharValue(*this);
}

Value* CharValue::clone() {
    return new CharValue(*chr);
}

void CharValue::set(Value* val) throw (int, InterpEx*) {
    if (val->type != TYPE_CHAR) throw E_NOT_CHAR;
    *chr = *((CharValue*) val)->chr;
}

void CharValue::incr() throw (int, InterpEx*) {
    ++(*chr);
}

void CharValue::decr() throw (int, InterpEx*) {
    --(*chr);
}

char CharValue::asChar() throw (int, InterpEx*) {
    return *chr;
}

int CharValue::asInteger() throw (int, InterpEx*) {
    return *chr;
}

char* CharValue::asString() throw (int, InterpEx*) {
    **str = *chr;
    return *str;
}

int CharValue::argSize() {
    return 4;
}

void CharValue::valArg(void* mem) {
    *(char*) mem = *chr;
}

void CharValue::refArg(void* mem) {
    *(char**) mem = chr;
}

void CharValue::read_c(void* res) {
    *chr = (char) (unsigned int) res;
}

void CharValue::read_fpc(void* res) {
    *chr = (char) (unsigned int) res;
}

//**** BEGIN BOOLEANVALUE DEFINITION ***

BooleanValue::BooleanValue(void* mem) : Value(TYPE_BOOLEAN, Type::getBoolean()) {
    refcount = new int;
    *refcount = 1;
    owns_mem = new bool;
    *owns_mem = false;
    boolean = (char*) mem;
    *boolean = false;
}

BooleanValue::BooleanValue(bool boolean_impl) : Value(TYPE_BOOLEAN, Type::getBoolean()) {
    refcount = new int;
    *refcount = 1;
    owns_mem = new bool;
    *owns_mem = true;
    boolean = new char;
    *boolean = boolean_impl != false ? 1 : 0;
}

BooleanValue::BooleanValue(BooleanValue& val) : Value(TYPE_BOOLEAN, Type::getBoolean()) {
    refcount = val.refcount;
    (*refcount)++;
    owns_mem = val.owns_mem;
    boolean = val.boolean;
}

BooleanValue::~BooleanValue() {
    if (!(--*refcount)) {
        delete refcount;
        if (*owns_mem) delete boolean;
        delete owns_mem;
    }
}

Value* BooleanValue::duplicate() {
    return new BooleanValue(*this);
}

Value* BooleanValue::clone() {
    return new BooleanValue(*boolean);
}

void BooleanValue::set(Value* val) throw (int, InterpEx*) {
    if (val->type != TYPE_BOOLEAN) throw E_NOT_BOOLEAN;
    *boolean = *((BooleanValue*) val)->boolean;
}

bool BooleanValue::asBoolean() throw (int, InterpEx*) {
    return *boolean != 0;
}

int BooleanValue::asInteger() throw (int, InterpEx*) {
    return *boolean;
}

int BooleanValue::argSize() {
    return 4;
}

void BooleanValue::valArg(void* mem) {
    *(char*) mem = *boolean;
}

void BooleanValue::refArg(void* mem) {
    *(char**) mem = boolean;
}

void BooleanValue::read_c(void* res) {
    *boolean = (bool)res;
}

void BooleanValue::read_fpc(void* res) {
    *boolean = (char) (unsigned int) res;
}

//**** BEGIN ARRAYVALUE DEFINITION ***

ArrayValue::ArrayValue(Array* arr, std::map<int, Type*> &typemap, void* mem_impl) : Value(TYPE_ARRAY, arr) {
    refcount = new int;
    *refcount = 1;
    owns_mem = new bool;
    *owns_mem = false;
    elemType = new Type*;
    *elemType = (Type*) arr->element;
    dynamic = new bool;
    *dynamic = arr->dynamic;
    start = new int;
    *start = arr->from;

    elemsz = new int;
    *elemsz = arr->element->sizeOf(typemap);
    int numelems = arr->to - arr->from + 1; //to == -1 and from == 0 for dynamic just for this

    if (arr->dynamic) {
        mem = (char**) mem_impl;
        *mem = new char[8 + (*elemsz) * numelems];
        objref = new int*;
        *objref = (int*) * mem;
        **objref = 1;
        asize = new int*;
        *asize = (int*) (*mem + 4);
        **asize = numelems;
    } else {
        mem = new char*;
        *mem = (char*) mem_impl;
        objref = new int*;
        *objref = new int;
        **objref = 1;
        asize = new int*;
        *asize = new int;
        **asize = numelems;
    }

    array = new Value**;
    *array = new Value*[**asize];
    pas_array = new char*;
    if (*dynamic) {
        *pas_array = (char*) (*mem + 8);
    } else {
        *pas_array = *mem;
    }

    for (int i = 0; i < numelems; i++) {
        (*array)[i] = Value::fromTypeMem(*elemType, typemap, (void*) (*pas_array + (*elemsz) * i));
    }
}

ArrayValue::ArrayValue(Array* arr, std::map<int, Type*> &typemap) : Value(TYPE_ARRAY, arr) {
    refcount = new int;
    *refcount = 1;
    owns_mem = new bool;
    *owns_mem = true;
    elemType = new Type*;
    *elemType = (Type*) arr->element;
    dynamic = new bool;
    *dynamic = arr->dynamic;
    start = new int;
    *start = arr->from;

    elemsz = new int;
    *elemsz = arr->element->sizeOf(typemap);
    int numelems = arr->to - arr->from + 1; //to == -1 and from == 0 for dynamic just for this
    if (*dynamic) {
        mem = new char*;
        *mem = new char[8 + (*elemsz) * numelems];
        objref = new int*;
        *objref = (int*) * mem;
        **objref = 1;
        asize = new int*;
        *asize = (int*) (*mem + 4);
        **asize = numelems;
    } else {
        mem = new char*;
        *mem = new char[(*elemsz) * numelems];
        objref = new int*;
        *objref = new int;
        **objref = 1;
        asize = new int*;
        *asize = new int;
        **asize = numelems;
    }

    array = new Value**;
    *array = new Value*[**asize];
    pas_array = new char*;
    if (*dynamic) {
        *pas_array = (char*) (*mem + 8);
    } else {
        *pas_array = *mem;
    }

    for (int i = 0; i < numelems; i++) {
        (*array)[i] = Value::fromTypeMem(*elemType, typemap, (void*) (*pas_array + (*elemsz) * i));
    }
}

ArrayValue::ArrayValue(Array* arr) : Value(TYPE_ARRAY, arr) {
    refcount = new int;
    *refcount = 1;
    owns_mem = new bool;
    *owns_mem = true;
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

ArrayValue::ArrayValue(ArrayValue& val) : Value(TYPE_ARRAY, (Type*) val.typeObj) {
    refcount = val.refcount;
    (*refcount)++;
    owns_mem = val.owns_mem;
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
        for (int i = 0; i < **asize; i++) {
            delete (*array)[i];
        }
        delete [] * array;
        if (*dynamic) {
            delete [] * mem;
            if (*owns_mem) delete mem;
        } else {
            if (*owns_mem) delete [] * mem;
            delete mem;
            delete *objref;
            delete *asize;
        }
        delete owns_mem;
        delete refcount;
        delete elemType;
        delete dynamic;
        delete start;
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
    ArrayValue* clone = new ArrayValue((Array*) typeObj);
    *clone->elemType = *elemType;
    *clone->dynamic = *dynamic;
    *clone->start = *start;

    *clone->elemsz = *elemsz;
    Array* arr = (Array*) typeObj;
    int numelems = arr->to - arr->from + 1; //to == -1 and from == 0 for dynamic just for this

    if (arr->dynamic) {
        *clone->mem = new char[8 + (*elemsz) * numelems];
        *clone->objref = (int*) * mem;
        **clone->objref = 1;
        *clone->asize = (int*) (*mem + 4);
        **clone->asize = numelems;
    } else {
        *clone->mem = new char[(*elemsz) * numelems];
        *clone->objref = new int;
        **clone->objref = 1;
        *clone->asize = new int;
        **clone->asize = numelems;
    }

    *clone->array = new Value*[**asize];
    if (*dynamic) {
        *clone->pas_array = (char*) (*mem + 8);
    } else {
        *clone->pas_array = *mem;
    }

    std::map<int, Type*> typemap;
    for (int i = 0; i < numelems; i++) {
        (*clone->array)[i] = Value::fromTypeMem(*elemType, typemap, (void*) (*clone->pas_array + (*elemsz) * i));
        (*clone->array)[i]->set((*array)[i]);
    }
}

void ArrayValue::set(Value* val) throw (int, InterpEx*) {
    if (val->type != TYPE_ARRAY) throw E_NOT_ARRAY;
    ArrayValue* arr = (ArrayValue*) val;
    if (*dynamic) {
        for (int i = 0; i < **asize; i++) {
            delete (*array)[i];
        }
        delete [] * array;
        delete [] * mem;

        *elemType = *arr->elemType;
        *dynamic = *arr->dynamic;
        *start = *arr->start;

        *elemsz = *arr->elemsz;
        int numelems = **arr->asize;

        if (*dynamic) {
            *mem = new char[8 + (*elemsz) * numelems];
            *objref = (int*) * mem;
            **objref = 1;
            *asize = (int*) (*mem + 4);
            **asize = numelems;
        } else {
            *mem = new char[(*elemsz) * numelems];
            *objref = new int;
            **objref = 1;
            *asize = new int;
            **asize = numelems;
        }

        *array = new Value*[**asize];
        if (*dynamic) {
            *pas_array = (char*) (*mem + 8);
        } else {
            *pas_array = *mem;
        }

        std::map<int, Type*> typemap;
        for (int i = 0; i < numelems; i++) {
            (*array)[i] = Value::fromTypeMem(*elemType, typemap, (void*) (*pas_array + (*elemsz) * i));
            (*array)[i]->set((*arr->array)[i]);
        }
    } else {
        if (**arr->asize != **asize) throw E_INDEX_BOUNDS;
        int numelems = **arr->asize;
        for (int i = 0; i < numelems; i++) {
            (*array)[i]->set((*arr->array)[i]);
        }
    }
}

int ArrayValue::size() throw (int, InterpEx*) {
    return **asize;
}

void ArrayValue::resize(int len, std::map<int, Type*>& typemap) throw (int, InterpEx*) {
    if (!*dynamic) throw E_STATIC_ARRAY;
    Value** oldarr = *array;
    char* newmem = new char[8 + (*elemsz) * len];
    int* newobjref = (int*) newmem;
    *newobjref = **objref;
    int* newsize = (int*) (newmem + 4);
    *newsize = len;
    Value** newarr = new Value*[len];
    char* newpas_array = (char*) (newmem + 8);
    if (len <= **asize) {
        int cutoff = len;
        for (int i = 0; i < cutoff; i++) {
            newarr[i] = Value::fromTypeMem(*elemType, typemap, (void*) (newpas_array + (*elemsz) * i));
            newarr[i]->set(oldarr[i]);
            delete oldarr[i];
        }
        for (int i = cutoff; i < * * this->asize; i++) {
            delete oldarr[i];
        }
    } else {
        int cutoff = * * this->asize;
        for (int i = 0; i < cutoff; i++) {
            newarr[i] = Value::fromTypeMem(*elemType, typemap, (void*) (newpas_array + (*elemsz) * i));
            newarr[i]->set(oldarr[i]);
            delete oldarr[i];
        }
        for (int i = cutoff; i < len; i++) {
            newarr[i] = Value::fromTypeMem(*elemType, typemap, (void*) (newpas_array + (*elemsz) * i));
        }
    }
    delete [] * array;
    delete [] * mem;
    *array = newarr;
    *mem = newmem;
    *objref = newobjref;
    *asize = newsize;
    *pas_array = newpas_array;
}

void ArrayValue::setIndex(int index, Value* val) throw (int, InterpEx*) {
    index -= *start;
    if (index >= **asize) throw E_INDEX_BOUNDS;
    (*array)[index]->set(val);
}

Value* ArrayValue::getIndex(int index) throw (int, InterpEx*) {
    index -= *start;
    if (index >= **asize) throw E_INDEX_BOUNDS;
    return (*array)[index];
}

int ArrayValue::argSize() {
    return 4;
}

void ArrayValue::valArg(void* mem) {
    *(char**) mem = *pas_array;
}

void ArrayValue::refArg(void* mem) {
    *(char***) mem = pas_array;
}

//**** BEGIN POINTERVALUE DEFINITION ***

PointerValue::PointerValue(Pointer* pt, std::map<int, Type*> &typemap, void* mem) {
    refcount = new int;
    *refcount = 1;
    owns_mem = new bool;
    *owns_mem = false;
    refType = new Type*;
    *refType = (Type*) pt->pointsTo;
    ref = new Value*;
    *ref = new Value();
    pas_ref = (void**) mem;
    (*ref)->refArg((void*) pas_ref);
}

PointerValue::PointerValue(Pointer* pt, std::map<int, Type*> &typemap) : Value(TYPE_POINTER, pt) {
    refcount = new int;
    *refcount = 1;
    owns_mem = new bool;
    *owns_mem = true;
    refType = new Type*;
    *refType = (Type*) pt->pointsTo;
    ref = new Value*;
    *ref = new Value();
    pas_ref = new void*;
    (*ref)->refArg((void*) pas_ref);
}

PointerValue::PointerValue(Pointer* pt) : Value(TYPE_POINTER, pt) {
    refcount = new int;
    *refcount = 1;
    owns_mem = new bool;
    *owns_mem = true;
    refType = new Type*;
    ref = new Value*;
    pas_ref = new void*;
}

PointerValue::PointerValue(PointerValue& val) : Value(TYPE_POINTER, (Type*) val.typeObj) {
    refcount = val.refcount;
    (*refcount)++;
    owns_mem = val.owns_mem;
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
        if (*owns_mem) delete pas_ref;
        delete owns_mem;
    }
}

Value* PointerValue::duplicate() {
    return new PointerValue(*this);
}

Value* PointerValue::clone() {
    PointerValue* pt = new PointerValue((Pointer*) typeObj);
    *(pt->refType) = *refType;
    *(pt->ref) = (*ref)->duplicate();
    (*ref)->refArg((void*) pt->pas_ref);
}

void PointerValue::set(Value* val) throw (int, InterpEx*) {
    if (val->type != TYPE_POINTER) throw E_NOT_POINTER;
    *refType = *((PointerValue*) val)->refType;
    delete *ref;
    *ref = (*((PointerValue*) val)->ref)->duplicate();
    (*ref)->refArg((void*) pas_ref);
}

Value* PointerValue::getRef() throw (int, InterpEx*) {
    return *ref;
}

Value* PointerValue::invoke(Value** args, int numArgs, Frame* cur) throw (int, InterpEx*) {
    if ((*ref)->type != TYPE_METH) throw E_NOT_METHOD;
    return (*ref)->invoke(args,numArgs,cur);
}

//FIXME Should check assignment types

void PointerValue::setRef(Value* ref_impl) throw (int, InterpEx*) {
    delete *ref;
    *ref = ref_impl->duplicate();
    (*ref)->refArg((void*) pas_ref);
}

int PointerValue::argSize() {
    return 4;
}

void PointerValue::valArg(void* mem) {
    *(void**) mem = *pas_ref;
}

void PointerValue::refArg(void* mem) {
    *(void***) mem = pas_ref;
}

//**** BEGIN RECORDVALUE DEFINITION ***

RecordValue::RecordValue(Record* rec, std::map<int, Type*> &typemap, void* mem_impl) : Value(TYPE_RECORD, rec) {
    refcount = new int;
    *refcount = 1;
    owns_mem = new bool;
    *owns_mem = false;
    memsize = new int;
    *memsize = rec->sizeOf(typemap);
    indexes = new int*;
    *indexes = new int[rec->fields.size()];
    mem = new char*;
    *mem = (char*) mem_impl;
    fields = new std::map<int, Value*>;
    std::list<Variable*>::iterator iter = rec->fields.begin();
    std::list<Variable*>::iterator end = rec->fields.end();
    int pos = 0;
    for (int i = 0; iter != end; i++, iter++) {
        Type* ftype = (Type*) (*iter)->type;
        (*fields)[(*iter)->name] = Value::fromTypeMem(ftype, typemap, *mem + pos);
        (*indexes)[i] = ftype->sizeOf(typemap);
        pos += (*indexes)[i];
    }
}

RecordValue::RecordValue(Record* rec, std::map<int, Type*> &typemap) : Value(TYPE_RECORD, rec) {
    refcount = new int;
    *refcount = 1;
    owns_mem = new bool;
    *owns_mem = true;
    memsize = new int;
    *memsize = rec->sizeOf(typemap);
    indexes = new int*;
    *indexes = new int[rec->fields.size()];
    mem = new char*;
    *mem = new char[*memsize];
    fields = new std::map<int, Value*>;
    std::list<Variable*>::iterator iter = rec->fields.begin();
    std::list<Variable*>::iterator end = rec->fields.end();
    int pos = 0;
    for (int i = 0; iter != end; i++, iter++) {
        Type* ftype = (Type*) (*iter)->type;
        (*fields)[(*iter)->name] = Value::fromTypeMem(ftype, typemap, *mem + pos);
        (*indexes)[i] = ftype->sizeOf(typemap);
        pos += (*indexes)[i];
    }
}

RecordValue::RecordValue(Record* rec) : Value(TYPE_RECORD, rec) {
    refcount = new int;
    *refcount = 1;
    owns_mem = new bool;
    *owns_mem = true;
    memsize = new int;
    indexes = new int*;
    fields = new std::map<int, Value*>;
    mem = new char*;
}

RecordValue::RecordValue(RecordValue& val) : Value(TYPE_RECORD, (Type*) val.typeObj) {
    refcount = val.refcount;
    (*refcount)++;
    owns_mem = val.owns_mem;
    memsize = val.memsize;
    indexes = val.indexes;
    fields = val.fields;
    mem = val.mem;
}

RecordValue::~RecordValue() {
    if (!(--(*refcount))) {
        std::map<int, Value*>::iterator iter = fields->begin();
        std::map<int, Value*>::iterator end = fields->end();
        //FIXME - serious leak, some error somewhere
        while (iter != end) {
            delete iter->second;
            iter++;
        }
        if (*owns_mem) delete [] * mem;
        delete [] * indexes;

        delete owns_mem;
        delete indexes;
        delete memsize;
        delete fields;
        delete refcount;
        delete mem;
    }
}

Value* RecordValue::duplicate() {
    return new RecordValue(*this);
}

Value* RecordValue::clone() {
    RecordValue* rec = new RecordValue((Record*) typeObj);

    *rec->memsize = *memsize;
    *rec->indexes = new int[((Record*) typeObj)->fields.size()];
    *rec->mem = new char[*memsize];
    std::list<Variable*>::iterator iter = ((Record*) typeObj)->fields.begin();
    std::list<Variable*>::iterator end = ((Record*) typeObj)->fields.end();
    int pos = 0;
    std::map<int, Type*> typemap;
    for (int i = 0; iter != end; i++, iter++) {
        Type* ftype = (*iter)->type;
        (*rec->fields)[(*iter)->name] = Value::fromTypeMem(ftype, typemap, *rec->mem + pos);
        (*rec->indexes)[i] = (*indexes)[i];
        pos += (*indexes)[i];
    }
}

void RecordValue::set(Value* val) throw (int, InterpEx*) {
    if (val->type != TYPE_RECORD) throw E_NOT_RECORD;
    RecordValue* rec = (RecordValue*) val;
    std::map<int, Value*>::iterator iter = rec->fields->begin();
    std::map<int, Value*>::iterator end = rec->fields->end();
    while (iter != end) {
        (*fields)[iter->first]->set(iter->second);
        iter++;
    }
}

Value* RecordValue::getField(int name) throw (int, InterpEx*) {
    if (fields->find(name) == fields->end()) throw E_NO_FIELD;
    return (*fields)[name];
}

void RecordValue::setField(int name, Value* value) throw (int, InterpEx*) {
    if (fields->find(name) == fields->end()) throw E_NO_FIELD;
    (*fields)[name]->set(value);
}

int RecordValue::argSize() {
    return *memsize;
}

void RecordValue::valArg(void* mem_impl) {
    *(char**) mem_impl = *mem;
}

void RecordValue::refArg(void* mem_impl) {
    *(char***) mem_impl = mem;
}

