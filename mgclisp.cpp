#include <iostream>
#include "mgclisp_lexer.hpp"
#include "mgclisp_evaluator.hpp"
using namespace std;

void print(TokenStream& mtokens) {
    for (TokenStream it = mtokens; it != TokenStream::end(); it++) {
        cout<<"<["<<tokenNames[(*it).first]<<", "<<(*it).second<<"]>"<<endl;
    }
}

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
        void intpret_line(string line) {
            auto m = lexer.lex(line);
            int res = evaluator.eval(m);
            cout<<"  "<<res<<endl;
            history.push_back(History(lineno+=5, m, res));
        }
    public:
        mgclisp(vector<string>& sexprs) {
            for (string expr : sexprs)
                intpret_line(expr);
        }
        mgclisp() {

        }
        void repl() {
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
};

int main() {
    vector<string> sexprs = {"(* 9 1)",
                            "(+ (+ 1 6) 3)",
                            "(+ (- 1 6) 3)",
                            "(* (/ 6 12) (- 1 10))"};
    mgclisp lisp;
    lisp.repl();
    return 0;
}