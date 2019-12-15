#pragma once

#include <cassert>     // for assert
#include <cstddef>     // for ptrdiff_t
#include <limits>      // for numeric_limits
#include "Regex/lrParser2.h" // for LRParser, type, type::ACCEPT, type::REDUCE
#include "Regex/parser.h"    // for Parser
#include <vector>      // for vector

namespace grammar2 {
class Grammar;
}
template <typename TokenType>
class TokenStream;

namespace lr_parser2 {
template <typename Attributes>
class AttributedLRParser : public Parser<typename Attributes::type, typename Attributes::Token> {
  public:
    AttributedLRParser(const grammar2::Grammar& grammar, Attributes a);

    typename Attributes::type parse(TokenStream<typename Attributes::Token>& a) override;

  private:
    LRParser parser_;
    Attributes attributes_;
};

template <typename Attributes>
AttributedLRParser<Attributes>::AttributedLRParser(const grammar2::Grammar& grammar, Attributes a)
    : parser_(grammar), attributes_(std::move(a)) {}

template <typename Attributes>
typename Attributes::type AttributedLRParser<Attributes>::parse(TokenStream<typename Attributes::Token>& a) {
    using T = typename Attributes::type;
    parser_.reset();
    std::vector<T> attributes{T()}; // Do we need this initialization?
    do {
        auto tag = attributes_.getTag(a.peek());
        auto _action = parser_.step(tag); // the type name is action already, sooo ...
        if (_action.getType() == type::SHIFT) {
            attributes.push_back(attributes_.shift(std::move(a.peek())));
            a.step();
        } else if (_action.getType() == type::REDUCE) {
            auto p = _action.getProduction();
            auto head = _action.getHead();
            const auto& production = parser_.grammar().getProductions(head)[p];
            ptrdiff_t length;
            if (production.at(0) == parser_.grammar().toGrammarElement(parser_.grammar().getEps()))
                length = 0;
            else {
                assert(production.size() <= std::numeric_limits<ptrdiff_t>::max());
                length = static_cast<ptrdiff_t>(production.size());
            }
            assert(attributes.size() > length);
            auto synthesized_attribute = attributes_.reduce(head, p, &*(attributes.end() - length), length);
            attributes.erase(attributes.end() - length, attributes.end());
            attributes.push_back(std::move(synthesized_attribute));
        } else if (_action.getType() == type::ACCEPT) {
            return attributes.back();
        } else {
            return T();
        }
    } while (true);
}
} // namespace lr_parser2
