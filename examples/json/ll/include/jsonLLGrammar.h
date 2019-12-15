#pragma once

#include "Regex/Grammar.h" // for kind
#include <cstddef>         // for size_t
#include <json_token.h>    // for kindOf, GrammarElement, NT, NT::VALUES, Non...
#include <limits>          // for numeric_limits
#include <unordered_map>   // for unordered_map
#include <unordered_set>   // for unordered_set
#include <vector>          // for vector

namespace jsonLL {
struct Grammar {
    using type = json::type;
    using TerminalID = json::TerminalID;
    using NonterminalID = json::NonterminalID;
    using GrammarElement = json::GrammarElement;

    // order: true terminals, EOF, EPS, nonterminals. S' is just the starting symbol

    using String = std::vector<GrammarElement>;

    static kind kindOf(GrammarElement X) { return json::kindOf(X); }

    static type getNumberOfTerminals() { return numberOfTerminals; }

    static type getNumberOfNonterminals() { return numberOfNonterminals; }

    static type getNumberOfGrammarElements() { return static_cast<type>(numberOfTerminals + numberOfNonterminals); }

    // static const std::unordered_map<NonterminalID, std::vector<String>>& getProductions();
    static const std::vector<std::vector<String>>& getProductions() { return productions; }

    static const std::vector<String>& getProductions(NonterminalID A);

    static const std::unordered_map<GrammarElement, std::unordered_set<TerminalID>>& getFirsts() { return firsts; }

    static const std::unordered_set<TerminalID>& getFirsts(const GrammarElement& X);

    static const std::unordered_map<NonterminalID, std::unordered_set<TerminalID>>& getFollows() { return follows; }

    static const std::unordered_set<TerminalID>& getFollows(const NonterminalID& X);

    using Token = json::Token;

    // static TerminalID getTag(const Token* x);
    static TerminalID getTag(const Token& x);

    static const NonterminalID start;
    static const TerminalID eof;
    static const TerminalID eps;
    static constexpr type numberOfNonterminals = type(json::NT::VALUES) + 1;
    static constexpr type numberOfTerminals = type(json::T::EPS) + 1;
    static_assert(size_t(numberOfTerminals) + size_t(numberOfNonterminals) <= size_t(std::numeric_limits<type>::max()));
    // static const std::unordered_map<NonterminalID, std::vector<String>> productions;
    static const std::vector<std::vector<String>> productions;
    static const std::unordered_map<GrammarElement, std::unordered_set<TerminalID>> firsts;
    static const std::unordered_map<NonterminalID, std::unordered_set<TerminalID>> follows;
};

inline auto Grammar::getTag(const Token& x) -> TerminalID { return x.first; }
} // namespace jsonLL
