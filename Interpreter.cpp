#include "Interpreter.h"
#include "lexer.h"
#include "parser.h"

#include <iostream>
#define debug(x) std::cout << x << '\n'

Interpreter::Interpreter(char* ppg) {
    char* tokens = lex(ppg,names);
    prog = parse(tokens);
    freetoks(tokens);
}

Interpreter::~Interpreter() {
    delete prog;
}

void Interpreter::run() {
    Frame* frame = new Frame(prog);
    evalBlock(&prog->block, frame);
    delete frame;
}

Frame::Frame(Container* container) : typemap(container->types), parent(0) {
    init(container);
}

Frame::Frame(Frame* frame, Container* container) : parent(frame) {
    typemap.insert(frame->typemap.begin(), frame->typemap.end());
    init(container);
}

void Frame::init(Container* container) {
    debug("init_frame");
    int numMethods = container->methods.size();
    for (int i = 0; i < numMethods; i++) {
        Method* meth = container->methods[i];
        slots[meth->name] = new Slot(meth);
        debug("init_meth=" << meth->name);
    }
    int numVariables = container->variables.size();
    for (int i = 0; i < numVariables; i++) {
        Variable* var = container->variables[i];
        slots[var->name] = new Slot(Value::fromType((Type*)var->type,typemap));
        debug("init_var=" << var->name);
    }
    std::map<int,Expression*>::iterator iter = container->constants.begin();
    std::map<int,Expression*>::iterator end = container->constants.end();
    while (iter != end) {
        Value* val = iter->second->eval(this);
        slots[iter->first] = new Slot(val);
        debug("init_const=" << iter->first);
        iter++;
    }
}

Frame::~Frame() {
    std::map<int,Slot*>::iterator iter = slots.begin();
    std::map<int,Slot*>::iterator end = slots.end();
    while (iter != end) {
        if (iter->second->type == SLOT_VALUE)
            delete iter->second->value;
        delete iter->second;
        iter++;
    }
}

Frame::Slot* Frame::find(int symbol) {
    Slot* slot = 0;
    if (slots.find(symbol) != slots.end()) {
        slot = slots[symbol];
    } else if (parent) {
        slot = parent->find(symbol);
    }
    return slot;
}

Value* Frame::resolve(int symbol, Value** args, int numArgs) {
    debug("resolve_symbol=" << symbol);
    Slot* slot = find(symbol);
    debug("resolve_slot=" << (void*)slot);
    if (!slot) return 0;
    switch (slot->type) {
        case SLOT_METHOD: {
            Method* meth = slot->method;
            if (numArgs != meth->arguments.size()) return 0;
            Frame* frame = new Frame(this, meth);
            debug("resolve_args");
            for (int i = 0; i < numArgs; i++) {
                Variable* var = meth->arguments[i];
                frame->slots[var->name] = new Slot(var->byRef ? args[i]->duplicate() : args[i]->clone());
            }
            if (meth->type) frame->slots[RES_RESULT] = new Slot(Value::fromType(meth->type,typemap));
            debug("resolve_method");
            evalBlock(&meth->block,frame);
            debug("resolve_return");
            if (meth->type) {
                Value* val = frame->slots[RES_RESULT]->value->duplicate();
                delete frame;
                return val;
            } else {
                delete frame;
                return new Value();
            }
        } break;
        case SLOT_VALUE: {
            debug("resolve_value=" << slot->value);
            return slot->value;
        } break;
    }
}

