class Interpreter;
class Frame;


#ifndef _INTERPRETER_H
#define	_INTERPRETER_H

#include "lexer.h"
#include "Container.h"
#include "Value.h"
#include "Expression.h"

class Interpreter {
public:
    Interpreter(char* ppg);
    ~Interpreter();

    void run();
    void addMethod(void* addr,  int conv, char* def);
private:
    InterpEx* exception;
    Program* prog;
    std::map<std::string,int> names;
};

class Frame {
    friend class MethodValue;
    friend class PointerValue;
public:
    Frame(Frame* frame, Container* container);
    Frame(Container* container);
    ~Frame();

    Value* resolve(int symbol) throw(int,InterpEx*);
    void registerTemp(Value* temp);

    std::map<int,Type*> typemap;
private:
    std::map<int,Value*> slots;
    std::list<Value*> temps; //FIXME - use this to make memory management faster
    Frame* parent;
    Container* container;

    void init(Container* container);
};

extern "C" {
    void* interp_init(char* pchar) __attribute__((cdecl));
    void interp_meth(void* interp, void* addr, char* def) __attribute__((cdecl));
    void interp_run(void* interp) __attribute__((cdecl));
    void interp_free(void* interp) __attribute__((cdecl));
}

#endif	/* _INTERPRETER_H */

