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

#ifdef DEBUG
//int ms_time() __attribute__((stdcall));
//int ms_time() {
//   timeb t;
//    ftime(&t);
//    return (t.time * 1000 + t.millitm) & 0x7FFFFFFF;
//}

//void ms_wait(int ms) __attribute__((stdcall));
//void ms_wait(int ms) {
//    struct timespec a;
//    a.tv_sec = ms/1000;
//    a.tv_nsec = (ms%1000)*1000000L;
//    struct timespec b;
//    while (nanosleep(&a,&b)) {
//        if (!nanosleep(&b,&a)) break;
//    }
//}

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

#endif

Interpreter::Interpreter(PreCompiler_Callback precomp, ErrorHandeler_Callback err) : exception(0), prog(0), ppg(0) {
    this->precomp = precomp;
    this->err = err;
    debug("exception:" << exception << " ppg:" << (int)(ppg) << " prog:" << (int)(prog));
    #ifdef DEBUG
    //addMethod((void*)&ms_time,CONV_C_STDCALL,(char*)"function time: integer;");
    //addMethod((void*)&ms_wait,CONV_C_STDCALL,(char*)"procedure wait(ms: integer);");
    addMethod((void*)&writenofeed,CONV_C_STDCALL,(char*)"procedure write(str: string);");
    addMethod((void*)&writeln,CONV_C_STDCALL,(char*)"procedure writeln(str: string);");
    addMethod((void*)&strtoint,CONV_C_STDCALL,(char*)"function strtoint(str: string): integer;");
    addMethod((void*)&inttostr,CONV_C_STDCALL,(char*)"function inttostr(i: integer): string;");
    addMethod((void*)&chartostr,CONV_C_STDCALL,(char*)"function chartostr(c: char): string;");
    addMethod((void*)&booltostr,CONV_C_STDCALL,(char*)"function booltostr(b: boolean): string;");
    addMethod((void*)&realtostr,CONV_C_STDCALL,(char*)"function realtostr(r: real): string;");
    #endif
}

Interpreter::~Interpreter() {
    if (ppg) delete [] ppg;
    if (prog) delete prog;
    if (exception)   delete exception;
}

void Interpreter::handle(InterpEx *ex) {
    handle(ex,ppg);
}

void Interpreter::handle(InterpEx *ex, char* ppg) {
    if (exception) {
        delete exception;
        exception = 0;
    }
    if (err) {
        int line, pos;
        const char* cause;
        ex->getData(ppg,line,pos,cause);
        err(line,pos,cause,ex->getType() == E_RUNTIME);
        delete ex;
    } else {
        ex->printStackTrace(ppg);
        exception = ex;
    }
}

void Interpreter::setScript(char* ppg) {
    if (this->ppg) delete [] ppg;
    this->ppg = new char[strlen(ppg)+1];
    strcpy(this->ppg,ppg);
}

bool Interpreter::compile() {
    if (!ppg) return false;
    try {
        char* tokens = lex(ppg, names, precomp);
        if (prog) delete prog;
        prog = parse(tokens,types,methods);
        freetoks(tokens);
    } catch (InterpEx* ex) {
        handle(ex);
        return false;
    } catch (int exi) {
        InterpEx* ex = new InterpEx(exi);
        ex->addTrace(-1);
        handle(ex);
        return false;
    }
    return true;
}
    
bool Interpreter::run() {
    debug("Total Symbols: " << names.size());
    if (!prog) return false;
    Frame* env = new Frame(this);
    Frame* frame = new Frame(env, prog);
    try {
        evalBlock(&prog->block, frame);
    } catch (InterpEx* ex) {
        handle(ex);
        delete frame;
        delete env;
        return false;
    } catch (int exi) {
        InterpEx* ex = new InterpEx(exi);
        ex->addTrace(-1);
        handle(ex);
        delete frame;
        delete env;
        return false;
    }
    delete frame;
    delete env;
    return true;
}

void Interpreter::addType(char* def) {
    debug("Importing Type: " << def);
    char* tokens = lex(def, names, precomp);
    char* cur = tokens;
    try {
        parseTypes(cur,types);
    } catch (InterpEx* ex) {
        char temp[1024];
        sprintf(temp, "Failed to import: %s \n\tBecause: %s",def,ex->what());
        delete ex;
        ex = new ParserEx(temp);
        ex->addTrace(-1);
        handle(ex,def);
        return;
    } catch (int exi) {
        char temp[1024];
        sprintf(temp, "Failed to import: %s",def);
        InterpEx* ex = new ParserEx(temp);
        ex->addTrace(-1);
        handle(ex,def);
        return;
    }
    freetoks(tokens);
}

void Interpreter::addMethod(void* addr, int conv, char* def) {
    debug("Importing Method: " << def);
    char* tokens = lex(def, names, precomp);
    char* cur = tokens;
    Method* meth;
    try {
        meth = parseMethod(cur,types);
    } catch (InterpEx* ex) {
        char temp[1024];
        sprintf(temp, "Failed to import: %s \n\tBecause: %s",def,ex->what());
        delete ex;
        ex = new ParserEx(temp);
        ex->addTrace(-1);
        handle(ex,def);
        return;
    } catch (int exi) {
        char temp[1024];
        sprintf(temp, "Failed to import: %s",def);
        InterpEx* ex = new ParserEx(temp);
        ex->addTrace(-1);
        handle(ex,def);
        return;
    }
    meth->address = addr;
    meth->mtype = conv;
    methods.push_back(meth);
    freetoks(tokens);
}

Frame::Frame(Interpreter *env) : parent(0), container(0) {
    this->env = env;
    numslots = env->names.size();
    slots = new Value*[numslots];
    memset(slots,0,numslots*sizeof(Value*));
    std::vector<Method*>::iterator iter = env->methods.begin();
    std::vector<Method*>::iterator end = env->methods.end();
    while (iter != end) {
        if (slots[(*iter)->name]) Value::decref(slots[(*iter)->name]);
        slots[(*iter)->name] = new MethodValue(*iter);
        iter++;
    }
}

Frame::Frame(Frame* frame, Container* container_impl) :  parent(frame), container(container_impl), env(0) {
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
    if (env) {
        std::vector<Method*>::iterator iter = env->methods.begin();
        std::vector<Method*>::iterator end = env->methods.end();
        while (iter != end) {
            if (slots[(*iter)->name]) Value::decref(slots[(*iter)->name]);
            slots[(*iter)->name] = 0;
            iter++;
        }
    } else {
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
    }
    delete [] slots;
}

Value* Frame::resolve(int symbol) throw(int, InterpEx*) {
    debug("resolve_symbol=" << symbol);
    Value* res = slots[symbol];
    if (res) return Value::incref(res);
    throw E_UNRESOLVABLE;
}

void* interp_init(PreCompiler_Callback precomp, ErrorHandeler_Callback err)  {
    return (void*) new Interpreter(precomp, err);
}

void interp_meth(void* interp, void* addr, char* def) {
    ((Interpreter*)interp)->addMethod(addr, CONV_FPC_STDCALL, def);
}

void interp_type(void* interp, char* def) {
    ((Interpreter*)interp)->addType(def);
}

void interp_set(void* interp, char *ppg) {
    ((Interpreter*)interp)->setScript(ppg);
}

bool interp_comp(void* interp) {
    return ((Interpreter*)interp)->compile();
}

bool interp_run(void* interp) {
    return ((Interpreter*)interp)->run();
}

void interp_free(void* interp) {
    delete (Interpreter*)interp;
}

