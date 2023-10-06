#ifndef mgclisp_tokensteam_hpp
#define mgclisp_tokensteam_hpp
#include <string>
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

bool __showDebug;

class TokenStream {
    private:
        TokenList* m_curr;
    public:
        TokenStream(TokenList* tokens) {
            m_curr = tokens;
        }
        TokenStream() : m_curr(nullptr) { }
        pair<Token,string> operator*() {
            return make_pair(m_curr->token, m_curr->str);
        }
        TokenStream operator++() {
            m_curr = m_curr->next;
            return *this;
        }
        TokenStream operator++(int) {
            TokenStream it = *this;
            ++*this;
            return it;
        }
        bool operator==(const TokenStream& o) const {
            return m_curr == o.m_curr;
        }
        bool operator!=(const TokenStream& o) const {
            return !(*this == o);
        }
        static const TokenStream end() {
            return static_cast<TokenStream>(nullptr);
        }
};

#endif