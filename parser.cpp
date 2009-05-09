#include "lexer.h"
#include "parser.h"
#include "Type.h"
#include "Expression.h"
#include "Variable.h"
#include "Container.h"
#include "Operator.h"
#include <iostream>
#include <map>
#include <list>
#include <stack>

#include <iostream>
//#define debug(x) std::cout << x << '\n'
#define debug(x)

Expression* parseExpr(char* &cur);
std::list<Expression*> parseBlock(char* &cur);
void parseContainer(char* &cur, Container* container);

inline char* next(char* cur) {
    switch (*cur) {
        case PBOOLEAN:
        case POPERATOR:
        case PSPECIAL:
        case PCHAR:
            debug("next(" << (int)*cur << "," << (int)*(cur+1) << ")");
            return cur + 2;
        case PNAME:
        case PINTEGER:
            debug("next(" << (int)*cur << "," << *(int*)(cur+1) << ")");
            return cur + sizeof(long) + 1;
        case PREAL:
            debug("next(" << (int)*cur << "," << *(float*)(cur+1) << ")");
            return cur + sizeof(float) + 1;
        case PSTRING:
            debug("next(" << (int)*cur << "," << (cur+1) << ")");
            char* res = cur + 1;
            while (*(res++)); //keeps going until next is one past null
            return res;
    }
    return 0;
}

inline bool reserved(int name) {
    return MAX_PROTECTED > name;
}

Type* parseType(char* &cur) {
    debug("parseType");
    cur = next(cur);
    if (cur[0] == POPERATOR && cur[1] == OP_DEREFGET)
        return Type::getPointerType(parseType(cur));
    int name = *(int*)(cur+1);
    switch (name) {
        case RES_ARRAY:
            cur = next(cur);
            if (cur[0] == PSPECIAL && cur[1] == SPC_LBRACE)  {
                char* tkfrom = next(cur);
                char* tkto = next(next(tkfrom));
                if (tkfrom[0] != tkto[0]) {
                    debug((int)tkfrom[0]);
                    debug((int)tkto[0]);
                    debug("we're fucked");
                    throw -1;
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
                        debug("its not that bad");
                        throw -1;
                }
                cur = next(next(tkto)); //skip the ] of
                Type* type = parseType(cur);
                return Type::getBoundArrayType(type, from, to);
            } else { //Already ate the of
                return Type::getDynamicArrayType(parseType(cur));
            }
        case RES_RECORD: {
            std::map<int,Type*> fields;
            while (*cur) {
                cur = next(cur);
                int field = *(int*)&cur[1];
                if (reserved(field)) {
                    return Type::getRecordType(fields);
                }
                cur = next(cur);
                Type* type = parseType(cur);
                cur = next(cur);
                fields[field] = type;
            }
        }
        default:
            return Type::getType(name);
    }
}

std::map<int,Type*> parseTypes(char* &cur) {
    debug("parseTypes");
    std::map<int,Type*> types;
    do {
        char* tok = next(cur);
        int name = *(int*)&tok[1];
        if (reserved(name)) {
            return types;
        }
        cur = next(tok); //skip =
        Type* type = parseType(cur);
        cur = next(cur); //skip ;
        types[name] = type;
    } while (*cur);
    return types;
}


std::map<int,Expression*> parseConsts(char* &cur) {
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
        Expression* expr = parseExpr(cur); //eats the ;
        consts[name] = expr;
        tok = next(cur);
    }
    return consts;
}

std::list<Variable*> parseVars(char* &cur) {
    debug("parseVars");
    std::list<Variable*> vars;
    std::stack<int> untyped;
    while (*cur) {
        char* tok = next(cur);
        if (tok[0] == PSPECIAL) {
            switch (tok[1]) { //silent skip ,
                case SPC_COLON: {
                    Type* type = parseType(tok);
                    while (!untyped.empty()) {
                        vars.push_back(new Variable(untyped.top(), type));
                        untyped.pop();
                    }
                    cur = next(tok);
                }
                default:
                    cur = tok;
            }
        } else {
            int name = *(int*)(tok+1);
            if (reserved(name)) {
                debug("numvars=" << vars.size());
                return vars;
            }
            untyped.push(name);
            cur = tok;
        }
    }
    return vars;
}

Method* parseMethod(char* &cur) {
    debug("parseMethod");
    cur = next(cur);
    Method* meth = new Method(*(int*)(cur+1));
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
                    Type* type = parseType(cur);
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
                case SPC_SEMICOLON:
                    goto end_defparse;
            }
        } else {
            int name = *(int*)(cur+1);
            if (name == RES_VAR) {
                byref = true;
            } else {
                untyped.push_back(name);
            }
        }
    }
    end_defparse:
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

Expression* parseExpr(char* &cur) {
    debug("parseExpr");
    static std::map<char,char> precedence = precedence_map();
    std::list<Element*> expr;
    std::stack<std::stack<opprec>*> parseStack;
    std::stack<opprec>* oper = new std::stack<opprec>();
    char lastType = -1, nextType = -1;
    bool prefix = false;
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
                }
                if (reserved(name)) goto end_exprparse; //All reserved words break expression parsing
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
                        expr.push_back(new Symbol(name,args));
                    } else {
                        expr.push_back(new Symbol(name));
                    }
                } else {
                    expr.push_back(new Symbol(name));
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
    debug("expr_created");
    return new Expression(expr);
}

Until* parseUntil(char* &cur)  {
   debug("parseUntil");
   std::list<Expression*> block = parseBlock(cur); //eats the until
   Expression* cond = parseExpr(cur);
   return new Until(block,cond);
}

Case* parseCase(char* &cur) {
    debug("parseCase");
    cur = next(cur); //skip case
    Expression* cond = parseExpr(cur);
    cur = next(cur); //skip of
    Case* pcase = new Case(cond);
    parsecase:
    while (*cur) {
        cur = next(cur);
        if (cur[0] == PNAME) {
            switch (*(int*)(cur+1)) {
                case RES_ELSE: {
                    std::list<Expression*> def = parseBlock(cur);
                    pcase->setDefault(def);
                    cur = next(cur); //skip end
                }
                case RES_END:
                    cur = next(cur); //skip ;
                    return pcase;
            }
        }
        std::stack<int> switches;
        do {
            if (cur[0] == PINTEGER) {
                switches.push(*(int*)(cur+1));
            } else if (cur[0] == PCHAR) {
                switches.push(cur[1]);
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

For* parseFor(char* &cur) {
    debug("parseFor");
    cur = next(cur); //skip for
    cur = next(cur); //get var
    int var = *(int*)(cur+1);
    cur = next(cur); //skip :=
    Expression* begin = parseExpr(cur);
    cur = next(cur); //grap the to/downto
    bool inc = cur[1] == RES_TO;
    Expression* end = parseExpr(cur);
    cur = next(cur); //skip do
    //System.out.println("End: " + end);
    std::list<Expression*> block = parseBlock(cur);
    return new For(var,begin,inc,end,block);
}

While* parseWhile(char* &cur) {
    debug("parseWhile");
    cur = next(cur); //skip while
    Expression* cond = parseExpr(cur);
    cur = next(cur); //skip do
    std::list<Expression*> block = parseBlock(cur);
    return new While(cond,block);
}

If* parseIf(char* &cur) {
    debug("parseIf");
    cur = next(cur); //skip if
    Expression* cond = parseExpr(cur);
    cur = next(cur); //skip then
    std::list<Expression*> block = parseBlock(cur);
    If* res = new If(cond,block);
    while (*cur) {
        char* tok = next(cur);
        if (tok[0] == PNAME && *(int*)(tok+1) == RES_ELSE) {
            char* temp = next(tok);
            if (temp[0] == PNAME && *(int*)(temp+1) == RES_IF) {
                cond = parseExpr(temp);
                temp = next(temp); //skip then
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

Try* parseTry(char* &cur) {
    debug("parseTry");
    std::list<Expression*> danger = parseBlock(cur);
    std::list<Expression*> saftey;
    std::list<Expression*> always;
    char* tok = next(cur);
    switch (*(int*)(tok+1)) {
        case RES_EXCEPT:
            saftey = parseBlock(cur);
            tok = next(cur);
            if (tok[0] != PNAME || *(int*)(tok+1) != RES_FINALLY)
                break;
        case RES_FINALLY:
            always = parseBlock(cur);
    }
    return new Try(danger,saftey,always);
}

std::list<Expression*> parseBlock(char* &cur) {
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
            debug("block_switch=" << *(int*)(tok+1));
            switch (*(int*)(tok+1)) { //Expressions alway begin with a PName?
                case RES_END:
                    cur = next(tok);
                    if (cur[0] != PSPECIAL || cur[1] != SPC_SEMICOLON)
                        cur = tok;
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
    }
    end_blockparse:
    return block;
}

void parseContainer(char* &cur, Container* container) {
    debug("parseContainer");
    char* tok;
    do {
        tok = next(cur);
        if (!*tok) return; // empty body or eof
        switch (*(int*)(tok+1)) {
            case RES_CONST: { //multi
                std::map<int,Expression*> consts = parseConsts(tok);
                container->constants.insert(consts.begin(),consts.end());
            } break;
            case RES_TYPE: { //multi
                std::map<int,Type*> types = parseTypes(tok);
                container->types.insert(types.begin(),types.end());
            } break;
            case RES_VAR: { //multi
                std::list<Variable*> vars = parseVars(tok);
                container->variables.insert(container->variables.end(),vars.begin(),vars.end());
            } break;
            case RES_PROCEDURE: //multi
            case RES_FUNCTION: { //multi
                Method* meth = parseMethod(tok);
                container->methods.push_back(meth);
            } break;
            case RES_BEGIN: { //single
                std::list<Expression*> block = parseBlock(cur);
                fillBlock(&container->block,&block);
                return;
            }
        }
        cur = tok;
    } while (*cur);
}

Program* parse(char* tokens) {
    debug("parse");
    char* cur = tokens;
    if (cur[0] != PNAME || *(int*)(cur+1) != RES_PROGRAM)
        return 0;
    cur = next(cur); //name
    Program* prog = new Program(*(int*)(cur+1));
    cur = next(cur); //skip ;
    parseContainer(cur,prog);
    return prog;
}
