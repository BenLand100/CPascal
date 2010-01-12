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

class Operator;
class Symbol;
class Size;
class Resize;
class ArrayDef;
class ArraySet;
class ArrayGet;
class FieldGet;
class FieldSet;

#ifndef _OPERATOR_H
#define	_OPERATOR_H

#include "Expression.h"
#include "Interpreter.h"
#include "Value.h"
#include "Element.h"
#include <stack>
#include <list>

class Operator : public Element {
public:
    static Operator* get(int type);

    const int type;
    virtual void preform(std::stack<Value*> &stack, Frame* frame) throw(int,InterpEx*) = 0;
    //virtual void native(std::stack<Value*> &stack,  std::vector<std::string> &native) throw(int,InterpEx*) = 0;

protected:
    Operator(int op);
    virtual ~Operator();
};

class Asgn : public Operator {
public:
    Asgn(int name);
    ~Asgn();

    const int name;
    void preform(std::stack<Value*> &stack, Frame* frame) throw(int,InterpEx*);
    //void native(std::stack<Value*> &stack,  std::vector<std::string> &native) throw(int,InterpEx*);
};

class Invoke : public Operator {
public:
    Invoke(std::list<Expression*> args);
    ~Invoke();

    const int numArgs;
    Expression** args;

    void preform(std::stack<Value*> &stack, Frame* frame) throw(int,InterpEx*);
    //void native(std::stack<Value*> &stack,  std::vector<std::string> &native) throw(int,InterpEx*);
};

class Symbol : public Operator {
public:
    Symbol(int name);
    ~Symbol();

    const int name;

    void preform(std::stack<Value*> &stack, Frame* frame) throw(int,InterpEx*);
    //void native(std::stack<Value*> &stack,  std::vector<std::string> &native) throw(int,InterpEx*);
};

class Size : public Operator {
public:
    Size(Expression* array);
    ~Size();

    void preform(std::stack<Value*> &stack, Frame* frame) throw(int,InterpEx*);
    //void native(std::stack<Value*> &stack,  std::vector<std::string> &native) throw(int,InterpEx*);
private:
    Expression* array;
};

class ArrayDef : public Operator {
public:
    ArrayDef(std::list<Expression*> elems);
    ~ArrayDef();

    void preform(std::stack<Value*> &stack, Frame* frame) throw(int,InterpEx*);
    //void native(std::stack<Value*> &stack,  std::vector<std::string> &native) throw(int,InterpEx*);
private:
    Expression** elems;
    int numElems;
};

class Resize : public Operator {
public:
    Resize(Expression* array, Expression* dim);
    ~Resize();

    void preform(std::stack<Value*> &stack, Frame* frame) throw(int,InterpEx*);
    //void native(std::stack<Value*> &stack,  std::vector<std::string> &native) throw(int,InterpEx*);
private:
    Expression* array;
    Expression* dim;
};

class ArrayGet : public Operator {
public:
    ArrayGet(std::list<Expression*> indexes);
    ~ArrayGet();

    void preform(std::stack<Value*> &stack, Frame* frame) throw(int,InterpEx*);
    //void native(std::stack<Value*> &stack,  std::vector<std::string> &native) throw(int,InterpEx*);
private:
    Expression** indexes;
    int numIndexes;
};

class ArraySet : public Operator {
public:
    ArraySet(std::list<Expression*> indexes);
    ~ArraySet();

    void preform(std::stack<Value*> &stack, Frame* frame) throw(int,InterpEx*);
    //void native(std::stack<Value*> &stack,  std::vector<std::string> &native) throw(int,InterpEx*);
private:
    Expression** indexes;
    int numIndexes;
};

class FieldGet : public Operator {
public:
    FieldGet(int name);
    ~FieldGet();

    void preform(std::stack<Value*> &stack, Frame* frame) throw(int,InterpEx*);
    //void native(std::stack<Value*> &stack,  std::vector<std::string> &native) throw(int,InterpEx*);
private:
    int name;
};

class FieldSet : public Operator {
public:
    FieldSet(int name);
    ~FieldSet();

    void preform(std::stack<Value*> &stack, Frame* frame) throw(int,InterpEx*);
    //void native(std::stack<Value*> &stack,  std::vector<std::string> &native) throw(int,InterpEx*);
private:
    int name;
};


#endif	/* _OPERATOR_H */

