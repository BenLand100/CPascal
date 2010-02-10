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

#include "Value.h"
#include "Exceptions.h"
#include "lexer.h"
#include "Interpreter.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
//#define debug(x) std::cout << x << '\n'
#define debug(x)

Value* Value::fromType(Type* type) throw (int, InterpEx*) {
    debug("fromType " << type->type);
    if (type->type == TYPE_REF) throw E_REF_TYPE;
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
            return new RecordValue((Record*) type);
        case TYPE_ARRAY:
            return new ArrayValue((Array*) type);
        case TYPE_POINTER:
            return new PointerValue((Pointer*) type);
        default:
            return 0;
    }
}

Value* Value::fromTypeMem(Type* type, void* mem)  throw (int, InterpEx*) {
    debug("fromType " << type->type);
    if (type->type == TYPE_REF) throw E_REF_TYPE;
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
            return new RecordValue((Record*) type, mem);
        case TYPE_ARRAY:
            return new ArrayValue((Array*) type, mem);
        case TYPE_POINTER:
            return new PointerValue((Pointer*) type, mem);
        default:
            return 0;
    }
}

Value* Value::incref(Value *val) throw(int,InterpEx*) {
    ++val->refcount;
    return val;
}

Value* Value::decref(Value *val) throw(int,InterpEx*) {
    if (--val->refcount)
        return val;
    delete val;
    return 0;
}

Value::Value() : Element(ELEM_VALUE), typeObj(Type::getNil()), type(TYPE_NIL), refcount(1) {
}

Value::Value(int impl_type, Type* impl_typeObj, bool ownsmem) : Element(ELEM_VALUE), typeObj(impl_typeObj), type(impl_type), refcount(1), owns_mem(ownsmem) {
}

Value::~Value() {
}

void Value::set(Value* val) throw (int, InterpEx*) {
    throw E_NULL_VAL;
}

Value* Value::clone() {
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

void Value::resize(int size) throw (int, InterpEx*) {
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

Value* Value::invoke(Value** args, unsigned int numArgs, Frame* frame) throw (int, InterpEx*) {
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

MethodValue::MethodValue(void* mem, Meth* type) : Value(TYPE_METH, type, false) {
    meth = (Method**) mem;
    *meth = 0;
}

MethodValue::MethodValue(Method* meth_impl) : Value(TYPE_METH, meth_impl->val_type, true) {
    meth = new Method*;
    *meth = meth_impl;
}

MethodValue::~MethodValue() {
    if (owns_mem) delete meth;
}

Value* MethodValue::clone() {
    return new MethodValue(*meth);
}

void MethodValue::set(Value* val) throw (int, InterpEx*) {
    if (val->type != TYPE_METH) throw E_NOT_METHOD;
    *meth = *((MethodValue*) val)->meth;
}

Value* MethodValue::invoke(Value** args, unsigned int numArgs, Frame* cur) throw (int, InterpEx*) {
    Method* meth = *this->meth;
    //std::cout << "N:" << meth->name << " A:" << meth->address << " G:" << numArgs << " E:" << meth->arguments.size() << '\n';
    if (numArgs != meth->numArgs()) throw E_WRONG_NUM_ARG;
    if (meth->address) {
        int argsz = 0;
        for (unsigned int i = 0; i < numArgs; i++)
            argsz += meth->getArg(i)->byRef ? 4 : args[i]->argSize();
        bool refarg = false;
        //This IF works for reference (pointer) types secret arg passing
        if ((meth->mtype == CONV_FPC_STDCALL) && meth->getResultType()) {
            switch (meth->getResultType()->type) {
                case TYPE_STRING:
                    argsz+=4;
                    refarg = true;
            }
        }
        char* cargs = new char[argsz];
        char* stack = cargs;
        int ref = 0; //Might not be used, but alloc here.
        //The following if will push a fake ref arg onto the stack if it is needed
        if ((meth->mtype == CONV_FPC_STDCALL) && refarg) {
            debug("Pushing void ref");
            ((void**)stack)[0] = &ref;
            stack += 4;
        }
        for (unsigned int i = 0; i < numArgs; i++) {
            if (meth->getArg(i)->byRef) {
                args[i]->refArg((void*) stack);
                debug("Arg: " << *(int*)stack);
                stack += 4;
            } else {
                args[i]->valArg((void*) stack);
                debug("Arg: " << *(int*)stack);
                stack += args[i]->argSize();
            }
        }
        stack -= 4;
        if (meth->getResultType()) {
            if (meth->getResultType()->type == TYPE_REAL) {
                double real;
                asm (
                    "pushl %%ecx \n"
                    "start_real: cmpl %%eax, %%ecx \n"
                    "ja mkcall_real \n"
                    "pushl (%%eax) \n"
                    "subl $4, %%eax \n"
                    "jmp start_real \n"
                    "mkcall_real: call *%%edx \n"
                    "popl %%ecx \n"
                    : "=t"(real)
                    : "a"(stack), "c"(cargs), "d"(meth->address)
                    : "memory"
                );
                delete[] cargs;
                return new RealValue(real);
            } else {
                int eax;
                asm (
                    "pushl %%ecx \n"
                    "start_all: cmpl %%eax, %%ecx \n"
                    "ja mkcall_all \n"
                    "pushl (%%eax) \n"
                    "subl $4, %%eax \n"
                    "jmp start_all \n"
                    "mkcall_all: call *%%edx \n"
                    "popl %%ecx \n"
                    : "=a"(eax)
                    : "a"(stack), "c"(cargs), "d"(meth->address)
                    : "memory"
                );
                delete[] cargs;
                Value* val = Value::fromType(meth->getResultType());
                if ((meth->mtype == CONV_FPC_STDCALL) && refarg) eax = ref;
                switch (meth->mtype) {
                    case CONV_C_STDCALL:
                        val->read_c((void*)eax);
                        break;
                    case CONV_FPC_STDCALL:
                        val->read_fpc((void*)eax);
                        break;
                }
                return val;
            }
        } else {
            asm (
                "pushl %%ecx \n"
                "start_void: cmpl %%eax, %%ecx \n"
                "ja mkcall_void \n"
                "pushl (%%eax) \n"
                "subl $4, %%eax \n"
                "jmp start_void \n"
                "mkcall_void: call *%%edx \n"
                "popl %%ecx \n"
                :
                : "a"(stack), "c"(cargs), "d"(meth->address)
                : "memory"
            );
            delete[] cargs;
            return new Value();
        }
    } else {
        //FIXME must check method types
        Frame* frame = new Frame(cur, meth, args);
        try {
            evalBlock(&meth->block, frame);
        } catch (InterpEx* ex) {
            if (ex->getCause() == E_EXIT) {
                delete ex;
                goto exit;
            }
            delete frame;
            throw ex;
        } catch (int exi) {
            if (exi == E_EXIT) goto exit;
            delete frame;
            throw exi;
        }
        exit:
        debug("resolve_return");
        if (meth->getResultType()) {
            Value* res = Value::incref(frame->slots[meth->getResultSlot()]);
            delete frame;
            return res;
        } else {
            delete frame;
            return new Value();
        }
    }
}

//**** BEGIN INTEGERVALUE DEFINITION ***

IntegerValue::IntegerValue(void* mem) : Value(TYPE_INTEGER, Type::getInteger(), false) {
    integer = (int*) mem;
    *integer = 0;
}

IntegerValue::IntegerValue(int i) : Value(TYPE_INTEGER, Type::getInteger(), true) {
    integer = new int;
    *integer = i;
}

IntegerValue::~IntegerValue() {
    if (owns_mem) delete integer;
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

StringValue::StringValue(void* mem_impl) : Value(TYPE_STRING, Type::getString(), false) {
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

StringValue::StringValue(char* str_impl) : Value(TYPE_STRING, Type::getString(), true) {
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

StringValue::StringValue(std::string cpp_str) : Value(TYPE_STRING, Type::getString(), true) {
    int len = cpp_str.length();
    mem = new char*;
    *mem = new char[8 + len + 1];
    objref = new int*;
    *objref = (int*) (*mem);
    **objref = -1;
    size = new int*;
    *size = (int*) (*mem + 4);
    **size = len;
    str = new char*;
    *str = (char*) (*mem + 8);
    strcpy(*str, cpp_str.c_str());
}

StringValue::~StringValue() {
    delete [] * mem;
    if (owns_mem) delete mem;
    delete objref;
    delete size;
    delete str;
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
    **objref = -1;
    *size = (int*) (*mem + 4);
    **size = len;
    *str = (char*) (*mem + 8);
    strcpy(*str, *sval->str);
}

char* StringValue::asString() throw (int, InterpEx*) {
    return *str;
}

void StringValue::setIndex(int index, Value* val) throw (int, InterpEx*) {
    if (index < 1 || index > **size) throw E_INDEX_BOUNDS;
    (*str)[index-1] = val->asChar();
}

Value* StringValue::getIndex(int index) throw (int, InterpEx*) {
    if (index < 1 || index > **size) throw E_INDEX_BOUNDS;
    return new CharValue((*str)[index-1]);
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

RealValue::RealValue(void* mem) : Value(TYPE_REAL, Type::getReal(), false) {
    real = (double*) mem;
    *real = 0.0;
}

RealValue::RealValue(double real_impl) : Value(TYPE_REAL, Type::getReal(), true) {
    real = new double;
    *real = real_impl;
}

RealValue::~RealValue() {
    if (owns_mem) delete real;
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

CharValue::CharValue(void* mem) : Value(TYPE_CHAR, Type::getChar(), false) {
    chr = (char*) mem;
    *chr = 0;
    str = new char*;
    *str = new char[2];
    (*str)[1] = 0;
}

CharValue::CharValue(char chr_impl) : Value(TYPE_CHAR, Type::getChar(), true) {
    chr = new char;
    *chr = chr_impl;
    str = new char*;
    *str = new char[2];
    (*str)[1] = 0;
}

CharValue::~CharValue() {
    if (owns_mem) delete chr;
    delete [] * str;
    delete str;
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

BooleanValue::BooleanValue(void* mem) : Value(TYPE_BOOLEAN, Type::getBoolean(), false) {
    boolean = (char*) mem;
    *boolean = false;
}

BooleanValue::BooleanValue(bool boolean_impl) : Value(TYPE_BOOLEAN, Type::getBoolean(), true) {
    boolean = new char;
    *boolean = boolean_impl != false ? 1 : 0;
}

BooleanValue::~BooleanValue() {
    if (owns_mem) delete boolean;
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

ArrayValue::ArrayValue(Array* arr, void* mem_impl) : Value(TYPE_ARRAY, arr, false) {
    elemType = new Type*;
    *elemType = (Type*) arr->element;
    dynamic = new bool;
    *dynamic = arr->dynamic;
    start = new int;
    *start = arr->from;

    elemsz = new int;
    *elemsz = arr->element->sizeOf();
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
        (*array)[i] = Value::fromTypeMem(*elemType, (void*) (*pas_array + (*elemsz) * i));
    }
}

ArrayValue::ArrayValue(Array* arr) : Value(TYPE_ARRAY, arr, true) {
    elemType = new Type*;
    *elemType = (Type*) arr->element;
    dynamic = new bool;
    *dynamic = arr->dynamic;
    start = new int;
    *start = arr->from;

    elemsz = new int;
    *elemsz = arr->element->sizeOf();
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
        (*array)[i] = Value::fromTypeMem(*elemType,  (void*) (*pas_array + (*elemsz) * i));
    }
}

ArrayValue::ArrayValue(Array* arr, bool ownsmem) : Value(TYPE_ARRAY, arr, ownsmem) {
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

ArrayValue::~ArrayValue() {
    if (*dynamic) {
        --**objref;
        for (int i = 0; i < **asize; i++) {
            Value::decref((*array)[i]);
        }
        delete [] * array;
        delete [] * mem;
        if (owns_mem) delete mem;
    } else {
        for (int i = 0; i < **asize; i++) {
             Value::decref((*array)[i]);
        }
        delete [] * array;
        if (owns_mem) delete [] * mem;
        delete mem;
        delete *objref;
        delete *asize;
    }
    delete elemType;
    delete dynamic;
    delete start;
    delete objref;
    delete asize;
    delete array;
    delete pas_array;
    delete elemsz;
}

Value* ArrayValue::clone() {
    ArrayValue* clone = new ArrayValue((Array*) typeObj, true);
    *clone->elemType = *elemType;
    *clone->dynamic = *dynamic;
    *clone->start = *start;

    *clone->elemsz = *elemsz;
    Array* arr = (Array*) typeObj;
    int numelems = arr->to - arr->from + 1; //to == -1 and from == 0 for dynamic just for this

    if (arr->dynamic) {
        numelems = **asize;
        *clone->mem = new char[8 + (*elemsz) * numelems];
        *clone->objref = (int*) * clone->mem;
        **clone->objref = 1;
        *clone->asize = (int*) (*clone->mem + 4);
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
        *clone->pas_array = (char*) (*clone->mem + 8);
    } else {
        *clone->pas_array = *clone->mem;
    }

    for (int i = 0; i < numelems; i++) {
        (*clone->array)[i] = Value::fromTypeMem(*elemType, (void*) (*clone->pas_array + (*elemsz) * i));
        (*clone->array)[i]->set((*array)[i]);
    }
    return clone;
}

void ArrayValue::set(Value* val) throw (int, InterpEx*) {
    if (val->type != TYPE_ARRAY) throw E_NOT_ARRAY;
    ArrayValue *arr = (ArrayValue*)val;
    if (*dynamic) {
        if (**asize != **arr->asize) resize(**arr->asize);
        for (int i = 0; i < **asize; i++) {
            (*array)[i]->set((*arr->array)[i]);
        }
    } else {
        if (**asize != **arr->asize) throw E_INDEX_BOUNDS;
        for (int i = 0; i < **asize; i++) {
            (*array)[i]->set((*arr->array)[i]);
        }
    }

}

int ArrayValue::size() throw (int, InterpEx*) {
    return **asize;
}

void ArrayValue::resize(int len) throw (int, InterpEx*) {
    if (!*dynamic) throw E_STATIC_ARRAY;

    char* mem = new char[8 + (*elemsz) * len];
    int* objref = (int*) mem;
    *objref = 1;
    int* asize = (int*) (mem + 4);
    *asize = len;
    Value** array = new Value*[len];
    char* pas_array = (char*) (mem + 8);

    for (int i = 0; i < len; i++) {
        array[i] = Value::fromTypeMem(*elemType, (void*) (pas_array + (*elemsz) * i));
    }
    int min = **this->asize < len ? **this->asize : len;
    for (int i = 0; i < min; i++) {
        array[i]->set((*this->array)[i]);
    }
    for (int i = 0; i < **this->asize; i++) {
        Value::decref((*this->array)[i]);
    }
    delete [] * this->array;
    delete [] * this->mem;

    *this->mem = mem;
    *this->objref = objref;
    *this->asize = asize;
    *this->array = array;
    *this->pas_array = pas_array;

}

void ArrayValue::setIndex(int index, Value* val) throw (int, InterpEx*) {
    index -= *start;
    if (index >= **asize)
        throw E_INDEX_BOUNDS;
    (*array)[index]->set(val);
}

Value* ArrayValue::getIndex(int index) throw (int, InterpEx*) {
    index -= *start;
    if (index >= **asize)
        throw E_INDEX_BOUNDS;
    return Value::incref((*array)[index]);
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

PointerValue::PointerValue(Pointer* pt, void* mem) : Value(TYPE_POINTER, pt, false) {
    refType = new Type*;
    *refType = (Type*) pt->pointsTo;
    ref = new Value*;
    *ref = new Value();
    pas_ref = (void**) mem;
    (*ref)->refArg((void*) pas_ref);
}

PointerValue::PointerValue(Pointer* pt) : Value(TYPE_POINTER, pt, true) {
    refType = new Type*;
    *refType = (Type*) pt->pointsTo;
    ref = new Value*;
    *ref = new Value();
    pas_ref = new void*;
    (*ref)->refArg((void*) pas_ref);
}

PointerValue::PointerValue(Pointer* pt, bool internal) : Value(TYPE_POINTER, pt, true) {
    refType = new Type*;
    ref = new Value*;
    pas_ref = new void*;
}

PointerValue::~PointerValue() {
    delete refType;
     Value::decref(*ref);
    delete ref;
    if (owns_mem) delete pas_ref;
}

Value* PointerValue::clone() {
    PointerValue* pt = new PointerValue((Pointer*) typeObj, true);
    *(pt->refType) = *refType;
    *(pt->ref) = Value::incref(*ref);
    (*ref)->refArg((void*) pt->pas_ref);
    return pt;
}

void PointerValue::set(Value* val) throw (int, InterpEx*) {
    if (val->type != TYPE_POINTER) throw E_NOT_POINTER;
    *refType = *((PointerValue*) val)->refType;
     Value::decref(*ref);
    *ref = Value::incref(*((PointerValue*) val)->ref);
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
     Value::decref(*ref);
    *ref = Value::incref(ref_impl);
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

RecordValue::RecordValue(Record* rec, void* mem_impl) : Value(TYPE_RECORD, rec, false) {
    memsize = new int;
    *memsize = rec->sizeOf();
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
        (*fields)[(*iter)->name] = Value::fromTypeMem(ftype, *mem + pos);
        (*indexes)[i] = ftype->sizeOf();
        pos += (*indexes)[i];
    }
}

RecordValue::RecordValue(Record* rec) : Value(TYPE_RECORD, rec, true) {
    memsize = new int;
    *memsize = rec->sizeOf();
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
        (*fields)[(*iter)->name] = Value::fromTypeMem(ftype, *mem + pos);
        (*indexes)[i] = ftype->sizeOf();
        pos += (*indexes)[i];
    }
}

RecordValue::RecordValue(Record* rec, bool internal) : Value(TYPE_RECORD, rec, true) {
    memsize = new int;
    indexes = new int*;
    fields = new std::map<int, Value*>;
    mem = new char*;
}

RecordValue::~RecordValue() {
    std::map<int, Value*>::iterator iter = fields->begin();
    std::map<int, Value*>::iterator end = fields->end();
    while (iter != end) {
        Value::decref(iter->second);
        iter++;
    }
    if (owns_mem) delete [] * mem;
    delete [] * indexes;

    delete indexes;
    delete memsize;
    delete fields;
    delete mem;
}

Value* RecordValue::clone() {
    RecordValue* rec = new RecordValue((Record*) typeObj, true);

    *rec->memsize = *memsize;
    *rec->indexes = new int[((Record*) typeObj)->fields.size()];
    *rec->mem = new char[*memsize];
    std::list<Variable*>::iterator iter = ((Record*) typeObj)->fields.begin();
    std::list<Variable*>::iterator end = ((Record*) typeObj)->fields.end();
    int pos = 0;
    for (int i = 0; iter != end; i++, iter++) {
        Type* ftype = (*iter)->type;
        (*(rec->fields))[(*iter)->name] = Value::fromTypeMem(ftype, *(rec->mem) + pos);
        (*(rec->fields))[(*iter)->name]->set((*(fields))[(*iter)->name]);
        (*(rec->indexes))[i] = (*indexes)[i];
        pos += (*indexes)[i];
    }
    return rec;
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

