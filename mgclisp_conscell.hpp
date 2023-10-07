#ifndef mgclisp_conscell_hpp
#define mgclisp_conscell_hpp
#include <iostream>
using namespace std;
#define INT 0
#define STR 1
#define CHR 2

struct Cell {
    int tag;
    struct {
        int num;
        char alpha;
        string str;
    } data;
    Cell* next;
    Cell() {

    }
};

Cell* conscell(int data) {
    Cell* cc = new Cell;
    cc->tag = 0;
    cc->data.num = data;
    cc->next = nullptr;
    return cc;
}

Cell* conscell(string data) {
    Cell* cc = new Cell;
    cc->tag = 1;
    cc->data.str = data;
    cc->next = nullptr;
    return cc;
}

Cell* conscell(char data) {
    Cell* cc = new Cell;
    cc->tag = 2;
    cc->data.alpha = data;
    cc->next = nullptr;
    return cc;
}

Cell* cons(Cell* car, Cell* cdr) {
    car->next = cdr;
    return car;
}

Cell* cdr(Cell* cell) {
    return cell->next;
}


#endif