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
#include "Element.h"
#include "Value.h"
#include <list>
#include <map>
#include <vector>

class Expression {
public:
    Expression(std::list<Element*> postfix);
    virtual ~Expression();

    virtual Value* eval(Frame* frame);
protected:
    Expression();
private:
    Element** elems;
    int length;
};

inline void evalBlock(Block* block, Frame* frame) {
    int numElems = block->length;
    Expression** elems = block->elems;
    for (int i = 0; i < numElems; i++, elems++)
        delete (*elems)->eval(frame);
}

inline void cleanBlock(Block* block) {
    if (block->elems) {
        Expression** elems = block->elems;
        int numElems = block->length;
        for (int i = 0; i < numElems; i++) {
            delete elems[i];
        }
        delete elems;
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
    Until(std::list<Expression*> block, Expression* condition);
    ~Until();

    Value* eval(Frame* frame);
private:
    Expression* cond;
    Block block;
};

class Case : public Expression {
public:
    Case(Expression* condition);
    ~Case();

    void setDefault(std::list<Expression*> branch);
    void addBranch(int value, std::list<Expression*> branch);
    Value* eval(Frame* frame);
private:
    Expression* value;
    std::map<int,Block*> branches;
    Block def;
};


class For : public Expression {
public:
    For(int var, Expression* begin, bool inc, Expression* end, std::list<Expression*> block);
    ~For();

    Value* eval(Frame* frame);
private:
    int var;
    Expression* begin;
    Expression* end;
    bool inc;
    Block block;
};

class While : public Expression {
public:
    While(Expression* condition, std::list<Expression*> block);
    ~While();

    Value* eval(Frame* frame);
private:
    Expression* cond;
    Block block;
};

class If : public Expression {
public:
    If(Expression* condition, std::list<Expression*> block);
    ~If();

    void addBranch(Expression* cond, std::list<Expression*> block);
    void setDefault(std::list<Expression*> block);
    Value* eval(Frame* frame);
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
    Try(std::list<Expression*> danger, std::list<Expression*> saftey, std::list<Expression*> always);
    ~Try();

    Value* eval(Frame* frame);
private:
    Block danger;
    Block saftey;
    Block always;
};


#endif	/* _EXPRESSION_H */

