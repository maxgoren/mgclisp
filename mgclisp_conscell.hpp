#ifndef mgclisp_conscell_hpp
#define mgclisp_conscell_hpp
#include <iostream>
using namespace std;


template <class T>
struct Cell {
    T data;
    Cell* next;
    Cell(T data_, Cell* next_) {
        data = data_;
        next = next_;
    }
};

template <class T>
Cell<T>* cons(T a, T b) {
    return new Cell(a, new Cell(b, nullptr));
}

template <class T>
Cell<T>* cons(Cell<T>* car, Cell<T>* cdr) {
    car->next = cdr;
    return car;
}

template <class T>
int car(Cell<T>* cell) {
    return cell->data;
}

template <class T>
Cell<T>* cdr(Cell<T>* cell) {
    return cell->next;
}


#endif