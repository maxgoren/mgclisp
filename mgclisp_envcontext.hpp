#ifndef ENV_CONTEXT_HPP
#define ENV_CONTEXT_HPP
#include "symboltable/hashmap.hpp"
#include "mgclisp_conscell.hpp"

class EnvContext {
    private:
        hashmap<string, int> variables;
        hashmap<string, Cell<int>*> lists;
        hashmap<string, string> strings;
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
            else if (type == STRING)
                strings[_id] = _value->data;
        }
        void setVariable(Type type, string _id, Cell<string>* _value) {
            if (type == STRING)
                strings[_id] = _value->data;
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