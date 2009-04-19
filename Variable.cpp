#include "Variable.h"

Variable::Variable(int name_impl, Type* type_impl) : name(name_impl), type(type_impl), byRef(false) { }

Variable::Variable(int name_impl, Type* type_impl, bool byRef_impl) : name(name_impl), type(type_impl), byRef(byRef_impl) { }
