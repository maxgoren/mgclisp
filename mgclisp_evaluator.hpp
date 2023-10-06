#ifndef mgclisp_evaluator_hpp
#define mgclisp_evaluator_hpp
#include "mgclisp_lexer.hpp"
#include "mgclisp_parser.hpp"
#include "mgclisp_tokenstream.hpp"
#include "mgclisp_stack.hpp"

class Evaluator {
    private:
        int parCount;
        Parser parser;
        Stack<int> valStack;
        Stack<Token> opStack;
        void evaluate() {
            for (;;) {
            if (parser.match(LPAREN)) {
                parCount++;
                if (parser.match(ADD)) opStack.push(ADD);
                else if (parser.match(MUL)) opStack.push(MUL);
                else if (parser.match(SUB)) opStack.push(SUB);
                else if (parser.match(DIV)) opStack.push(DIV);
                continue;
            } else if (parser.match(RPAREN)) {
                parCount--;
                Token op = opStack.pop();
                int a = valStack.pop();
                int b = valStack.pop();
                //cout<<a<<" "<<tokenNames[op]<<" "<<b<<endl;
                if (op == MUL) valStack.push(a*b);
                if (op == ADD) valStack.push(a+b);
                if (op == DIV) valStack.push(a/b);
                if (op == SUB) valStack.push(a-b);
                    //cout<<valStack.top()<<endl;
                    if (parCount == 0 && opStack.empty())
                        return;
                    continue;
                } else if (parser.matchToken(parser.curr_token(), NUM)) {
                    valStack.push(atoi(parser.curr_value().c_str()));
                    parser.nexttoken();
                }
            }
        }
    public:
        Evaluator(TokenStream expr) {
            parser = Parser(expr);
        }
        Evaluator() { }
        int eval(TokenStream m) {
            parser = Parser(m);
            evaluate();
            return valStack.top();
        }
};

#endif