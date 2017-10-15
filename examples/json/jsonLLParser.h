#pragma once
#include "llParser.h"
#include "jsonLLGrammar.h"
#include "jsonValue.h"

#include <array>
#include <list>

namespace jsonLL {
struct Parser : public LLParser<Grammar, value*> {
    size_t next_value_index = 10000U; // this ensures that we will initilize a new "page" with the next new_value call
    std::list<std::array<value,10000>> values;
    value* new_value();
    using Base = LLParser<Grammar, value*>;
    using Base::Base;

    // simplify assembly (of includers of this file) by hiding these in the compilation unit
    Parser();
    ~Parser();
};
}
