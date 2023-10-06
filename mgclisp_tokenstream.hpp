#ifndef mgclisp_tokensteam_hpp
#define mgclisp_tokensteam_hpp
#include <string>
#include <vector>
#include "mgclisp_token.hpp"
using namespace std;

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

void print(TokenStream& mtokens) {
    for (TokenStream it = mtokens; it != TokenStream::end(); it++) {
        cout<<"<["<<tokenNames[(*it).first]<<", "<<(*it).second<<"]>"<<endl;
    }
}

#endif