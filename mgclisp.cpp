#include <iostream>
#include "mgclisp.hpp"
using namespace std;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        mgclisp lisp;
        lisp.repl();
    } else {
        if (argv[1][1] == 'v') {
            mgclisp lisp(true);
            lisp.repl();
        }
    }
    return 0;
}