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

#include "Expression.h"
#include "Value.h"
#include "Operator.h"

#include <stack>

Expression::Expression(int offset_impl) : offset(offset_impl), elems(0), length(0) { }
Expression::Expression(std::list<Element*> postfix,int offset_impl) :  offset(offset_impl), elems(new Element*[postfix.size()]), length(postfix.size())  {
    std::list<Element*>::iterator iter = postfix.begin();
    for (int i = 0; i < length; i++)
        elems[i] = *(iter++);
}
Expression::~Expression() {
    if (elems) {
        for (int i = 0; i < length; i++)
            delete elems[i];
        delete [] elems;
    }
}

void Expression::eval(Frame* frame, std::stack<Value*>& stack) throw(InterpEx*, int) {
    for (int i = 0; i < length; i++) {
        Element* elem = elems[i];
        switch(elem->eltype) {
            case ELEM_VALUE:
                debug("\t-Value{" << ((Value*)elem)->type << "}");
                stack.push(Value::incref(((Value*)elem)));
                break;
            case ELEM_OPERATOR:
                debug("\t-Operator{" << ((Operator*)elem)->type << "}");
                ((Operator*)elem)->preform(stack,frame);
                break;
        }
    }
}

Until::Until(std::list<Expression*> block_impl, Expression* cond_impl, int offset_impl) : Expression(offset_impl), cond(cond_impl) {
    fillBlock(&block,&block_impl);
}
Until::~Until() {
    cleanBlock(&block);
    delete cond;
}
void Until::eval(Frame* frame, std::stack<Value*>& stack) throw(InterpEx*, int) {
    Value* res = 0;
    try {
        do {
            if (res) Value::decref(res);
            evalBlock(&block,frame,stack);
            res = evalExpr(cond,frame,stack);
        } while (!res->asBoolean());
        Value::decref(res);
    } catch (int exi) {
        if (exi == E_BREAK)
            return;
        throw exi;
    } catch (InterpEx* ex) {
        if (ex->getCause() == E_BREAK) {
            delete ex;
            return;
        }
        throw ex;
    }
}

Case::Case(Expression* condition, int offset_impl) : Expression(offset_impl), value(condition) { }
Case::~Case() {
    delete value;
    std::map<int,Block*>::iterator iter = branches.begin();
    while (iter != branches.end()) {
        cleanBlock(iter->second);
        delete iter->second;
        iter++;
    }
    cleanBlock(&def);
}
void Case::setDefault(std::list<Expression*> branch) {
    fillBlock(&def,&branch);
}
void Case::addBranch(int value, std::list<Expression*> branch) {
    Block* block = new Block;
    fillBlock(block, &branch);
    if (branches.find(value) != branches.end()) {
        Block* old = branches[value];
        cleanBlock(old);
        delete old;
    }
    branches[value] = block;
}
void Case::eval(Frame* frame, std::stack<Value*>& stack) throw(InterpEx*, int) {
    Value* val = evalExpr(value,frame,stack);
    int i = val->asInteger();
    Value::decref(val);
    try {
        if (branches.find(i) != branches.end()) {
            evalBlock(branches[i],frame,stack);
        } else if (def.elems) {
            evalBlock(&def,frame,stack);
        }
    } catch (int exi) {
        if (exi == E_BREAK)
            return;
        throw exi;
    } catch (InterpEx* ex) {
        if (ex->getCause() == E_BREAK) {
            delete ex;
            return;
        }
        throw ex;
    }
}

For::For(int var_impl, Expression* begin_impl, bool inc_impl, Expression* end_impl, std::list<Expression*> block_impl, int offset_impl) : Expression(offset_impl), var(var_impl), begin(begin_impl), end(end_impl), inc(inc_impl) {
    fillBlock(&block,&block_impl);
}
For::~For() {
    cleanBlock(&block);
    delete begin;
    delete end;
}
void For::eval(Frame* frame, std::stack<Value*>& stack) throw(InterpEx*, int) {
    Value* varval = frame->resolve(var);
    Value* temp = evalExpr(begin,frame,stack);
    varval->set(temp);
    Value::decref(temp);
    try {
        if (inc) { //simply reduces tests and increases code size
            temp = evalExpr(end,frame,stack);
            while (varval->asInteger() <= temp->asInteger()) {
                Value::decref(temp);
                evalBlock(&block,frame,stack);
                temp = evalExpr(end,frame,stack);
                varval->incr();
            }
            Value::decref(temp);
        } else {
            temp = evalExpr(end,frame,stack);
            while (varval->asInteger() >= temp->asInteger()) {
                Value::decref(temp);
                evalBlock(&block,frame,stack);
                temp = evalExpr(end,frame,stack);
                varval->decr();
            }
            Value::decref(temp);
        }
        Value::decref(varval);
    } catch (int exi) {
        Value::decref(varval);
        if (exi == E_BREAK)
            return;
        throw exi;
    } catch (InterpEx* ex) {
        Value::decref(varval);
        if (ex->getCause() == E_BREAK) {
            delete ex;
            return;
        }
        throw ex;
    }
}

While::While(Expression* cond_impl, std::list<Expression*> block_impl, int offset_impl) : Expression(offset_impl), cond(cond_impl) {
    fillBlock(&block,&block_impl);
}
While::~While() {
    cleanBlock(&block);
    delete cond;
}
void While::eval(Frame* frame, std::stack<Value*>& stack) throw(InterpEx*, int) {
    Value* res = evalExpr(cond,frame,stack);
    debug("while_restype=" << res->type);
    try {
        while (res->asBoolean()) {
            Value::decref(res);
            evalBlock(&block,frame,stack);
            res = evalExpr(cond,frame,stack);
        }
        Value::decref(res);
    } catch (int exi) {
        if (exi == E_BREAK)
            return;
        throw exi;
    } catch (InterpEx* ex) {
        if (ex->getCause() == E_BREAK) {
            delete ex;
            return;
        }
        throw ex;
    }
}

If::If(Expression* cond_impl, std::list<Expression*> block_impl, int offset_impl) : Expression(offset_impl) {
    addBranch(cond_impl,block_impl);
}
If::~If() {
    int numBranches = branches.size();
    for (int i = 0; i < numBranches; i++) {
        Condition* cond = branches[i];
        cleanBlock(cond->block);
        delete cond->block;
        delete cond->cond;
        delete cond;
    }
    cleanBlock(&def);
}
void If::addBranch(Expression* cond_impl, std::list<Expression*> block_impl) {
    Condition* cond = new Condition();
    cond->cond = cond_impl;
    cond->block = new Block;
    fillBlock(cond->block,&block_impl);
    branches.push_back(cond);
}
void If::setDefault(std::list<Expression*> block) {
    fillBlock(&def,&block);
}
void If::eval(Frame* frame, std::stack<Value*>& stack) throw(InterpEx*, int) {
    int numBranches = branches.size();
    for (int i = 0; i < numBranches; i++) {
        Value* test = evalExpr(branches[i]->cond,frame,stack);
        if (test->asBoolean()) {
            Value::decref(test);
            evalBlock(branches[i]->block,frame,stack);
            return;
        }
        Value::decref(test);
    }
    evalBlock(&def,frame,stack);
}

Try::Try(std::list<Expression*> danger_impl, std::list<Expression*> saftey_impl, std::list<Expression*> always_impl, int offset_impl) : Expression(offset_impl) {
    fillBlock(&danger,&danger_impl);
    fillBlock(&saftey,&saftey_impl);
    fillBlock(&always,&always_impl);
}
Try::~Try() {
    cleanBlock(&danger);
    cleanBlock(&saftey);
    cleanBlock(&always);
}
void Try::eval(Frame* frame, std::stack<Value*>& stack) throw(InterpEx*, int) {
    try {
        evalBlock(&danger,frame,stack);
    } catch (int exi) {
        if (InterpEx::getType(exi) == E_NOCATCH) {
            evalBlock(&always,frame,stack);
            throw exi;
        }
        evalBlock(&saftey,frame,stack);
    } catch (InterpEx* ex) {
        if (ex->getType() == E_NOCATCH) {
            evalBlock(&always,frame,stack);
            throw ex;
        }
        delete ex;
        evalBlock(&saftey,frame,stack);
    }
    evalBlock(&always,frame,stack);
}

