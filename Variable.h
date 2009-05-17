class Variable;

#ifndef _VARIABLE_H
#define	_VARIABLE_H

#include "Type.h"

class Variable {
public:
    const int name;
    Type* type;
    const bool byRef;

    Variable(int impl_name, Type* impl_type);
    Variable(int impl_name, Type* impl_type, bool byRef);
};


#endif	/* _VARIABLE_H */

