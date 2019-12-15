#pragma once

#include "jsonLRGrammar.h" // for Grammar
#include "jsonValue.h"     // for value
#include "json_token.h"    // for operator<<, operator==, NT, NonterminalID, T
#include <array>           // for array
#include <cstddef>         // for size_t
#include <cstdint>         // for uint32_t
#include <list>            // for list
#include "Regex/lrParser.h"      // for LRParser

namespace jsonLR {
struct Parser : public lr_parser::LRParser<Grammar, json::value*> {
    using T = json::T;
    using NT = json::NT;
    using Token = json::Token;
    size_t next_value_index = 10000U; // this ensures that we will initilize a new "page" with the next new_value call
    std::list<std::array<json::value, 10000>> values;
    json::value* new_value();
    using Base = LRParser<Grammar, json::value*>;
    using Base::Base;

    // simplify assembly (of includers of this file) by hiding these in the compilation unit
    Parser();
    ~Parser();

    json::value* reduce(json::NonterminalID A, uint32_t production, json::value** alpha, size_t); // override;

    json::value* shift(Token&& token); // override;
};
} // namespace jsonLR
