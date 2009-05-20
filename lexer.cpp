#include "lexer.h"
#include <cstring>
#include <cstdlib>
#include <map>
#include <string>
#include <iostream>

#include <iostream>
#define debug(x) 

int reserved(std::map<std::string,int> &names) {
    names["program"] = RES_PROGRAM;
    names["begin"] = RES_BEGIN;
    names["end"] = RES_END;
    names["var"] = RES_VAR;
    names["procedure"] = RES_PROCEDURE;
    names["function"] = RES_FUNCTION;
    names["if"] = RES_IF;
    names["then"] = RES_THEN;
    names["else"] = RES_ELSE;
    names["for"] = RES_FOR;
    names["to"] = RES_TO;
    names["downto"] = RES_DOWNTO;
    names["do"] = RES_DO;
    names["while"] = RES_WHILE;
    names["repeat"] = RES_REPEAT;
    names["until"] = RES_UNTIL;
    names["case"] = RES_CASE;
    names["of"] = RES_OF;
    names["const"] = RES_CONST;
    names["type"] = RES_TYPE;
    names["array"] = RES_ARRAY;
    names["record"] = RES_RECORD;
    names["try"] = RES_TRY;
    names["except"] = RES_EXCEPT;
    names["finally"] = RES_FINALLY;
    names["char"] = RES_CHAR;
    names["string"] = RES_STRING;
    names["integer"] = RES_INTEGER;
    names["real"] = RES_REAL;
    names["boolean"] = RES_BOOLEAN;
    names["size"] = RES_SIZE;
    names["resize"] = RES_RESIZE;
    names["nil"] = RES_NIL;
    names["result"] = RES_RESULT;
    return MAX_RES; //return next
}

inline void whitespace(char* &ppg) {
    for (;;) {
        switch (*ppg) {
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                ++ppg;
                break;
            default:
                return;
        }
    }
}

inline int namechar(char c) {
    return (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_';
}

inline int numchar(char c) {
    return (c >= '0' && c <= '9');
}

inline void tolower(char* ppg) {
    while (*ppg) {
        char c = *ppg;
        if (c >= 'A' && c <= 'Z')
            *ppg = c - 'A' + 'a';
        ++ppg;
    }
}

char* lex(char* ppg, std::map<std::string,int> &names) {
    int nextord = reserved(names);
    char* res = new char[strlen(ppg)*6]; //this should suffice, haha.
    char* toks = res;
    char* last = toks;
    char* start = ppg;
    char* pos = ppg;
    whitespace(ppg);
    tolower(ppg);
    debug(std::cout << "lexing...\n";)
    char c;
    while (c = *ppg) {
        char* last = toks;
        debug(std::cout << c << ' ';)
        pos = ppg;
        switch (c) {
            //***BEGIN NAME/FAUX_OPERATOR***
            case 'a':
                if (ppg[1] == 'n' && ppg[2] == 'd' && !namechar(ppg[3])) {
                    *(toks++) = POPERATOR;
                    *(toks++) = OP_AND;
                    ppg += 2;
                    break;
                }
            case 'b':
            case 'c':
                goto name;
            case 'd':
                if (ppg[1] == 'i' && ppg[2] == 'v' && !namechar(ppg[3])) {
                    *(toks++) = POPERATOR;
                    *(toks++) = OP_IDIV;
                    ppg += 2;
                    break;
                }
            case 'e':
                goto name;
            case 'f':
                if (ppg[1] == 'a' && ppg[2] == 'l' && ppg[3] == 's' && ppg[4] == 'e' && !namechar(ppg[5])) {
                    *(toks++) = (char)PBOOLEAN;
                    *(toks++) = (char)BOOL_FALSE;
                    ppg += 4;
                    break;
                }
            case 'g':
            case 'h':
            case 'i':
            case 'j':
            case 'k':
            case 'l':
            case 'm':
                if (ppg[1] == 'o' && ppg[2] == 'd' && !namechar(ppg[3])) {
                    *(toks++) = POPERATOR;
                    *(toks++) = OP_MOD;
                    ppg += 2;
                    break;
                }
                goto name;
            case 'n':
                if (ppg[1] == 'o' && ppg[2] == 't' && !namechar(ppg[3])) {
                    *(toks++) = POPERATOR;
                    *(toks++) = OP_NOT;
                    toks += 4;
                    ppg += 2;
                    break;
                }
                goto name;
            case 'o':
                if (ppg[1] == 'r' && !namechar(ppg[2])) {
                    *(toks++) = POPERATOR;
                    *(toks++) = OP_OR;
                    toks += 3;
                    ppg += 2;
                    break;
                }
            case 'p':
            case 'q':
            case 'r':
            case 's':
                goto name;
            case 't':
                if (ppg[1] == 'r' && ppg[2] == 'u' && ppg[3] == 'e' && !namechar(ppg[4])) {
                    *(toks++) = (char)PBOOLEAN;
                    *(toks++) = (char)BOOL_TRUE;
                    ppg += 3;
                    break;
                }
            case 'u':
            case 'v':
            case 'w':
            case 'x':
            case 'y':
            case 'z': {
                name:
                *(toks++) = PNAME;
                char* name = toks;
                *(toks++) = c;
                for (c = *(++ppg); namechar(c); c = *(++ppg))
                    *(toks++) = c;
                ppg--;
                *(toks++) = 0;
                std::string key(name);
                if (names.find(key) != names.end()) {
                    *(int*)name = names[key];
                } else {
                    names[key] = *(int*)name = nextord++;
                }
                toks = name + sizeof(int);
            } break;
            //***BEGIN STRING/CHAR***
            case '\'': {
                char* type = toks;
                *(toks++) = PSTRING;
                for (c = *(++ppg); c != '\''; c = *(++ppg)) {
                    if (c == '\\') {
                        *(toks++) = c;
                        c = *(++ppg);
                    }
                    *(toks++) = c;
                }
                if (toks - type == 2) {
                    *type = PCHAR;
                } else {
                    *(toks++) = 0;
                }
            } break;
            //***BEGIN INTEGER/REAL***
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9': {
                char* type = toks;
                *(toks++) = PINTEGER;
                *(toks++) = c;
                for (c = *(++ppg); numchar(c); c = *(++ppg)) {
                    *(toks++) = c;
                }
                if (c == '.' && *(ppg+1) != '.') {
                    *type = PREAL;
                    *(toks++) = '.';
                    for (c = *(++ppg); numchar(c); c = *(++ppg)) {
                        *(toks++) = c;
                    }
                }
                ppg--;
                *(toks++) = 0;
                toks = type + 1;
                if (*type == PINTEGER) {
                    long i = atol(toks);
                    *(long*)(toks) = i;
                    toks += sizeof(long);
                } else {
                    float f = atof(toks);
                    *(float*)(toks) = f;
                    toks += sizeof(float);
                }
            } break;
            //***BEGIN COMMENT***
            case '{':
                while (*(++ppg) != '}'); //; is important
                break;
            //***BEGIN SPECIAL***
            case ',':
                *(toks++) = PSPECIAL;
                *(toks++) = SPC_COMMA;
                break;
            case ';':
                *(toks++) = PSPECIAL;
                *(toks++) = SPC_SEMICOLON;
                break;
            case ':':
                if (ppg[1] == '=') {
                     ++ppg;
                    *(toks++) = POPERATOR;
                    *(toks++) = OP_ASGN;
                } else {
                    *(toks++) = PSPECIAL;
                    *(toks++) = SPC_COLON;
                }
                break;
            case '(':
                *(toks++) = PSPECIAL;
                *(toks++) = SPC_LPAREN;
                break;
            case ')':
                *(toks++) = PSPECIAL;
                *(toks++) = SPC_RPAREN;
                break;
            case '[':
                *(toks++) = PSPECIAL;
                *(toks++) = SPC_LBRACE;
                break;
            case ']':
                *(toks++) = PSPECIAL;
                *(toks++) = SPC_RBRACE;
                break;
                //***BEGIN OPERATOR***
            case '.':
                if (ppg[1] == '.') {
                     ++ppg;
                    *(toks++) = PSPECIAL;
                    *(toks++) = SPC_THRU;
                } else {
                    *(toks++) = POPERATOR;
                    *(toks++) = OP_FIELDGET;
                }
                break;
            case '^':
                *(toks++) = POPERATOR;
                *(toks++) = OP_DEREFGET;
                break;
            case '@':
                *(toks++) = POPERATOR;
                *(toks++) = OP_ADDR;
                break;
            case '+':
                *(toks++) = POPERATOR;
                *(toks++) = OP_ADD;
                break;
            case '-':
                *(toks++) = POPERATOR;
                *(toks++) = OP_SUB;
                break;
            case '/':
                if (ppg[1] == '/') {
                    while (*(++ppg) != '\n'); //; is important
                    break;
                }
                *(toks++) = POPERATOR;
                *(toks++) = OP_FDIV;
                break;
            case '*':
                *(toks++) = POPERATOR;
                *(toks++) = OP_MUL;
                break;
            case '=':
                *(toks++) = POPERATOR;
                *(toks++) = OP_EQU;
                break;
            case '<':
                if (ppg[1] == '=') {
                    ++ppg;
                    *(toks++) = POPERATOR;
                    *(toks++) = OP_LESSEQ;
                } else if (ppg[1] == '>') {
                    ++ppg;
                    *(toks++) = POPERATOR;
                    *(toks++) = OP_NEQ;
                } else {
                    *(toks++) = POPERATOR;
                    *(toks++) = OP_LESS;
                }
                break;
            case '>':
                if (ppg[1] == '=') {
                     ++ppg;
                    *(toks++) = POPERATOR;
                    *(toks++) = OP_GREATEQ;
                } else {
                    *(toks++) = POPERATOR;
                    *(toks++) = OP_GREAT;
                }
                break;
        }
        ++ppg;
        whitespace(ppg);
        if (toks != last) {
            *(int*)toks = (pos - start);
            toks += 4;
        }
        debug(
            std::cout << (int)*last << ' ' << *(int*)(toks - 4) << ' ';
            if (*last == PSTRING) {
                std::cout << "s=" << (last+1) << '\n';
            } else if (*last == PCHAR) {
                std::cout << "c=" << *(last+1) << '\n';
            } else {
                std::cout << '\n';
            }
        )
    }
    *(toks++) = 0;
    return res;
}

void freetoks(char* tokens) {
    delete [] tokens;
}
