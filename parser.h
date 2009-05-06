#ifndef _PARSER_H
#define	_PARSER_H

#include "Container.h"

Program* parse(char* tokens);
Method* parseMethod(char* &cur);

#endif	/* _PARSER_H */

