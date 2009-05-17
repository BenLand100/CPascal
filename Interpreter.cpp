#include "Interpreter.h"
#include "lexer.h"
#include "parser.h"
#include "Exceptions.h"

#include <iostream>
//#define debug(x) std::cout << x << '\n'
#define debug(x)

Interpreter::Interpreter(char* ppg) {
    char* tokens = lex(ppg, names);
    prog = parse(tokens);
    freetoks(tokens);
}

Interpreter::~Interpreter() {
    delete prog;
}

void Interpreter::run() {
    std::map<std::string, int>::iterator iter = names.begin();
    while (iter != names.end()) {
        debug(iter->first << ">>" << iter->second);
        iter++;
    }
    Frame* frame = new Frame(prog);
    evalBlock(&prog->block, frame);
    delete frame;
}

void Interpreter::addMethod(void* addr, int conv, char* def) {
    char* tokens = lex(def, names);
    char* cur = tokens;
    Method* meth = parseMethod(cur);
    meth->address = addr;
    meth->mtype = conv;
    prog->methods.push_back(meth);
    freetoks(tokens);
}

Frame::Frame(Container* container_impl) : typemap(container_impl->types), container(container_impl), parent(0) {
    init(container_impl);
}

Frame::Frame(Frame* frame, Container* container_impl) : slots(frame->slots), typemap(frame->typemap), container(container_impl), parent(frame) {
    std::map<int, Type*>::iterator iter = container_impl->types.begin();
    std::map<int, Type*>::iterator end = container_impl->types.end();
    while (iter != end) {
        typemap[iter->first] = iter->second;
    }
    init(container_impl);
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
        slots[var->name] = new Slot(Value::fromType((Type*) var->type, typemap));
        debug("init_var=" << var->name);
    }
    std::map<int, Expression*>::iterator iter = container->constants.begin();
    std::map<int, Expression*>::iterator end = container->constants.end();
    while (iter != end) {
        Value* val = iter->second->eval(this);
        slots[iter->first] = new Slot(val);
        debug("init_const=" << iter->first);
        iter++;
    }
}

Frame::~Frame() {
    int numVariables = container->variables.size();
    for (int i = 0; i < numVariables; i++) {
        Slot* s = slots[container->variables[i]->name];
        delete s->value;
        delete s;
    }
    std::map<int, Expression*>::iterator iter = container->constants.begin();
    std::map<int, Expression*>::iterator end = container->constants.end();
    while (iter != end) {
        Slot* s = slots[iter->first];
        delete s->value;
        delete s;
        iter++;
    }
}

Value* Frame::resolve(int symbol, Value** args, int numArgs) throw (int) {
    debug("resolve_symbol=" << symbol);
    Slot* slot;
    if (slots.find(symbol) != slots.end()) {
        slot = slots[symbol];
    } else {
        throw E_UNRESOLVABLE;
    }
    debug("resolve_slot=" << (void*) slot);
    if (!slot) return 0;
    switch (slot->type) {
        case SLOT_METHOD:
        {
            Method* meth = slot->method;
            if (numArgs != meth->arguments.size()) return 0;
            if (meth->address) {
                int argsz = 0;
                for (int i = 0; i < numArgs; i++)
                    argsz += args[i]->argSize();
                char* cargs = new char[argsz];
                char* stack = cargs;
                for (int i = 0; i < numArgs; i++) {
                    args[i]->valArg((void*) stack);
                    stack += args[i]->argSize();
                }
                stack -= 4;
                if (meth->type) {
                    if (meth->type->type == TYPE_REAL) {
                        double real;
                        asm volatile (
                                    "cmpl  $0, %%edx \n"
                                    "jz mkcall_real \n"
                                    "start_real: pushl (%%eax) \n"
                                    "cmpl %%eax, %%ecx \n"
                                    "jz mkcall_real \n"
                                    "subl $4, %%eax \n"
                                    "jmp start_real \n"
                                    "mkcall_real: call *%%ebx \n"
                                    : "=t"(real)
                                    : "a"(stack), "b"(meth->address), "c"(cargs), "d"(argsz)
                                    : "memory"
                                    );
                        delete[] cargs;
                        return new RealValue(real);
                    } else {
                        void* eax;
                        asm volatile (
                                    "cmpl  $0, %%edx \n"
                                    "jz mkcall_all \n"
                                    "start_all: pushl (%%eax) \n"
                                    "cmpl %%eax, %%ecx \n"
                                    "jz mkcall_all \n"
                                    "subl $4, %%eax \n"
                                    "jmp start_all \n"
                                    "mkcall_all: call *%%ebx \n"
                                    : "=a" (eax)
                                    : "a"(stack), "b"(meth->address), "c"(cargs), "d"(argsz)
                                    : "memory"
                                    );
                        delete[] cargs;
                        Value* val = Value::fromType(meth->type, typemap);
                        //val->read(eax);
                        return val;
                    }
                } else {
                    asm volatile (
                                "cmpl  $0, %%edx \n"
                                "jz mkcall_void \n"
                                "start_void: pushl (%%eax) \n"
                                "cmpl %%eax, %%ecx \n"
                                "jz mkcall_void \n"
                                "subl $4, %%eax \n"
                                "jmp start_void \n"
                                "mkcall_void: call *%%ebx \n"
                                :
                                : "a"(stack), "b"(meth->address), "c"(cargs), "d"(argsz)
                                : "memory"
                                );
                    delete[] cargs;
                    return new Value();
                }
            } else {
                //FIXME must check method types
                Frame* frame = new Frame(this, meth);
                debug("resolve_args");
                for (int i = 0; i < numArgs; i++) {
                    Variable* var = meth->arguments[i];
                    frame->slots[var->name] = new Slot(var->byRef ? args[i]->duplicate() : args[i]->clone());
                }
                if (meth->type) frame->slots[RES_RESULT] = new Slot(Value::fromType(meth->type, typemap));
                debug("resolve_method");
                evalBlock(&meth->block, frame);
                debug("resolve_return");
                if (meth->type) {
                    Value* val = frame->slots[RES_RESULT]->value->duplicate();
                    delete frame;
                    return val;
                } else {
                    delete frame;
                    return new Value();
                }
            }
        }
        break;
        case SLOT_VALUE:
        {
            debug("resolve_value=" << slot->value);
            return slot->value->duplicate();
        }
        break;
    }
}

