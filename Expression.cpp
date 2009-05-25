#include "Expression.h"
#include "Value.h"
#include "Operator.h"

#include <stack>

Expression::Expression(int offset_impl) : length(0), elems(0), offset(offset_impl) { }
Expression::Expression(std::list<Element*> postfix,int offset_impl) : length(postfix.size()), elems(new Element*[postfix.size()]), offset(offset_impl)  {
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
                stack.push(((Value*)elem)->duplicate());
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
    do {
        if (res) delete res;
        evalBlock(&block,frame);
        res = evalExpr(cond,frame,stack);
    } while (!res->asBoolean());
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
    delete val;
    if (branches.find(i) != branches.end()) {
        evalBlock(branches[i],frame);
    } else if (def.elems) {
        evalBlock(&def,frame);
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
    delete temp;
    if (inc) { //simply reduces tests and increases code size
        temp = evalExpr(end,frame,stack);
        while (varval->asInteger() <= temp->asInteger()) {
            delete temp;
            evalBlock(&block,frame);
            temp = evalExpr(end,frame,stack);
            varval->incr();
        }
        delete temp;
    } else {
        temp = evalExpr(end,frame,stack);
        while (varval->asInteger() <= temp->asInteger()) {
            delete temp;
            evalBlock(&block,frame);
            temp = evalExpr(end,frame,stack);
            varval->decr();
        }
        delete temp;
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
    while (res->asBoolean()) {
        delete res;
        evalBlock(&block,frame);
        res = evalExpr(cond,frame,stack);
    }
    delete res;
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
            delete test;
            evalBlock(branches[i]->block,frame);
            return;
        }
        delete test;
    }
    evalBlock(&def,frame);
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
        evalBlock(&danger,frame);
    } catch (int) {
        evalBlock(&saftey,frame);
    } catch (InterpEx* ex) {
        delete ex;
        evalBlock(&saftey,frame);
    }
    evalBlock(&always,frame);
}

