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

class Evaluator {
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
        void applySay(EnvContext& context);
        int doBinOp(int a, int b, Token op);
    public:
        Evaluator(TokenStream expr);
        Evaluator();
        evalResult eval(TokenStream m, EnvContext& ctx);
};

Evaluator::Evaluator(TokenStream expr) {
    initBinOps();
    parser = Parser(expr);
}
Evaluator::Evaluator() { 
    initBinOps();
}

void Evaluator::initBinOps() {
    binOps[ADD] = tokenNames[ADD];
    binOps[SUB] = tokenNames[SUB];
    binOps[MUL] = tokenNames[MUL];
    binOps[DIV] = tokenNames[DIV];
    binOps[LTSYM] = tokenNames[LTSYM];
    binOps[GTSYM] = tokenNames[GTSYM];
    binOps[EQSYM] = tokenNames[EQSYM];
    binOps[NEQSYM] = tokenNames[NEQSYM];
}

evalResult Evaluator::eval(TokenStream m, EnvContext& ctx) {
    parser = Parser(m);
    parCount = 0;
    return eval(ctx);
}


evalResult Evaluator::eval(EnvContext& context) {
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
                        localVal.push(car(res.value));
                    } else if (parser.matchToken(parser.curr_token(), NUM)) {
                        localVal.push(atoi(parser.curr_value().c_str()));
                        parser.nexttoken(); 
                    } else if (parser.matchToken(parser.curr_token(), IDSYM)) {
                        int val = context.getVariable(parser.curr_value());
                        localVal.push(val);
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
            } else if (parser.match(QUOTESYM)) {
                return applyString(context);
            } else if (parser.match(SAY)) {
                applySay(context);
                return evalResult(OUTPUT, nullptr);
            }
        } else {
            cout<<"What?"<<endl;
            return evalResult(ERROR, new Cell<int>(-255, ERROR, nullptr));
        }
    }
    return evalResult(INT, new Cell<int>(localVal.top(), INT, nullptr));
}

evalResult Evaluator::applyLet(EnvContext& context) {
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
                    console_log(_id + ": " + to_string(context.getVariable(_id)));
                    parser.nexttoken();
                } else if (parser.matchToken(parser.curr_token(), IDSYM) && context.exists(parser.curr_value())) {
                    _value = context.getVariable(parser.curr_value());
                    context.setVariable(INT, _id, new Cell<int>(_value, INT, nullptr));
                    console_log(_id + ": " + to_string(context.getVariable(_id)));
                    parser.nexttoken();
                } else if (parser.matchToken(parser.curr_token(), LPAREN)) {
                    evalResult res = eval(context);
                    context.setVariable(res.type, _id, res.value);
                    console_log(_id + ": " + to_string(context.getVariable(_id)));
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
    return evalResult(INT, new Cell<int>(_value, INT, nullptr));
}

evalResult Evaluator::applyList(EnvContext& context) {
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
            _value = context.getVariable(parser.curr_value());
            c->next = new Cell<int>(_value, INT, nullptr);
            c = c->next;
            console_log("New Cell: " + to_string(_value));
            parser.nexttoken();
        } else if (parser.matchToken(parser.curr_token(), LPAREN)) {
            lpar++;
            evalResult res = eval(context);
            c->next = res.value;
            while (c->next) c = c->next;
            console_log("New Cell: " + to_string(_value));
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

void Evaluator::applySay(EnvContext& context) {
    string _id;
    int _value;
    int lpar = 1;
    int rpar = 0;
    int quoteCount = 0;
    while (lpar != rpar) {
        if (parser.matchToken(parser.curr_token(), NUM)) {
            _value = atoi(parser.curr_value().c_str());
            cout<<_value<<" ";
            parser.nexttoken();
        } else if (parser.matchToken(parser.curr_token(), IDSYM) && context.exists(parser.curr_value())) {
            _value = context.getVariable(parser.curr_value());
            cout<<_value<<" ";
            parser.nexttoken();
        } else if (parser.matchToken(parser.curr_token(), LPAREN)) {
            evalResult res = eval(context);
            cout<<" --> ";
            for (Cell<int>* cell = (res.type == LIST) ? cdr(res.value):res.value; cell != nullptr; cell = cell->next) {
                cout<<cell->data<<" ";
            }
        } else if (parser.match(QUOTESYM)) {
            quoteCount += 1;
            if (quoteCount % 2 != 0 && parser.matchToken(parser.curr_token(), STRSYM)) {
                cout<<parser.curr_value()<<" ";
                parser.nexttoken();
            } else {
                parser.nexttoken();
            }
        } else if (parser.matchToken(parser.curr_token(), RPAREN)) {
                parCount -= 1;
                rpar += 1;
                parser.nexttoken();
        }
        console_log("par: " + to_string(parCount) + ", lpar: " + to_string(lpar) + ", rpar: " + to_string(rpar));
    }
}

evalResult Evaluator::applyString(EnvContext& context) {
    return evalResult(STRING, nullptr);
}

int Evaluator::applyBinOps(Stack<int> localVal, Token op) {
    int val = localVal.pop();
    while (!localVal.empty()) {            
        val = doBinOp(val, localVal.pop(), op);
    }
    return val;
}

int Evaluator::doBinOp(int a, int b, Token op) {
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