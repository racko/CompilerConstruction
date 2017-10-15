#ifndef WHILE_H_
#define WHILE_H_

#include "Grammar.h"

#include <cstdint>
#include <vector>

#include <boost/dynamic_bitset.hpp>

namespace While {
struct Grammar;
enum class T : uint32_t;
enum class NT : uint32_t;
}

template<>
struct NonterminalID<While::Grammar>;

template<>
struct TerminalID<While::Grammar>;

template<>
struct GrammarElement<While::Grammar>;

namespace While {
enum class T : uint32_t {
    DEF, LEFTCURLY, RIGHTCURLY, SEMICOLON, BASIC, RECORD, IF, ELSE, WHILE, DO, BREAK, CONTINUE, RETURN, LEFTBR, RIGHTBR, ID, PERIOD, ASSIGN, OR, AND, EQ, NEQ, LT, LE, GE, GT, PLUS, MINUS, TIMES, DIV, NOT, LEFTPAR, RIGHTPAR, NUM, REAL, TRUE, FALSE, STRING, COMMA, WS, EOI,
    EPS
};

enum class NT : uint32_t {
    S, PROGRAM, FUNCS, FUNC, OPTPARAMS, PARAMS, BLOCK, DECLS, DECL, TYPE, STMTS, STMT, LOC, ASSIGN, BOOL, JOIN, EQ, REL, EXPR, TERM, UNARY, FACTOR, FUNCALL, OPTARGS, ARGS
};

extern const char* nt_strings[];
extern const char* t_strings[];

struct Token;
struct Basic;
struct Num;
struct Real;
struct ID;
struct String;

struct Grammar {
    using type = std::uint32_t; // TODO: actually use this typedef

    // order: true terminals, EOF, EPS, nonterminals. S' is just the starting symbol

    using String = std::vector<GrammarElement<Grammar>>;

    static kind kindOf(GrammarElement<Grammar> X);

    static size_t getNumberOfTerminals() {
        return numberOfTerminals;
    }

    static size_t getNumberOfNonterminals() {
        return numberOfNonterminals;
    }

    static size_t getNumberOfGrammarElements() {
        return numberOfTerminals + numberOfNonterminals;
    }

    static const std::unordered_map<NonterminalID<Grammar>, std::vector<String>>& getProductions() {
        return productions;
    }

    static const std::vector<String>& getProductions(NonterminalID<Grammar> A);

    static const std::unordered_map<GrammarElement<Grammar>, std::unordered_set<TerminalID<Grammar>>>& getFirsts() {
        return firsts;
    }

    static const std::unordered_set<TerminalID<Grammar>>& getFirsts(const GrammarElement<Grammar>& X) ;


    //using Token_const_reference = const While::Token*;
    using Token_rv_reference = const Token*;

    //  static TerminalID<Grammar> getTag(const While::Token& x);
    static TerminalID<Grammar> getTag(const While::Token* x);

    static const NonterminalID<Grammar> start;
    static const TerminalID<Grammar> eof;
    static const TerminalID<Grammar> eps;
    static const size_t numberOfNonterminals;
    static const size_t numberOfTerminals;
    static const std::unordered_map<NonterminalID<Grammar>, std::vector<String>> productions;
    static const std::unordered_map<GrammarElement<Grammar>, std::unordered_set<TerminalID<Grammar>>> firsts;

};
}

template<>
struct TerminalID<While::Grammar> {
    uint32_t x;

    constexpr TerminalID(While::T _x) : x(uint32_t(_x)) {}

    explicit TerminalID(uint32_t _x) : x(_x) {
        assert(_x < While::Grammar::getNumberOfTerminals());
    }

    //  operator GrammarElement<While::Grammar>() const;

    operator While::T() const {
        return While::T(x);
    }
};

template<>
struct NonterminalID<While::Grammar> {
    uint32_t x;

    constexpr NonterminalID(While::NT _x) : x(uint32_t(_x)) {}

    explicit NonterminalID(uint32_t _x) : x(_x) {
        assert(_x < While::Grammar::getNumberOfNonterminals());
    }

    //  operator GrammarElement<While::Grammar>() const;

    operator While::NT() const {
        return While::NT(x);
    }
};

template<>
struct GrammarElement<While::Grammar> {
    uint32_t x;
    explicit GrammarElement(uint32_t _x) : x(_x) {
        assert(_x < While::Grammar::getNumberOfGrammarElements());
    }
    GrammarElement(While::T _x) : x(TerminalID<While::Grammar>(_x).x) {}
    GrammarElement(While::NT _x) : x(NonterminalID<While::Grammar>(_x).x + While::Grammar::numberOfTerminals) {}
    GrammarElement(TerminalID<While::Grammar> _x) : x(_x.x) {}
    GrammarElement(NonterminalID<While::Grammar> _x) : x(_x.x + While::Grammar::numberOfTerminals) {}
    operator TerminalID<While::Grammar>() const {
        return TerminalID<While::Grammar>(x);
    }

    operator NonterminalID<While::Grammar>() const {
        return NonterminalID<While::Grammar>(x - While::Grammar::numberOfTerminals);
    }
};

struct While::Token {
    TerminalID<Grammar> tag;

    Token(TerminalID<Grammar> _tag) : tag(_tag) {}
    virtual ~Token() {}
};

struct While::Basic : public While::Token {
    std::string value;
    Basic(const std::string& _value) : While::Token(TerminalID<Grammar>(T::BASIC)), value(_value) {}
};

struct While::Num : public While::Token {
    uint64_t value;
    Num(uint64_t _value) : While::Token(TerminalID<Grammar>(T::NUM)),value(_value) {}
};

struct While::Real : public While::Token {
    double value;
    Real(double _value) : While::Token(TerminalID<Grammar>(T::REAL)), value(_value) {}
};

struct While::ID : public While::Token {
    std::string value;
    ID(const std::string& _value) : While::Token(TerminalID<Grammar>(T::ID)), value(_value) {}
};

struct While::String : public While::Token {
    std::string value;
    String(const std::string& _value) : While::Token(TerminalID<Grammar>(T::STRING)), value(_value) {}
};

//inline TerminalID<While::Grammar> While::Grammar::getTag(const While::Token& x) {
//  return x.tag;
//}

inline TerminalID<While::Grammar> While::Grammar::getTag(const While::Token* x) {
    return x->tag;
}

//inline TerminalID<While::Grammar>::operator GrammarElement<While::Grammar>() const {
//  return GrammarElement<While::Grammar>(x);
//}
//
//inline NonterminalID<While::Grammar>::operator GrammarElement<While::Grammar>() const {
//  return GrammarElement<While::Grammar>(x + While::Grammar::numberOfTerminals);
//}

inline std::ostream& operator<<(std::ostream& s, While::T const& a) {
    return s << While::t_strings[uint32_t(a)];
}

inline std::ostream& operator<<(std::ostream& s, While::NT const& A) {
    return s << While::nt_strings[uint32_t(A)];
}

inline std::ostream& operator<<(std::ostream& s, TerminalID<While::Grammar> const& a) {
    return s << While::T(a);
}

inline std::ostream& operator<<(std::ostream& s, NonterminalID<While::Grammar> const& A) {
    return s << While::NT(A);
}

#endif /* WHILE_H_ */
