#pragma once

#include "Regex/Grammar.h"           // for kind
#include "Regex/variant.h"           // for null
#include <assert.h>                  // for assert
#include <boost/variant/variant.hpp> // for variant
#include <cstdint>                   // for uint8_t
#include <functional>                // for hash
#include <iosfwd>                    // for ostream, size_t
#include <list>                      // for list
#include <num_view.h>
#include <string_view>   // for string_view
#include <unordered_set> // for unordered_set
#include <utility>       // for pair, pair<>::first, pair<>::pair
#include <vector>        // for vector

namespace json {
using type = std::uint8_t;

enum class T : type {
    BEGIN_ARRAY,
    BEGIN_OBJECT,
    END_ARRAY,
    END_OBJECT,
    NAME_SEPARATOR,
    VALUE_SEPARATOR,
    FALSE,
    NIL,
    TRUE,
    NUM,
    STRING,
    WS,
    EOI,
    EPS
};

enum class NT : type { START, JSON_TEXT, VALUE, OBJECT, MEMBER, MEMBERS, ARRAY, VALUES };

static constexpr type numberOfNonterminals = type(NT::VALUES) + 1;
static constexpr type numberOfTerminals = type(T::EPS) + 1;

inline type getNumberOfTerminals() { return numberOfTerminals; }

inline type getNumberOfNonterminals() { return numberOfNonterminals; }

inline type getNumberOfGrammarElements() { return static_cast<type>(numberOfTerminals + numberOfNonterminals); }

extern const char* const nt_strings[];
extern const char* const t_strings[];

std::ostream& operator<<(std::ostream& s, T const& a);

std::ostream& operator<<(std::ostream& s, NT const& A);

struct TerminalID {
    json::type x;

    constexpr TerminalID(json::T _x) : x(json::type(_x)) {}

    explicit TerminalID(json::type _x) : x(_x) { assert(_x < json::getNumberOfTerminals()); }

    operator json::T() const { return json::T(x); }
};

struct NonterminalID {
    json::type x;

    constexpr NonterminalID(json::NT _x) : x(json::type(_x)) {}

    explicit NonterminalID(json::type _x) : x(_x) { assert(_x < json::getNumberOfNonterminals()); }

    operator json::NT() const { return json::NT(x); }
};

struct GrammarElement {
    json::type x;
    explicit GrammarElement(json::type _x) : x(_x) { assert(_x < json::getNumberOfGrammarElements()); }
    GrammarElement(json::T _x) : x(TerminalID(_x).x) {}
    GrammarElement(json::NT _x) : x(static_cast<json::type>(NonterminalID(_x).x + json::numberOfTerminals)) {}
    GrammarElement(TerminalID _x) : x(_x.x) {}
    GrammarElement(NonterminalID _x) : x(static_cast<json::type>(_x.x + json::numberOfTerminals)) {}
    operator TerminalID() const { return TerminalID(x); }

    operator NonterminalID() const { return NonterminalID(static_cast<json::type>(x - json::numberOfTerminals)); }
};

kind kindOf(GrammarElement X);

inline bool operator==(const TerminalID lhs, const TerminalID rhs) { return lhs.x == rhs.x; }

inline bool operator!=(const TerminalID lhs, const TerminalID rhs) { return !(lhs == rhs); }

inline bool operator<(const TerminalID lhs, const TerminalID rhs) { return lhs.x < rhs.x; }

inline bool operator==(const GrammarElement lhs, const GrammarElement rhs) { return lhs.x == rhs.x; }

inline bool operator!=(const GrammarElement lhs, const GrammarElement rhs) { return !(lhs == rhs); }

inline bool operator<(const GrammarElement lhs, const GrammarElement rhs) { return lhs.x < rhs.x; }

inline bool operator==(const NonterminalID lhs, const NonterminalID rhs) { return lhs.x == rhs.x; }

inline bool operator!=(const NonterminalID lhs, const NonterminalID rhs) { return !(lhs == rhs); }

inline bool operator<(const NonterminalID lhs, const NonterminalID rhs) { return lhs.x < rhs.x; }

std::ostream& operator<<(std::ostream& s, GrammarElement const X);
} // namespace json

namespace std {
template <>
struct hash<json::TerminalID> {
  public:
    size_t operator()(const json::TerminalID a) const { return std::hash<decltype(a.x)>()(a.x); }
};

template <>
struct hash<json::NonterminalID> {
  public:
    size_t operator()(const json::NonterminalID A) const { return std::hash<decltype(A.x)>()(A.x); }
};

template <>
struct hash<json::GrammarElement> {
  public:
    size_t operator()(const json::GrammarElement X) const { return std::hash<decltype(X.x)>()(X.x); }
};
} // namespace std

namespace json {
std::ostream& operator<<(std::ostream& s, const std::vector<GrammarElement>& alpha);

std::ostream& operator<<(std::ostream& s, const std::list<GrammarElement>& alpha);

std::ostream& operator<<(std::ostream& s, const std::unordered_set<TerminalID>& alpha);

using Token_ = std::pair<TerminalID, variant<null, bool, num_view, std::string_view>>;
struct Token : private Token_ {
    using Token_::first;
    using Token_::second;
    using Token_::Token_;

    Token() : Token_(TerminalID(0U), {}) {}
};

std::ostream& operator<<(std::ostream& s, TerminalID const& a);

std::ostream& operator<<(std::ostream& s, NonterminalID const& A);

} // namespace json
