#pragma once

#include <array>
#include <attributed_lrparser.h>
#include <jsonLRGrammar2.h>
#include <jsonValue.h>
#include <json_token.h>
#include <list>

namespace jsonLR2 {
struct Attributes {
    using T = json::T;
    using NT = json::NT;
    using TerminalID = grammar2::TerminalID;
    using NonterminalID = grammar2::NonterminalID;
    using Token = json::Token;
    using type = json::value*;
    size_t next_value_index = 10000U; // this ensures that we will initilize a new "page" with the next new_value call
    std::list<std::array<json::value, 10000>> values;
    json::value* new_value();

    // simplify assembly (of includers of this file) by hiding these in the compilation unit
    // Attributes();
    //~Attributes();

    json::value* reduce(NonterminalID A, uint32_t production, json::value** alpha, size_t);

    json::value* shift(Token&& token);
    static TerminalID getTag(const Token& token) { return TerminalID{token.first.x}; }
};

class Parser : public lr_parser2::AttributedLRParser<Attributes> {
  public:
    using Base = lr_parser2::AttributedLRParser<Attributes>;
    using Base::parse;

    Parser() : Base(grammar, Attributes()) {}

  private:
};
} // namespace jsonLR2
