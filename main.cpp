#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include "parser.h"
#include "lexer.h"
#include <map>
#include <list>
#include <sstream>

using namespace std;

ostream& operator<< (ostream& out, map<int,Type*>& m) {
    map<int,Type*>::iterator iter = m.begin();
    while (iter != m.end()) {
        cout << iter->first << ":" << iter->second << ",";
        iter++;
    }
    return out;
}

ostream& operator<< (ostream& out, map<int,Expression*>& m) {
    map<int,Expression*>::iterator iter = m.begin();
    while (iter != m.end()) {
        cout << iter->first << ":" << iter->second << ",";
        iter++;
    }
    return out;
}

ostream& operator<< (ostream& out, vector<Method*>& v) {
    for (int i = 0; i < v.size(); i++) {
        cout << v[i];
    }
    return out;
}

ostream& operator<< (ostream& out, vector<Variable*>& v) {
    for (int i = 0; i < v.size(); i++) {
        cout << v[i];
    }
    return out;
}

ostream& operator<< (ostream& out, Block& b) {
    for (int i = 0; i < b.length; i++) {
        cout << b.elems[i];
    }
    return out;
}

ostream& operator<< (ostream& out, Container* c) {
    out << c->name << "\n+" << c->methods << "\n+" << c->types << "\n+" << c->constants << "\n+" << c->variables << "\n+" << c->block << "\n";
    return out;
}

void writeln(char* str) __attribute__((stdcall));
void writeln(char* str) {
    cout << str << '\n';
}

char* inttostr(int i) __attribute__((stdcall));
char* inttostr(int i) {
    stringstream strm;
    strm << i;
    string str = strm.str();
    char* res = new char[str.length()+1];
    strcpy(res,str.c_str());
    return res;
}

char* chartostr(char i) __attribute__((stdcall));
char* chartostr(char i) {
    stringstream strm;
    strm << i;
    string str = strm.str();
    char* res = new char[str.length()+1];
    strcpy(res,str.c_str());
    return res;
}

char* booltostr(bool i) __attribute__((stdcall));
char* booltostr(bool i) {
    stringstream strm;
    strm << i;
    string str = strm.str();
    char* res = new char[str.length()+1];
    strcpy(res,str.c_str());
    return res;
}

char* realtostr(double i) __attribute__((stdcall));
char* realtostr(double i) {
    stringstream strm;
    strm << i;
    string str = strm.str();
    char* res = new char[str.length()+1];
    strcpy(res,str.c_str());
    return res;
}

double e() __attribute__((stdcall));
double e() {
    return 2.78000000000;
}


int main(int argc, char** argv) {
    ifstream in;
    in.open("./test.pas", ifstream::ate | ifstream::binary);
    if (!in) return 1;
    int size = in.tellg();
    char* ppg = new char[size+1];
    in.seekg(0,ios::beg);
    in.read(ppg,size);
    in.close();
    ppg[size] = '\0';
    Interpreter* interp = new Interpreter(ppg);
    interp->addMethod((void*)&writeln,CONV_C_STDCALL,(char*)"procedure writeln(str: string);");
    interp->addMethod((void*)&inttostr,CONV_C_STDCALL,(char*)"function inttostr(i: integer): string;");
    interp->addMethod((void*)&chartostr,CONV_C_STDCALL,(char*)"function chartostr(c: char): string;");
    interp->addMethod((void*)&booltostr,CONV_C_STDCALL,(char*)"function booltostr(b: boolean): string;");
    interp->addMethod((void*)&realtostr,CONV_C_STDCALL,(char*)"function realtostr(r: real): string;");
    interp->addMethod((void*)&e,CONV_C_STDCALL,(char*)"function e: real;");
    delete ppg;
    interp->run();
    delete interp;
    return 0;
}

