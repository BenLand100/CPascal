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

#ifndef _PARSER_H
#define	_PARSER_H

#include "Container.h"

Program* parse(char* tokens, std::map<int,Type*> &types, std::vector<Method*> &methods) throw(InterpEx*) ;
Method* parseMethod(char* &cur, std::map<int,Type*> &reftypes)  throw(InterpEx*,int);
void parseTypes(char* &cur, std::map<int,Type*> &reftypes) throw(InterpEx*,int);

#endif	/* _PARSER_H */

