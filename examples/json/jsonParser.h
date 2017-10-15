#pragma once
#include "llParser.h"
#include "jsonLLGrammar.h"
#include "jsonValue.h"

#include <array>
#include <list>

namespace json = jsonLL;

struct Parser : public LLParser<json::Grammar, value*> {
    using T = json::T;
    using NT = json::NT;
    using Token = json::Token;
    size_t next_value_index = 10000U; // this ensures that we will initilize a new "page" with the next new_value call
    std::list<std::array<value,10000>> values;
    value* new_value();
    using Base = LLParser<json::Grammar, value*>;
    using Base::Base;

    // simplify assembly (of includers of this file) by hiding these in the compilation unit
    Parser();
    ~Parser();

    value* reduce(NonterminalID<json::Grammar> A, uint32_t production, value** alpha, size_t);// override;

    value* shift(Token&& token);// override;
};
