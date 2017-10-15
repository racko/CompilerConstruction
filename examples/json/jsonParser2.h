#pragma once
#include "attributed_lrparser.h"
#include "jsonGrammar2.h"
#include "jsonValue.h"

#include <array>
#include <list>

struct Attributes {
    using T = json::T;
    using NT = json::NT;
    using Token = json::Token;
    using type = value*;
    size_t next_value_index = 10000U; // this ensures that we will initilize a new "page" with the next new_value call
    std::list<std::array<value,10000>> values;
    value* new_value();

    // simplify assembly (of includers of this file) by hiding these in the compilation unit
    //Attributes();
    //~Attributes();

    value* reduce(NonterminalID<json::Grammar> A, uint32_t production, value** alpha, size_t);

    value* shift(Token&& token);
    static TerminalID<json::Grammar> getTag(const Token& token) { return token.first; }
};

class Parser : AttributedLRParser<Attributes> {
public:
    using Base = AttributedLRParser<Attributes>;
    using Base::Base;
    using Base::parse;
private:
};

