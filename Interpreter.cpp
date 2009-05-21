#include "Interpreter.h"
#include "lexer.h"
#include "parser.h"
#include "Exceptions.h"

#include <cstring>
#include <sstream>
#include <iostream>
#include <ctime>
#include <sys/timeb.h>

//#define debug(x) std::cout << x << '\n'
#define debug(x)

int ms_time() __attribute__((stdcall));
int ms_time() {
    timeb t;
    ftime(&t);
    return (t.time * 1000 + t.millitm) & 0x7FFFFFFF;
}

void ms_wait(int ms) __attribute__((stdcall));
void ms_wait(int ms) {
    struct timespec a;
    a.tv_sec = ms/1000;
    a.tv_nsec = (ms%1000)*1000000L;
    struct timespec b;
    while (nanosleep(&a,&b)) {
        if (!nanosleep(&b,&a)) break;
    }
}

void writeln(char* str) __attribute__((stdcall));
void writeln(char* str) {
    printf("%s\n",str);
}

char* inttostr(int i) __attribute__((stdcall));
char* inttostr(int i) {
    std::stringstream strm;
    strm << i;
    std::string str = strm.str();
    char* res = new char[str.length()+1];
    strcpy(res,str.c_str());
    return res;
}

char* chartostr(char i) __attribute__((stdcall));
char* chartostr(char i) {
    std::stringstream strm;
    strm << i;
    std::string str = strm.str();
    char* res = new char[str.length()+1];
    strcpy(res,str.c_str());
    return res;
}

char* booltostr(bool i) __attribute__((stdcall));
char* booltostr(bool i) {
    std::stringstream strm;
    strm << i;
    std::string str = strm.str();
    char* res = new char[str.length()+1];
    strcpy(res,str.c_str());
    return res;
}

char* realtostr(double i) __attribute__((stdcall));
char* realtostr(double i) {
    std::stringstream strm;
    strm << i;
    std::string str = strm.str();
    char* res = new char[str.length()+1];
    strcpy(res,str.c_str());
    return res;
}

Interpreter::Interpreter(char* ppg) : exception(0) {
    char* tokens = lex(ppg, names);
    prog = parse(tokens);
    freetoks(tokens);
    addMethod((void*)&ms_time,CONV_C_STDCALL,(char*)"function time: integer;");
    addMethod((void*)&ms_wait,CONV_C_STDCALL,(char*)"procedure wait(ms: integer);");
    addMethod((void*)&writeln,CONV_C_STDCALL,(char*)"procedure writeln(str: string);");
    addMethod((void*)&inttostr,CONV_C_STDCALL,(char*)"function inttostr(i: integer): string;");
    addMethod((void*)&chartostr,CONV_C_STDCALL,(char*)"function chartostr(c: char): string;");
    addMethod((void*)&booltostr,CONV_C_STDCALL,(char*)"function booltostr(b: boolean): string;");
    addMethod((void*)&realtostr,CONV_C_STDCALL,(char*)"function realtostr(r: real): string;");
}

Interpreter::~Interpreter() {
    delete prog;
}

void Interpreter::run() {
    if (exception) {
        delete exception;
        exception = 0;
    }
    std::map<std::string, int>::iterator iter = names.begin();
    while (iter != names.end()) {
        debug(iter->first << ">>" << iter->second);
        iter++;
    }
    Frame* frame = new Frame(prog);
    try {
        evalBlock(&prog->block, frame);
    } catch (InterpEx* ex) {
        ex->printStackTrace();
        exception = ex;
    }
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

Value* Frame::resolve(int symbol, Value** args, int numArgs) throw(int, InterpEx*) {
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
                    argsz += meth->arguments[i]->byRef ? 4 : args[i]->argSize();
                char* cargs = new char[argsz];
                char* stack = cargs;
                for (int i = 0; i < numArgs; i++) {
                    if (meth->arguments[i]->byRef) {
                        args[i]->refArg((void*) stack);
                        stack += 4;
                    } else {
                        args[i]->valArg((void*) stack);
                        stack += args[i]->argSize();
                    }
                }
                stack -= 4;
                if (meth->type) {
                    if (meth->type->type == TYPE_REAL) {
                        double real;
                        asm volatile (
                                    "pushl %%ecx \n"
                                    "cmpl  $0, %%edx \n"
                                    "jz mkcall_real \n"
                                    "start_real: pushl (%%eax) \n"
                                    "cmpl %%eax, %%ecx \n"
                                    "jz mkcall_real \n"
                                    "subl $4, %%eax \n"
                                    "jmp start_real \n"
                                    "mkcall_real: call *%%ebx \n"
                                    "popl %%ecx \n"
                                    : "=t"(real)
                                    : "a"(stack), "b"(meth->address), "c"(cargs), "d"(argsz)
                                    : "memory"
                                    );
                        delete[] cargs;
                        return new RealValue(real);
                    } else {
                        void* eax;
                        asm volatile (
                                    "pushl %%ecx \n"
                                    "cmpl  $0, %%edx \n"
                                    "jz mkcall_all \n"
                                    "start_all: pushl (%%eax) \n"
                                    "cmpl %%eax, %%ecx \n"
                                    "jz mkcall_all \n"
                                    "subl $4, %%eax \n"
                                    "jmp start_all \n"
                                    "mkcall_all: call *%%ebx \n"
                                    "popl %%ecx \n"
                                    : "=a" (eax)
                                    : "a"(stack), "b"(meth->address), "c"(cargs), "d"(argsz)
                                    : "memory"
                                    );
                        delete[] cargs;
                        Value* val = Value::fromType(meth->type, typemap);
                        switch (meth->mtype) {
                            case CONV_C_STDCALL:
                                val->read_c(eax);
                                break;
                            case CONV_FPC_STDCALL:
                                val->read_fpc(eax);
                                break;
                        }
                        return val;
                    }
                } else {
                    asm volatile (
                                "pushl %%ecx \n"
                                "cmpl  $0, %%edx \n"
                                "jz mkcall_void \n"
                                "start_void: pushl (%%eax) \n"
                                "cmpl %%eax, %%ecx \n"
                                "jz mkcall_void \n"
                                "subl $4, %%eax \n"
                                "jmp start_void \n"
                                "mkcall_void: call *%%ebx \n"
                                "popl %%ecx \n"
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

void Frame::registerTemp(Value* temp) {
    temps.push_back(temp);
}

void* interp_init(char* ppg) {
    return (void*) new Interpreter(ppg);
}

void interp_meth(void* interp, void* addr, char* def) {
    ((Interpreter*)interp)->addMethod(addr, CONV_FPC_STDCALL, def);
}

void interp_run(void* interp) {
    ((Interpreter*)interp)->run();
}

void interp_free(void* interp) {
    delete (Interpreter*)interp;
}
