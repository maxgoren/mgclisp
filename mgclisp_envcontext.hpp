#ifndef ENV_CONTEXT_HPP
#define ENV_CONTEXT_HPP
#include "symboltable/hashmap.hpp"
#include "mgclisp_conscell.hpp"

class EnvContext {
    private:
        hashmap<string, int> variables;
        hashmap<string, Cell<int>*> lists;
        hashmap<string, string> strings;
        Type findType(string _id) {
            if (strings.find(_id) != strings.end())
                return STRING;
            if (variables.find(_id) != variables.end())
                return INT;
            if (lists.find(_id) != lists.end())
                return LIST;
            return ERROR;
        }
    public:
        EnvContext(hashmap<string, int>& ctx) {
            variables = ctx;
        }
        EnvContext() {

        }
        ~EnvContext() {
            for (auto t :lists) {
                if (t.second != nullptr)
                    freeCell(t.second);
            }
        }
        int& getInt(string _id) {
            return variables[_id];
        }
        string& getString(string _id) {
            return strings[_id];
        }
        Cell<int>* getList(string id) {
            return lists[id];
        }
        void setVariable(Type type, string _id, Cell<int>* _value) {
            if (type == INT)
                variables[_id] = _value->data;
            else if (type == LIST) {
                Cell<int>* t = _value->next;
                lists[_id] = t;
                _value->next = nullptr;
            } else if (type == STRING)
                strings[_id] = _value->data;
            if (type != LIST) {
                delete _value;
            }
        }
        void setVariable(Type type, string _id, Cell<string>* _value) {
            if (type == STRING)
                strings[_id] = _value->data;
            delete _value;
        }
        void addList(string id, Cell<int>* list) {
            lists[id] = list;
        }
        bool exists(string _id) {
            return findType(_id) != ERROR;
        }
        Type getType(string _id) {
            return findType(_id);
        }
        int num_lists() {
            return lists.size();
        }
};

#endif