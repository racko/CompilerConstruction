#pragma once

#include <cassert>
#include <ostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace grammar2 {

enum class kind { TERMINAL, NONTERMINAL, EPS, EOI };

struct TerminalID {
    uint32_t x;
    explicit constexpr TerminalID(uint32_t _x) : x(_x) {}
};

struct NonterminalID {
    uint32_t x;
    explicit constexpr NonterminalID(uint32_t _x) : x(_x) {}
};

struct GrammarElement {
    uint32_t x;
    explicit constexpr GrammarElement(uint32_t _x) : x(_x) {}
};

inline bool operator==(const TerminalID lhs, const TerminalID rhs) { return lhs.x == rhs.x; }

inline bool operator!=(const TerminalID lhs, const TerminalID rhs) { return !(lhs == rhs); }

inline bool operator<(const TerminalID lhs, const TerminalID rhs) { return lhs.x < rhs.x; }

inline bool operator>(const TerminalID lhs, const TerminalID rhs) { return lhs.x > rhs.x; }

inline bool operator==(const GrammarElement lhs, const GrammarElement rhs) { return lhs.x == rhs.x; }

inline bool operator!=(const GrammarElement lhs, const GrammarElement rhs) { return !(lhs == rhs); }

inline bool operator<(const GrammarElement lhs, const GrammarElement rhs) { return lhs.x < rhs.x; }

inline bool operator>(const GrammarElement lhs, const GrammarElement rhs) { return lhs.x > rhs.x; }

inline bool operator==(const NonterminalID lhs, const NonterminalID rhs) { return lhs.x == rhs.x; }

inline bool operator!=(const NonterminalID lhs, const NonterminalID rhs) { return !(lhs == rhs); }

inline bool operator<(const NonterminalID lhs, const NonterminalID rhs) { return lhs.x < rhs.x; }

inline bool operator>(const NonterminalID lhs, const NonterminalID rhs) { return lhs.x > rhs.x; }
} // namespace grammar2

namespace std {
template <>
struct hash<grammar2::TerminalID> {
  public:
    size_t operator()(const grammar2::TerminalID a) const { return std::hash<decltype(a.x)>()(a.x); }
};

template <>
struct hash<grammar2::NonterminalID> {
  public:
    size_t operator()(const grammar2::NonterminalID A) const { return std::hash<decltype(A.x)>()(A.x); }
};

template <>
struct hash<grammar2::GrammarElement> {
  public:
    size_t operator()(const grammar2::GrammarElement X) const { return std::hash<decltype(X.x)>()(X.x); }
};
} // namespace std

namespace grammar2 {
class Grammar {
  public:
    using String = std::vector<GrammarElement>;
    using Production = std::vector<String>;
    using Productions = std::vector<Production>;

  private:
    uint32_t numberOfTerminals_;
    uint32_t numberOfNonterminals_;
    NonterminalID start_;
    TerminalID eps_;
    TerminalID eoi_;
    std::vector<const char*> terminalStrings_;
    std::vector<const char*> nonTerminalStrings_;
    Productions productions_;
    std::unordered_map<GrammarElement, std::unordered_set<TerminalID>> firsts_;

  public:
    Grammar(uint32_t numberOfTerminals, uint32_t numberOfNonterminals, NonterminalID start, TerminalID eps,
            TerminalID eoi, std::vector<const char*> terminalStrings, std::vector<const char*> nonTerminalStrings,
            Productions productions);
    uint32_t getNumberOfTerminals() const { return numberOfTerminals_; }
    uint32_t getNumberOfNonterminals() const { return numberOfNonterminals_; }
    NonterminalID getStart() const { return start_; }
    TerminalID getEps() const { return eps_; }
    TerminalID getEoi() const { return eoi_; }
    const Productions& getProductions() const { return productions_; }
    const std::vector<String>& getProductions(NonterminalID A) const { return productions_[A.x]; }

    const std::unordered_map<GrammarElement, std::unordered_set<TerminalID>>& getFirsts() const { return firsts_; }

    const std::unordered_set<TerminalID>& getFirsts(const GrammarElement X) const { return firsts_.at(X); }

    size_t getNumberOfGrammarElements() const { return getNumberOfTerminals() + getNumberOfNonterminals(); }

    GrammarElement toGrammarElement(TerminalID t) const { return GrammarElement(t.x); }

    GrammarElement toGrammarElement(NonterminalID A) const { return GrammarElement(A.x + getNumberOfTerminals()); }

    TerminalID toTerminalID(GrammarElement X) const {
        assert(kindOf(X) == kind::TERMINAL ||
               kindOf(X) ==
                   kind::EOI); // TODO Is this really what we want? Or should we have a "toTerminalID_or_EOI" function?
        return TerminalID(X.x);
    }

    NonterminalID toNonterminalID(GrammarElement X) const {
        assert(kindOf(X) == kind::NONTERMINAL);
        return NonterminalID(X.x - getNumberOfTerminals());
    }

    kind kindOf(GrammarElement X) const {
        if (X == toGrammarElement(eps_))
            return kind::EPS;
        else if (X == toGrammarElement(eoi_))
            return kind::EOI;
        else if (X.x < getNumberOfTerminals())
            return kind::TERMINAL;
        else if (X.x < getNumberOfGrammarElements())
            return kind::NONTERMINAL;
        else
            throw std::logic_error("kindOf: invalid GrammarElement");
    }

    const char* toString(TerminalID t) const { return terminalStrings_[t.x]; }
    const char* toString(NonterminalID A) const { return nonTerminalStrings_[A.x]; }
    const char* toString(GrammarElement X) const {
        switch (kindOf(X)) {
        case kind::TERMINAL:
            return toString(toTerminalID(X));
        case kind::EPS:
            return "EPS";
        case kind::EOI:
            return "EOF";
        case kind::NONTERMINAL:
            return toString(toNonterminalID(X));
        default:
            throw std::logic_error("unhandled case in toString(GrammarElement)");
        }
    }
};

template <typename Iterator>
void print(std::ostream& s, const Grammar& grammar, Iterator it, const Iterator stop) {
    if (it == stop)
        return;
    s << grammar.toString(*it);
    ++it;
    for (; it != stop; ++it) {
        s << ' ' << grammar.toString(*it);
    }
}

template <typename Range>
void print(std::ostream& s, const Grammar& grammar, const Range& r) {
    print(s, grammar, std::begin(r), std::end(r));
}

void first(TerminalID eps, const std::vector<GrammarElement>& alpha,
           const std::unordered_map<GrammarElement, std::unordered_set<TerminalID>>& fs,
           std::unordered_set<TerminalID>& out);

bool updateFirsts(const Grammar& grammar, std::unordered_map<GrammarElement, std::unordered_set<TerminalID>>& fs);

std::unordered_map<GrammarElement, std::unordered_set<TerminalID>> allFirsts(const Grammar& grammar);

std::unordered_set<TerminalID> first(const Grammar& grammar, const std::vector<GrammarElement>& alpha);
} // namespace grammar2
