#ifndef FUNCTIONAL_H_
#define FUNCTIONAL_H_

#include "Grammar.h"

#include <cstdint>
#include <vector>

#include <boost/dynamic_bitset.hpp>
#include <iostream>

namespace Functional {
using type = uint32_t;
struct Grammar;
enum class T : uint32_t;
enum class NT : uint32_t;
}

template<>
struct NonterminalID<Functional::Grammar>;

template<>
struct TerminalID<Functional::Grammar>;

template<>
struct GrammarElement<Functional::Grammar>;

namespace Functional {
enum class T : type {
    SEMICOLON, ASSIGN, PERIOD, LPAREN, RPAREN, PLUS, MINUS, TIMES, DIV, LT, GT, AND, OR, BSL, LE, EQ, NE, GE, LET, IN, LETREC, VAR, NUM, WS, EOI, EPS
};

enum class NT : type {
    START, P, S, D, C, A, B, L
};

extern const char* nt_strings[];
extern const char* t_strings[];

struct Token;
template<class T>
    struct TokenBase;
struct Keyword;
struct BinOp;
struct Num;
struct Other;
struct Var;
struct Whitespace;

struct Grammar {
    using type = Functional::type;

    // order: true terminals, EOF, EPS, nonterminals. S' is just the starting symbol

    using String = std::vector<GrammarElement<Grammar>>;

    static kind kindOf(GrammarElement<Grammar> X);

    static type getNumberOfTerminals() {
        return numberOfTerminals;
    }

    static type getNumberOfNonterminals() {
        return numberOfNonterminals;
    }

    static type getNumberOfGrammarElements() {
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

    //using Token_const_reference = const Functional::Token*;
    using Token_rv_reference = const Token*;

    static TerminalID<Grammar> getTag(const Token* x);

    static const NonterminalID<Grammar> start;
    static const TerminalID<Grammar> eof;
    static const TerminalID<Grammar> eps;
    static const type numberOfNonterminals;
    static const type numberOfTerminals;
    static const std::unordered_map<NonterminalID<Grammar>, std::vector<String>> productions;
    static const std::unordered_map<GrammarElement<Grammar>, std::unordered_set<TerminalID<Grammar>>> firsts;

};
}

template<>
struct TerminalID<Functional::Grammar> {
    Functional::type x;

    constexpr TerminalID(Functional::T _x) : x(Functional::type(_x)) {}

    explicit TerminalID(Functional::type _x) : x(_x) {
        assert(_x < Functional::Grammar::getNumberOfTerminals());
    }

    //  operator GrammarElement<Functional::Grammar>() const;

    operator Functional::T() const {
        return Functional::T(x);
    }
};

template<>
struct NonterminalID<Functional::Grammar> {
    Functional::type x;

    constexpr NonterminalID(Functional::NT _x) : x(Functional::type(_x)) {}

    explicit NonterminalID(Functional::type _x) : x(_x) {
        assert(_x < Functional::Grammar::getNumberOfNonterminals());
    }

    //  operator GrammarElement<Functional::Grammar>() const;

    operator Functional::NT() const {
        return Functional::NT(x);
    }
};

template<>
struct GrammarElement<Functional::Grammar> {
    Functional::type x;
    explicit GrammarElement(Functional::type _x) : x(_x) {
        assert(_x < Functional::Grammar::getNumberOfGrammarElements());
    }
    GrammarElement(Functional::T _x) : x(TerminalID<Functional::Grammar>(_x).x) {}
    GrammarElement(Functional::NT _x) : x(NonterminalID<Functional::Grammar>(_x).x + Functional::Grammar::numberOfTerminals) {}
    GrammarElement(TerminalID<Functional::Grammar> _x) : x(_x.x) {}
    GrammarElement(NonterminalID<Functional::Grammar> _x) : x(_x.x + Functional::Grammar::numberOfTerminals) {}
    operator TerminalID<Functional::Grammar>() const {
        return TerminalID<Functional::Grammar>(x);
    }

    operator NonterminalID<Functional::Grammar>() const {
        return NonterminalID<Functional::Grammar>(x - Functional::Grammar::numberOfTerminals);
    }
};

using std::ostream;
using std::cout;
using std::cerr;
using std::endl;
using std::exception;
using std::string;
using std::array;

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
        TerminalID<Grammar> tag;
        Token(TerminalID<Grammar> t) : tag(t) {}
        virtual void accept(TokenVisitor& v);
        virtual void accept(TokenConstVisitor& v) const;
        virtual ~Token() {}
    };

    template<typename T>
        struct TokenBase : public Token {
            using Token::Token;
            void accept(TokenVisitor& v) override {
                v.visit(*static_cast<T*>(this));
            }
            void accept(TokenConstVisitor& v) const override {
                v.visit(*static_cast<const T*>(this));
            }
        };

    struct Num : public TokenBase<Num> {
        uint64_t n;
        Num(unsigned long long n_) : TokenBase<Num>(T::NUM), n(n_) {}
    };

    struct Var : public TokenBase<Var> {
        std::string s;
        Var(string s_) : TokenBase<Var>(T::VAR), s(std::move(s_)) {}
    };

    inline TerminalID<Grammar> Grammar::getTag(const Functional::Token *x) {
        return x->tag;
    }
}

inline std::ostream& operator<<(std::ostream& s, Functional::T const& a) {
    return s << Functional::t_strings[Functional::type(a)];
}

inline std::ostream& operator<<(std::ostream& s, Functional::NT const& A) {
    return s << Functional::nt_strings[Functional::type(A)];
}

inline std::ostream& operator<<(std::ostream& s, TerminalID<Functional::Grammar> const& a) {
    return s << Functional::T(a);
}

inline std::ostream& operator<<(std::ostream& s, NonterminalID<Functional::Grammar> const& A) {
    return s << Functional::NT(A);
}

#endif /* FUNCTIONAL_H_ */
