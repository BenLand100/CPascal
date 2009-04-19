class Container;
class Program;
class Method;

#ifndef _CONTAINER_H
#define	_CONTAINER_H

#include "Expression.h"
#include "Variable.h"
#include <vector>
#include <map>

class Container {
public:
    const int name;
    std::vector<Method*> methods;
    std::vector<Variable*> variables;
    std::map<int,Expression*> constants;
    std::map<int,Type*> types; //DO NOT FREE TYPES (they don't leak, they are managed by the Type class);
    Block block;
protected:
    Container(int name);
    virtual ~Container();
};

class Program : public Container {
public:
    Program(int name);
    ~Program();
};

class Method : public Container {
public:
    std::vector<Variable*> arguments;
    Type* type; //DO NOT FREE TYPES (they don't leak, they are managed by the Type class);
    Method(int name);
    ~Method();
};

#endif	/* _CONTAINER_H */

