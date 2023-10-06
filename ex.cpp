#include <iostream>
#include "mgclisp.hpp"
using namespace std;

int main(int argc, char *argv[]) {
    vector<string> sexprs = {"(* 9 1)",
                            "(+ (+ 1 6) 3)",
                            "(+ (- 1 6) 3)",
                            "(* (/ 6 12) (- 1 10))"};
    for (string s : sexprs) {
        cout<<s<<endl;
    }
    if (argc < 2) {
        mgclisp lisp(sexprs);
    } else {
        if (argv[1][1] == 'v') {
            mgclisp lisp(sexprs, true);
        }
    }
    return 0;
}