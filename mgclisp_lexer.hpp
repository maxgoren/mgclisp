#ifndef mgclisp_lexer_hpp
#define mgclisp_lexer_hpp
#include "mgclisp_tokenstream.hpp"
#include <unordered_map>
#include "symboltable/hashmap.hpp"
#include <iostream>
using namespace std;

class Lexer {
    private:
        int lparCount;
        int rparCount;
        hashmap<char, Token> reserved;
        hashmap<string, Token> keywords;
        void initLex();
        TokenStream lexConsume(string str);
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

void Lexer::initLex() {
    reserved['('] = LPAREN;
    reserved[')'] = RPAREN;
    reserved['+'] = ADD;
    reserved['-'] = SUB;
    reserved['/'] = DIV;
    reserved['*'] = MUL;
    reserved['.'] = PERIOD;
    reserved['<'] = LTSYM;
    reserved['>'] = GTSYM;
    reserved['='] = EQSYM;
    reserved['#'] = NEQSYM;
    reserved['"'] = QUOTESYM;
    keywords["let"] = LETSYM;
    keywords["cons"] = CONSSYM;
    keywords["list"] = LISTSYM;
    keywords["car"] = CAR;
    keywords["cdr"] = CDR;
    keywords["say"] = SAY;
    keywords["doif"] = DOIF;
    lparCount = 0;
    rparCount = 0;
}

TokenStream Lexer::lexConsume(string str) {
    TokenList dummy; TokenList* t = &dummy;
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == '(') lparCount++;
        if (str[i] == ')') rparCount++;
        int lpos = i;
        int rpos = lpos;
        if (reserved.find(str[rpos]) != reserved.end()) {
            t->next = new TokenList(reserved[str[rpos]], str.substr(lpos, 1), lpos, rpos+1, nullptr);
            t = t->next;
            if (str[rpos] == '"') {
                lpos++;
                rpos++;
                do {
                    rpos++;
                } while (str[rpos] != '"');
                 t->next = new TokenList(STRSYM, str.substr(lpos, rpos-lpos), lpos, rpos, nullptr);
                 t = t->next;
                 lpos = rpos;
                 rpos++;
                 t->next = new TokenList(reserved[str[lpos]], str.substr(lpos, 1), lpos, rpos+1, nullptr);
                 t = t->next;
            }
        } else if (isdigit(str[rpos])) {
            string num = "";
            while (isdigit(str[rpos])) {
                num.push_back(str[rpos]);
                rpos++;
            }
            t->next = new TokenList(NUM, str.substr(lpos,rpos-lpos), lpos, rpos, nullptr);
            t = t->next;
            rpos--;
        } else if (isalpha(str[rpos])) {
            string str_ = "";
            while (isalpha(str[rpos]) && (str[rpos] != ' ' && reserved.find(str[rpos]) == reserved.end())) {
                str_.push_back(str[rpos]);
                rpos++;
            }
            if (str[rpos] == ')') { rparCount++; rpos -= 1; }
            if (keywords.find(str_) != keywords.end()) { 
                t->next = new TokenList(keywords[str_], str.substr(lpos, rpos-lpos), lpos, rpos, nullptr);
            } else {
                t->next = new TokenList(IDSYM, str_, lpos, rpos, nullptr);
            }
            t = t->next;
        }
        i = rpos;
    }
    if (lparCount > rparCount) {
        cout<<"Lexer: Syntax error: missing ')'"<<endl;
        return TokenStream::end();
    } else if (lparCount > rparCount) {
        cout<<"Lexer: Syntax error: missing '('"<<endl;
        return TokenStream::end();
    }
    return TokenStream(dummy.next);
}

#endif