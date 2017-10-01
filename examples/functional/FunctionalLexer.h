#pragma once

#include "FunctionalGrammar.h"
#include "Regex.h"

#include <sstream>
#include <vector>

struct myLexer : public Lexer<Functional::Token*> {
    using Token = Functional::Token;
    using T= Functional::T;
    static const vector<unsigned int> m;

    DFA_t getDFA();

    myLexer() : Lexer(getDFA()) {}

    Token* eofToken() const override {
        return new Token(T::EOI);
    }

    Token* whiteSpaceToken() const override {
        return nullptr;
    }

    Token* action(const char* s, size_t n, TokenId t) override {
        cout << "action(\"";
        cout.write(s, n);
        cout << "\", " << n << ", " << int(t) << ")" << endl;
        Token* tok;
        auto tid = TerminalID<Functional::Grammar>(t - 1);
        switch(tid) {
        case T::SEMICOLON:
        case T::ASSIGN:
        case T::PERIOD:
        case T::LPAREN:
        case T::RPAREN:
        case T::BSL:
        case T::PLUS:
        case T::MINUS:
        case T::TIMES:
        case T::DIV:
        case T::LT:
        case T::GT:
        case T::AND:
        case T::OR:
        case T::LE:
        case T::EQ:
        case T::NE:
        case T::GE:
        case T::LET:
        case T::IN:
        case T::LETREC:
            tok = new Token(tid);
            break;
        case T::VAR:
            tok = new Functional::Var(string(s, n));
            break;
        case T::NUM: {
            unsigned long long i;
            std::stringstream ss;
            ss.exceptions(std::stringstream::failbit | std::stringstream::badbit);
            ss.write(s, n);
            ss >> i;
            tok = new Functional::Num(i);
            break;
        }
        case T::WS:
            tok = nullptr;
            break;
        default: {
            std::stringstream ss;
            ss << "invalid token type: " << t;
            throw std::runtime_error(ss.str());
        }

        }
        return tok;
    }
};


