#ifndef mgclisp_lexer_hpp
#define mgclisp_lexer_hpp
#include "mgclisp_tokenstream.hpp"
#include <unordered_map>
#include <iostream>
using namespace std;

class Lexer {
    private:
        int lparCount;
        int rparCount;
        unordered_map<char, Token> keywords;
        void initLex() {
            keywords['('] = LPAREN;
            keywords[')'] = RPAREN;
            keywords['+'] = ADD;
            keywords['-'] = SUB;
            keywords['/'] = DIV;
            keywords['*'] = MUL;
            keywords['.'] = PERIOD;
            lparCount = 0;
            rparCount = 0;
        }

        TokenStream lexConsume(string str) {
            TokenList dummy; TokenList* t = &dummy;
            for (int i = 0; i < str.length(); i++) {
                if (str[i] == '(') lparCount++;
                if (str[i] == ')') rparCount++;
                int lpos = i;
                int rpos = lpos;
                if (keywords.find(str[rpos]) != keywords.end()) {
                    TokenList* node = new TokenList(keywords[str[rpos]], str.substr(lpos, 1), lpos, rpos+1, nullptr);
                    t->next = node;
                    t = node;
                } else if (isdigit(str[rpos])) {
                    string num = "";
                    while (isdigit(str[rpos])) {
                        num.push_back(str[rpos]);
                        rpos++;
                    }
                    TokenList* node = new TokenList(NUM, str.substr(lpos,rpos-lpos), lpos, rpos, nullptr);
                    t->next = node;
                    t = node;
                    rpos--;
                } else if (isalpha(str[rpos])) {
                    string str_ = "";
                    while (isalpha(str[rpos]) && (str[rpos] != ' ' && keywords.find(str[rpos]) == keywords.end())) {
                        str_.push_back(str[rpos]);
                        rpos++;
                    }
                    TokenList* node = new TokenList(IDSYM, str.substr(lpos, rpos-lpos), lpos, rpos, nullptr);
                    t->next = node;
                    t = node;
                }
                i = rpos;
            }
            if (lparCount > rparCount) {
                cout<<"Syntax error: missing ')'"<<endl;
                return TokenStream::end();
            } else if (lparCount > rparCount) {
                cout<<"Syntax error: missing '('"<<endl;
                return TokenStream::end();
            }
            return TokenStream(dummy.next);
        }
    public:
        Lexer() {
            initLex();
        }
        TokenStream lex(string line) {
            lparCount = 0;
            rparCount = 0;
            return lexConsume(line);
        }
        TokenStream end() {
            return TokenStream(nullptr);
        }
};

#endif