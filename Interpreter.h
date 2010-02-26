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

class Container;
class Program;
class Method;

class Interpreter;
class Frame;

#ifndef _INTERPRETER_H
#define	_INTERPRETER_H

#include "lexer.h"
#include "Value.h"
#include "Expression.h"
#include "Exceptions.h"
#include "Variable.h"
#include <vector>
#include <map>

#define CONV_INTERNAL       0
#define CONV_C_STDCALL      1
#define CONV_FPC_STDCALL    2

typedef enum { METHOD, VARIABLE, ARGUMENT, CONSTANT, TYPE } local_type;

typedef struct {
    local_type localtype;
    union { Method *meth; Variable *var; Value *val; Type *type; };
    int index;
} local;

class Container {
public:
    friend class Frame;
    const int name;
    Block block;

    virtual bool addMethod(Method* meth);
    virtual bool addVariable(Variable* var);
    virtual bool addConst(int name, Value* c);
    virtual bool addType(int name, Type* type);

    Method* getMethod(int name);
    Variable* getVariable(int name);
    Value* getConst(int name);
    Type* getType(int name);

    int getNameSlot(int name);
    virtual Type* getSlotType(int slot);

    //E.G. a generic container
    Container(int name, Container *super_scope);
    virtual ~Container();
protected:

    local* resolve(int name);

    Container *super_scope;
    int slot_start; //length of super_scope at time of creation
    int local_start;
    std::map<int,local*> scope_map;
    std::vector<Variable*> locals;

};

class Program : public Container {
public:
    friend class Frame;
    Program(int name, Container *const_scope);
    ~Program();
};

class Method : public Container {
public:
    friend class Frame;
    bool setResultType(Type *type);
    bool addArgument(Variable *arg);
    Variable* getArg(int idx);
    unsigned int numArgs();
    Type* getResultType();
    int getResultSlot();
    virtual Type* getSlotType(int slot);

    void* address;
    int mtype;
    Meth* val_type;

    Method(int name, Container *super_scope);
    ~Method();

protected:
    Type* type; //DO NOT FREE TYPES (they don't leak, they are managed by the Type class);
    int res_idx;
    std::vector<Variable*> arguments;
};

class Interpreter : public Container {
    friend class Frame;
public:
    Interpreter(PreCompiler_Callback precomp, ErrorHandeler_Callback err);
    ~Interpreter();

    bool run();
    bool compile();
    void setScript(char *ppg);
    virtual void addMethod(void* addr,  int conv, char* def);
    virtual void addType(char* def);
private:
    PreCompiler_Callback precomp;
    ErrorHandeler_Callback err;
    InterpEx* exception;
    Program* prog;
    char* ppg;
    std::map<std::string,int> names;

    void handle(InterpEx *ex);
    void handle(InterpEx *ex, char* ppg);
};

class Frame {
    friend class MethodValue;
    friend class PointerValue;
public:
    Frame();
    Frame(Program *container);
    Frame(Frame* frame, Method* container, Value **args);
    ~Frame();

    Value* resolve(int symbol) throw(int,InterpEx*);

private:
    int numslots;
    int localstart;
    int localcount;
    Value** slots;
    Frame* parent;
    Container* container;

    void init(Container* container)  throw(int,InterpEx*);
};

extern "C" {
    void interp_mem(MALLOC malloc, REALLOC realloc, FREE free) __attribute__((cdecl));
    void* interp_init(PreCompiler_Callback precomp, ErrorHandeler_Callback err) __attribute__((cdecl));
    void interp_meth(void* interp, void* addr, char* def) __attribute__((cdecl));
    void interp_type(void* interp, char* def) __attribute__((cdecl));
    void interp_set(void* interp, char *ppg) __attribute__((cdecl));
    bool interp_run(void* interp) __attribute__((cdecl));
    bool interp_comp(void* interp) __attribute__((cdecl));
    void interp_free(void* interp) __attribute__((cdecl));
}

#endif	/* _INTERPRETER_H */

