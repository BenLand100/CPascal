#include <iostream>
#include <fstream>
#include <string>
#include "parser.h"
#include "lexer.h"
#include <map>
#include <list>

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

int main(int argc, char** argv) {
    ifstream in;
    in.open("./test.pas", ifstream::ate | ifstream::binary);
    if (!in) return EXIT_FAILURE;
    int size = in.tellg();
    char* ppg = new char[size+1];
    in.seekg(0,ios::beg);
    in.read(ppg,size);
    in.close();
    ppg[size] = '\0';
    Interpreter* interp = new Interpreter(ppg);
    delete ppg;
    interp->run();
    delete interp;
    return (EXIT_SUCCESS);
}

