#ifndef ENV_CONTEXT_HPP
#define ENV_CONTEXT_HPP
#include "symboltable/hashmap.hpp"
#include "mgclisp_conscell.hpp"

enum Type {
    INT,
    LIST,
    ERROR
};

class EnvContext {
    private:
        hashmap<string, int> variables;
        hashmap<string, Cell<int>*> lists;
    public:
        EnvContext(hashmap<string, int>& ctx) {
            variables = ctx;
        }
        EnvContext() {

        }
        int getVariable(string _id) {
            return variables[_id];
        }
        void setVariable(Type type, string _id, Cell<int>* _value) {
            if (type == INT)
                variables[_id] = _value->data;
            else if (type == LIST)
                lists[_id] = _value;
        }
        Cell<int>* getList(string id) {
            return lists[id];
        }
        void addList(string id, Cell<int>* list) {
            lists[id] = list;
        }
        bool exists(string _id) {
            return variables.find(_id) != variables.end();
        }
        int num_lists() {
            return lists.size();
        }
};

#endif