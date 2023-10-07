#ifndef mgclisp_evaluator_hpp
#define mgclisp_evaluator_hpp
#include "mgclisp_lexer.hpp"
#include "mgclisp_parser.hpp"
#include "mgclisp_tokenstream.hpp"
#include "mgclisp_stack.hpp"
#include "mgclisp_envcontext.hpp"
#include "symboltable/hashmap.hpp"


class Evaluator {
    private:
        int parCount;
        Parser parser;
        Stack<int> valStack;
        Stack<Token> opStack;
        hashmap<Token, string> binOps;
        void handleCons(EnvContext& context);
        void handleList(EnvContext& context);
        void handleLet(EnvContext& context);
        void evaluate(EnvContext& context);
        int eval(EnvContext& context);
        void initBinOps();
        int calculateScope(Stack<int> sf, Token op);
    public:
        Evaluator(TokenStream expr);
        Evaluator();
        int eval(TokenStream m, EnvContext& ctx);
};

Evaluator::Evaluator(TokenStream expr) {
    initBinOps();
    parser = Parser(expr);
}
Evaluator::Evaluator() { 
    initBinOps();
}

void Evaluator::initBinOps() {
    binOps[ADD] = tokenNames[ADD];
    binOps[SUB] = tokenNames[SUB];
    binOps[MUL] = tokenNames[MUL];
    binOps[DIV] = tokenNames[DIV];
    binOps[LTSYM] = tokenNames[LTSYM];
    binOps[GTSYM] = tokenNames[GTSYM];
    binOps[EQSYM] = tokenNames[EQSYM];
    binOps[NEQSYM] = tokenNames[NEQSYM];
}

int Evaluator::eval(TokenStream m, EnvContext& ctx) {
    parser = Parser(m);
    parCount = 0;
    return eval(ctx);
}


void Evaluator::handleLet(EnvContext& context) {
    string _id;
    int _value;
    if (parser.match(LPAREN)) {
        parCount++;
        if (parser.matchToken(parser.curr_token(), IDSYM)) {
            _id = parser.curr_value();
            parser.nexttoken();
            if (parser.matchToken(parser.curr_token(), NUM)) {
                _value = atoi(parser.curr_value().c_str());
                context.setVariable(_id, _value);
                console_log(_id + ": " + to_string(context.getVariable(_id)));
                parser.nexttoken();
            } else {
                cout<<"Assigned value must be a number"<<endl;
            }
            valStack.push(_value);
            return;
        } else {
            cout<<"variable names must contain only letters"<<endl;
        }
    }
}


int Evaluator::eval(EnvContext& context) {
    Stack<int> localVal;
    bool isLet = false;
    int noOpLoopCount = 0;
    while (parser.getState() != DONE) {
        if (parser.match(LPAREN)) {
            parCount++;
            if (binOps.find(parser.curr_token()) != binOps.end()) {
                Token op = parser.curr_token();
                cout<<"Op: "<<tokenNames[op]<<endl;
                parser.nexttoken();
                int value = 0;
                while (!parser.match(RPAREN)) {
                    if (parser.matchToken(parser.curr_token(), LPAREN)) {
                        int res = eval(context);
                        localVal.push(res);
                    } else if (parser.matchToken(parser.curr_token(), NUM)) {
                        localVal.push(atoi(parser.curr_value().c_str()));
                        parser.nexttoken(); 
                    } else if (parser.matchToken(parser.curr_token(), IDSYM)) {
                        int val = context.getVariable(parser.curr_value());
                        localVal.push(val);
                        parser.nexttoken();
                    }
                }
                parCount--;
                int tmp = calculateScope(localVal, op);
                localVal.push(tmp);
                return tmp;
            }    
        } else if (parser.match(LETSYM)) {
            handleLet(context);
            return valStack.top();
        }
    }
    return localVal.top();
}

int Evaluator::calculateScope(Stack<int> localVal, Token op) {
    int val = localVal.pop();
    while (!localVal.empty()) {            
        console_log(to_string(localVal.top()) + " " + tokenNames[op] + " " + to_string(val));
        if (op == MUL) val *= localVal.pop();
        if (op == ADD) val += localVal.pop();
        if (op == DIV) val = localVal.pop() / val;
        if (op == SUB) val = localVal.pop() - val;
        if (op == LTSYM) val = localVal.pop() < val;
        if (op == GTSYM) val = localVal.pop() > val;
        if (op == EQSYM) val = localVal.pop() == val;
        if (op == NEQSYM) val = localVal.pop() != val;
    }
    return val;
}

#endif