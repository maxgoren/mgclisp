#ifndef stack_queue_hpp
#define stack_queue_hpp
#include <stack>
#include <queue>
using namespace std;

template <typename T>
struct Stack : public stack<T> {
    T pop() {
        T ret = stack<T>::top();
        stack<T>::pop();
        //cout<<"Pop: "<<ret<<endl;
        return ret;
    }
    void push(T info) {
        //cout<<"Push: "<<info<<endl;
        stack<T>::push(info);
    }
};


#endif