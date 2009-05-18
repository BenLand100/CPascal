
#include <stack>


#include <iostream>
#include "Value.h"
#include "Type.h"
#include "Expression.h"
#include "Operator.h"
#include "lexer.h"
#include "Exceptions.h"

class IDiv : public Operator {
public:
    IDiv() : Operator(OP_IDIV) { };
    ~IDiv() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int) {
        Value* b = stack.top();
        stack.pop();
        Value* a = stack.top();
        stack.pop();
        int res;
        if (b->type == TYPE_REAL) {
            int idiv = (int)b->asReal();
            if (idiv == 0) throw E_DIV_ZERO;
            res = (a->type == TYPE_REAL ? ((int)a->asReal()) : a->asInteger()) / idiv;
        } else {
            int idiv = b->asInteger();
            if (idiv == 0) throw E_DIV_ZERO;
            res = (a->type == TYPE_REAL ? ((int)a->asReal()) : a->asInteger()) / idiv;
        }
        stack.push(new IntegerValue(res));
        delete a;
        delete b;
    }
};

class FDiv : public Operator {
public:
    FDiv() : Operator(OP_FDIV) { };
    ~FDiv() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int) {
        Value* b = stack.top();
        stack.pop();
        Value* a = stack.top();
        stack.pop();
        if (b->asReal() == 0) throw E_DIV_ZERO;
        double res = a->asReal() / b->asReal();
        stack.push(new RealValue(res));
        delete a;
        delete b;
    }
};

#include <math.h>

class Mod : public Operator {
public:
    Mod() : Operator(OP_MOD) { };
    ~Mod() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int) {
        Value* b = stack.top();
        stack.pop();
        Value* a = stack.top();
        stack.pop();
        if (a->type == TYPE_REAL || b->type == TYPE_REAL) {
            if (b->asReal() == 0) throw E_DIV_ZERO;
            double res = fmod(a->asReal(),b->asReal());
            stack.push(new RealValue(res));
        } else {
            if (b->asInteger() == 0) throw E_DIV_ZERO;
            int res = a->asInteger() % b->asInteger();
            stack.push(new IntegerValue(res));
        }
        delete a;
        delete b;
    }
};

class Mul : public Operator {
public:
    Mul() : Operator(OP_MUL) { };
    ~Mul() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int) {
        Value* b = stack.top();
        stack.pop();
        Value* a = stack.top();
        stack.pop();
        if (a->type == TYPE_REAL || b->type == TYPE_REAL) {
            double res = a->asReal() * b->asReal();
            stack.push(new RealValue(res));
        } else {
            int res = a->asInteger() * b->asInteger();
            stack.push(new IntegerValue(res));
        }
        delete a;
        delete b;
    }
};

class Add : public Operator {
public:
    Add() : Operator(OP_ADD) { };
    ~Add() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int) {
        Value* b = stack.top();
        stack.pop();
        Value* a = stack.top();
        stack.pop();
        if (a->type == TYPE_STRING || b->type == TYPE_STRING) {
            std::string str(a->asString());
            str.append(b->asString());
            stack.push(new StringValue(str));
        } else if (a->type == TYPE_REAL || b->type == TYPE_REAL) {
            double res = a->asReal() + b->asReal();
            stack.push(new RealValue(res));
        } else {
            int res = a->asInteger() + b->asInteger();
            stack.push(new IntegerValue(res));
        }
        delete a;
        delete b;
    }
};

class Sub : public Operator {
public:
    Sub() : Operator(OP_SUB) { };
    ~Sub() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int) {
        Value* b = stack.top();
        stack.pop();
        Value* a = stack.top();
        stack.pop();
        if (a->type == TYPE_REAL || b->type == TYPE_REAL) {
            double res = a->asReal() - b->asReal();
            stack.push(new RealValue(res));
        } else {
            int res = a->asInteger() - b->asInteger();
            stack.push(new IntegerValue(res));
        }
        delete a;
        delete b;
    }
};

class Not : public Operator {
public:
    Not() : Operator(OP_NOT) { };
    ~Not() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int) {
        Value* a = stack.top();
        stack.pop();
        stack.push(new BooleanValue(!a->asBoolean()));
        delete a;
    }
};

class Or : public Operator {
public:
    Or() : Operator(OP_OR) { };
    ~Or() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int) {
        Value* b = stack.top();
        stack.pop();
        Value* a = stack.top();
        stack.pop();
        stack.push(new BooleanValue(a->asBoolean() || b->asBoolean()));
        delete a;
        delete b;
    }
};

class And : public Operator {
public:
    And() : Operator(OP_AND) { };
    ~And() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int) {
        Value* b = stack.top();
        stack.pop();
        Value* a = stack.top();
        stack.pop();
        stack.push(new BooleanValue(a->asBoolean() && b->asBoolean()));
        delete a;
        delete b;
    }
};

class Equ : public Operator {
public:
    Equ() : Operator(OP_EQU) { };
    ~Equ() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int) {
        Value* b = stack.top();
        stack.pop();
        Value* a = stack.top();
        stack.pop();
        if (a->type == TYPE_REAL || b->type == TYPE_REAL) {
            stack.push(new BooleanValue(a->asReal() == b->asReal()));
        } else {
            stack.push(new BooleanValue(a->asInteger() == b->asInteger()));
        }
        delete a;
        delete b;
    }
};

class Neq : public Operator {
public:
    Neq() : Operator(OP_NEQ) { };
    ~Neq() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int) {
        Value* b = stack.top();
        stack.pop();
        Value* a = stack.top();
        stack.pop();
        if (a->type == TYPE_REAL || b->type == TYPE_REAL) {
            stack.push(new BooleanValue(a->asReal() != b->asReal()));
        } else {
            stack.push(new BooleanValue(a->asInteger() != b->asInteger()));
        }
        delete a;
        delete b;
    }
};

class Less : public Operator {
public:
    Less() : Operator(OP_LESS) { };
    ~Less() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int) {
        Value* b = stack.top();
        stack.pop();
        Value* a = stack.top();
        stack.pop();
        if (a->type == TYPE_REAL || b->type == TYPE_REAL) {
            stack.push(new BooleanValue(a->asReal() < b->asReal()));
        } else {
            stack.push(new BooleanValue(a->asInteger() < b->asInteger()));
        }
        delete a;
        delete b;
    }
};

class LessEq : public Operator {
public:
    LessEq() : Operator(OP_LESSEQ) { };
    ~LessEq() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int) {
        Value* b = stack.top();
        stack.pop();
        Value* a = stack.top();
        stack.pop();
        if (a->type == TYPE_REAL || b->type == TYPE_REAL) {
            stack.push(new BooleanValue(a->asReal() <= b->asReal()));
        } else {
            stack.push(new BooleanValue(a->asInteger() <= b->asInteger()));
        }
        delete a;
        delete b;
    }
};

class Great : public Operator {
public:
    Great() : Operator(OP_GREAT) { };
    ~Great() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int) {
        Value* b = stack.top();
        stack.pop();
        Value* a = stack.top();
        stack.pop();
        if (a->type == TYPE_REAL || b->type == TYPE_REAL) {
            stack.push(new BooleanValue(a->asReal() > b->asReal()));
        } else {
            stack.push(new BooleanValue(a->asInteger() > b->asInteger()));
        }
        delete a;
        delete b;
    }
};

class GreatEq : public Operator {
public:
    GreatEq() : Operator(OP_GREATEQ) { };
    ~GreatEq() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int) {
        Value* b = stack.top();
        stack.pop();
        Value* a = stack.top();
        stack.pop();
        if (a->type == TYPE_REAL || b->type == TYPE_REAL) {
            stack.push(new BooleanValue(a->asReal() >= b->asReal()));
        } else {
            stack.push(new BooleanValue(a->asInteger() >= b->asInteger()));
        }
        delete a;
        delete b;
    }
};

class Asgn : public Operator {
public:
    Asgn() : Operator(OP_ASGN) { };
    ~Asgn() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int) {
        Value* val = stack.top();
        stack.pop();
        Value* var = stack.top();
        stack.pop();
        var->set(val);
        stack.push(val);
        delete var;
    }
};

class Addr : public Operator {
public:
    Addr() : Operator(OP_ADDR) { };
    ~Addr() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int) {
        Value* val = stack.top();
        stack.pop();
        Pointer* pt = Type::getPointerType((Type*)val->typeObj);
        PointerValue* ref = new PointerValue(pt,frame->typemap);
        ref->setRef(val);
        stack.push(ref);
        delete val;
    }
};

class DerefGet : public Operator {
public:
    DerefGet() : Operator(OP_DEREFGET) { };
    ~DerefGet() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int) {
        Value* ref = stack.top();
        stack.pop();
        stack.push(ref->getRef()->duplicate());
        delete ref;
    }
};

class DerefSet : public Operator {
public:
    DerefSet() : Operator(OP_DEREFSET) { };
    ~DerefSet() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int) {
        Value* val = stack.top();
        stack.pop();
        Value* pval = stack.top();
        stack.pop();
        pval->getRef()->set(val);
        stack.push(val);
        delete pval;
    }
};

class Neg : public Operator {
public:
    Neg() : Operator(OP_NEG) { };
    ~Neg() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int) {
        Value* a = stack.top()->clone();
        delete stack.top();
        stack.pop();
        a->negate();
        stack.push(a);
    }
};

Operator* Operator::get(int type) {
    switch (type) {
        case OP_IDIV:
            return new IDiv();
        case OP_FDIV:
            return new FDiv();
        case OP_MOD:
            return new Mod();
        case OP_MUL:
            return new Mul();
        case OP_ADD:
            return new Add();
        case OP_SUB:
            return new Sub();
        case OP_NOT:
            return new Not();
        case OP_OR:
            return new Or();
        case OP_AND:
            return new And();
        case OP_EQU:
            return new Equ();
        case OP_NEQ:
            return new Neq();
        case OP_LESS:
            return new Less();
        case OP_LESSEQ:
            return new LessEq();
        case OP_GREAT:
            return new Great();
        case OP_GREATEQ:
            return new GreatEq();
        case OP_ASGN:
            return new Asgn();
        case OP_DEREFGET:
            return new DerefGet();
        case OP_DEREFSET:
            return new DerefSet();
        case OP_ADDR:
            return new Addr();
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
    delete [] args;
}
void Symbol::preform(std::stack<Value*>& stack, Frame* frame)  throw(int) {
    Value** vals = new Value*[numArgs];
    for (int i = 0; i < numArgs; i++) {
        vals[i] = args[i]->eval(frame);
    }
    stack.push(frame->resolve(name,vals,numArgs));
    for (int i = 0; i < numArgs; i++) {
        delete vals[i];
    }
}

Size::Size(Expression* array_impl) : Operator(OP_SIZE), array(array_impl) { }
Size::~Size() { delete array; }
void Size::preform(std::stack<Value*>& stack, Frame* frame)  throw(int) {
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
    delete [] elems;
}
void ArrayDef::preform(std::stack<Value*>& stack, Frame* frame) throw(int) {
    Value** vals = new Value*[numElems];
    for (int i = 0; i < numElems; i++)
        vals[i] = elems[i]->eval(frame);
    Type* arrType = Type::getDynamicArrayType((Type*)vals[0]->typeObj);
    ArrayValue* val = new ArrayValue((Array*)arrType,frame->typemap);
    val->resize(numElems, frame->typemap);
    for (int i = 0; i < numElems; i++) {
        val->setIndex(i, vals[i]);
        delete vals[i];
    }
    stack.push(val);
}

Resize::Resize(Expression* array_impl, Expression* dim_impl) : Operator(OP_RESIZE), array(array_impl), dim(dim_impl) { }
Resize::~Resize() { delete array; delete dim; }
void Resize::preform(std::stack<Value*>& stack, Frame* frame) throw(int) {
    Value* arr = array->eval(frame);
    Value* size = dim->eval(frame);
    arr->resize(size->asInteger(), frame->typemap);
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
    delete [] indexes;
}
void ArrayGet::preform(std::stack<Value*>& stack, Frame* frame) throw(int) {
    Value* arr = stack.top();
    Value* res = arr;
    stack.pop();
    for (int i = 0; i < numIndexes; i++) {
        Value* index = indexes[i]->eval(frame);
        int i = index->asInteger();
        res = res->getIndex(i);
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
    delete [] indexes;
}
void ArraySet::preform(std::stack<Value*>& stack, Frame* frame) throw(int) {
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
void FieldGet::preform(std::stack<Value*>& stack, Frame* frame) throw(int) {
    Value* record = stack.top();
    stack.pop();
    Value* res = record->getField(name);
    delete record;
    stack.push(res->duplicate());
}

FieldSet::FieldSet(int name_impl) : Operator(OP_FIELDSET), name(name_impl) { }
FieldSet::~FieldSet() { }
void FieldSet::preform(std::stack<Value*>& stack, Frame* frame) throw(int) {
    Value* val = stack.top();
    stack.pop();
    Value* record = stack.top();
    stack.pop();
    record->setField(name,val);
    delete record;
    stack.push(val);
}
