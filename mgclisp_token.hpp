#ifndef mgclisp_token_hpp
#define mgclisp_token_hpp
#include <iostream>
#include <vector>
using namespace std;

enum Token {
    LPAREN, RPAREN, ADD, SUB, MUL, DIV, NUM, PERIOD, IDSYM, LETSYM, CONSSYM, LISTSYM
};
vector<Token> tokenVals = {   LPAREN, RPAREN, ADD, SUB, MUL, DIV, NUM, PERIOD, IDSYM, LETSYM, CONSSYM, LISTSYM };
vector<string> tokenNames = {"LPAREN", "RPAREN", "ADD", "SUB", "MUL", "DIV", "NUM", "PERIOD", "IDSYM", "LET", "CONS", "LISTSYM"};

bool isToken(Token tok) {
    for (auto t : tokenVals)
        if (t == tok)
            return true;
    return false;
}

struct TokenList {
    Token token;
    string str;
    int lpos;
    int rpos;
    TokenList* next;
    TokenList(Token tok, string s, int lp, int rp, TokenList* n) {
        token = tok; str = s; lpos = lp; rpos = rp; next = n;
    }
    TokenList() {
        lpos = 0; rpos = 0;
        next = nullptr; str = "";
    }
};

#endif