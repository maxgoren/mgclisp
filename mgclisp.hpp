#ifndef mgclisp_hpp
#define mgclisp_hpp
#include <iostream>
#include <fstream>
#include "mgclisp_lexer.hpp"
#include "mgclisp_evaluator.hpp"
#include "mgclisp_envcontext.hpp"
using namespace std;

class mgclisp {
    private:
        struct History {
            int lineno;
            string text;
            TokenStream m;
            int result;
            History(int ln = 0, string str = "", TokenStream ts = 0, int r = 0) {
                lineno = ln; text = str; m = ts; result = r;
            }
        };
        vector<History> history;
        int lineno;
        Lexer lexer;
        EvalApply evaluator;
        EnvContext env;
        void intpret_line(string line);
    public:
        mgclisp(vector<string>& sexprs, bool verbosity);
        mgclisp(bool verbosity = false);
        ~mgclisp();
        void repl();
};

mgclisp::mgclisp(vector<string>& sexprs, bool verbosity) {
    __showDebug = verbosity;
    for (string expr : sexprs) {
        cout<<expr<<endl;
        intpret_line(expr);
    }
}

mgclisp::mgclisp(bool verbosity) {
    __showDebug = verbosity;
}

mgclisp::~mgclisp() {
    if (!history.empty()) {
        ofstream hist;
        hist.open(".mgclisp_history");
        if (hist.is_open()) {
            for (auto t : history) {
                hist << t.text <<endl;
            }
            hist.close();
        }
    }
}

void mgclisp::repl() {
    string input = "";
    bool running = true;
    while (running) {
        cout<<"mgclisp> ";
        getline(cin, input);
        if (input == "exit" || input == "quit") {
            running = false;
            break;
        } else if (input.at(0) != '(') {
            cout<<"Syntax Error: Missing '('"<<endl;
        } else {
            intpret_line(input);
        }
    }
}

void mgclisp::intpret_line(string line) {
    auto m = lexer.lex(line);
    if (__showDebug)
        print(m);
    evalResult res = evaluator.eval(m, env);
    if (res.type != OUTPUT) {
        cout<<" --> ";
        for (Cell<int>* cell = (res.type == LIST) ? res.value->next:res.value; cell != nullptr; cell = cell->next) {
            cout<<cell->data<<" ";
        }
        cout<<endl;
    }
    string t = (res.type == LIST ? "list":(res.type == INT ? "int":(res.type == STRING ? "string":(res.type == OUTPUT ? "output":"error"))));
    console_log("type: " + t);
    //history.push_back(History(lineno+=5, line, m, res));
}

#endif