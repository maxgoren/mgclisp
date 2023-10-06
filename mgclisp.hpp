#ifndef mgclisp_hpp
#define mgclisp_hpp
#include <iostream>
#include "mgclisp_lexer.hpp"
#include "mgclisp_evaluator.hpp"
#include "mgclisp_envcontext.hpp"
using namespace std;

class mgclisp {
    private:
        struct History {
            int lineno;
            TokenStream m;
            int result;
            History(int ln = 0, TokenStream ts = 0, int r = 0) {
                lineno = ln; m = ts; result = r;
            }
        };
        vector<History> history;
        int lineno;
        Lexer lexer;
        Evaluator evaluator;
        EnvContext env;
        void intpret_line(string line);
    public:
        mgclisp(vector<string>& sexprs, bool verbosity = false) {
            __showDebug = verbosity;
            for (string expr : sexprs) {
                cout<<expr<<endl;
                intpret_line(expr);
            }
        }
        mgclisp(bool verbosity = false) {
            __showDebug = verbosity;
        }
        void repl();
};

void mgclisp::repl() {
    string input = "";
    bool running = true;
    while (running) {
        cout<<"mgclisp> ";
        getline(cin, input);
        if (input == "exit" || input == "quit") {
            running = false;
            break;
        }
        intpret_line(input);
    }
}

void mgclisp::intpret_line(string line) {
    auto m = lexer.lex(line);
    if (__showDebug)
        print(m);
    int res = evaluator.eval(m, env);
    cout<<" --> "<<res<<endl;
    history.push_back(History(lineno+=5, m, res));
}

#endif