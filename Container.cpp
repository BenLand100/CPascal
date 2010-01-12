#include "Container.h"

Container::Container(int name_impl) : name(name_impl), block() { }
Container::~Container() {
    for (int i = 0; i < methods.size(); i++)
        delete methods[i];
    for (int i = 0; i < variables.size(); i++)
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
    for (int i = 0; i < arguments.size(); i++)
        delete arguments[i];
}
