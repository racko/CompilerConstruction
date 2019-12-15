#pragma once

#include <memory>
#include "Regex/parser.h"

namespace json {
struct value;
struct Token;
struct myLexer;

/// creating myLexer and Parser is expensive, so creating them in every run call isn't an option.
/// That's why we are not offering a free function.
/// (Furthermore, currently the parser only returns a non-owning view to it's internal data, so we
/// would neet to return the parser with the value)
struct JsonParser {
    using Parser = ::Parser<json::value*,json::Token>;
    JsonParser();
    JsonParser(std::unique_ptr<Parser> parser);
    ~JsonParser();

    json::value* run(const char* text);

    std::unique_ptr<json::myLexer> lex_;
    std::unique_ptr<Parser> parser_;
};
} // namespace json
