#pragma once

#include "Regex/nfaBuilder_fwd.h"

#include "Regex/Print.h"
#include <cassert>
#include <exception>
#include <iostream>
#include <tuple>
#include <unordered_map>
#include <vector>

template <typename State, typename TokenId>
struct node_t {
    TokenId kind = 0;
    std::unordered_map<size_t, std::vector<State>> ns;
};

template <typename Symbol, typename State, typename TokenId>
struct nfaBuilder {
    using Pair = std::pair<State, State>;
    using node = node_t<State, TokenId>;

    State start;
    Symbol eps;
    std::vector<node> ns;
    std::vector<Symbol> idToSymbol;
    std::unordered_map<Symbol, size_t> symbolToId;

    nfaBuilder(Symbol _eps = Symbol());

    Pair getPair();
    Pair alt(const Pair& nfa1, const Pair& nfa2);
    Pair closure(const Pair& nfa1);
    Pair opt(const Pair& nfa1);
    Pair cat(const Pair& nfa1, const Pair& nfa2);

    void match(std::istream& in, Symbol c) const;
    Pair singletonLexer(char c);
    Pair lexH(std::istream& in);
    Pair lexG(std::istream& in);
    Pair lexFR(std::istream& in, const Pair& nfa1);
    Pair lexF(std::istream& in);
    Pair lexTR(std::istream& in, const Pair& nfa1);
    Pair lexT(std::istream& in);
    Pair lexER(std::istream& in, const Pair& nfa1);
    Pair lexE(std::istream& in);
    void lexRegex(std::istream& in, TokenId kind);
};

template <typename Symbol, typename State, typename TokenId>
nfaBuilder<Symbol, State, TokenId>::nfaBuilder(Symbol _eps) : start(), eps(_eps) {
    ns.emplace_back();
    idToSymbol.push_back(eps);
    symbolToId[eps] = 0;
}

template <typename Symbol, typename State, typename TokenId>
auto nfaBuilder<Symbol, State, TokenId>::getPair() -> Pair {
    Pair out;
    out.first = ns.size();
    ns.emplace_back();
    out.second = ns.size();
    ns.emplace_back();
    return out;
}

template <typename Symbol, typename State, typename TokenId>
auto nfaBuilder<Symbol, State, TokenId>::alt(const Pair& nfa1, const Pair& nfa2) -> Pair {
    assert(symbolToId.at(eps) == 0);
    auto out = getPair();
    ns[out.first].ns[0].push_back(nfa1.first);
    ns[out.first].ns[0].push_back(nfa2.first);
    ns[nfa1.second].ns[0].push_back(out.second);
    ns[nfa2.second].ns[0].push_back(out.second);
    return out;
}

template <typename Symbol, typename State, typename TokenId>
auto nfaBuilder<Symbol, State, TokenId>::closure(const Pair& nfa1) -> Pair {
    assert(symbolToId.at(eps) == 0);
    auto out = getPair();
    ns[out.first].ns[0].push_back(nfa1.first);
    ns[out.first].ns[0].push_back(out.second);
    ns[nfa1.second].ns[0].push_back(nfa1.first);
    ns[nfa1.second].ns[0].push_back(out.second);
    return out;
}

template <typename Symbol, typename State, typename TokenId>
auto nfaBuilder<Symbol, State, TokenId>::opt(const Pair& nfa1) -> Pair {
    assert(symbolToId.at(eps) == 0);
    auto out = getPair();
    ns[out.first].ns[0].push_back(nfa1.first);
    ns[out.first].ns[0].push_back(out.second);
    ns[nfa1.second].ns[0].push_back(out.second);
    return out;
}

template <typename Symbol, typename State, typename TokenId>
auto nfaBuilder<Symbol, State, TokenId>::cat(const Pair& nfa1, const Pair& nfa2) -> Pair {
    assert(symbolToId.at(eps) == 0);
    ns[nfa1.second].ns[0].push_back(nfa2.first);

    Pair out;
    out.first = nfa1.first;
    out.second = nfa2.second;
    return out;
}

template <typename Symbol, typename State, typename TokenId>
void nfaBuilder<Symbol, State, TokenId>::match(std::istream& in, Symbol c) const {
    std::cout << "match('" << showCharEscaped(c) << "')\n";
    // static_assert(std::is_same<Symbol,int>::value, "Symbol needs to be int.");
    auto d = Symbol(in.get());
    if (d != c) {
        throw std::runtime_error("expected '" + printEscaped(c) + "', got '" + printEscaped(d) + "'");
    }
}

// E -> E '|' E | T
// T -> T T | F
// F -> F* | F? | G
// G -> (E) | \\H | ...
// H -> \\ | '|' | * | ( | )

// E -> T E'
// E' -> '|' T E' | eps
// T -> F T'
// T' -> F T' | eps
// F -> G F'
// F' -> *F' | ?F' | eps
// G -> ( E ) | \\H | [any char except ...]
// H -> \\ | '|' | * | ? | ( | )

//    | FIRST                  | FOLLOW
// -----------------------------------------
// E  | FIRST(T)               | $, )
// E' | '|', eps               | FOLLOW(E)
// T  | FIRST(F)               | FIRST(E') + FOLLOW(E) + FOLLOW(E')
// T' | FIRST(F), eps          | FOLLOW(T)
// F  | FIRST(G)               | FIRST(T') + FOLLOW(T) + FOLLOW(T')
// F' | *, ?, eps              | FOLLOW(F)
// G  | (, \\, ...             | FIRST(F') + FOLLOW(F)
// H  | \\, '|', *, ?, (, |, ) | FOLLOW(G)

//    | FIRST                  | FOLLOW
// -----------------------------------------
// E  | (, \\, ...             | $, )
// E' | '|', eps               | $, )
// T  | (, \\, ...             | '|', $, )
// T' | (, \\, ..., eps        | '|', $, )
// F  | (, \\, ...             | (, \\, ..., '|', $, )
// F' | *, ?, eps              | (, \\, ..., '|', $, )
// G  | (, \\, ...             | *, ?, (, \\, ..., '|', $, )
// H  | \\, '|', *, ?, (, |, ) | *, ?, (, \\, ..., '|', $, )

// symbols: (, \\, '|', *, ?, ), $, ...

// E -> T E'
// production 1: (, \\, ...
// invalid: '|', *, ?, ), $

// E' -> '|' T E' | eps
// production 1: '|'
// production 2: $, )
// invalid: (, \\, *, ?, ...

// T -> F T'
// production 1: (, \\, ...
// invalid: '|', *, ?, ), $

// T' -> F T' | eps
// production 1: (, \\, ...
// production 2: '|', $, )
// invalid: *, ?

// F -> G F'
// production 1: (, \\, ...
// invalid: '|', *, ?, ), $

// F' -> *F' | ?F' | eps
// production 1: *
// production 2: ?
// production 3: (, \\, '|', $, ), ...
// invalid: (empty)

// G -> ( E ) | \\H | [any char except ...]
// production 1: (
// production 2: \\ [don't end comments with \]
// production 3: ...
// invalid: '|', *, ?, ), $

// H -> \\ | '|' | * | ? | ( | ) | n | t | r
// ...
// invalid: $, ...

template <typename Symbol, typename State, typename TokenId>
auto nfaBuilder<Symbol, State, TokenId>::singletonLexer(char c) -> Pair {
    auto out = getPair();

    auto it = symbolToId.find(c);
    unsigned int id;
    if (it == symbolToId.end()) {
        id = symbolToId.size();
        std::cout << "new symbol: '" << showCharEscaped(c) << "' (id: " << id << ")\n";
        symbolToId[c] = id;
        idToSymbol.push_back(c);
    } else
        id = it->second;
    ns[out.first].ns[id].push_back(out.second);
    return out;
}

template <typename Symbol, typename State, typename TokenId>
auto nfaBuilder<Symbol, State, TokenId>::lexH(std::istream& in) -> Pair {
    auto c = in.peek();
    std::cout << "lexH (peek: '" << showCharEscaped(c) << "')\n";
    switch (c) {
    case '\\':
    case '|':
    case '*':
    case '?':
    case '(':
    case ')': {
        match(in, c);
        auto out = singletonLexer(c);
        std::cout << "lexH out: " << show(out) << '\n';
        return out;
    }
    default:
        throw std::runtime_error("lexH Error: '" + printEscaped(c) + "'");
    }
}

template <typename Symbol, typename State, typename TokenId>
auto nfaBuilder<Symbol, State, TokenId>::lexG(std::istream& in) -> Pair {
    auto c = in.peek();
    std::cout << "lexG (peek: '" << showCharEscaped(c) << "')\n";
    switch (c) {
    case '(': {
        match(in, '(');
        auto nfa1 = lexE(in);
        match(in, ')');
        std::cout << "lexG out: " << show(nfa1) << '\n';
        return nfa1;
    }
    case '\\': {
        match(in, '\\');
        auto out = lexH(in);
        std::cout << "lexG out: " << show(out) << '\n';
        return out;
    }
    case '|':
    case EOF:
    case ')':
    case '*':
    case '?':
        throw std::runtime_error("lexG Error: '" + printEscaped(c) + "'");
    default: {
        match(in, c);
        auto out = singletonLexer(c);
        std::cout << "lexG out: " << show(out) << '\n';
        return out;
    }
    }
}

template <typename Symbol, typename State, typename TokenId>
auto nfaBuilder<Symbol, State, TokenId>::lexFR(std::istream& in, const Pair& nfa1) -> Pair {
    auto c = in.peek();
    std::cout << "lexFR(" << show(nfa1) << ") (peek: '" << showCharEscaped(c) << "')\n";
    switch (c) {
    case '*': {
        match(in, '*');
        auto nfa2 = closure(nfa1);
        auto out = lexFR(in, nfa2);
        std::cout << "lexFR out: " << show(out) << '\n';
        return out;
    }
    case '?': {
        match(in, '?');
        auto nfa2 = opt(nfa1);
        auto out = lexFR(in, nfa2);
        std::cout << "lexFR out: " << show(out) << '\n';
        return out;
    }
    default:
        std::cout << "lexFR out: " << show(nfa1) << '\n';
        return nfa1;
    }
}

template <typename Symbol, typename State, typename TokenId>
auto nfaBuilder<Symbol, State, TokenId>::lexF(std::istream& in) -> Pair {
    auto c = in.peek();
    std::cout << "lexF (peek: '" << showCharEscaped(c) << "')\n";
    switch (c) {
    case '|':
    case EOF:
    case ')':
    case '*':
    case '?':
        throw std::runtime_error("lexF Error: '" + printEscaped(c) + "'");
    default: {
        auto nfa1 = lexG(in);
        auto nfa2 = lexFR(in, nfa1);
        std::cout << "lexF out: " << show(nfa2) << '\n';
        return nfa2;
    }
    }
}

template <typename Symbol, typename State, typename TokenId>
auto nfaBuilder<Symbol, State, TokenId>::lexTR(std::istream& in, const Pair& nfa1) -> Pair {
    auto c = in.peek();
    std::cout << "lexTR(" << show(nfa1) << ") (peek: '" << showCharEscaped(c) << "')\n";
    switch (c) {
    case '|':
    case EOF:
    case ')':
        std::cout << "lexTR out: " << show(nfa1) << '\n';
        return nfa1;
    case '*':
    case '?':
        throw std::runtime_error("lexTR Error: '" + printEscaped(c) + "'");
    default: {
        auto nfa2 = lexF(in);
        auto nfa3 = cat(nfa1, nfa2);
        auto nfa4 = lexTR(in, nfa3);
        std::cout << "lexTR out: " << show(nfa4) << '\n';
        return nfa4;
    }
    }
}

template <typename Symbol, typename State, typename TokenId>
auto nfaBuilder<Symbol, State, TokenId>::lexT(std::istream& in) -> Pair {
    auto c = in.peek();
    std::cout << "lexT (peek: '" << showCharEscaped(c) << "')\n";
    switch (c) {
    case '|':
    case EOF:
    case ')':
    case '*':
    case '?':
        throw std::runtime_error("lexT Error: '" + printEscaped(c) + "'");
    default: {
        auto nfa1 = lexF(in);
        auto nfa2 = lexTR(in, nfa1);
        std::cout << "lexT out: " << show(nfa2) << '\n';
        return nfa2;
    }
    }
}

template <typename Symbol, typename State, typename TokenId>
auto nfaBuilder<Symbol, State, TokenId>::lexER(std::istream& in, const Pair& nfa1) -> Pair {
    auto c = in.peek();
    std::cout << "lexER(" << show(nfa1) << ") (peek: '" << showCharEscaped(c) << "')\n";
    switch (c) {
    case '|': {
        match(in, '|');
        auto nfa2 = lexT(in);

        auto nfa3 = alt(nfa1, nfa2);

        auto nfa4 = lexER(in, nfa3);
        std::cout << "lexER out: " << show(nfa4) << '\n';
        return nfa4;
    }
    case EOF:
    case ')':
        std::cout << "lexER out: " << show(nfa1) << '\n';
        return nfa1;
    default:
        throw std::runtime_error("lexER Error: '" + printEscaped(c) + "'");
    }
}

template <typename Symbol, typename State, typename TokenId>
auto nfaBuilder<Symbol, State, TokenId>::lexE(std::istream& in) -> Pair {
    auto c = in.peek();
    std::cout << "lexE (peek: '" << showCharEscaped(c) << "')\n";
    switch (c) {
    case '|':
    case EOF:
    case ')':
    case '*':
    case '?':
        throw std::runtime_error("lexE Error: '" + printEscaped(c) + "'");
    default: {
        auto nfa1 = lexT(in);
        auto nfa2 = lexER(in, nfa1);
        std::cout << "lexE out: " << show(nfa2) << '\n';
        return nfa2;
    }
    }
}

template <typename Symbol, typename State, typename TokenId>
void nfaBuilder<Symbol, State, TokenId>::lexRegex(std::istream& in, TokenId kind) {
    std::cout << "entered lexRegex\n";
    Pair nfa1 = lexE(in);
    assert(symbolToId.at(eps) == 0);
    ns[start].ns[0].push_back(nfa1.first);
    ns[nfa1.second].kind = kind;
    std::cout << "done with lexRegex\n";
    std::cout << "out: (" << nfa1.first << "," << nfa1.second << ")\n";
}
