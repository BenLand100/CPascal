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

