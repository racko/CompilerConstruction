#pragma once

#include "Grammar.h"

#include <cstdint>
#include <vector>

#include <boost/dynamic_bitset.hpp>

namespace While {
enum class T : uint32_t {
    DEF,
    LEFTCURLY,
    RIGHTCURLY,
    SEMICOLON,
    BASIC,
    RECORD,
    IF,
    ELSE,
    WHILE,
    DO,
    BREAK,
    CONTINUE,
    RETURN,
    LEFTBR,
    RIGHTBR,
    ID,
    PERIOD,
    ASSIGN,
    OR,
    AND,
    EQ,
    NEQ,
    LT,
    LE,
    GE,
    GT,
    PLUS,
    MINUS,
    TIMES,
    DIV,
    NOT,
    LEFTPAR,
    RIGHTPAR,
    NUM,
    REAL,
    TRUE,
    FALSE,
    STRING,
    COMMA,
    WS,
    EOI,
    EPS
};

enum class NT : uint32_t {
    S,
    PROGRAM,
    FUNCS,
    FUNC,
    OPTPARAMS,
    PARAMS,
    BLOCK,
    DECLS,
    DECL,
    TYPE,
    STMTS,
    STMT,
    LOC,
    ASSIGN,
    BOOL,
    JOIN,
    EQ,
    REL,
    EXPR,
    TERM,
    UNARY,
    FACTOR,
    FUNCALL,
    OPTARGS,
    ARGS
};

constexpr size_t getNumberOfTerminals() { return uint32_t(T::EPS) + 1; }

constexpr size_t getNumberOfNonterminals() { return uint32_t(NT::ARGS) + 1; }

constexpr size_t getNumberOfGrammarElements() { return getNumberOfTerminals() + getNumberOfNonterminals(); }

extern const char* nt_strings[];
extern const char* t_strings[];

struct Token;
struct Basic;
struct Num;
struct Real;
struct ID;
struct String;

struct TerminalID {
    uint32_t x;

    constexpr TerminalID(T _x) : x(uint32_t(_x)) {}

    explicit TerminalID(uint32_t _x) : x(_x) { assert(_x < getNumberOfTerminals()); }

    //  operator GrammarElement() const;

    operator T() const { return T(x); }
};

struct NonterminalID {
    uint32_t x;

    constexpr NonterminalID(NT _x) : x(uint32_t(_x)) {}

    explicit NonterminalID(uint32_t _x) : x(_x) { assert(_x < getNumberOfNonterminals()); }

    //  operator GrammarElement() const;

    operator NT() const { return NT(x); }
};

struct GrammarElement {
    uint32_t x;
    explicit GrammarElement(uint32_t _x) : x(_x) { assert(_x < getNumberOfGrammarElements()); }
    GrammarElement(T _x) : x(TerminalID(_x).x) {}
    GrammarElement(NT _x) : x(NonterminalID(_x).x + getNumberOfTerminals()) {}
    GrammarElement(TerminalID _x) : x(_x.x) {}
    GrammarElement(NonterminalID _x) : x(_x.x + getNumberOfTerminals()) {}
    operator TerminalID() const { return TerminalID(x); }

    operator NonterminalID() const { return NonterminalID(x - getNumberOfTerminals()); }
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

} // namespace While

namespace std {
template <>
struct hash<While::TerminalID> {
  public:
    size_t operator()(const While::TerminalID a) const { return std::hash<decltype(a.x)>()(a.x); }
};

template <>
struct hash<While::NonterminalID> {
  public:
    size_t operator()(const While::NonterminalID A) const { return std::hash<decltype(A.x)>()(A.x); }
};

template <>
struct hash<While::GrammarElement> {
  public:
    size_t operator()(const While::GrammarElement X) const { return std::hash<decltype(X.x)>()(X.x); }
};
} // namespace std

namespace While {
std::ostream& operator<<(std::ostream& s, const std::vector<GrammarElement>& alpha);
std::ostream& operator<<(std::ostream& s, const std::list<GrammarElement>& alpha);
std::ostream& operator<<(std::ostream& s, const std::unordered_set<TerminalID>& alpha);

struct Grammar {
    using type = std::uint32_t; // TODO: actually use this typedef
    using TerminalID = While::TerminalID;
    using NonterminalID = While::NonterminalID;
    using GrammarElement = While::GrammarElement;

    // order: true terminals, EOF, EPS, nonterminals. S' is just the starting symbol

    using String = std::vector<GrammarElement>;

    static kind kindOf(GrammarElement X) { return While::kindOf(X); }

    static size_t getNumberOfTerminals() { return numberOfTerminals; }

    static size_t getNumberOfNonterminals() { return numberOfNonterminals; }

    static size_t getNumberOfGrammarElements() { return numberOfTerminals + numberOfNonterminals; }

    static const std::unordered_map<NonterminalID, std::vector<String>>& getProductions() { return productions; }

    static const std::vector<String>& getProductions(NonterminalID A);

    static const std::unordered_map<GrammarElement, std::unordered_set<TerminalID>>& getFirsts() { return firsts; }

    static const std::unordered_set<TerminalID>& getFirsts(const GrammarElement& X);

    // using Token_const_reference = const Token*;
    using Token = std::unique_ptr<Token>;

    //  static TerminalID getTag(const Token& x);
    static TerminalID getTag(const Token& x);

    static const NonterminalID start;
    static const TerminalID eof;
    static const TerminalID eps;
    static const size_t numberOfNonterminals;
    static const size_t numberOfTerminals;
    static const std::unordered_map<NonterminalID, std::vector<String>> productions;
    static const std::unordered_map<GrammarElement, std::unordered_set<TerminalID>> firsts;
};

struct Token {
    TerminalID tag;

    Token(TerminalID _tag) : tag(_tag) {}
    virtual ~Token() {}
};

struct Basic : public Token {
    std::string value;
    Basic(const std::string& _value) : Token(TerminalID(T::BASIC)), value(_value) {}
};

struct Num : public Token {
    uint64_t value;
    Num(uint64_t _value) : Token(TerminalID(T::NUM)), value(_value) {}
};

struct Real : public Token {
    double value;
    Real(double _value) : Token(TerminalID(T::REAL)), value(_value) {}
};

struct ID : public Token {
    std::string value;
    ID(const std::string& _value) : Token(TerminalID(T::ID)), value(_value) {}
};

struct String : public Token {
    std::string value;
    String(const std::string& _value) : Token(TerminalID(T::STRING)), value(_value) {}
};

// inline TerminalID Grammar::getTag(const Token& x) {
//  return x.tag;
//}

inline TerminalID Grammar::getTag(const Grammar::Token& x) { return x->tag; }

// inline TerminalID::operator GrammarElement() const {
//  return GrammarElement(x);
//}
//
// inline NonterminalID::operator GrammarElement() const {
//  return GrammarElement(x + Grammar::numberOfTerminals);
//}

inline std::ostream& operator<<(std::ostream& s, T const& a) { return s << t_strings[uint32_t(a)]; }

inline std::ostream& operator<<(std::ostream& s, NT const& A) { return s << nt_strings[uint32_t(A)]; }

std::ostream& operator<<(std::ostream& s, TerminalID const& a);

std::ostream& operator<<(std::ostream& s, NonterminalID const& A);
} // namespace While
