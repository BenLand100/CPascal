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

Interpreter::Interpreter(PreCompiler_Callback precomp, ErrorHandeler_Callback err) : Container(-1,0), exception(0), prog(0), ppg(0) {
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
    if (exception) delete exception;
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
        prog = parse(tokens,this);
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
    Frame* frame = new Frame(prog);
    try {
        evalBlock(&prog->block, frame);
    } catch (InterpEx* ex) {
        handle(ex);
        delete frame;
        return false;
    } catch (int exi) {
        InterpEx* ex = new InterpEx(exi);
        ex->addTrace(-1);
        handle(ex);
        delete frame;
        return false;
    }
    delete frame;
    return true;
}

void Interpreter::addType(char* def) {
    debug("Importing Type: " << def);
    char* tokens = lex(def, names, precomp);
    char* cur = tokens;
    try {
        parseTypes(cur,this);
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
        meth = parseMethod(cur,this);
        //std::cout << def << "\n => "<< meth->arguments.size() << " N:" << meth->name << " A:" << addr << '\n';
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
    Container::addMethod(meth);
    freetoks(tokens);
}

Frame::Frame() : parent(0), container(0) {
    localstart = 0;
    localcount = 0;
    numslots = 0;
    slots = new Value*[numslots];
}

Frame::Frame(Program *dat) : parent(0), container(dat) {
    localstart = dat->local_start;
    localcount = dat->locals.size();
    numslots = localstart + localcount;
    slots = new Value*[numslots];
    for (int i = 0; i < localcount; i++) {
        slots[i + localstart] = Value::fromType((Type*) container->locals[i]->type);
        debug("init_var=" << i << '@' << i+localstart);
    }
}

Frame::Frame(Frame* frame, Method* dat, Value **args) :  parent(frame), container(dat) {
    localstart = dat->slot_start;
    int numargs = dat->arguments.size();
    localcount = dat->locals.size() + numargs;
    numslots = localstart + localcount;
    slots = new Value*[numslots];
    if (frame) memcpy(slots,frame->slots,localstart*sizeof(Value*));
    int i;
    for (i = 0; i < numargs; i++) {
        Variable* var = dat->getArg(i);
        slots[i + localstart] = var->byRef ? Value::incref(args[i]) : args[i]->clone();
        debug("set_arg=" << i << '@' << i+localstart);
    }
    int offset = dat->local_start;
    for (; i < localcount; i++) {
        slots[i + localstart] = Value::fromType((Type*) container->locals[i-offset]->type);
        debug("init_var="  << i << '@' << i+localstart);
    }
}

Frame::~Frame() {
    for (int i = localstart; i < numslots; i++) {
        Value::decref(slots[i]);
    }
    delete [] slots;
}

Value* Frame::resolve(int slot) throw(int, InterpEx*) {
    debug("resolve_slot=" << slot-localstart << '@' << slot);
    return Value::incref(slots[slot]);
}


Container::Container(int name_impl, Container *super_scope) : name(name_impl), block() {
    if (super_scope)
        slot_start = super_scope->locals.size();
    else
        slot_start = 0;
    local_start = slot_start;
    this->super_scope = super_scope;
}

Container::~Container() {
    std::map<int,local*>::iterator iter = scope_map.begin();
    std::map<int,local*>::iterator end = scope_map.end();
    while (iter != end) {
        switch (iter->second->localtype) {
            case METHOD:
                delete iter->second->meth;
                break;
            case VARIABLE:
            case ARGUMENT:
                delete iter->second->var;
                break;
            case CONSTANT:
                Value::decref(iter->second->val);
                break;
            case TYPE:
                break;
        }
        delete iter->second;
        iter++;
    }
    cleanBlock(&block);
}

bool Container::addMethod(Method* meth) {
    if (scope_map.find(meth->name) != scope_map.end()) return false;
    local *l = new local;
    l->localtype = METHOD;
    l->meth = meth;
    l->index = -1;
    scope_map[meth->name] = l;
    return true;
}

bool Container::addVariable(Variable* var) {
    if (scope_map.find(var->name) != scope_map.end()) return false;
    local *l = new local;
    l->localtype = VARIABLE;
    l->var = var;
    l->index = locals.size() + local_start;
    scope_map[var->name] = l;
    locals.push_back(var);
    return true;
}

bool Container::addConst(int name, Value* val) {
    if (scope_map.find(name) != scope_map.end()) return false;
    local *l = new local;
    l->localtype = CONSTANT;
    l->val = val;
    l->index = -1;
    scope_map[name] = l;
    return true;
}

bool Container::addType(int name, Type* type) {
    if (scope_map.find(name) != scope_map.end()) return false;
    local *l = new local;
    l->localtype = TYPE;
    l->type = type;
    l->index = -1;
    scope_map[name] = l;
    return true;
}

local* Container::resolve(int name) {
    std::map<int,local*>::iterator find = scope_map.find(name);
    if (find == scope_map.end()) return super_scope ? super_scope->resolve(name) : 0;
    return find->second;
}

Method* Container::getMethod(int name) {
    local *l = resolve(name);
    if (!l ||  l->localtype != METHOD) return 0;
    return l->meth;
}

Variable* Container::getVariable(int name) {
    local *l = resolve(name);
    if (!l ||  !(l->localtype == VARIABLE || l->localtype == ARGUMENT)) return 0;
    return l->var;
}

Value* Container::getConst(int name) {
    local *l = resolve(name);
    if (!l ||  l->localtype != CONSTANT) return 0;
    return l->val;
}

Type* Container::getType(int name) {
    local *l = resolve(name);
    if (!l ||  l->localtype != TYPE) return 0;
    return l->type;
}

int Container::getNameSlot(int name) {
    local *l = resolve(name);
    if (!l) return -1;
    return l->index;
}

Type* Container::getSlotType(int slot) {
    if (slot >= local_start + (int)locals.size()) return 0;
    if (slot < local_start) return super_scope ? super_scope->getSlotType(slot) : 0;
    return locals[slot]->type;
}

Program::Program(int name, Container *static_scope) : Container(name,static_scope) { }
Program::~Program() { }

Method::Method(int name, Container *super_scope) : Container(name, super_scope), address(0), mtype(CONV_INTERNAL), type(0) {

}
Method::~Method() {
}

bool Method::setResultType(Type *type) {
    this->type = type;
    if (!addVariable(new Variable(RES_RESULT,type))) return false;
    res_idx = getNameSlot(RES_RESULT);
    return true;
}
Type* Method::getResultType() {
    return type;
}

int Method::getResultSlot() {
    return res_idx;
}


Type* Method::getSlotType(int slot) {
    if (slot >= local_start + (int)locals.size()) return 0;
    if (slot < local_start) {
        if (slot < slot_start) return super_scope ? super_scope->getSlotType(slot) : 0;
        return arguments[slot-slot_start]->type;
    } else {
        return locals[slot-local_start]->type;
    }
}

bool Method::addArgument(Variable *arg) {
    if (scope_map.find(arg->name) != scope_map.end()) return false; //exists
    if (locals.size() != 0) return false; //args must be added first
    local_start++;
    local *l = new local;
    l->localtype = ARGUMENT;
    l->var = arg;
    l->index = arguments.size() + slot_start;
    scope_map[arg->name] = l;
    arguments.push_back(arg);
    return true;
}

Variable* Method::getArg(int idx) {
    return arguments[idx];
}

unsigned int Method::numArgs() {
    return arguments.size();
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

