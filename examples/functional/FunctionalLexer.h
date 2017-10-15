#pragma once

#include "FunctionalGrammar.h"
#include "Regex.h"

#include <sstream>
#include <vector>

//struct myLexer : public Lexer<Functional::Token*> {
struct myLexer {
    using Token = Functional::Token;
    using T= Functional::T;

    using DFA_t = Lexer::DFA_t;
    using Symbol = Lexer::Symbol;
    using State = Lexer::State;
    using TokenId = Lexer::TokenId;

    //static const vector<unsigned int> m;

    Lexer lexer;
    Token* token = nullptr;

    static DFA_t getDFA();

    myLexer() : lexer(getDFA(), {nullptr, 0, static_cast<Functional::type>(T::EOI) + 1}, static_cast<Functional::type>(T::WS) + 1) {}

    //Token* eofToken() const override {
    //    return new Token(T::EOI);
    //}

    //Token* whiteSpaceToken() const override {
    //    return nullptr;
    //}

    static Token* action(const char* s, size_t n, TokenId t);
    static Token* t2t(::Token t) { return action(t.start, t.length, t.tokenId); }

    struct iterator {
        myLexer* lex_;

        iterator(myLexer* lex) : lex_(lex) {}

        const Token* operator*() const { return *(*lex_); }
        Token* operator*() { return *(*lex_); }
        iterator& operator++() { ++(*lex_); return *this; }
    };

    //iterator begin() { return iterator(*this); }
    iterator begin() { return iterator(this); }
    void setText(const char* text) { lexer.c = text; token = t2t(lexer.getToken()); }

    const Token* operator*() const { return token; }
    Token* operator*() { return token; }
    myLexer& operator++() { token = t2t(lexer.getToken()); return *this; }
};


