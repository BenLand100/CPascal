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

#include "Container.h"

Container::Container(int name_impl) : name(name_impl), block() { }
Container::~Container() {
    for (unsigned int i = 0; i < methods.size(); i++)
        delete methods[i];
    for (unsigned int i = 0; i < variables.size(); i++)
        delete variables[i];
    std::map<int,Expression*>::iterator iter = constants.begin();
    while (iter != constants.end()) {
        delete iter->second;
        iter++;
    }
    cleanBlock(&block);
}

Program::Program(int name) : Container(name) { }
Program::~Program() { }

Method::Method(int name) : Container(name), address(0), mtype(CONV_INTERNAL), type(0) { }
Method::~Method() {
    for (unsigned int i = 0; i < arguments.size(); i++)
        delete arguments[i];
}
