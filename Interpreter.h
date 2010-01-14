/**
 *  Copyright 2010 by Benjamin J. Land (a.k.a. BenLand100)
 *
 *  This file is part of CPascal.
 *
 *  CPascal is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  CPascal is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CPascal. If not, see <http://www.gnu.org/licenses/>.
 */

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

private:
    std::map<int,Value*> slots;
    Frame* parent;
    Container* container;

    void init(Container* container)  throw(int,InterpEx*);
};

extern "C" {
    void* interp_init(char* pchar) __attribute__((cdecl));
    void interp_meth(void* interp, void* addr, char* def) __attribute__((cdecl));
    void interp_run(void* interp) __attribute__((cdecl));
    void interp_free(void* interp) __attribute__((cdecl));
}

#endif	/* _INTERPRETER_H */

