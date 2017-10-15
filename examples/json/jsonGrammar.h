#pragma once

#include "Grammar.h"

#include <cstdint>
#include <vector>
#include <boost/variant.hpp>
#include <boost/utility/string_view.hpp>

#include <iosfwd>
#undef NULL

namespace json {
using type = uint8_t;
struct Grammar;

enum class T : type {
    BEGIN_ARRAY, BEGIN_OBJECT, END_ARRAY, END_OBJECT, NAME_SEPARATOR, VALUE_SEPARATOR, FALSE, NULL, TRUE, NUM, STRING, WS, EOI, EPS
};

enum class NT : type {
    START, JSON_TEXT, VALUE, OBJECT, MEMBER, MEMBERS, ARRAY, VALUES
};

static constexpr type numberOfNonterminals = type(NT::VALUES) + 1;
static constexpr type numberOfTerminals = type(T::EPS) + 1;

inline type getNumberOfTerminals() {
    return numberOfTerminals;
}

inline type getNumberOfNonterminals() {
    return numberOfNonterminals;
}

inline type getNumberOfGrammarElements() {
    return static_cast<type>(numberOfTerminals + numberOfNonterminals);
}
}

//template<>
//struct NonterminalID<json::Grammar>;
//
//template<>
//struct TerminalID<json::Grammar>;
//
//template<>
//struct GrammarElement<json::Grammar>;

template<>
struct TerminalID<json::Grammar> {
    json::type x;

    constexpr TerminalID(json::T _x) : x(json::type(_x)) {}

    explicit TerminalID(json::type _x) : x(_x) {
        assert(_x < json::getNumberOfTerminals());
    }

    //  operator GrammarElement<json::Grammar>() const;

    operator json::T() const {
        return json::T(x);
    }
};

template<>
struct NonterminalID<json::Grammar> {
    json::type x;

    constexpr NonterminalID(json::NT _x) : x(json::type(_x)) {}

    explicit NonterminalID(json::type _x) : x(_x) {
        assert(_x < json::getNumberOfNonterminals());
    }

    //  operator GrammarElement<json::Grammar>() const;

    operator json::NT() const {
        return json::NT(x);
    }
};

template<>
struct GrammarElement<json::Grammar> {
    json::type x;
    explicit GrammarElement(json::type _x) : x(_x) {
        assert(_x < json::getNumberOfGrammarElements());
    }
    GrammarElement(json::T _x) : x(TerminalID<json::Grammar>(_x).x) {}
    GrammarElement(json::NT _x) : x(static_cast<json::type>(NonterminalID<json::Grammar>(_x).x + json::numberOfTerminals)) {}
    GrammarElement(TerminalID<json::Grammar> _x) : x(_x.x) {}
    GrammarElement(NonterminalID<json::Grammar> _x) : x(static_cast<json::type>(_x.x + json::numberOfTerminals)) {}
    operator TerminalID<json::Grammar>() const {
        return TerminalID<json::Grammar>(x);
    }

    operator NonterminalID<json::Grammar>() const {
        return NonterminalID<json::Grammar>(static_cast<json::type>(x - json::numberOfTerminals));
    }
};


namespace json {
extern const char* nt_strings[];
extern const char* t_strings[];

struct num_view {
    boost::string_view x;

    num_view(const boost::string_view& x_) : x(x_) {}
    num_view(const char* s, size_t n) : x(s, n) {}
};

inline bool operator==(const num_view& a, const num_view& b) {
    return a.x == b.x;
}

using Token_ = std::pair<TerminalID<Grammar>,boost::variant<boost::blank, bool, num_view, boost::string_view>>;
struct Token : private Token_ {
    using Token_::Token_;
    using Token_::first;
    using Token_::second;

    Token() : Token_(TerminalID<Grammar>(0U), {}) {}
};

inline bool operator==(const Token& a, const Token& b) {
    return a.first == b.first && a.second == b.second;
}

inline bool operator!=(const Token& a, const Token& b) {
    return !(a == b);
}

struct Grammar {
    using type = json::type;

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
        return static_cast<type>(numberOfTerminals + numberOfNonterminals);
    }

    //static const std::unordered_map<NonterminalID<Grammar>, std::vector<String>>& getProductions();
    static const std::vector<std::vector<String>>& getProductions() {
        return productions;
    }

    static const std::vector<String>& getProductions(NonterminalID<Grammar> A);

    static const std::unordered_map<GrammarElement<Grammar>, std::unordered_set<TerminalID<Grammar>>>& getFirsts() {
        return firsts;
    }

    static const std::unordered_set<TerminalID<Grammar>>& getFirsts(const GrammarElement<Grammar>& X) ;

    static const std::unordered_map<NonterminalID<Grammar>, std::unordered_set<TerminalID<Grammar>>>& getFollows() {
        return follows;
    }

    static const std::unordered_set<TerminalID<Grammar>>& getFollows(const NonterminalID<Grammar>& X) ;

    using Token_rv_reference = json::Token&&;

    //static TerminalID<Grammar> getTag(const Token* x);
    static TerminalID<Grammar> getTag(const Token& x);

    static const NonterminalID<Grammar> start;
    static const TerminalID<Grammar> eof;
    static const TerminalID<Grammar> eps;
    static constexpr type numberOfNonterminals = type(NT::VALUES) + 1;
    static constexpr type numberOfTerminals = type(T::EPS) + 1;
    static_assert(size_t(numberOfTerminals) + size_t(numberOfNonterminals) <= size_t(std::numeric_limits<type>::max()));
    //static const std::unordered_map<NonterminalID<Grammar>, std::vector<String>> productions;
    static const std::vector<std::vector<String>> productions;
    static const std::unordered_map<GrammarElement<Grammar>, std::unordered_set<TerminalID<Grammar>>> firsts;
    static const std::unordered_map<NonterminalID<Grammar>, std::unordered_set<TerminalID<Grammar>>> follows;

};

}

namespace boost {
template<> struct has_nothrow_copy<json::num_view> : boost::true_type {};
template<> struct has_nothrow_constructor<json::num_view> : boost::true_type {};
}

//template<>
//struct TerminalID<json::Grammar> {
//    json::type x;
//
//    constexpr TerminalID(json::T _x) : x(json::type(_x)) {}
//
//    explicit TerminalID(json::type _x) : x(_x) {
//        assert(_x < json::Grammar::getNumberOfTerminals());
//    }
//
//    //  operator GrammarElement<json::Grammar>() const;
//
//    operator json::T() const {
//        return json::T(x);
//    }
//};
//
//template<>
//struct NonterminalID<json::Grammar> {
//    json::type x;
//
//    constexpr NonterminalID(json::NT _x) : x(json::type(_x)) {}
//
//    explicit NonterminalID(json::type _x) : x(_x) {
//        assert(_x < json::Grammar::getNumberOfNonterminals());
//    }
//
//    //  operator GrammarElement<json::Grammar>() const;
//
//    operator json::NT() const {
//        return json::NT(x);
//    }
//};
//
//template<>
//struct GrammarElement<json::Grammar> {
//    json::type x;
//    explicit GrammarElement(json::type _x) : x(_x) {
//        assert(_x < json::Grammar::getNumberOfGrammarElements());
//    }
//    GrammarElement(json::T _x) : x(TerminalID<json::Grammar>(_x).x) {}
//    GrammarElement(json::NT _x) : x(static_cast<json::type>(NonterminalID<json::Grammar>(_x).x + json::Grammar::numberOfTerminals)) {}
//    GrammarElement(TerminalID<json::Grammar> _x) : x(_x.x) {}
//    GrammarElement(NonterminalID<json::Grammar> _x) : x(static_cast<json::type>(_x.x + json::Grammar::numberOfTerminals)) {}
//    operator TerminalID<json::Grammar>() const {
//        return TerminalID<json::Grammar>(x);
//    }
//
//    operator NonterminalID<json::Grammar>() const {
//        return NonterminalID<json::Grammar>(static_cast<json::type>(x - json::Grammar::numberOfTerminals));
//    }
//};

namespace json {
    //struct TokenVisitor {
    //    virtual void visit(Num&) = 0;
    //    virtual void visit(String&) = 0;
    //    virtual void visit(Token&) = 0;
    //};

    //struct TokenConstVisitor {
    //    virtual void visit(const Num&) = 0;
    //    virtual void visit(const String&) = 0;
    //    virtual void visit(const Token&) = 0;
    //};

    //struct Token {
    //    TerminalID<Grammar> tag;
    //    Token(TerminalID<Grammar> t) : tag(t) {}
    //    virtual void accept(TokenVisitor& v);
    //    virtual void accept(TokenConstVisitor& v) const;
    //    virtual ~Token() = default;
    //};

    //template<typename T>
    //    struct TokenBase : public Token {
    //        using Token::Token;
    //        void accept(TokenVisitor& v) override {
    //            v.visit(*static_cast<T*>(this));
    //        }
    //        void accept(TokenConstVisitor& v) const override {
    //            v.visit(*static_cast<const T*>(this));
    //        }
    //    };

    //struct Num : public TokenBase<Num> {
    //    //double n;
    //    //Num(double n_) : TokenBase<Num>(T::NUM), n(n_) {}
    //    const char* s; // NOT OWNED!!
    //    size_t n;
    //    Num(const char* s_, size_t n_) : TokenBase<Num>(T::NUM), s(s_), n(n_) {}
    //};

    //struct String : public TokenBase<String> {
    //    //std::string s;
    //    //String(string s_) : TokenBase<String>(T::STRING), s(std::move(s_)) {}
    //    const char* s; // NOT OWNED!!
    //    size_t n;
    //    String(const char* s_, size_t n_) : TokenBase<String>(T::STRING), s(s_), n(n_) {}
    //};

    //inline TerminalID<Grammar> Grammar::getTag(const json::Token *x) {
    //    return x->tag;
    //}
    inline TerminalID<Grammar> Grammar::getTag(const json::Token& x) {
        return x.first;
    }
}

std::ostream& operator<<(std::ostream& s, json::T const& a);

std::ostream& operator<<(std::ostream& s, json::NT const& A);

std::ostream& operator<<(std::ostream& s, TerminalID<json::Grammar> const& a);

std::ostream& operator<<(std::ostream& s, NonterminalID<json::Grammar> const& A);
