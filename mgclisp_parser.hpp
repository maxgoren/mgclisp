#ifndef mgclisp_parser_hpp
#define mgclisp_parser_hpp
#include <string>
#include <iostream>
#include "mgclisp_tokenstream.hpp"
using namespace std;

void console_log(string log) {
    if (__showDebug)
        cout<<log<<endl;
}

enum ParserState {
    PARSING, DONE
};

class Parser {
    private:
        TokenStream m_curr;
        Token lookahead;
        ParserState pState;
        void initParser(TokenStream tokens) {
            m_curr = tokens;
            lookahead = (*m_curr).first;
            pState = PARSING;
        }
    public:
        Parser(TokenStream& tokens) {
            initParser(tokens);
        }
        Parser() {

        }
        bool matchToken(Token token, Token other);
        bool match(Token token);
        bool nexttoken();
        string curr_value();
        Token curr_token();
        ParserState getState();
};

bool Parser::matchToken(Token token, Token other) {
    return token == other;
}
bool Parser::match(Token token) {
    if (matchToken(token, lookahead)) {
        //cout<<tokenNames[token]<<endl;
        bool r = nexttoken();
        if (r == false)
            exit(0);
        return true;
    }
    return false;
}
bool Parser::nexttoken() {
    if (m_curr != TokenStream::end()) {
        ++m_curr;
        if (m_curr != TokenStream::end())
            lookahead = curr_token();
        return true;
    }
    console_log("out of tokens");
    pState = DONE;
    return false;
}
string Parser::curr_value() {
    return (*m_curr).second;
}
Token Parser::curr_token() {
    if (m_curr == TokenStream::end())
        return ERRSYM;
    return (*m_curr).first;
}

ParserState Parser::getState() {
    return pState;
}

#endif