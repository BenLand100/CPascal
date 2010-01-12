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

#ifndef _CONTAINER_H
#define	_CONTAINER_H

#define CONV_INTERNAL       0
#define CONV_C_STDCALL      1
#define CONV_FPC_STDCALL    2

#include "Variable.h"
#include <vector>
#include "Expression.h"
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
    void* address;
    int mtype;
    Type* type; //DO NOT FREE TYPES (they don't leak, they are managed by the Type class);
    Meth* val_type;
    Method(int name);
    ~Method();
};

#endif	/* _CONTAINER_H */

