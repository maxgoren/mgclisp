#ifndef mgclisp_evaluator_hpp
#define mgclisp_evaluator_hpp
#include "mgclisp_lexer.hpp"
#include "mgclisp_parser.hpp"
#include "mgclisp_tokenstream.hpp"
#include "mgclisp_stack.hpp"
#include "mgclisp_envcontext.hpp"
#include "symboltable/hashmap.hpp"

void console_log(string log) {
    if (__showDebug)
        cout<<log<<endl;
}

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
        void initBinOps();
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
    evaluate(ctx);
    return valStack.top();
}

void Evaluator::handleList(EnvContext& context) {
    if (parser.matchToken(parser.curr_token(), IDSYM) || parser.matchToken(parser.curr_token(), NUM)) {

    }
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

void Evaluator::evaluate(EnvContext& context) {
    bool isLet = false;
    for (;;) {
        if (parser.match(LPAREN)) {
            parCount++;
            if (binOps.find(parser.curr_token()) != binOps.end()) {
                opStack.push(parser.curr_token());
                parser.nexttoken();
            } else if (parser.match(LETSYM)) {
                handleLet(context);
                isLet = true;
                return;
            } else if (parser.match(LISTSYM)) {
                handleList(context);
            }
            console_log("Push: " + tokenNames[opStack.top()]);
            continue;
        } else if (parser.match(RPAREN) && !isLet) {
            parCount--;
            Token op = opStack.pop();
            int a = valStack.pop();
            int b = valStack.pop();
            console_log("Pop: " + tokenNames[op]);
            console_log("Pop: " + to_string(a));
            console_log("Pop: " + to_string(b));
            console_log(to_string(a) + " " + tokenNames[op] + " " + to_string(b));
            if (op == MUL) valStack.push(a*b);
            if (op == ADD) valStack.push(a+b);
            if (op == DIV) valStack.push(b/a);
            if (op == SUB) valStack.push(b-a);
            if (op == LTSYM) valStack.push(b < a);
            if (op == GTSYM) valStack.push(b > a);
            if (op == EQSYM) valStack.push(a == b);
            if (op == NEQSYM) valStack.push(a != b);
            console_log("Result: " + to_string(valStack.top()));
            if (parCount == 0 && opStack.empty())
                return;
            continue;
        } else if (parser.match(RPAREN) && isLet) {
            parCount--;
            return;
            continue;
        } else if (parser.matchToken(parser.curr_token(), NUM)) {
            valStack.push(atoi(parser.curr_value().c_str()));
            console_log("Push: " + to_string(valStack.top()));
            parser.nexttoken();
            continue;
        } else if (parser.matchToken(parser.curr_token(), IDSYM)) {
            string _id = parser.curr_value();
            if (context.exists(_id)) {
                valStack.push(context.getVariable(_id));
                console_log("Push: " + to_string(valStack.top()));
                parser.nexttoken();
            } else {
                cout<<"Error: unknown Identifier: '"<<_id<<"'"<<endl;
                valStack.push(-1);
                return;
            }
        }
        if (parCount == 0 && opStack.empty())
                return;
    }
}

#endif