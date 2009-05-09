class Interpreter;
class Frame;

#ifndef _INTERPRETER_H
#define	_INTERPRETER_H

#include "Container.h"
#include "Value.h"
#include "Expression.h"

class Interpreter {
public:
    Interpreter(char* ppg);
    ~Interpreter();

    void run();
    void addCMethod(void* addr, char* def); //stdcall only
private:
    Program* prog;
    std::map<std::string,int> names;
};

#define SLOT_METHOD 0
#define SLOT_VALUE  1

class Frame {
public:
    Frame(Frame* frame, Container* container);
    Frame(Container* container);
    ~Frame();

    typedef struct _Slot {
        _Slot(Method* m) : type(SLOT_METHOD), method(m) { };
        _Slot(Value* v) : type(SLOT_VALUE), value(v) { };
        int type;
        union {
            Method* method;
            Value* value;
        };
    } Slot;

    Value* resolve(int symbol, Value** args, int numArgs);

    std::map<int,Type*> typemap;
private:
    std::map<int,Slot*> slots;
    Frame* parent;
    Container* container;

    void init(Container* container);
};

#endif	/* _INTERPRETER_H */

