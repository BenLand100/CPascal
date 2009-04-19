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
    virtual void preform(std::stack<Value*> &stack, Frame* frame) = 0;

protected:
    Operator(int op);
    virtual ~Operator();
};

class Symbol : public Operator {
public:
    Symbol(int name);
    Symbol(int name, std::list<Expression*> args);
    ~Symbol();

    const int name;
    const int numArgs;
    Expression** args;

    void preform(std::stack<Value*> &stack, Frame* frame);
};

class Size : public Operator {
public:
    Size(Expression* array);
    ~Size();

    void preform(std::stack<Value*> &stack, Frame* frame);
private:
    Expression* array;
};

class ArrayDef : public Operator {
public:
    ArrayDef(std::list<Expression*> elems);
    ~ArrayDef();

    void preform(std::stack<Value*> &stack, Frame* frame);
private:
    Expression** elems;
    int numElems;
};

class Resize : public Operator {
public:
    Resize(Expression* array, Expression* dim);
    ~Resize();

    void preform(std::stack<Value*> &stack, Frame* frame);
private:
    Expression* array;
    Expression* dim;
};

class ArrayGet : public Operator {
public:
    ArrayGet(std::list<Expression*> indexes);
    ~ArrayGet();

    void preform(std::stack<Value*> &stack, Frame* frame);
private:
    Expression** indexes;
    int numIndexes;
};

class ArraySet : public Operator {
public:
    ArraySet(std::list<Expression*> indexes);
    ~ArraySet();

    void preform(std::stack<Value*> &stack, Frame* frame);
private:
    Expression** indexes;
    int numIndexes;
};

class FieldGet : public Operator {
public:
    FieldGet(int name);
    ~FieldGet();

    void preform(std::stack<Value*> &stack, Frame* frame);
private:
    int name;
};

class FieldSet : public Operator {
public:
    FieldSet(int name);
    ~FieldSet();

    void preform(std::stack<Value*> &stack, Frame* frame);
private:
    int name;
};


#endif	/* _OPERATOR_H */

