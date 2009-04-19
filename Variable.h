class Variable;

#ifndef _VARIABLE_H
#define	_VARIABLE_H

#include "Type.h"

class Variable {
public:
    const int name;
    const Type* type;
    const bool byRef;

    Variable(int impl, Type* impl);
    Variable(int impl, Type* impl, bool byRef);
};


#endif	/* _VARIABLE_H */

