#ifndef mgclisp_parser_hpp
#define mgclisp_parser_hpp
#include <string>
#include <iostream>
#include "mgclisp_tokenstream.hpp"
using namespace std;


class Parser {
    private:
        TokenStream m_curr;
        Token lookahead;
        void  initParser(TokenStream tokens) {
            m_curr = tokens;
            lookahead = (*m_curr).first;
        }
    public:
        Parser(TokenStream& tokens) {
            initParser(tokens);
        }
        Parser() {

        }
        bool matchToken(Token token, Token other) {
            return token == other;
        }
        bool match(Token token) {
            if (matchToken(token, lookahead)) {
                //cout<<tokenNames[token]<<endl;
                bool r = nexttoken();
                if (r == false)
                    exit(0);
                return true;
            }
            return false;
        }
        bool nexttoken() {
            if (m_curr != TokenStream::end()) {
                ++m_curr;
                if (m_curr != TokenStream::end())
                    lookahead = curr_token();
                return true;
            }
            cout<<"Error: out of tokens"<<endl;
            return false;
        }
        string curr_value() {
            return (*m_curr).second;
        }
        Token curr_token() {
            return (*m_curr).first;
        }
};

#endif