#include "Type.h"
#include "lexer.h"
#include <sstream>

#include <iostream>
//#define debug(x) std::cout << x << '\n'
#define debug(x)

Type* Type::nil = getType(RES_NIL);
Type* Type::string = getType(RES_STRING);
Type* Type::integer = getType(RES_INTEGER);
Type* Type::real = getType(RES_REAL);
Type* Type::boolean = getType(RES_BOOLEAN);
Type* Type::chr = getType(RES_CHAR);

std::map<std::string, Type*> Type::types;

Record* Type::getRecordType(std::map<int,Type*> fields) {
    std::stringstream str;
    str << '{';
    std::list<Variable*> vars;
    std::map<int,Type*>::iterator iter = fields.begin();
    while (iter != fields.end()) {
        vars.push_back(new Variable(iter->first, iter->second));
        str << iter->first << ':' << iter->second->descr << ',';
        iter++;
    }
    str << '}';
    std::string descr(str.str());
    if (types.find(descr) != types.end())
        return (Record*)types[descr];
    return new Record(descr, vars);
}

Array* Type::getBoundArrayType(Type* element, int from, int to) {
    std::stringstream str;
    str << '[' << from << ".." << to << ']' << element->descr;
    std::string descr(str.str());
    if (types.find(descr) != types.end())
        return (Array*)types[descr];
    return new Array(descr, element, from, to);

}

Array* Type::getDynamicArrayType(Type* element) {
    std::stringstream str;
    str << '[' << element->descr;
    std::string descr(str.str());
    if (types.find(descr) != types.end())
        return (Array*)types[descr];
    return new Array(descr, element);
}

Pointer* Type::getPointerType(Type* type) {
    std::stringstream str;
    str << type->descr << '^';
    std::string descr(str.str());
    if (types.find(descr) != types.end())
        return (Pointer*)types[descr];
    return new Pointer(descr, type);
}

Type* Type::getType(int name) {
    std::stringstream str;
    str << name;
    std::string descr(str.str());
    if (types.find(descr) != types.end())
        return types[descr];
    debug("getType=" << name);
    switch (name) {
        case RES_NIL:
            return new Type(descr, TYPE_NIL);
        case RES_STRING:
            return new Type(descr, TYPE_STRING);
        case RES_INTEGER:
            return new Type(descr, TYPE_INTEGER);
        case RES_REAL:
            return new Type(descr, TYPE_REAL);
        case RES_BOOLEAN:
            return new Type(descr, TYPE_BOOLEAN);
        case RES_CHAR:
            return new Type(descr, TYPE_CHAR);
        default:
            return new RefType(descr, name);
    }
}

Type* Type::getNil() {
    return nil;
}

Type* Type::getString() {
    return string;
}

Type* Type::getInteger() {
    return integer;
}

Type* Type::getReal() {
    return real;
}

Type* Type::getBoolean() {
    return boolean;
}

Type* Type::getChar() {
    return chr;
}

Type::Type(std::string descr_impl, int type_impl) : type(type_impl), descr(descr_impl) {
}

bool Type::instanceOf(Type* type) {
    return this->type == TYPE_REF ? this->descr == type->descr : this->type == type->type;
}

Pointer::Pointer(std::string descr_impl, Type* pointsTo_impl) : Type(descr_impl, TYPE_POINTER), pointsTo(pointsTo_impl) { }

bool Pointer::instanceOf(Type* type) {
    return this->type == type->type && this->pointsTo == ((Pointer*)type)->pointsTo;
}

Array::Array(std::string descr_impl, Type* element_impl, int from_impl, int to_impl) : Type(descr_impl,TYPE_ARRAY), element(element_impl), dynamic(false), from(from_impl), to(to_impl) { }

Array::Array(std::string descr_impl, Type* element_impl) : Type(descr_impl,TYPE_ARRAY), element(element_impl), dynamic(true), from(0), to(-1)  { }

bool Array::instanceOf(Type* type) {
    return this->type == type->type && this->element == ((Array*)type)->element;
}

Record::Record(std::string descr_impl, std::list<Variable*> fields_impl) : Type(descr_impl,TYPE_RECORD), fields(fields_impl) { }

bool Record::instanceOf(Type* type) {
    return this->type == type->type && this->fields == ((Record*)type)->fields;
}

RefType::RefType(std::string descr, int name_impl) : Type(descr,TYPE_REF), name(name_impl) { }

bool RefType::instanceOf(Type* type) {
    return this->type == type->type && this->name == ((RefType*)type)->name;
}
