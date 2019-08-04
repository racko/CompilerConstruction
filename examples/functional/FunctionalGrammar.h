#ifndef FUNCTIONAL_H_
#define FUNCTIONAL_H_

#include "Grammar.h"

#include <cstdint>
#include <vector>

#include <boost/dynamic_bitset.hpp>
#include <iostream>

namespace Functional {
using type = uint32_t;

enum class T : type {
    SEMICOLON,
    ASSIGN,
    PERIOD,
    LPAREN,
    RPAREN,
    PLUS,
    MINUS,
    TIMES,
    DIV,
    LT,
    GT,
    AND,
    OR,
    BSL,
    LE,
    EQ,
    NE,
    GE,
    LET,
    IN,
    LETREC,
    VAR,
    NUM,
    WS,
    EOI,
    EPS
};

enum class NT : type { START, P, S, D, C, A, B, L };

extern const char* nt_strings[];
extern const char* t_strings[];

constexpr type getNumberOfTerminals() { return type(T::EPS) + 1; }

constexpr type getNumberOfNonterminals() { return type(NT::L) + 1; }

constexpr type getNumberOfGrammarElements() { return getNumberOfTerminals() + getNumberOfNonterminals(); }

struct Token;
template <class T>
struct TokenBase;
struct Keyword;
struct BinOp;
struct Num;
struct Other;
struct Var;
struct Whitespace;

struct TerminalID {
    Functional::type x;

    constexpr TerminalID(T _x) : x(type(_x)) {}

    explicit TerminalID(type _x) : x(_x) { assert(_x < getNumberOfTerminals()); }

    //  operator GrammarElement() const;

    operator T() const { return T(x); }
};

struct NonterminalID {
    type x;

    constexpr NonterminalID(NT _x) : x(type(_x)) {}

    explicit NonterminalID(type _x) : x(_x) { assert(_x < getNumberOfNonterminals()); }

    //  operator GrammarElement() const;

    operator NT() const { return NT(x); }
};

struct GrammarElement {
    type x;
    explicit GrammarElement(type _x) : x(_x) { assert(_x < getNumberOfGrammarElements()); }
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

} // namespace Functional

namespace std {
template <>
struct hash<Functional::TerminalID> {
  public:
    size_t operator()(const Functional::TerminalID a) const { return std::hash<decltype(a.x)>()(a.x); }
};

template <>
struct hash<Functional::NonterminalID> {
  public:
    size_t operator()(const Functional::NonterminalID A) const { return std::hash<decltype(A.x)>()(A.x); }
};

template <>
struct hash<Functional::GrammarElement> {
  public:
    size_t operator()(const Functional::GrammarElement X) const { return std::hash<decltype(X.x)>()(X.x); }
};
} // namespace std

namespace Functional {
std::ostream& operator<<(std::ostream& s, const std::vector<GrammarElement>& alpha);
std::ostream& operator<<(std::ostream& s, const std::list<GrammarElement>& alpha);
std::ostream& operator<<(std::ostream& s, const std::unordered_set<TerminalID>& alpha);

struct Grammar {
    using type = Functional::type;
    using TerminalID = Functional::TerminalID;
    using NonterminalID = Functional::NonterminalID;
    using GrammarElement = Functional::GrammarElement;

    // order: true terminals, EOF, EPS, nonterminals. S' is just the starting symbol

    using String = std::vector<GrammarElement>;

    static kind kindOf(GrammarElement X) { return Functional::kindOf(X); }

    static type getNumberOfTerminals() { return numberOfTerminals; }

    static type getNumberOfNonterminals() { return numberOfNonterminals; }

    static type getNumberOfGrammarElements() { return numberOfTerminals + numberOfNonterminals; }

    static const std::unordered_map<NonterminalID, std::vector<String>>& getProductions() { return productions; }

    static const std::vector<String>& getProductions(NonterminalID A);

    static const std::unordered_map<GrammarElement, std::unordered_set<TerminalID>>& getFirsts() { return firsts; }

    static const std::unordered_set<TerminalID>& getFirsts(const GrammarElement& X);

    // using Token_const_reference = const Functional::Token*;
    using Token = std::unique_ptr<Token>;

    static TerminalID getTag(const Token& x);

    static const NonterminalID start;
    static const TerminalID eof;
    static const TerminalID eps;
    static const type numberOfNonterminals;
    static const type numberOfTerminals;
    static const std::unordered_map<NonterminalID, std::vector<String>> productions;
    static const std::unordered_map<GrammarElement, std::unordered_set<TerminalID>> firsts;
};
} // namespace Functional

using std::array;
using std::cerr;
using std::cout;
using std::endl;
using std::exception;
using std::ostream;
using std::string;

namespace Functional {
struct TokenVisitor {
    virtual void visit(Num&) = 0;
    virtual void visit(Var&) = 0;
    virtual void visit(Token&) = 0;
};

struct TokenConstVisitor {
    virtual void visit(const Num&) = 0;
    virtual void visit(const Var&) = 0;
    virtual void visit(const Token&) = 0;
};

struct Token {
    TerminalID tag;
    Token(TerminalID t) : tag(t) {}
    virtual void accept(TokenVisitor& v);
    virtual void accept(TokenConstVisitor& v) const;
    virtual ~Token() {}
};

template <typename T>
struct TokenBase : public Token {
    using Token::Token;
    void accept(TokenVisitor& v) override { v.visit(*static_cast<T*>(this)); }
    void accept(TokenConstVisitor& v) const override { v.visit(*static_cast<const T*>(this)); }
};

struct Num : public TokenBase<Num> {
    uint64_t n;
    Num(unsigned long long n_) : TokenBase<Num>(T::NUM), n(n_) {}
};

struct Var : public TokenBase<Var> {
    std::string s;
    Var(string s_) : TokenBase<Var>(T::VAR), s(std::move(s_)) {}
};

inline TerminalID Grammar::getTag(const Token& x) { return x->tag; }

std::ostream& operator<<(std::ostream& s, TerminalID const& a);

std::ostream& operator<<(std::ostream& s, NonterminalID const& A);

inline std::ostream& operator<<(std::ostream& s, T const& a) { return s << t_strings[type(a)]; }

inline std::ostream& operator<<(std::ostream& s, NT const& A) { return s << nt_strings[type(A)]; }
} // namespace Functional

#endif /* FUNCTIONAL_H_ */
