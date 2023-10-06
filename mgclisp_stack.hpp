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
        return ret;
    }
    void push(T info) {
        stack<T>::push(info);
    }
};


#endif