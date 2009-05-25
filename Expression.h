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
protected:
    Expression(int offset);
private:
    Element** elems;
    int length;
};

inline Value* evalExpr(Expression* expr, Frame* frame, std::stack<Value*>& stack) {
    expr->eval(frame,stack);
    Value* res = stack.top();
    stack.pop();
    return res;
}

inline void cleanStack(std::stack<Value*>& stack) {
    while (!stack.empty()) {
        delete stack.top();
        stack.pop();
    }
}

inline void evalBlock(Block* block, Frame* frame) throw(InterpEx*) {
    std::stack<Value*> stack;
    int numElems = block->length;
    Expression** elems = block->elems;
    debug("evalBlock{" << elems << "," << numElems << "}");
    for (int i = 0; i < numElems; i++) {
        debug("eval " << elems[i]);
        try {
            elems[i]->eval(frame,stack);
        } catch (InterpEx* ex) {
            debug("tossed");
            ex->addTrace(elems[i]->offset);
            throw ex;
        } catch (int exi) {
            debug("caught");
            InterpEx* ex = new InterpEx(exi);
            ex->addTrace(elems[i]->offset);
            throw ex;
        }
    }
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
private:
    Block danger;
    Block saftey;
    Block always;
};


#endif	/* _EXPRESSION_H */

