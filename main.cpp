#include <iostream>
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
    char* ppg = (char*)"program test; procedure testing(var i: integer); var z: integer; begin z; end; begin testing(5); end.\n";
    char* tokens = lex(ppg);
    Program* prog = parse(tokens);
    freetoks(tokens);
    cout << prog;
    Interpreter* interp = new Interpreter(prog);
    interp->run();
    delete interp;
    return (EXIT_SUCCESS);
}

