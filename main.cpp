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

