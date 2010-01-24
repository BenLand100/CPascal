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

#include "Interpreter.h"
#include "lexer.h"
#include "parser.h"
#include "Exceptions.h"

#include <cstring>
#include <sstream>
#include <iostream>
#include <ctime>
#include <stdlib.h>
#include <cstdio>
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

void writenofeed(char* str) __attribute__((stdcall));
void writenofeed(char* str) {
    printf("%s",str);
}

void writeln(char* str) __attribute__((stdcall));
void writeln(char* str) {
    printf("%s\n",str);
}

int strtoint(char* str) __attribute__((stdcall));
int strtoint(char* str) {
    return atoi(str);
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
    strm.precision(16);
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
    this->ppg = new char[strlen(ppg)+1];
    strcpy(this->ppg,ppg);
    addMethod((void*)&ms_time,CONV_C_STDCALL,(char*)"function time: integer;");
    addMethod((void*)&ms_wait,CONV_C_STDCALL,(char*)"procedure wait(ms: integer);");
    addMethod((void*)&writenofeed,CONV_C_STDCALL,(char*)"procedure write(str: string);");
    addMethod((void*)&writeln,CONV_C_STDCALL,(char*)"procedure writeln(str: string);");
    addMethod((void*)&strtoint,CONV_C_STDCALL,(char*)"function strtoint(str: string): integer;");
    addMethod((void*)&inttostr,CONV_C_STDCALL,(char*)"function inttostr(i: integer): string;");
    addMethod((void*)&chartostr,CONV_C_STDCALL,(char*)"function chartostr(c: char): string;");
    addMethod((void*)&booltostr,CONV_C_STDCALL,(char*)"function booltostr(b: boolean): string;");
    addMethod((void*)&realtostr,CONV_C_STDCALL,(char*)"function realtostr(r: real): string;");
}

Interpreter::~Interpreter() {
    delete [] ppg;
    delete prog;
    if (exception) {
        delete exception;
        exception = 0;
    }
}

void Interpreter::run() {
    if (exception) {
        delete exception;
        exception = 0;
    }
    debug("Symbols: " << names.size() << '\n');
    Frame* frame = new Frame(names.size(), prog);
    try {
        evalBlock(&prog->block, frame);
    } catch (InterpEx* ex) {
        ex->printStackTrace(ppg);
        exception = ex;
    }
    delete frame;
}

void Interpreter::addMethod(void* addr, int conv, char* def) {
    char* tokens = lex(def, names);
    char* cur = tokens;
    Method* meth = parseMethod(cur,prog->types);
    meth->address = addr;
    meth->mtype = conv;
    prog->methods.push_back(meth);
    freetoks(tokens);
}

Frame::Frame(int numslots, Container* container_impl) :  parent(0), container(container_impl) {
    this->numslots = numslots;
    slots = new Value*[numslots];
    memset(slots,0,numslots*sizeof(Value*));
    init(container_impl);
}

Frame::Frame(Frame* frame, Container* container_impl) :  parent(frame), container(container_impl) {
    numslots = frame->numslots;
    slots = new Value*[numslots];
    memset(slots,0,numslots*sizeof(Value*));
    memcpy(slots,frame->slots,numslots*sizeof(Value*));
    init(container_impl);
}

void Frame::init(Container* container) throw(int,InterpEx*) {
    debug("init_frame");
    int numMethods = container->methods.size();
    for (int i = 0; i < numMethods; i++) {
        Method* meth = container->methods[i];
        slots[meth->name] = new MethodValue(meth);
        debug("init_meth=" << meth->name);
    }
    int numVariables = container->variables.size();
    for (int i = 0; i < numVariables; i++) {
        Variable* var = container->variables[i];
        slots[var->name] = Value::fromType((Type*) var->type);
        debug("init_var=" << var->name);
    }
    std::map<int, Expression*>::iterator iter = container->constants.begin();
    std::map<int, Expression*>::iterator end = container->constants.end();
    std::stack<Value*> stack;
    Frame* me = this;
    while (iter != end) {
        Value* val = evalExpr(iter->second,me,stack);
        slots[iter->first] = val;
        debug("init_const=" << iter->first);
        iter++;
    }
    cleanStack(stack);
}

Frame::~Frame() {
    int numMethods = container->methods.size();
    for (int i = 0; i < numMethods; i++) {
        Value* s = slots[container->methods[i]->name];
        Value::decref(s);
    }
    int numVariables = container->variables.size();
    for (int i = 0; i < numVariables; i++) {
        Value* s = slots[container->variables[i]->name];
        Value::decref(s);
    }
    std::map<int, Expression*>::iterator iter = container->constants.begin();
    std::map<int, Expression*>::iterator end = container->constants.end();
    while (iter != end) {
        Value::decref(slots[iter->first]);
        iter++;
    }
    delete [] slots;
}

Value* Frame::resolve(int symbol) throw(int, InterpEx*) {
    debug("resolve_symbol=" << symbol);
    Value* res = slots[symbol];
    if (res) return Value::incref(res);
    throw E_UNRESOLVABLE;
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
