#ifndef mgclisp_evaluator_hpp
#define mgclisp_evaluator_hpp
#include "mgclisp_lexer.hpp"
#include "mgclisp_parser.hpp"
#include "mgclisp_tokenstream.hpp"
#include "mgclisp_stack.hpp"
#include "mgclisp_envcontext.hpp"
#include "symboltable/hashmap.hpp"

//its returning either a singleton, or a list of numbers. 
//if singleton, just store in Cell's data field.
struct evalResult {
    Type type;
    Cell<int>* value; 
    evalResult(Type type_, Cell<int>* result_) {
        type = type_;
        value = result_;
    }
};

class EvalApply {
    private:
        int parCount;
        Parser parser;
        Stack<Token> opStack;
        hashmap<Token, string> binOps;
        void evaluate(EnvContext& context);
        evalResult eval(EnvContext& context);
        void initBinOps();
        int applyBinOps(Stack<int> sf, Token op);
        evalResult applyLet(EnvContext& context);
        evalResult applyCons(EnvContext& context);
        evalResult applyList(EnvContext& context);
        evalResult applyString(EnvContext& context);
        evalResult applyCar(EnvContext& context);
        evalResult applyCdr(EnvContext& context);
        void applySay(EnvContext& context);
        int doBinOp(int a, int b, Token op);
        void handleVariables(EnvContext& context, string _id);
    public:
        EvalApply(TokenStream expr);
        EvalApply();
        evalResult eval(TokenStream m, EnvContext& ctx);
};

EvalApply::EvalApply(TokenStream expr) {
    initBinOps();
    parser = Parser(expr);
}
EvalApply::EvalApply() { 
    initBinOps();
}

void EvalApply::initBinOps() {
    binOps[ADD] = tokenNames[ADD];
    binOps[SUB] = tokenNames[SUB];
    binOps[MUL] = tokenNames[MUL];
    binOps[DIV] = tokenNames[DIV];
    binOps[LTSYM] = tokenNames[LTSYM];
    binOps[GTSYM] = tokenNames[GTSYM];
    binOps[EQSYM] = tokenNames[EQSYM];
    binOps[NEQSYM] = tokenNames[NEQSYM];
}

evalResult EvalApply::eval(TokenStream m, EnvContext& ctx) {
    parser = Parser(m);
    parCount = 0;
    return eval(ctx);
}

void EvalApply::handleVariables(EnvContext& context, string _id) {
    string _value;
    Type valType = context.getType(parser.curr_value());
    if (valType == INT) {
        int _ivalue = context.getInt(parser.curr_value());
        context.setVariable(INT, _id, new Cell<int>(_ivalue, INT, nullptr));
        console_log(_id + ": " + to_string(context.getInt(_id)));
    } else if (valType == STRING) {
        string _svalue = context.getString(parser.curr_value());
        context.setVariable(STRING, _id, new Cell<string>(_svalue, STRING, nullptr));
        console_log(_id + ": " + _svalue);
    } else if (valType == LIST) {
        Cell<int>* _lvalue = context.getList(_id);
        context.setVariable(LIST, _id, _lvalue);
        if (__showDebug) {
            cout<<_id<<": ";
            _printcelllist(_lvalue);
        }
    }
}


evalResult EvalApply::eval(EnvContext& context) {
    Stack<int> localVal;
    Type resultType;
    while (parser.getState() != DONE) {
        if (parser.match(LPAREN)) {
            parCount++;
            if (binOps.find(parser.curr_token()) != binOps.end()) {
                Token op = parser.curr_token();
                console_log("Op: " + tokenNames[op]);
                parser.nexttoken();
                int value = 0;
                while (!parser.match(RPAREN)) {
                    if (parser.matchToken(parser.curr_token(), LPAREN)) {
                        evalResult res = eval(context);
                        if (res.type == INT) {
                            localVal.push(res.value->data);
                        } else if (res.type == LIST) {
                            for (Cell<int>* t = res.value; t != nullptr; t = t->next) {
                                localVal.push(t->data);
                            }
                        }
                    } else if (parser.matchToken(parser.curr_token(), NUM)) {
                        localVal.push(atoi(parser.curr_value().c_str()));
                        parser.nexttoken(); 
                    } else if (parser.matchToken(parser.curr_token(), IDSYM)) {
                        Type valType = context.getType(parser.curr_value());
                        if (valType == INT) {
                            localVal.push(context.getInt(parser.curr_value()));
                        } else if (valType == LIST) {
                            Cell<int>* head = context.getList(parser.curr_value());
                            for (Cell<int>* t = head; t != nullptr; t = t->next) {
                                localVal.push(t->data);
                            }
                        }
                        parser.nexttoken();
                    } else if (parser.match(QUOTESYM) || parser.match(STRSYM)) {
                        cout<<"Error: "<<tokenNames[op]<<" can not be performed on strings!"<<endl;
                        parser.nexttoken();
                    }
                }
                parCount--;
                int tmp = applyBinOps(localVal, op);
                localVal.push(tmp);
                return evalResult(INT, new Cell<int>(tmp, INT, nullptr));
            } else if (parser.match(LETSYM)) {
                return applyLet(context);
            } else if (parser.match(LISTSYM)) {
                return applyList(context);
            } else if (parser.match(CONSSYM)) {
                return applyList(context); // ^_^
            } else if (parser.match(CAR)) {
                return applyCar(context);
            } else if (parser.match(CDR)) {
                return applyCdr(context);
            } else if (parser.match(QUOTESYM)) {
                return applyString(context);
            } else if (parser.match(SAY)) {
                applySay(context);
                cout<<endl;
                return evalResult(OUTPUT, nullptr);
            } 
        } else {
            cout<<"What?"<<parser.curr_value()<<", "<<tokenNames[parser.curr_token()]<<endl;
            return evalResult(ERROR, new Cell<int>(-255, ERROR, nullptr));
        }
    }
    return evalResult(INT, new Cell<int>(localVal.top(), INT, nullptr));
}

evalResult EvalApply::applyLet(EnvContext& context) {
    string _id;
    int _value;
    int lpar = 1;
    int rpar = 0;
    while (lpar != rpar) {
        if (parser.match(LPAREN)) {
            parCount++; lpar++;
            if (parser.matchToken(parser.curr_token(), IDSYM)) {
               _id = parser.curr_value();
                parser.nexttoken();
                if (parser.matchToken(parser.curr_token(), NUM)) {
                    _value = atoi(parser.curr_value().c_str());
                    context.setVariable(INT, _id, new Cell<int>(_value, INT, nullptr));
                    console_log(_id + ": " + to_string(context.getInt(_id)));
                    parser.nexttoken();
                } else if (parser.matchToken(parser.curr_token(), IDSYM) && context.exists(parser.curr_value())) {
                    handleVariables(context, _id);                    
                    parser.nexttoken();
                } else if (parser.matchToken(parser.curr_token(), LPAREN)) {
                    evalResult res = eval(context);
                    context.setVariable(res.type, _id, res.value);
                    if (res.type == LIST) {
                        if (__showDebug) {
                            cout<<_id<<": ";
                            _printcelllist(context.getList(_id));
                            cout<<endl;
                        }
                    }
                    if (res.type == INT)
                        console_log(_id + ": " + to_string(context.getInt(_id)));
                } else if (parser.match(QUOTESYM)) {
                    context.setVariable(STRING, _id, new Cell<string>(parser.curr_value(), STRING, nullptr));
                    parser.nexttoken();
                    parser.nexttoken();
                } else {
                    cout<<"Error assigning value: "<<parser.curr_value()<<endl;
                    return evalResult(ERROR, new Cell<int>(-255, ERROR, nullptr));
                }
            } else {
                cout<<"variable names must contain only letters"<<endl;
            }
        } else if (parser.match(RPAREN)) {
                parCount--;
                rpar++;
        }
        console_log("par: " + to_string(parCount) + ", lpar: " + to_string(lpar) + ", rpar: " + to_string(rpar));
    }
    return evalResult(context.getType(_id), new Cell<int>(_value, context.getType(_id), nullptr));
}

evalResult EvalApply::applyList(EnvContext& context) {
    Cell<int> dummy(0, LIST, nullptr);
    Cell<int>* c = &dummy;
    string _id;
    int _value;
    int lpar = 1;
    int rpar = 0;
    while (lpar != rpar) {
        if (parser.matchToken(parser.curr_token(), NUM)) {
            _value = atoi(parser.curr_value().c_str());
            c->next = new Cell<int>(_value, INT, nullptr);
            c = c->next;
            console_log("New Cell: " + to_string(_value));
            parser.nexttoken();
        } else if (parser.matchToken(parser.curr_token(), IDSYM) && context.exists(parser.curr_value())) {
            Type valType = context.getType(parser.curr_value());
            if (valType == INT) {
                _value = context.getInt(parser.curr_value());
                c->next = new Cell<int>(_value, INT, nullptr);
                c = c->next;
                console_log("New Cell: " + to_string(_value));
            } else if (valType == LIST) {
                Cell<int>* head = context.getList(parser.curr_value());
                for (Cell<int>* t = head; t != nullptr; t = t->next) {
                    c->next = new Cell<int>(t->data, t->type, nullptr);
                    c = c->next; 
                }
                console_log("Appended: ");
                if  (__showDebug)
                    _printcelllist(context.getList(parser.curr_value()));
            }
            parser.nexttoken();
        } else if (parser.matchToken(parser.curr_token(), LPAREN)) {
            lpar++;
            evalResult res = eval(context);
            c->next = (res.type == LIST) ? res.value->next:res.value;
            while (c->next) c = c->next;
            if (res.type == INT)
                console_log("New Cell: " + to_string(res.value->data));
            if (res.type == STRING)
                console_log("New Cell: " + to_string(res.value->data));
            if (res.type == LIST && __showDebug) {
                cout<<"Appended cells: ";
                _printcelllist(res.value->next);
            }
            rpar++;
        } else if (parser.match(RPAREN)) {
            rpar++;
        } else {
            cout<<"Error creating cell: "<<parser.curr_value()<<endl;
            break;
        }
    }
    context.addList(to_string(context.num_lists()), dummy.next);
    return evalResult(LIST, new Cell<int>(context.num_lists() - 1, LIST, dummy.next));
}

void EvalApply::applySay(EnvContext& context) {
    string _id;
    int _value;
    int lpar = 1;
    int rpar = 0;
    int quoteCount = 0;
    int noOpLoop = 0;
    while (lpar != rpar) {
        if (parser.matchToken(parser.curr_token(), NUM)) {
            _value = atoi(parser.curr_value().c_str());
            cout<<_value<<" ";
            parser.nexttoken();
        } else if (parser.matchToken(parser.curr_token(), IDSYM) && context.exists(parser.curr_value())) {
            Type valType = context.getType(parser.curr_value());
            if (valType == INT) {
                cout<<context.getInt(parser.curr_value())<<" ";
            } else if (valType == STRING) {
                cout<<context.getString(parser.curr_value())<<" ";
            } else if (valType == LIST) {
                _printcelllist(context.getList(parser.curr_value()));
            }
            parser.nexttoken();
        } else if (parser.matchToken(parser.curr_token(), LPAREN)) {
            evalResult res = eval(context);
            _printcelllist((res.type == LIST) ? cdr(res.value):res.value);
            rpar++;
        } else if (parser.match(QUOTESYM)) {
            if (parser.matchToken(parser.curr_token(), STRSYM)) {
                cout<<parser.curr_value()<<" ";
                parser.nexttoken();
                if (!parser.match(QUOTESYM)) {
                    cout<<"Error: missing \""<<endl;
                    return;
                }
            }
        } else {
            console_log("^");
            noOpLoop++;
            if (noOpLoop > 5) {
                console_log("Infinite loop detected, bailing out");
                cout<<"(!!!)"<<endl;
                return;
            }
        }
        if (parser.match(RPAREN)) {
                parCount -= 1;
                rpar += 1;
                if (lpar != rpar)
                    parser.nexttoken();
        }
        console_log("\npar: " + to_string(parCount) + ", lpar: " + to_string(lpar) + ", rpar: " + to_string(rpar));
    }
}

//place holder till i figure out what im doing with this.
evalResult EvalApply::applyString(EnvContext& context) {
    return evalResult(STRING, nullptr);
}

evalResult EvalApply::applyCar(EnvContext& context) {
    if (parser.matchToken(LPAREN, parser.curr_token())) {
        evalResult res = eval(context);
        if (res.type == LIST || res.type == INT)
            return evalResult(INT, new Cell<int>(res.value->next->data, INT, nullptr));
        else cout<<"Error: That only works on lists and cons!"<<endl;
    }
    if (parser.matchToken(IDSYM, parser.curr_token())) {
        Type valType = context.getType(parser.curr_value());
        if (valType == LIST) {
            Cell<int>* res = context.getList(parser.curr_value());
            return evalResult(INT, new Cell<int>(res->data, INT, nullptr));
        } else if (valType == INT) {
            return evalResult(INT, new Cell<int>(context.getInt(parser.curr_value()), INT, nullptr));
        } else cout<<"Error: That only works on lists and cons! "<<valType<<endl;
    }
    return evalResult(ERROR, nullptr); 
}

evalResult EvalApply::applyCdr(EnvContext& context) {
    return evalResult(ERROR, nullptr);
}

int EvalApply::applyBinOps(Stack<int> localVal, Token op) {
    int val = localVal.pop();
    while (!localVal.empty()) {            
        val = doBinOp(val, localVal.pop(), op);
    }
    return val;
}

int EvalApply::doBinOp(int a, int b, Token op) {
    console_log(to_string(a) + " " + tokenNames[op] + " " + to_string(b));
    switch (op) {
        case MUL: return a *= b;
        case ADD: return a += b;
        case DIV: return b / a;
        case SUB: return b - a;
        case LTSYM: return b < a;
        case GTSYM: return b > a;
        case EQSYM: return b == a;
        case NEQSYM: return b != a;
    }
    cout<<"Invalid Op: "<<tokenNames[op]<<endl;
    return -255;
}

#endif