#ifndef ENV_CONTEXT_HPP
#define ENV_CONTEXT_HPP
#include "symboltable/hashmap.hpp"

class EnvContext {
    private:
        hashmap<string, int> variables;
    public:
        EnvContext(hashmap<string, int>& ctx) {
            variables = ctx;
        }
        EnvContext() {

        }
        int getVariable(string _id) {
            return variables[_id];
        }
        void setVariable(string _id, int _value) {
            variables[_id] = _value;
        }
        bool exists(string _id) {
            return variables.find(_id) != variables.end();
        }
};

#endif