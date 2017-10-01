#pragma once
#include "Regex.h"
#include "jsonGrammar.h"

struct myLexer : public Lexer<json::Token> {
    using T = json::T;
    using Token = json::Token;
    static const vector<unsigned int> m;

    DFA_t getDFA();

    myLexer() : Lexer(getDFA()) {}

    Token eofToken() const override {
        return {T::EOI,boost::blank()};
    }

    Token whiteSpaceToken() const override {
        return {T::WS,boost::blank()};
    }

    Token action(const char* s, size_t n, TokenId t) override {
        //cout << "action(\"";
        //cout.write(s, n);
        //cout << "\", " << n << ", " << t << ")" << endl;
        auto tid = TerminalID<json::Grammar>(t - 1);
        switch(tid) {
        case T::TRUE:
            return {tid,true};
        case T::FALSE:
            return {tid,false};
        case T::NULL:
        case T::BEGIN_ARRAY:
        case T::BEGIN_OBJECT:
        case T::END_ARRAY:
        case T::END_OBJECT:
        case T::NAME_SEPARATOR:
        case T::VALUE_SEPARATOR:
            return {tid,boost::blank()};
        case T::STRING:
            return {T::STRING,boost::string_view(s+1, n-2)}; // the arithmetic removes quotation marks
        case T::NUM: {
            return {T::NUM,json::num_view(s, n)};
        }
        case T::WS:
            return {T::WS,boost::blank()};
        default: {
            std::stringstream ss;
            ss << "invalid token type: " << t;
            throw std::runtime_error(ss.str());
        }

        }
    }
};

