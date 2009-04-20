
#include <iostream>
#include "Value.h"
#include "Type.h"
#include "Expression.h"
#include "Operator.h"
#include "lexer.h"

class Neg : public Operator {
public:
    Neg() : Operator(OP_NEG) { };
    ~Neg() { };
    void preform(std::stack<Value*> &stack, Frame* frame) {
        Value* a = stack.top()->clone();
        delete stack.top();
        stack.pop();
        a->negate();
        stack.push(a);
    }
};

Operator* Operator::get(int type) {
    std::cout << "Operator::get(" << type << ")\n";
    switch (type) {
        case OP_NEG:
            return new Neg();
    }
}

Operator::Operator(int op) : type(op), Element(ELEM_OPERATOR) { }
Operator::~Operator() { }

Symbol::Symbol(int name_impl, std::list<Expression*> args_impl) : Operator(OP_SYMBOL), name(name_impl), numArgs(args_impl.size()), args(new Expression*[args_impl.size()]) {
    std::list<Expression*>::iterator iter = args_impl.begin();
    for (int i = 0; i < numArgs; i++) {
        args[i] = *iter;
        iter++;
    }
}
Symbol::Symbol(int name_impl) : Operator(OP_SYMBOL), name(name_impl), numArgs(0), args(0) { }
Symbol::~Symbol() {
    for (int i = 0; i < numArgs; i++) {
        delete args[i];
    }
    delete args;
}
void Symbol::preform(std::stack<Value*>& stack, Frame* frame) {
    Value** vals = new Value*[numArgs];
    for (int i = 0; i < numArgs; i++) {
        vals[i] = args[i]->eval(frame);
    }
    stack.push(frame->resolve(name,vals,numArgs)->duplicate());
    for (int i = 0; i < numArgs; i++) {
        delete vals[i];
    }
}

Size::Size(Expression* array_impl) : Operator(OP_SIZE), array(array_impl) { }
Size::~Size() { delete array; }
void Size::preform(std::stack<Value*>& stack, Frame* frame) {
    Value* arr = array->eval(frame);
    stack.push(new IntegerValue(arr->size()));
    delete arr;
}

ArrayDef::ArrayDef(std::list<Expression*> elems_impl) : Operator(OP_ARRAYDEF), numElems(elems_impl.size()), elems(new Expression*[elems_impl.size()]) {
    std::list<Expression*>::iterator iter = elems_impl.begin();
    for (int i = 0; i < numElems; i++) {
        elems[i] = *iter;
        iter++;
    }
}
ArrayDef::~ArrayDef() {
    for (int i = 0; i < numElems; i++) {
        delete elems[i];
    }
    delete elems;
}
void ArrayDef::preform(std::stack<Value*>& stack, Frame* frame) {
    Value** vals = new Value*[numElems];
    for (int i = 0; i < numElems; i++)
        vals[i] = elems[i]->eval(frame);
    Type* arrType = Type::getDynamicArrayType((Type*)vals[0]->typeObj);
    std::map<int,Type*> typemap;
    ArrayValue* val = new ArrayValue((Array*)arrType,typemap);
    val->resize(numElems);
    for (int i = 0; i < numElems; i++) {
        val->setIndex(i, vals[i]);
        delete vals[i];
    }
    stack.push(val);
}

Resize::Resize(Expression* array_impl, Expression* dim_impl) : Operator(OP_RESIZE), array(array_impl), dim(dim_impl) { }
Resize::~Resize() { delete array; delete dim; }
void Resize::preform(std::stack<Value*>& stack, Frame* frame) {
    Value* arr = array->eval(frame);
    Value* size = dim->eval(frame);
    arr->resize(size->asInteger());
    delete size;
    stack.push(arr);
}

ArrayGet::ArrayGet(std::list<Expression*> indexes_impl) : Operator(OP_ARRAYGET), numIndexes(indexes_impl.size()), indexes(new Expression*[indexes_impl.size()]) {
    std::list<Expression*>::iterator iter = indexes_impl.begin();
    for (int i = 0; i < numIndexes; i++) {
        indexes[i] = *iter;
        iter++;
    }
}
ArrayGet::~ArrayGet() {
    for (int i = 0; i < numIndexes; i++) {
        delete indexes[i];
    }
    delete indexes;
}
void ArrayGet::preform(std::stack<Value*>& stack, Frame* frame) {
    Value* arr = stack.top();
    Value* res = arr;
    stack.pop();
    for (int i = 0; i < numIndexes; i++) {
        Value* index = indexes[i]->eval(frame);
        res = res->getIndex(index->asInteger());
        delete index;
    }
    stack.push(res->duplicate());
    delete arr;
}

ArraySet::ArraySet(std::list<Expression*> indexes_impl) : Operator(OP_ARRAYSET), numIndexes(indexes_impl.size()), indexes(new Expression*[indexes_impl.size()]) {
    std::list<Expression*>::iterator iter = indexes_impl.begin();
    for (int i = 0; i < numIndexes; i++) {
        indexes[i] = *iter;
        iter++;
    }
}
ArraySet::~ArraySet() {
    for (int i = 0; i < numIndexes; i++) {
        delete indexes[i];
    }
    delete indexes;
}
void ArraySet::preform(std::stack<Value*>& stack, Frame* frame) {
    Value* value = stack.top();
    stack.pop();
    Value* arr = stack.top();
    Value* res = arr;
    stack.pop();
    for (int i = 0; i < numIndexes - 1; i++) {
        Value* index = indexes[i]->eval(frame);
        res = res->getIndex(index->asInteger());
        delete index;
    }
    Value* index = indexes[numIndexes - 1]->eval(frame);
    res->setIndex(index->asInteger(),value);
    delete index;
    delete arr;
    stack.push(value);
}

FieldGet::FieldGet(int name_impl) : Operator(OP_FIELDGET), name(name_impl) { }
FieldGet::~FieldGet() { }
void FieldGet::preform(std::stack<Value*>& stack, Frame* frame) {
    Value* record = stack.top();
    stack.pop();
    Value* res = record->getField(name);
    delete record;
    stack.push(res);
}

FieldSet::FieldSet(int name_impl) : Operator(OP_FIELDSET), name(name_impl) { }
FieldSet::~FieldSet() { }
void FieldSet::preform(std::stack<Value*>& stack, Frame* frame) {
    Value* val = stack.top();
    stack.pop();
    Value* record = stack.top();
    stack.pop();
    record->setField(name,val);
    delete record;
    stack.push(val);
}
