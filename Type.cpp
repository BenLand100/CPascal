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

#include "Type.h"
#include "Variable.h"
#include "lexer.h"
#include "Interpreter.h"
#include <sstream>

#include <iostream>
//#define debug(x) std::cout << x << '\n'
#define debug(x)

std::map<std::string, Type*>* Type::alltypes() {
    static std::map<std::string, Type*> types;
    return &types;
}

Meth* Type::getMethodType(Method* meth) {
    int numArgs = meth->numArgs();
    Type* ret = meth->getResultType();
    std::string res(ret ? ret->descr : "nil");
    std::string descr(res);
    descr.append("(");
    for (int i = 0; i < numArgs; i++) {
        descr.append(meth->getArg(i)->type->descr);
        descr.append(";");
    }
    descr.append(")");
    std::map<std::string, Type*>* types = alltypes();
    if (types->find(descr) != types->end())
        return (Meth*)(*types)[descr];
    return new Meth(descr, meth);
}

Record* Type::getRecordType(std::list<Variable*> fields) {
    std::stringstream str;
    str << '{';
    std::list<Variable*>::iterator iter = fields.begin();
    while (iter != fields.end()) {
        str << (*iter)->name << ':' << (*iter)->type->descr << ',';
        iter++;
    }
    str << '}';
    std::string descr(str.str());
    std::map<std::string, Type*>* types = alltypes();
    if (types->find(descr) != types->end())
        return (Record*)(*types)[descr];
    return new Record(descr, fields);
}

Array* Type::getBoundArrayType(Type* element, int from, int to) {
    std::stringstream str;
    str << '[' << from << ".." << to << ']' << element->descr;
    std::string descr(str.str());
    std::map<std::string, Type*>* types = alltypes();
    if (types->find(descr) != types->end())
        return (Array*)(*types)[descr];
    return new Array(descr, element, from, to);

}

Array* Type::getDynamicArrayType(Type* element) {
    std::string descr("[");
    descr.append(element->descr);
    std::map<std::string, Type*>* types = alltypes();
    if (types->find(descr) != types->end())
        return (Array*)(*types)[descr];
    return new Array(descr, element);
}

Pointer* Type::getPointerType(Type* type) {
    std::stringstream str;
    str << type->descr << '^';
    std::string descr(str.str());
    std::map<std::string, Type*>* types = alltypes();
    if (types->find(descr) != types->end())
        return (Pointer*)(*types)[descr];
    return new Pointer(descr, type);
}

Type* Type::getType(int name) {
    std::stringstream str;
    str << name;
    std::string descr(str.str());
    std::map<std::string, Type*>* types = alltypes();
    if (types->find(descr) != types->end())
        return (*types)[descr];
    debug("getType=" << name << ' ' << descr);
    Type* res;
    switch (name) {
        case RES_NIL:
            res = new Type(descr, TYPE_NIL);
            break;
        case RES_STRING:
            res = new Type(descr, TYPE_STRING);
            break;
        case RES_INTEGER:
            res = new Type(descr, TYPE_INTEGER);
            break;
        case RES_REAL:
            res = new Type(descr, TYPE_REAL);
            break;
        case RES_BOOLEAN:
            res = new Type(descr, TYPE_BOOLEAN);
            break;
        case RES_CHAR:
            res = new Type(descr, TYPE_CHAR);
            break;
        default:
            res = new RefType(descr, name);
    }
    (*types)[descr] = res;
    return res;
}

Type* Type::getNil() {
    static Type* nil = getType(RES_NIL);
    return nil;
}

Type* Type::getString() {
    static Type* string = getType(RES_STRING);
    return string;
}

Type* Type::getInteger() {
    static Type* integer = getType(RES_INTEGER);
    return integer;
}

Type* Type::getReal() {
    static Type* real = getType(RES_REAL);
    return real;
}

Type* Type::getBoolean() {
    static Type* boolean = getType(RES_BOOLEAN);
    return boolean;
}

Type* Type::getChar() {
    static Type* chr = getType(RES_CHAR);
    return chr;
}

Type::Type(std::string descr_impl, int type_impl) : type(type_impl), descr(descr_impl) {
}

bool Type::instanceOf(Type* type) {
    return this->type == TYPE_REF ? this->descr == type->descr : this->type == type->type;
}

int Type::sizeOf() {
    switch (type) {
        case TYPE_NIL:
            return *((char*)0);
        case TYPE_STRING:
        case TYPE_INTEGER:
            return 4;
        case TYPE_REAL:
            return 8;
        case TYPE_BOOLEAN:
        case TYPE_CHAR:
            return 1;
        default:
            return -1;
    }
}

Meth::Meth(std::string descr_impl, Method* meth_impl) : Type(descr_impl, TYPE_METH), meth(meth_impl) {
}

bool Meth::instanceOf(Type* type) {
    return this->type == TYPE_METH ? this->descr == type->descr : false;
}

int Meth::sizeOf() {
    return 4;
}

Pointer::Pointer(std::string descr_impl, Type* pointsTo_impl) : Type(descr_impl, TYPE_POINTER), pointsTo(pointsTo_impl) { }

bool Pointer::instanceOf(Type* type) {
    return this->type == type->type && this->pointsTo == ((Pointer*)type)->pointsTo;
}

int Pointer::sizeOf(std::map<int,Type*>& typemap) {
    return 4;
}

Array::Array(std::string descr_impl, Type* element_impl, int from_impl, int to_impl) : Type(descr_impl,TYPE_ARRAY), dynamic(false), element(element_impl), from(from_impl), to(to_impl) { }

Array::Array(std::string descr_impl, Type* element_impl) : Type(descr_impl,TYPE_ARRAY), dynamic(true), element(element_impl), from(0), to(-1)  { }

bool Array::instanceOf(Type* type) {
    return this->type == type->type && this->element == ((Array*)type)->element;
}

int Array::sizeOf() {
    if (dynamic) {
        return 4;
    } else {
        int elemsz = element->sizeOf();
        int len = to - from + 1;
        return elemsz*len;
    }
}

Record::Record(std::string descr_impl, std::list<Variable*>& fields) : Type(descr_impl,TYPE_RECORD), size(0) {
    std::list<Variable*>::iterator iter = fields.begin();
    std::list<Variable*>::iterator end = fields.end();
    for (int i = 0; iter != end; i++, iter++) {
        namemap[(*iter)->name] = i;
        slots.push_back((*iter)->type);
    }
}

bool Record::instanceOf(Type* type) {
    return this->type == type->type && this->slots == ((Record*)type)->slots && (this->namemap == ((Record*)type)->namemap);
}

int Record::getNameSlot(int name) {
    return namemap[name];
}

int Record::sizeOf() {
    if (size) return size;
    std::vector<Type*>::iterator iter = slots.begin();
    std::vector<Type*>::iterator end = slots.end();
    while (iter != end) {
        size += (*iter)->sizeOf();
        iter++;
    }
    return size;
}

RefType::RefType(std::string descr, int name_impl) : Type(descr,TYPE_REF), name(name_impl) { }

bool RefType::instanceOf(Type* type) {
    return this->type == type->type && this->name == ((RefType*)type)->name;
}

int RefType::sizeOf() {
    return 0;
}
