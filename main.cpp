#include <iostream>
#include <string>
#include "parser.h"
#include "lexer.h"

using namespace std;

int main(int argc, char** argv) {
    char* ppg = "program test; procedure testing(var i: integer); var z: integer; begin z:= i + 5; end; begin testing(5); end.\n";
    char* tokens = lex(ppg);
    parse(tokens);
    freetoks(tokens);
    return (EXIT_SUCCESS);
}

