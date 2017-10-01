#pragma once

#include "jsonGrammar.h"

#include <memory>

struct value;
struct myLexer;
struct Parser;

/// creating myLexer and Parser is expensive, so creating them in every run call isn't an option.
/// That's why we are not offering a free function. 
/// (Furthermore, currently the parser only returns a non-owning view to it's internal data, so we
/// would neet to return the parser with the value)
struct JsonParser {
    JsonParser();
    ~JsonParser();

    value* run(const char* text);

    std::unique_ptr<myLexer> lex;
    std::unique_ptr<Parser> parser;
};

