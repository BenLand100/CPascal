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

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include "parser.h"
#include "lexer.h"
#include <map>
#include <list>
#include <sstream>

/**
 * This file need not be linked with the interpreter, only for testing
 */

using namespace std;

double e() __attribute__((stdcall));
double e() {
    return 2.78000000000;
}

void add1p(int* i) __attribute__((stdcall));
void add1p(int* i) {
    (*i)++;
}

void add1v(int& i) __attribute__((stdcall));
void add1v(int& i) {
    i++;
}

int main(int argc, char** argv) {
    ifstream in;
    in.open("./fpc/test2.pas", ifstream::ate | ifstream::binary);
    if (!in) return 1;
    int size = in.tellg();
    char* ppg = new char[size+1];
    in.seekg(0,ios::beg);
    in.read(ppg,size);
    in.close();
    ppg[size] = '\0';
    Interpreter* interp = new Interpreter(ppg);
    interp->addMethod((void*)&e,CONV_C_STDCALL,(char*)"function e: real;");
    interp->addMethod((void*)&add1v,CONV_C_STDCALL,(char*)"procedure add1v(var i: integer);");
    interp->addMethod((void*)&add1p,CONV_C_STDCALL,(char*)"procedure add1p(i: ^integer);");
    delete [] ppg;
    interp->run();
    delete interp;
    return 0;
}

