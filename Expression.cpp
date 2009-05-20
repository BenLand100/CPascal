#include "Value.h"
#include <stack>
#include "Expression.h"
#include "Operator.h"

Expression::Expression(int offset_impl) : length(0), elems(0), offset(offset_impl) { }
Expression::Expression(std::list<Element*> postfix,int offset_impl) : length(postfix.size()), elems(new Element*[postfix.size()]), offset(offset_impl)  {
    debug("Expression()");
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
Value* Expression::eval(Frame* frame) throw(InterpEx*, int) {
    std::stack<Value*> stack;
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
    Value* res = stack.top();
    stack.pop();
    while (!stack.empty()) {
        delete stack.top();
        stack.pop();
    }
    return res;
}

Until::Until(std::list<Expression*> block_impl, Expression* cond_impl, int offset_impl) : Expression(offset_impl), cond(cond_impl) {
    fillBlock(&block,&block_impl);
}
Until::~Until() {
    cleanBlock(&block);
    delete cond;
}
Value* Until::eval(Frame* frame) throw(InterpEx*, int) {
    Value* res = 0;
    do {
        if (res) delete res;
        evalBlock(&block,frame);
        res = cond->eval(frame);
    } while (!res->asBoolean());
    return new Value();
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
Value* Case::eval(Frame* frame) throw(InterpEx*, int) {
    Value* val = value->eval(frame);
    int i = val->asInteger();
    delete val;
    if (branches.find(i) != branches.end()) {
        evalBlock(branches[i],frame);
    } else if (def.elems) {
        evalBlock(&def,frame);
    }
    return new Value();
}

For::For(int var_impl, Expression* begin_impl, bool inc_impl, Expression* end_impl, std::list<Expression*> block_impl, int offset_impl) : Expression(offset_impl), var(var_impl), begin(begin_impl), end(end_impl), inc(inc_impl) {
    fillBlock(&block,&block_impl);
}
For::~For() {
    cleanBlock(&block);
    delete begin;
    delete end;
}
Value* For::eval(Frame* frame) throw(InterpEx*, int) {
    Value* varval = frame->resolve(var,NULL,0);
    Value* temp = begin->eval(frame);
    varval->set(temp);
    delete temp;
    if (inc) { //simply reduces tests and increases code size
        temp = end->eval(frame);
        while (varval->asInteger() <= temp->asInteger()) {
            delete temp;
            evalBlock(&block,frame);
            temp = end->eval(frame);
            varval->incr();
        }
        delete temp;
    } else {
        temp = end->eval(frame);
        while (varval->asInteger() <= temp->asInteger()) {
            delete temp;
            evalBlock(&block,frame);
            temp = end->eval(frame);
            varval->decr();
        }
        delete temp;
    }
    return new Value();
}

While::While(Expression* cond_impl, std::list<Expression*> block_impl, int offset_impl) : Expression(offset_impl), cond(cond_impl) {
    fillBlock(&block,&block_impl);
}
While::~While() {
    cleanBlock(&block);
    delete cond;
}
Value* While::eval(Frame* frame) throw(InterpEx*, int) {
    Value* res = cond->eval(frame);
    debug("while_restype=" << res->type);
    while (res->asBoolean()) {
        delete res;
        evalBlock(&block,frame);
        res = cond->eval(frame);
    }
    delete res;
    return new Value();
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
Value* If::eval(Frame* frame) throw(InterpEx*, int) {
    int numBranches = branches.size();
    for (int i = 0; i < numBranches; i++) {
        Value* test = branches[i]->cond->eval(frame);
        if (test->asBoolean()) {
            delete test;
            evalBlock(branches[i]->block,frame);
            return new Value();
        }
        delete test;
    }
    evalBlock(&def,frame);
    return new Value();
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
Value* Try::eval(Frame* frame) throw(InterpEx*, int) {
    try {
        evalBlock(&danger,frame);
    } catch (int) {
        evalBlock(&saftey,frame);
    } catch (InterpEx* ex) {
        delete ex;
        evalBlock(&saftey,frame);
    }
    evalBlock(&always,frame);
    return new Value();
}

