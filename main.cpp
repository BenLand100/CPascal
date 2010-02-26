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

#ifdef DEBUG

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include "parser.h"
#include "lexer.h"
#include <map>
#include <list>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

/**
 * This file need not be linked with the interpreter, only for testing
 */

using namespace std;

bool precomp(char* name, char* arg) __attribute__((stdcall));
bool precomp(char* name, char* arg) {
    //cout << "Precompiler Command: " << name << '\n';
    return true;
}

int main(int argc, char** argv) {
    ifstream in;
    if (argc != 2) {
        cout << "Usage: cpascal [sourcefile]\n";
        return 1;
    }
    in.open(argv[1], ifstream::ate | ifstream::binary);
    if (!in) {
        cout << "File not found!\n";
        return 1;
    }
    int size = in.tellg();
    char* ppg = new char[size+1];
    in.seekg(0,ios::beg);
    in.read(ppg,size);
    in.close();
    ppg[size] = '\0';

    interp_mem((MALLOC)&malloc,(REALLOC)&realloc,(FREE)&free);
    void* interp = interp_init(&precomp,0);
    interp_set(interp,ppg);
    interp_comp(interp);
    interp_run(interp);
    interp_free(interp);
    
    delete [] ppg;
    return 0;
}

#endif
