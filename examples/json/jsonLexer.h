#pragma once

#include <memory>

namespace jsonLL { struct Token; }
namespace json = jsonLL;

struct myLexer {

    myLexer();
    ~myLexer();
    
    struct iterator {
        myLexer* lex_;

        iterator(myLexer* lex) : lex_(lex) {}

        const json::Token& operator*() const { return *(*lex_); }
        json::Token& operator*() { return *(*lex_); }
        iterator& operator++() { ++(*lex_); return *this; }
    };

    //iterator begin() { return iterator(*this); }
    iterator begin() { return iterator(this); }
    void setText(const char*);

    const json::Token& operator*() const;
    json::Token& operator*();
    myLexer& operator++();

    struct impl;
    std::unique_ptr<impl> pimpl;
};

