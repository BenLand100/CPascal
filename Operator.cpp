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
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int,InterpEx*) {
        Value* b = stack.top();
        stack.pop();
        Value* a = stack.top();
        stack.pop();
        int res;
        if (b->type == TYPE_REAL) {
            double fdiv = (int)b->asReal();
            if (fdiv == 0) throw E_DIV_ZERO;
            res = (a->type == TYPE_REAL ? ((int)a->asReal()) : a->asInteger()) / fdiv;
        } else {
            int idiv = b->asInteger();
            if (idiv == 0) throw E_DIV_ZERO;
            res = (a->type == TYPE_REAL ? ((int)a->asReal()) : a->asInteger()) / idiv;
        }

        stack.push(new IntegerValue(res));
        Value::decref(a);
        Value::decref(b);
    }
};

class FDiv : public Operator {
public:
    FDiv() : Operator(OP_FDIV) { };
    ~FDiv() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int,InterpEx*) {
        Value* b = stack.top();
        stack.pop();
        Value* a = stack.top();
        stack.pop();
        if (b->asReal() == 0) throw E_DIV_ZERO;
        double res = a->asReal() / b->asReal();
        stack.push(new RealValue(res));
        Value::decref(a);
        Value::decref(b);
    }
};

#include <math.h>

class Mod : public Operator {
public:
    Mod() : Operator(OP_MOD) { };
    ~Mod() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int,InterpEx*) {
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
        Value::decref(a);
        Value::decref(b);
    }
};

class Mul : public Operator {
public:
    Mul() : Operator(OP_MUL) { };
    ~Mul() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int,InterpEx*) {
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
        Value::decref(a);
        Value::decref(b);
    }
};

class Add : public Operator {
public:
    Add() : Operator(OP_ADD) { };
    ~Add() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int,InterpEx*) {
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
        Value::decref(a);
        Value::decref(b);
    }
};

class Sub : public Operator {
public:
    Sub() : Operator(OP_SUB) { };
    ~Sub() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int,InterpEx*) {
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
        Value::decref(a);
        Value::decref(b);
    }
};

class Not : public Operator {
public:
    Not() : Operator(OP_NOT) { };
    ~Not() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int,InterpEx*) {
        Value* a = stack.top();
        stack.pop();
        stack.push(new BooleanValue(!a->asBoolean()));
        Value::decref(a);
    }
};

class Or : public Operator {
public:
    Or() : Operator(OP_OR) { };
    ~Or() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int,InterpEx*) {
        Value* b = stack.top();
        stack.pop();
        Value* a = stack.top();
        stack.pop();
        stack.push(new BooleanValue(a->asBoolean() || b->asBoolean()));
        Value::decref(a);
        Value::decref(b);
    }
};

class And : public Operator {
public:
    And() : Operator(OP_AND) { };
    ~And() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int,InterpEx*) {
        Value* b = stack.top();
        stack.pop();
        Value* a = stack.top();
        stack.pop();
        stack.push(new BooleanValue(a->asBoolean() && b->asBoolean()));
        Value::decref(a);
        Value::decref(b);
    }
};

class Equ : public Operator {
public:
    Equ() : Operator(OP_EQU) { };
    ~Equ() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int,InterpEx*) {
        Value* b = stack.top();
        stack.pop();
        Value* a = stack.top();
        stack.pop();
        if (a->type == TYPE_REAL || b->type == TYPE_REAL) {
            stack.push(new BooleanValue(a->asReal() == b->asReal()));
        } else {
            stack.push(new BooleanValue(a->asInteger() == b->asInteger()));
        }
        Value::decref(a);
        Value::decref(b);
    }
};

class Neq : public Operator {
public:
    Neq() : Operator(OP_NEQ) { };
    ~Neq() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int,InterpEx*) {
        Value* b = stack.top();
        stack.pop();
        Value* a = stack.top();
        stack.pop();
        if (a->type == TYPE_REAL || b->type == TYPE_REAL) {
            stack.push(new BooleanValue(a->asReal() != b->asReal()));
        } else {
            stack.push(new BooleanValue(a->asInteger() != b->asInteger()));
        }
        Value::decref(a);
        Value::decref(b);
    }
};

class Less : public Operator {
public:
    Less() : Operator(OP_LESS) { };
    ~Less() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int,InterpEx*) {
        Value* b = stack.top();
        stack.pop();
        Value* a = stack.top();
        stack.pop();
        if (a->type == TYPE_REAL || b->type == TYPE_REAL) {
            stack.push(new BooleanValue(a->asReal() < b->asReal()));
        } else {
            stack.push(new BooleanValue(a->asInteger() < b->asInteger()));
        }
        Value::decref(a);
        Value::decref(b);
    }
};

class LessEq : public Operator {
public:
    LessEq() : Operator(OP_LESSEQ) { };
    ~LessEq() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int,InterpEx*) {
        Value* b = stack.top();
        stack.pop();
        Value* a = stack.top();
        stack.pop();
        if (a->type == TYPE_REAL || b->type == TYPE_REAL) {
            stack.push(new BooleanValue(a->asReal() <= b->asReal()));
        } else {
            stack.push(new BooleanValue(a->asInteger() <= b->asInteger()));
        }
        Value::decref(a);
        Value::decref(b);
    }
};

class Great : public Operator {
public:
    Great() : Operator(OP_GREAT) { };
    ~Great() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int,InterpEx*) {
        Value* b = stack.top();
        stack.pop();
        Value* a = stack.top();
        stack.pop();
        if (a->type == TYPE_REAL || b->type == TYPE_REAL) {
            stack.push(new BooleanValue(a->asReal() > b->asReal()));
        } else {
            stack.push(new BooleanValue(a->asInteger() > b->asInteger()));
        }
        Value::decref(a);
        Value::decref(b);
    }
};

class GreatEq : public Operator {
public:
    GreatEq() : Operator(OP_GREATEQ) { };
    ~GreatEq() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int,InterpEx*) {
        Value* b = stack.top();
        stack.pop();
        Value* a = stack.top();
        stack.pop();
        if (a->type == TYPE_REAL || b->type == TYPE_REAL) {
            stack.push(new BooleanValue(a->asReal() >= b->asReal()));
        } else {
            stack.push(new BooleanValue(a->asInteger() >= b->asInteger()));
        }
        Value::decref(a);
        Value::decref(b);
    }
};

class Addr : public Operator {
public:
    Addr() : Operator(OP_ADDR) { };
    ~Addr() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int,InterpEx*) {
        Value* val = stack.top();
        stack.pop();
        Pointer* pt = Type::getPointerType((Type*)val->typeObj);
        PointerValue* ref = new PointerValue(pt);
        ref->setRef(val);
        stack.push(ref);
        Value::decref(val);
    }
};

class DerefGet : public Operator {
public:
    DerefGet() : Operator(OP_DEREFGET) { };
    ~DerefGet() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int,InterpEx*) {
        Value* ref = stack.top();
        stack.pop();
        stack.push(Value::incref(ref->getRef()));
        Value::decref(ref);
    }
};

class DerefSet : public Operator {
public:
    DerefSet() : Operator(OP_DEREFSET) { };
    ~DerefSet() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int,InterpEx*) {
        Value* val = stack.top();
        stack.pop();
        Value* pval = stack.top();
        stack.pop();
        pval->getRef()->set(val);
        stack.push(val);
        Value::decref(pval);
    }
};

class Neg : public Operator {
public:
    Neg() : Operator(OP_NEG) { };
    ~Neg() { };
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int,InterpEx*) {
        Value* a = stack.top()->clone();
        Value::decref(stack.top());
        stack.pop();
        a->negate();
        stack.push(a);
    }
};

Throw::Throw(int number_impl) : Operator(OP_THROW), number(number_impl) { }
Throw::~Throw() { }
void Throw::preform(std::stack<Value*> &stack, Frame* frame) throw(int,InterpEx*) {
    throw number;
}

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
        case OP_DEREFGET:
            return new DerefGet();
        case OP_DEREFSET:
            return new DerefSet();
        case OP_ADDR:
            return new Addr();
        case OP_NEG:
            return new Neg();
        default:
            return 0;
    }
}

Operator::Operator(int op) : Element(ELEM_OPERATOR), type(op) { }
Operator::~Operator() { }

Asgn::Asgn(int name_impl) : Operator(OP_ASGN), name(name_impl) { }
Asgn::~Asgn() { }
void Asgn::preform(std::stack<Value*> &stack, Frame* frame) throw(int,InterpEx*) {
    Value* val = stack.top();
    Value* var = frame->resolve(name);
    var->set(val);
    Value::decref(var);
}

Invoke::Invoke(std::list<Expression*> args_impl) : Operator(OP_SYMBOL), numArgs(args_impl.size()), args(new Expression*[args_impl.size()]) {
    std::list<Expression*>::iterator iter = args_impl.begin();
    for (int i = 0; i < numArgs; i++) {
        args[i] = *iter;
        iter++;
    }
}

Invoke::~Invoke() {
    for (int i = 0; i < numArgs; i++) {
        delete args[i];
    }
    delete [] args;
}
void Invoke::preform(std::stack<Value*>& stack, Frame* frame)  throw(int,InterpEx*) {
    Value* val = stack.top();
    stack.pop();
    if (val->type != TYPE_METH) throw E_NOT_METHOD;
    Value** vals = new Value*[numArgs];
    for (int i = 0; i < numArgs; i++) {
        vals[i] = evalExpr(args[i],frame,stack); //Beware of corrupting the stack
    }
    stack.push(val->invoke(vals, numArgs, frame));
    Value::decref(val);
    for (int i = 0; i < numArgs; i++) {
        Value::decref(vals[i]);
    }
    delete [] vals;
}

Symbol::Symbol(int name_impl) : Operator(OP_SYMBOL), name(name_impl) { }
Symbol::~Symbol() { }
void Symbol::preform(std::stack<Value*>& stack, Frame* frame)  throw(int,InterpEx*) {
    stack.push(frame->resolve(name));
}

Size::Size(Expression* array_impl) : Operator(OP_SIZE), array(array_impl) { }
Size::~Size() { delete array; }
void Size::preform(std::stack<Value*>& stack, Frame* frame)  throw(int,InterpEx*) {
    Value* arr = evalExpr(array,frame,stack);
    stack.push(new IntegerValue(arr->size()));
    Value::decref(arr);
}

ArrayDef::ArrayDef(std::list<Expression*> elems_impl) : Operator(OP_ARRAYDEF), elems(new Expression*[elems_impl.size()]), numElems(elems_impl.size()) {
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
void ArrayDef::preform(std::stack<Value*>& stack, Frame* frame) throw(int,InterpEx*) {
    Value** vals = new Value*[numElems];
    for (int i = 0; i < numElems; i++)
        vals[i] = evalExpr(elems[i],frame,stack);
    Type* arrType = Type::getDynamicArrayType((Type*)vals[0]->typeObj);
    ArrayValue* val = new ArrayValue((Array*)arrType);
    val->resize(numElems);
    for (int i = 0; i < numElems; i++) {
        val->setIndex(i, vals[i]);
        Value::decref(vals[i]);
    }
    stack.push(val);
}

Resize::Resize(Expression* array_impl, Expression* dim_impl) : Operator(OP_RESIZE), array(array_impl), dim(dim_impl) { }
Resize::~Resize() { delete array; delete dim; }
void Resize::preform(std::stack<Value*>& stack, Frame* frame) throw(int,InterpEx*) {
    Value* arr = evalExpr(array,frame,stack);
    Value* size = evalExpr(dim,frame,stack);
    arr->resize(size->asInteger());
    Value::decref(size);
    stack.push(arr);
}

ArrayGet::ArrayGet(std::list<Expression*> indexes_impl) : Operator(OP_ARRAYGET), indexes(new Expression*[indexes_impl.size()]), numIndexes(indexes_impl.size()) {
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
void ArrayGet::preform(std::stack<Value*>& stack, Frame* frame) throw(int,InterpEx*) {
    Value* arr = stack.top();
    Value* res = 0;
    stack.pop();
    for (int i = 0; i < numIndexes; i++) {
        Value* index = evalExpr(indexes[i],frame,stack);
        res = arr->getIndex(index->asInteger());
        Value::decref(arr);
        Value::decref(index);
        arr = res;
    }
    stack.push(res);
}

ArraySet::ArraySet(std::list<Expression*> indexes_impl) : Operator(OP_ARRAYSET), indexes(new Expression*[indexes_impl.size()]), numIndexes(indexes_impl.size()) {
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
void ArraySet::preform(std::stack<Value*>& stack, Frame* frame) throw(int,InterpEx*) {
    Value* value = stack.top();
    stack.pop();
    Value* arr = stack.top();
    Value* res = arr;
    stack.pop();
    for (int i = 0; i < numIndexes - 1; i++) {
        Value* index = evalExpr(indexes[i],frame,stack);
        res = arr->getIndex(index->asInteger());
        Value::decref(arr);
        Value::decref(index);
        arr = res;
    }
    Value* index = evalExpr(indexes[numIndexes - 1],frame,stack);
    res->setIndex(index->asInteger(),value);
    Value::decref(index);
    Value::decref(res);
    stack.push(value);
}

FieldGet::FieldGet(int name_impl) : Operator(OP_FIELDGET), name(name_impl) { }
FieldGet::~FieldGet() { }
void FieldGet::preform(std::stack<Value*>& stack, Frame* frame) throw(int,InterpEx*) {
    Value* record = stack.top();
    stack.pop();
    Value* res = record->getField(name);
    Value::decref(record);
    stack.push(Value::incref(res));
}

FieldSet::FieldSet(int name_impl) : Operator(OP_FIELDSET), name(name_impl) { }
FieldSet::~FieldSet() { }
void FieldSet::preform(std::stack<Value*>& stack, Frame* frame) throw(int,InterpEx*) {
    Value* val = stack.top();
    stack.pop();
    Value* record = stack.top();
    stack.pop();
    record->setField(name,val);
    Value::decref(record);
    stack.push(val);
}
