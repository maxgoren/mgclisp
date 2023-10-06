#ifndef stack_queue_hpp
#define stack_queue_hpp
#include <stack>
#include "mgclisp_tokenstream.hpp"
using namespace std;

template <typename T>
struct Stack : public stack<T> {
    T pop() {
        T ret = stack<T>::top();
        stack<T>::pop();
        if (__showDebug)
            cout<<"Pop: "<<ret<<endl;
        return ret;
    }
    void push(T info) {
        if (__showDebug)
            cout<<"Push: "<<info<<endl;
        stack<T>::push(info);
    }
};


#endif