class Interpreter;
class Frame;

#ifndef _INTERPRETER_H
#define	_INTERPRETER_H

#include "Container.h"
#include "Value.h"
#include "Expression.h"

class Interpreter {
public:
    Interpreter(Program* prog);
    ~Interpreter();

    void run();
private:
    Program* prog;
};

class Frame {
public:
    Frame(Frame* frame, Container* container);
    Frame(Container* container);
    ~Frame();
    Value* resolve(int symbol, Value** args, int numArgs);
};

#endif	/* _INTERPRETER_H */

