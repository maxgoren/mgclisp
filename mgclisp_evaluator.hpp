#ifndef mgclisp_evaluator_hpp
#define mgclisp_evaluator_hpp
#include "mgclisp_lexer.hpp"
#include "mgclisp_parser.hpp"
#include "mgclisp_tokenstream.hpp"
#include "mgclisp_stack.hpp"
#include "mgclisp_envcontext.hpp"

class Evaluator {
    private:
        void console_log(string log) {
            if (__showDebug)
                cout<<log<<endl;
        }
        int parCount;
        Parser parser;
        Stack<int> valStack;
        Stack<Token> opStack;
        void handleLet(EnvContext& context) {
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
        void evaluate(EnvContext& context) {
            bool isLet = false;
            for (;;) {
                if (parser.match(LPAREN)) {
                    parCount++;
                    if (parser.match(ADD)) opStack.push(ADD);
                    else if (parser.match(MUL)) opStack.push(MUL);
                    else if (parser.match(SUB)) opStack.push(SUB);
                    else if (parser.match(DIV)) opStack.push(DIV);
                    else if (parser.match(LETSYM)) {
                        handleLet(context);
                        isLet = true;
                        return;
                    }
                    continue;
                } else if (parser.match(RPAREN) && !isLet) {
                    parCount--;
                    Token op = opStack.pop();
                    int a = valStack.pop();
                    int b = valStack.pop();
                    console_log(to_string(a) + " " + tokenNames[op] + " " + to_string(b));
                    if (op == MUL) valStack.push(a*b);
                    if (op == ADD) valStack.push(a+b);
                    if (op == DIV) valStack.push(a/b);
                    if (op == SUB) valStack.push(a-b);
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
                    parser.nexttoken();
                    continue;
                } else if (parser.matchToken(parser.curr_token(), IDSYM)) {
                    string _id = parser.curr_value();
                    if (context.exists(_id)) {
                        valStack.push(context.getVariable(_id));
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
    public:
        Evaluator(TokenStream expr) {
            parser = Parser(expr);
        }
        Evaluator() { }
        int eval(TokenStream m, EnvContext& ctx) {
            parser = Parser(m);
            parCount = 0;
            evaluate(ctx);
            return valStack.top();
        }
};

#endif