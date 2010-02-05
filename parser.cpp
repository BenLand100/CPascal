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

#include "lexer.h"
#include "parser.h"
#include "Type.h"
#include "Expression.h"
#include "Variable.h"
#include "Container.h"
#include "Operator.h"
#include "Exceptions.h"
#include <iostream>
#include <map>
#include <list>
#include <stack>

#include <iostream>
//#define debug(x) std::cout << x << '\n'
#define debug(x)

Expression* parseExpr(char* &cur) throw(InterpEx*,int) ;
std::list<Variable*> parseVars(char* &cur, std::map<int,Type*> &reftypes) throw(InterpEx*,int) ;
std::list<Expression*> parseBlock(char* &cur) throw(InterpEx*,int) ;
void parseContainer(char* &cur, Container* container) throw(InterpEx*,int) ;

inline int offset(char* cur) {
    if (!cur) return -1;
    switch (*cur) {
        case PBOOLEAN:
        case POPERATOR:
        case PSPECIAL:
        case PCHAR:
            return *(int*)(cur + 2);
        case PNAME:
        case PINTEGER:
            return *(int*)(cur + sizeof(long) + 1);
        case PREAL:
            return *(int*)(cur + sizeof(float) + 1);
        case PSTRING: //sort of slow...
            char* res = cur + 1;
            while (*(res++)); //keeps going until next is one past null
            return *(int*)res;
    }
    return -1;
}

inline void fatal(std::string message, char* tok) throw(InterpEx*) __attribute__((noreturn));
inline void fatal(std::string message, char* tok) throw(InterpEx*) {
    InterpEx* ex = new ParserEx((char*)message.c_str());
    ex->addTrace(offset(tok));
    throw ex;
}

inline char* next(char* cur) {
    switch (*cur) {
        case PBOOLEAN:
        case POPERATOR:
        case PSPECIAL:
        case PCHAR:
            debug("next(" << (int)*cur << "," << (int)*(cur+1) << ")");
            return cur + 2 + 4;
        case PNAME:
        case PINTEGER:
            debug("next(" << (int)*cur << "," << *(int*)(cur+1) << ")");
            return cur + sizeof(long) + 1 + 4;
        case PREAL:
            debug("next(" << (int)*cur << "," << *(float*)(cur+1) << ")");
            return cur + sizeof(float) + 1 + 4;
        case PSTRING:
            debug("next(" << (int)*cur << "," << (cur+1) << ")");
            char* res = cur + 1;
            while (*(res++)); //keeps going until next is one past null
            return res + 4;
    }
    return 0;
}

inline bool reserved(int name) {
    return MAX_PROTECTED > name;
}

Type* parseType(char* &cur, std::map<int,Type*> &reftypes) throw(InterpEx*,int) {
    debug("parseType");
    if (cur[0] == POPERATOR && cur[1] == OP_DEREFGET) {
        cur = next(cur);
        return Type::getPointerType(parseType(cur, reftypes));
    }
    if (cur[0] != PNAME) fatal("Type definition expected",cur);
    int name = *(int*)(cur+1);
    switch (name) {
        case RES_FUNCTION:
        case RES_PROCEDURE: {
            Method* meth = new Method(-1);
            cur = next(cur);
            std::list<int> untyped;
            bool byref = false;
            bool hasArgs = false;
            bool moreArgs = true;
            while (*cur) {
                cur = next(cur);
                if (cur[0] == PSPECIAL) {
                    switch (cur[1]) {
                        case SPC_RPAREN:
                            moreArgs = false;
                            break;
                        case SPC_LPAREN:
                            hasArgs = true;
                            break;
                        case SPC_COLON: {
                            cur = next(cur);
                            Type* type = parseType(cur, reftypes);
                            if (!hasArgs || !moreArgs) {
                                meth->type = type;
                                cur = next(cur);
                                goto end_defparse;
                            } else {
                                while (!untyped.empty()) {
                                    meth->arguments.push_back(new Variable(untyped.front(), type, byref));
                                    untyped.pop_front();
                                }
                                byref = false;
                            }
                            break;
                        }
                        case SPC_COMMA:
                            break;
                        case SPC_SEMICOLON:
                            goto end_defparse;
                        default:
                            fatal("Unexpected symbol",cur);
                    }
                } else if (cur[0] == PNAME) {
                    int name = *(int*)(cur+1);
                    if (name == RES_VAR) {
                        byref = true;
                    } else {
                        untyped.push_back(name);
                    }
                } else {
                    fatal("Unexpected symbol", cur);
                }
            }
            end_defparse:
            meth->val_type = Type::getMethodType(meth);
            return Type::getPointerType(Type::getMethodType(meth));
        }
        case RES_ARRAY:
            cur = next(cur);
            if (cur[0] == PSPECIAL && cur[1] == SPC_LBRACE)  {
                char* tkfrom = next(cur);
                char* tkto = next(next(tkfrom));
                if (tkfrom[0] != tkto[0]) {
                    debug((int)tkfrom[0]);
                    debug((int)tkto[0]);
                    fatal("Malformed array specifier",tkfrom);
                }
                int from, to;
                switch (tkfrom[0]) {
                    case PCHAR:
                        from = tkfrom[1];
                        to = tkto[1];
                        break;
                    case PINTEGER:
                        from = *(int*)(tkfrom+1);
                        to = *(int*)(tkto+1);
                        break;
                    default:
                        fatal("Malformed array specifier",tkfrom);
                }
                cur = next(next(next(tkto))); //skip the ] of
                Type* type = parseType(cur,reftypes);
                return Type::getBoundArrayType(type, from, to);
            } else { //Already ate the of
                if (cur[0] != PNAME || cur[1] != RES_OF) fatal("Of expected",cur);
                cur = next(cur);
                return Type::getDynamicArrayType(parseType(cur,reftypes));
            }
        case RES_RECORD: {
            std::list<Variable*> vars = parseVars(cur,reftypes);
            cur = next(cur);
            std::map<int,Type*> fields;
            std::list<Variable*>::iterator iter = vars.begin();
            std::list<Variable*>::iterator end = vars.end();
            while (iter != end) {
                Variable* var = *iter;
                fields[var->name] = var->type;
                delete var;
                iter++;
            }
            return Type::getRecordType(fields);
        }
        default:
            Type* res = Type::getType(name);
            if (res->type == TYPE_REF) {
                if (reftypes.find(name) != reftypes.end()) {
                    return reftypes[name];
                } else {
                    fatal("Unknown type reference",cur);
                    return res;
                }
            } else {
                return res;
            }
    }
}

void parseTypes(char* &cur, std::map<int,Type*> &reftypes) throw(InterpEx*,int) {
    debug("parseTypes");
    if (cur[0] != PNAME || *(int*)(cur+1) != RES_TYPE) fatal("Type Expected",cur);
    char* tok = next(cur);
    do {
        if (tok[0] != PNAME) fatal("Unexpected symbol",tok);
        int name = *(int*)&tok[1];
        if (reserved(name)) return;
        cur = next(tok); //skip =
        if (cur[0] != POPERATOR || cur[1] != OP_EQU) fatal("Type assignment expected",cur);
        cur = next(cur);
        Type* type = parseType(cur, reftypes);
        cur = next(cur); //skip ;
        if (cur[0] != PSPECIAL || cur[1] != SPC_SEMICOLON) fatal("Semicolon expected",cur);
        if (reftypes.find(name) != reftypes.end()) fatal("Type already defined",cur);
        reftypes[name] = type;
        tok = next(cur);
    } while (*tok);
}


std::map<int,Expression*> parseConsts(char* &cur) throw(InterpEx*,int)  {
    debug("parseConsts");
    std::map<int,Expression*> consts;
    char* tok = next(cur);
    while (*tok) {
        int name = *(int*)(tok+1);
        if (reserved(name)) {
            return consts;
            debug("consts_return =" << name);
        }
        cur = next(tok); //skip =
        if (cur[0] != POPERATOR || cur[1] != OP_EQU) fatal("Const assignment expected",cur);
        Expression* expr = parseExpr(cur); //eats the ;
        consts[name] = expr;
        tok = next(cur);
    }
    fatal("Reached end of file",0);
    return consts;
}

std::list<Variable*> parseVars(char* &cur, std::map<int,Type*> &reftypes) throw(InterpEx*,int) {
    debug("parseVars");
    std::list<Variable*> vars;
    std::stack<int> untyped;
    while (*cur) {
        char* tok = next(cur);
        if (tok[0] == PSPECIAL) {
            switch (tok[1]) { 
                case SPC_COLON: {
                    tok = next(tok);
                    Type* type = parseType(tok, reftypes);
                    while (!untyped.empty()) {
                        vars.push_back(new Variable(untyped.top(), type));
                        untyped.pop();
                    }
                    cur = next(tok);
                    break;
                }
                case SPC_COMMA:
                    cur = tok;
                    break;
                default:
                    fatal("Unexpected symbol",tok);
            }
        } else if (tok[0] == PNAME) {
            int name = *(int*)(tok+1);
            if (reserved(name)) {
                debug("numvars=" << vars.size());
                return vars;
            }
            untyped.push(name);
            cur = tok;
        } else {
            fatal("Unexpected symbol",tok);
        }
    }
    return vars;
}

Method* parseMethod(char* &cur, std::map<int,Type*> &reftypes) throw(InterpEx*,int) {
    debug("parseMethod");
    cur = next(cur);
    Method* meth = new Method(*(int*)(cur+1));
    meth->types.insert(reftypes.begin(),reftypes.end());
    std::list<int> untyped;
    bool byref = false;
    bool hasArgs = false;
    bool moreArgs = true;
    while (*cur) {
        cur = next(cur);
        if (cur[0] == PSPECIAL) {
            switch (cur[1]) {
                case SPC_RPAREN:
                    moreArgs = false;
                    break;
                case SPC_LPAREN:
                    hasArgs = true;
                    break;
                case SPC_COLON: {
                    cur = next(cur);
                    Type* type = parseType(cur,reftypes);
                    if (!hasArgs || !moreArgs) {
                        meth->type = type;
                        cur = next(cur);
                        goto end_defparse;
                    } else {
                        while (!untyped.empty()) {
                            debug("argument:" << untyped.front());
                            meth->arguments.push_back(new Variable(untyped.front(), type, byref));
                            untyped.pop_front();
                        }
                        byref = false;
                        cur = next(cur);
                        if (cur[0] == PSPECIAL && cur[1] == SPC_RPAREN) moreArgs = false;
                    }
                    break;
                }
                case SPC_SEMICOLON:
                    goto end_defparse;
                case SPC_COMMA:
                    break;
                default: fatal("Unexpected symbol",cur);
            }
        } else if (cur[0] == PNAME) {
            int name = *(int*)(cur+1);
            if (name == RES_VAR) {
                byref = true;
            } else {
                untyped.push_back(name);
            }
        } else {
            fatal("Unexpected symbol",cur);
        }
    }
    fatal("Reached end of file",0);
    end_defparse:
    meth->val_type = Type::getMethodType(meth);
    meth->types.insert(reftypes.begin(),reftypes.end());
    parseContainer(cur,meth);
    return meth;
}

std::map<char,char> precedence_map() {
    std::map <char,char> prec;
    prec[OP_ARRAYGET] = 5; //Careful
    prec[OP_ARRAYSET] = 5; //Careful
    prec[OP_FIELDGET] = 5; //Careful
    prec[OP_FIELDSET] = 5; //Careful
    prec[OP_DEREFGET] = 5; //Careful
    prec[OP_DEREFSET] = 5; //Careful
    
    prec[OP_ADDR] =     4; //Careful
    prec[OP_NOT] =      4;
    prec[OP_NEG] =      4; //Careful, this has to be parsed from a urinary -
    
    prec[OP_MUL] =      3;
    prec[OP_FDIV] =     3;
    prec[OP_IDIV] =     3;
    prec[OP_MOD] =      3;
    prec[OP_AND] =      3;
    
    prec[OP_OR] =       2;
    prec[OP_ADD] =      2;
    prec[OP_SUB] =      2;
    
    prec[OP_EQU] =      1;
    prec[OP_NEQ] =      1;
    prec[OP_LESS] =     1;
    prec[OP_LESSEQ] =   1;
    prec[OP_GREAT] =    1;
    prec[OP_GREATEQ] =  1;
    
    prec[OP_ASGN] =     0;
    prec[OP_ARRAYSET] = 0; //Careful
    prec[OP_FIELDSET] = 0; //Careful
    prec[OP_DEREFSET] = 0; //Careful
    
    return prec;
}

typedef struct {
    Operator* op;
    char prec;
} opprec;

inline opprec mk_opprec(Operator* op, char prec) {
    opprec res;
    res.op = op;
    res.prec = prec;
    return res;
}

Expression* parseExpr(char* &cur) throw(InterpEx*,int) {
    debug("parseExpr");
    static std::map<char,char> precedence = precedence_map();
    std::list<Element*> expr;
    std::stack<std::stack<opprec>*> parseStack;
    std::stack<opprec>* oper = new std::stack<opprec>();
    char lastType = -1, nextType = -1;
    bool prefix = false;
    if (!*cur || !*next(cur)) fatal("Expression expected",cur);
    int off = offset(next(cur));
    while (*cur) {
        char* tok = next(cur);
        lastType = nextType;
        nextType = tok[0];
        switch (nextType) {
            case PSPECIAL:
                switch (tok[1]) {
                    case SPC_SEMICOLON:
                    case SPC_RBRACE:
                    case SPC_COMMA:
                        cur = tok;
                        goto end_exprparse;
                    case SPC_RPAREN:
                        if (parseStack.empty()) {
                            goto end_exprparse;
                        }
                        while (!oper->empty()) {
                            expr.push_back(oper->top().op);
                            oper->pop();
                        }
                        delete oper;
                        oper = parseStack.top();
                        parseStack.pop();
                        nextType = -1;
                        goto next_exprparse;
                    case SPC_LPAREN:
                        parseStack.push(oper);
                        oper = new std::stack<opprec>();
                        goto next_exprparse;
                    case SPC_LBRACE: {
                        std::list<Expression*> indexes;
                        do {
                            indexes.push_back(parseExpr(tok));
                        } while (tok[0] != PSPECIAL && tok[1] != SPC_RBRACE);
                        char* temp = next(tok);
                        if (temp[0] == POPERATOR && temp[1] == OP_ASGN) {
                            tok = temp;
                            char prec = precedence[OP_ARRAYSET];
                            while (!oper->empty() && prec <= oper->top().prec) {
                                expr.push_back(oper->top().op);
                                oper->pop();
                            }
                            oper->push(mk_opprec(new ArraySet(indexes),prec));
                        } else {
                            char prec = precedence[OP_ARRAYGET];
                            while (!oper->empty() && prec <= oper->top().prec) {
                                expr.push_back(oper->top().op);
                                oper->pop();
                            }
                            oper->push(mk_opprec(new ArrayGet(indexes),prec));
                        }
                    } goto next_exprparse;
                    default:
                        fatal("Unexpected symbol",tok);
                } break;
            case POPERATOR: {
                switch (tok[1]) {
                    case OP_DEREFGET: {
                        char* temp = next(tok);
                        if (temp[0] == POPERATOR && temp[1] == OP_ASGN) {
                            tok = temp;
                            char prec = precedence[OP_DEREFSET];
                            while (!oper->empty() && prec <= oper->top().prec) {
                                expr.push_back(oper->top().op);
                                oper->pop();
                            }
                            oper->push(mk_opprec(Operator::get(OP_DEREFSET),prec));
                        } else {
                            char prec = precedence[OP_DEREFGET];
                            while (!oper->empty() && prec <= oper->top().prec) {
                                expr.push_back(oper->top().op);
                                oper->pop();
                            }
                            oper->push(mk_opprec(Operator::get(OP_DEREFGET),prec));
                        }
                    } goto next_exprparse;
                    case OP_FIELDGET: {
                        tok = next(tok);
                        int name = *(int*)(tok+1);
                        char* temp = next(tok);
                        if (temp[0] == POPERATOR && temp[1] == OP_ASGN) {
                            tok = temp;
                            char prec = precedence[OP_FIELDSET];
                            while (!oper->empty() && prec <= oper->top().prec) {
                                expr.push_back(oper->top().op);
                                oper->pop();
                            }
                            oper->push(mk_opprec(new FieldSet(name),prec));
                        } else {
                            char prec = precedence[OP_FIELDGET];
                            while (!oper->empty() && prec <= oper->top().prec) {
                                expr.push_back(oper->top().op);
                                oper->pop();
                            }
                            oper->push(mk_opprec(new FieldGet(name),prec));
                        }
                    } goto next_exprparse;
                    case OP_ASGN: {
                        Symbol* symbol = (Symbol*)expr.back();
                        expr.pop_back();
                        int name = symbol->name;
                        delete symbol;
                        char prec = precedence[OP_ASGN];
                        while (!oper->empty() && prec <= oper->top().prec) {
                            expr.push_back(oper->top().op);
                            oper->pop();
                        }
                        oper->push(mk_opprec(new Asgn(name),prec));
                    } goto next_exprparse;
                }
                if (lastType == POPERATOR && tok[0] == POPERATOR && tok[1] == OP_SUB) //special operator renaming
                    tok[1] = OP_NEG;
                char prec = precedence[tok[1]];
                switch (tok[1]) {
                    case OP_NEG:
                    case OP_NOT:
                    case OP_ADDR:
                        prefix = true;
                        oper->push(mk_opprec(Operator::get(tok[1]),prec));
                        goto next_exprparse;
                }
                while (!oper->empty() && prec <= oper->top().prec) {
                    expr.push_back(oper->top().op);
                    oper->pop();
                }
                oper->push(mk_opprec(Operator::get(tok[1]),prec));
            } break;
            case PNAME: {//Could be a variable or method
                //***FIXME*** This will eventually remap names to indexes in a scope...
                int name = *(int*)(tok+1);
                switch (name) { //handle faux-functions
                    case RES_SIZE: {
                        tok = next(tok);
                        Expression* array = parseExpr(tok); //shouldn't eat the )
                        tok = next(tok);
                        expr.push_back(new Size(array));
                    } goto next_exprparse;
                    case RES_RESIZE:{
                        tok = next(tok);
                        Expression* array = parseExpr(tok); //eats the ,
                        Expression* dim = parseExpr(tok); //shouldn't eat the )
                        tok = next(tok);
                        expr.push_back(new Resize(array,dim));
                    } goto next_exprparse;
                    case RES_ARRAY:{
                        tok = next(tok);
                        std::list<Expression*> elems;
                        char* temp = next(tok);
                        while (temp[0] != PSPECIAL || temp[0] != SPC_RPAREN) {
                            elems.push_back(parseExpr(tok)); //eats the ,
                            temp = next(tok);
                        }
                        tok = temp;
                        expr.push_back(new ArrayDef(elems));
                    } goto next_exprparse;
                    case RES_EXIT:{
                        expr.push_back(new Throw(E_EXIT));
                    } goto next_exprparse;
                    case RES_BREAK:{
                        expr.push_back(new Throw(E_BREAK));
                    } goto next_exprparse;
                }
                if (reserved(name)) goto end_exprparse; //All reserved words break expression parsing
                expr.push_back(new Symbol(name));
                if (*tok) {
                    char* temp = next(tok);
                    if (temp[0] == PSPECIAL && temp[1] == SPC_LPAREN) {
                        std::list<Expression*> args;
                        tok = temp;
                        while (*temp) {
                            temp = next(tok);
                            if (temp[0] == PSPECIAL && temp[1] == SPC_RPAREN) {
                                tok = temp;
                                break;
                            }
                            args.push_back(parseExpr(tok));
                        }
                        expr.push_back(new Invoke(args));
                    }
                }
            } break;
            case PINTEGER:
                expr.push_back(new IntegerValue(*(int*)(tok+1)));
                break;
            case PREAL:
                expr.push_back(new RealValue(*(float*)(tok+1)));
                break;
            case PSTRING:
                expr.push_back(new StringValue(tok+1));
                break;
            case PBOOLEAN:
                expr.push_back(new BooleanValue(tok[1] != BOOL_FALSE));
                break;
            case PCHAR:
                expr.push_back(new CharValue(tok[1]));
        }
        next_exprparse:
        debug("next_exprparse");
        if (!*tok) fatal("Reached end of file!",cur);
        cur = tok;
        if (!prefix && !oper->empty() && oper->top().prec == 4) {
            expr.push_back(oper->top().op);
            oper->pop();
        }
        if (prefix) prefix = false;
    }
    end_exprparse:
    debug("end_exprparse");
    while (!oper->empty()) {
        expr.push_back(oper->top().op);
        oper->pop();
    }
    delete oper;
    debug("expr_created");
    return new Expression(expr, off);
}

Until* parseUntil(char* &cur) throw(InterpEx*,int) {
   debug("parseUntil");
   char* tok = next(cur);
   if (tok[0] != PNAME || *(int*)(tok+1) != RES_REPEAT) fatal("Repeat expected",cur);
   int off = offset(cur);
   std::list<Expression*> block = parseBlock(cur); //eats the until
   if (cur[0] != PNAME || *(int*)(cur+1) != RES_UNTIL) fatal("Until expected",cur);
   Expression* cond = parseExpr(cur);
   return new Until(block,cond,off);
}

Case* parseCase(char* &cur) throw(InterpEx*,int) {
    debug("parseCase");
    int off = offset(cur);
    cur = next(cur); //skip case
    if (cur[0] != PNAME || *(int*)(cur+1) != RES_CASE) fatal("Case expected",cur);
    Expression* cond = parseExpr(cur);
    cur = next(cur); //skip of
    if (cur[0] != PNAME || *(int*)(cur+1) != RES_OF) fatal("Of expected",cur);
    Case* pcase = new Case(cond,off);
    while (*cur) {
        cur = next(cur);
        if (cur[0] == PNAME) {
            switch (*(int*)(cur+1)) {
                case RES_ELSE: {
                    std::list<Expression*> def = parseBlock(cur);
                    pcase->setDefault(def);
                    cur = next(cur); //skip end
                    if (cur[0] != PNAME || *(int*)(cur+1) != RES_END) fatal("End expected",cur);
                }
                case RES_END:
                    cur = next(cur); //skip ;
                    if (cur[0] != PSPECIAL || cur[1] != SPC_SEMICOLON) fatal("Semicolon expected",cur);
                    return pcase;
            }
        }
        std::stack<int> switches;
        do {
            if (cur[0] == PINTEGER) {
                switches.push(*(int*)(cur+1));
            } else if (cur[0] == PCHAR) {
                switches.push(cur[1]);
            } else {
                fatal("Constant value expected",cur);
            }
            cur = next(cur);
        } while (cur[0] != PSPECIAL || cur[1] != SPC_COLON); //breaks on colon
        std::list<Expression*> block = parseBlock(cur);
        while (!switches.empty()) {
            pcase->addBranch(switches.top(),block);
            switches.pop();
        }
    }
    return pcase;
}

For* parseFor(char* &cur) throw(InterpEx*,int)  {
    debug("parseFor");
    int off = offset(cur);
    cur = next(cur); //skip for
    if (cur[0] != PNAME || *(int*)(cur+1) != RES_FOR) fatal("For expected",cur);
    cur = next(cur); //get var
    if (cur[0] != PNAME) fatal("Symbol expected",cur);
    int var = *(int*)(cur+1);
    cur = next(cur); //skip :=
    if (cur[0] != POPERATOR || cur[1] != OP_ASGN) fatal("Assignment expected",cur);
    Expression* begin = parseExpr(cur);
    cur = next(cur); //grap the to/downto
    if (cur[0] != PNAME) fatal("to/downto expected",cur);
    bool inc = cur[1] == RES_TO;
    Expression* end = parseExpr(cur);
    cur = next(cur); //skip do
    if (cur[0] != PNAME || *(int*)(cur+1) != RES_DO) fatal("Do expected",cur);
    std::list<Expression*> block = parseBlock(cur);
    return new For(var,begin,inc,end,block,off);
}

While* parseWhile(char* &cur) throw(InterpEx*,int)  {
    debug("parseWhile");
    int off = offset(cur);
    cur = next(cur); //skip while
    if (cur[0] != PNAME || *(int*)(cur+1) != RES_WHILE) fatal("While expected",cur);
    Expression* cond = parseExpr(cur);
    cur = next(cur); //skip do
    if (cur[0] != PNAME || *(int*)(cur+1) != RES_DO) fatal("Do expected",cur);
    std::list<Expression*> block = parseBlock(cur);
    return new While(cond,block,off);
}

If* parseIf(char* &cur) throw(InterpEx*,int)  {
    debug("parseIf");
    int off = offset(cur);
    cur = next(cur); //skip if
    if (cur[0] != PNAME || *(int*)(cur+1) != RES_IF) fatal("If expected",cur);
    Expression* cond = parseExpr(cur);
    cur = next(cur); //skip then
    if (cur[0] != PNAME || *(int*)(cur+1) != RES_THEN) fatal("Then expected",cur);
    std::list<Expression*> block = parseBlock(cur);
    If* res = new If(cond,block,off);
    while (*cur) {
        char* tok = next(cur);
        if (tok[0] == PNAME && *(int*)(tok+1) == RES_ELSE) {
            char* temp = next(tok);
            if (temp[0] == PNAME && *(int*)(temp+1) == RES_IF) {
                cond = parseExpr(temp);
                temp = next(temp); //skip then
                if (temp[0] != PNAME || *(int*)(temp+1) != RES_THEN) fatal("Then expected",temp);
                block = parseBlock(temp);
                res->addBranch(cond, block);
                cur = temp;
            } else {
                block = parseBlock(tok);
                res->setDefault(block);
                cur = tok;
            }
        } else {
            break;
        }
    }
    return res;
}

Try* parseTry(char* &cur) throw(InterpEx*,int) {
    debug("parseTry");
    int off = offset(cur);
    std::list<Expression*> danger = parseBlock(cur);
    std::list<Expression*> saftey;
    std::list<Expression*> always;
    char* tok = next(cur);
    if (*tok != PNAME) fatal("Symbol expected", tok);
    switch (*(int*)(tok+1)) {
        case RES_EXCEPT:
            saftey = parseBlock(cur);
            tok = next(cur);
            if (tok[0] != PNAME || *(int*)(tok+1) != RES_FINALLY)
                break;
        case RES_FINALLY:
            always = parseBlock(cur);
            break;
        default:
            fatal("Unclosed try statement", tok);
    }
    return new Try(danger,saftey,always,off);
}

std::list<Expression*> parseBlock(char* &cur) throw(InterpEx*,int) {
    debug("parseBlock");
    std::list<Expression*> block;
    bool fullblock;
    char* tok = next(cur);
    if (tok[0] == PNAME) {
        switch (*(int*)(tok+1)) {
            case RES_BEGIN:
            case RES_REPEAT:
            case RES_TRY:
            case RES_EXCEPT:
            case RES_FINALLY:
                fullblock = true;
                cur = tok;
                break;
            default:
                fullblock = false;
        }
        debug("fullblock=" << fullblock);
        do {
            tok = next(cur);
            if (!*tok) fatal("Unclosed block", cur);
            debug("block_switch=" << *(int*)(tok+1));
            switch (*(int*)(tok+1)) { //Expressions alway begin with a PName?
                case RES_END:
                    cur = next(tok);
                    if ((cur[0] == PSPECIAL && cur[1] != SPC_SEMICOLON) || (cur[0] == POPERATOR && cur[1] != OP_FIELDGET)) //Shisty way for not ; or .
                        fatal("Semicolon/Period expected",cur);
                    goto end_blockparse;
                case RES_ELSE:
                case RES_UNTIL:
                    cur = tok;
                    goto end_blockparse;
                case RES_EXCEPT:
                case RES_FINALLY:
                    goto end_blockparse;
                case RES_IF:
                    block.push_back(parseIf(cur));
                    break;
                case RES_WHILE:
                    block.push_back(parseWhile(cur));
                    break;
                case RES_CASE:
                    block.push_back(parseCase(cur));
                    break;
                case RES_REPEAT:
                    block.push_back(parseUntil(cur));
                    break;
                case RES_FOR:
                    block.push_back(parseFor(cur));
                    break;
                case RES_TRY:
                    block.push_back(parseTry(cur));
                    break;
                default: //Name but not reserved
                    block.push_back(parseExpr(cur));
                    break;
            }
        } while (fullblock);
    } else {
        fatal("Code block expected",tok);
    }
    end_blockparse:
    return block;
}

void parseContainer(char* &cur, Container* container) throw(InterpEx*,int) {
    debug("parseContainer");
    char* tok;
    do {
        if (!*cur) return; // empty body or eof
        tok = next(cur);
        if (!*tok) return; // empty body or eof
        if (*tok != PNAME) fatal("Symbol expected but not found",tok);
        switch (*(int*)(tok+1)) {
            case RES_CONST: { //multi
                std::map<int,Expression*> consts = parseConsts(tok);
                container->constants.insert(consts.begin(),consts.end());
            } break;
            case RES_TYPE: { //multi
                parseTypes(tok,container->types);
            } break;
            case RES_VAR: { //multi
                std::list<Variable*> vars = parseVars(tok,container->types);
                container->variables.insert(container->variables.end(),vars.begin(),vars.end());
            } break;
            case RES_PROCEDURE: //multi
            case RES_FUNCTION: { //multi
                 Method* meth = parseMethod(tok,container->types);
                 container->methods.push_back(meth);
            } break;
            case RES_BEGIN: { //single
                std::list<Expression*> block = parseBlock(cur);
                fillBlock(&container->block,&block);
                return;
            }
            default:
                fatal("Unexpected symbol",tok);
        }
        cur = tok;
    } while (*cur);
}

//FIXME methods is unused currently. It will link at compile time eventually, this is for that
Program* parse(char* tokens, std::map<int,Type*> &types, std::vector<Method*> &methods) throw(InterpEx*) {
    debug("parse");
    Program* prog = 0;
    try {
        char* cur = tokens;
        if (cur[0] != PNAME || *(int*)(cur+1) != RES_PROGRAM)
            fatal("Identifier not found: program",cur);
        cur = next(cur); //name
        if (*cur != PNAME) fatal("Program name expected",cur);
        prog = new Program(*(int*)(cur+1));
        cur = next(cur); //skip ;
        if (cur[0] != PSPECIAL || cur[1] != SPC_SEMICOLON) fatal("Semicolon expected",cur);
        std::map<int,Type*>::iterator iter = types.begin();
        std::map<int,Type*>::iterator end = types.end();
        while (iter != end) {
            prog->types[iter->first] = iter->second;
            iter++;
        }
        parseContainer(cur,prog);
    } catch (int exi) {
        if (prog) delete prog;
        InterpEx* ex = new InterpEx(exi);
        ex->addTrace(-1);
        throw ex;
    } catch (InterpEx* ex) {
        if (prog) delete prog;
        throw ex;
    }
    return prog;
}
