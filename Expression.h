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

class Expression;
class Until;
class Case;
class For;
class While;
class If;
class Try;

#ifndef _EXPRESSION_H
#define	_EXPRESSION_H

typedef struct _Block {
    _Block() : elems(0), length(0) { }
    Expression** elems;
    int length;
} Block;

#include "Interpreter.h"
#include "Exceptions.h"
#include "Element.h"
#include "Value.h"
#include <list>
#include <map>
#include <vector>
#include <stack>

#include <iostream>
//#define debug(x) std::cout << x << '\n'
#define debug(x)

class Expression {
public:
    Expression(std::list<Element*> postfix, int offset);
    virtual ~Expression();

    const int offset;

    virtual void eval(Frame* frame, std::stack<Value*>& stack) throw(InterpEx*, int);
    //virtual void native(std::stack<Value*> &stack, std::vector<std::string> &native);
protected:
    Expression(int offset);
private:
    Element** elems;
    int length;
};

inline Value* evalExpr(Expression* expr, Frame* frame, std::stack<Value*>& stack) throw(InterpEx*,int)  {
    int start = stack.size();
    expr->eval(frame,stack);
    Value* res = stack.top();
    stack.pop();
    int num = stack.size() - start;
    for (int i = 0; i < num; i++) {
        Value::decref(stack.top());
        stack.pop();
    }
    return res;
}

inline void cleanStack(std::stack<Value*>& stack) {
    while (!stack.empty()) {
        Value::decref(stack.top());
        stack.pop();
    }
}

inline void evalBlock(Block* block, Frame* frame, std::stack<Value*>& stack) throw(InterpEx*,int)  {
    Expression** elems = block->elems;
    Expression** end = elems + block->length;
    debug("evalBlock{" << elems << "}");
    while (elems != end) {
        debug("eval " << *elems);
        try {
            int start = stack.size();
            (*elems)->eval(frame,stack);
            int num = stack.size() - start;
            for (int i = 0; i < num; i++) {
                Value::decref(stack.top());
                stack.pop();
            }
            elems++;
        } catch (InterpEx* ex) {
            debug("tossed");
            ex->addTrace((*elems)->offset);
            cleanStack(stack);
            throw ex;
        } catch (int exi) {
            debug("caught");
            InterpEx* ex = new InterpEx(exi);
            ex->addTrace((*elems)->offset);
            cleanStack(stack);
            throw ex;
        }
    }
}

inline void evalBlock(Block* block, Frame* frame) throw(InterpEx*,int) {
    std::stack<Value*> stack;
    evalBlock(block,frame,stack);
    cleanStack(stack);
}

inline void cleanBlock(Block* block) {
    if (block->elems) {
        Expression** elems = block->elems;
        int numElems = block->length;
        for (int i = 0; i < numElems; i++) {
            delete elems[i];
        }
        delete [] elems;
    }
    block->elems = 0;
    block->length = 0;
}

inline void fillBlock(Block* block, std::list<Expression*>* exprs) {
    cleanBlock(block);
    block->length = exprs->size();
    block->elems = new Expression*[block->length];
    std::list<Expression*>::iterator iter = exprs->begin();
    std::list<Expression*>::iterator end = exprs->end();
    for (Expression** exprs = block->elems; iter != end; exprs++, iter++)
        *exprs = *iter;
}

class Until : public Expression {
public:
    Until(std::list<Expression*> block, Expression* condition, int offset);
    ~Until();

    void eval(Frame* frame, std::stack<Value*>& stack) throw(InterpEx*, int);
    //void native(std::stack<Value*> &stack,  std::vector<std::string> &native) throw(int,InterpEx*);
private:
    Expression* cond;
    Block block;
};

class Case : public Expression {
public:
    Case(Expression* condition, int offset);
    ~Case();

    void setDefault(std::list<Expression*> branch);
    void addBranch(int value, std::list<Expression*> branch);
    void eval(Frame* frame, std::stack<Value*>& stack) throw(InterpEx*, int);
    //void native(std::stack<Value*> &stack,  std::vector<std::string> &native) throw(int,InterpEx*);
private:
    Expression* value;
    std::map<int,Block*> branches;
    Block def;
};


class For : public Expression {
public:
    For(int var, Expression* begin, bool inc, Expression* end, std::list<Expression*> block, int offset);
    ~For();

    void eval(Frame* frame, std::stack<Value*>& stack) throw(InterpEx*, int);
    //void native(std::stack<Value*> &stack,  std::vector<std::string> &native) throw(int,InterpEx*);
private:
    int var;
    Expression* begin;
    Expression* end;
    bool inc;
    Block block;
};

class While : public Expression {
public:
    While(Expression* condition, std::list<Expression*> block, int offset);
    ~While();

    void eval(Frame* frame, std::stack<Value*>& stack) throw(InterpEx*, int);
    //void native(std::stack<Value*> &stack,  std::vector<std::string> &native) throw(int,InterpEx*);
private:
    Expression* cond;
    Block block;
};

class If : public Expression {
public:
    If(Expression* condition, std::list<Expression*> block, int offset);
    ~If();

    void addBranch(Expression* cond, std::list<Expression*> block);
    void setDefault(std::list<Expression*> block);
    void eval(Frame* frame, std::stack<Value*>& stack) throw(InterpEx*, int);
    //void native(std::stack<Value*> &stack,  std::vector<std::string> &native) throw(int,InterpEx*);
private:
    typedef struct {
        Expression* cond;
        Block* block;
    } Condition;
    std::vector<Condition*> branches;
    Block def;
};

class Try : public Expression {
public:
    Try(std::list<Expression*> danger, std::list<Expression*> saftey, std::list<Expression*> always, int offset);
    ~Try();

    void eval(Frame* frame, std::stack<Value*>& stack) throw(InterpEx*, int);
    //void native(std::stack<Value*> &stack,  std::vector<std::string> &native) throw(int,InterpEx*);
private:
    Block danger;
    Block saftey;
    Block always;
};


#endif	/* _EXPRESSION_H */

