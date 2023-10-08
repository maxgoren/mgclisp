#ifndef mgclisp_conscell_hpp
#define mgclisp_conscell_hpp
#include <iostream>
using namespace std;

enum Type {
    INT,
    LIST,
    STRING,
    OUTPUT,
    ERROR
};

template <class T>
struct Cell {
    Type type;
    T data;
    Cell* next;
    Cell(T data_, Type type_, Cell* next_) {
        data = data_;
        type = type_;
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
T car(Cell<T>* cell) {
    return cell->data;
}

template <class T>
Cell<T>* cdr(Cell<T>* cell) {
    return cell->next;
}

template <class T>
Type typeIs(Cell<T>* cell) {
    return cell->type;
}

template <class T>
void freeCell(Cell<T>* cell) {
    if (cell != nullptr) {
        freeCell(cell->next);
        delete cell;
    }
}

template <class T>
void _printcelllist(Cell<T>* head) {
    for (Cell<int>* cell = head; cell != nullptr; cell = cell->next) {
        cout<<cell->data<<" ";
    }
}

#endif