#pragma once

#include "constexpr.h"

#include <cstdint>
#include <array>
#include <cassert>

inline std::string printEscaped2(int c) {
    // TODO: Put all 256 values into an array?
    static const char* const controlChars[] = { "EOF", "NUL", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL", "BS", "HT", "LF", "VT", "FF", "CR", "SO", "SI", "DLE", "DC1", "DC2", "DC3", "DC4", "NAK", "SYN", "ETB", "CAN", "EM", "SUB", "ESC", "FS", "GS", "RS", "US", "SPACE" };
    if (c < -1 || c > 127)
        return std::to_string(c);
    else if (c < 33)
        return controlChars[c + 1];
    else if (c == 127)
        return "DEL";
    else if (c == '"')
        return "\\\"";
    else if (c == '\\')
        return "BSL";
    else
        return std::string(1, static_cast<char>(c));
}

template<typename State, typename TokenId, int64_t MaxTransitions, int64_t MaxResultStates>
class node_t {
public:
    using result_states = vector<State,MaxResultStates>;
    using transitions = unordered_map<int64_t,result_states,MaxTransitions>;

    constexpr node_t() = default;

    constexpr void addTransition(int64_t label, State destination) {
        ns[label].push_back(destination);
    }

    constexpr void setKind(TokenId k) { kind = k; }
    constexpr TokenId getKind() const { return kind; }

    constexpr typename transitions::iterator begin() { return ns.begin(); }
    constexpr typename transitions::iterator end() { return ns.end(); }
    constexpr typename transitions::const_iterator begin() const { return ns.begin(); }
    constexpr typename transitions::const_iterator end() const { return ns.end(); }
    constexpr typename transitions::const_iterator cbegin() const { return ns.begin(); }
    constexpr typename transitions::const_iterator cend() const { return ns.end(); }
private:
    transitions ns;
    TokenId kind = 0;
};

template<typename State, typename TokenId>
class CountingGraph {
public:
    constexpr State addState() {
        ++stateCount;
        return State();
    }

    constexpr void addEdge(State, State, int64_t) {
        ++edgeCount;
    }

    constexpr void addEpsilonEdge(State, State) {
        ++edgeCount;
    }

    constexpr int64_t getStateCount() const { return stateCount; }

    constexpr void setKind(State, TokenId) {}

    constexpr TokenId getKind(State) const { return TokenId(); }

private:
    int64_t stateCount = 0;
    int64_t edgeCount = 0;
};

template<typename State, typename TokenId, int64_t StateCount>
class TransitionCountingGraph {
public:
    constexpr State addState() {
        if (ns_.size() > std::numeric_limits<State>::max())
            throw std::range_error("TransitionCountingGraph::addState");
        auto id = static_cast<State>(ns_.size());
        ns_.emplace_back();
        return id;
    }

    constexpr void addEdge(State source, State, int64_t) {
        ++ns_[source];
    }

    constexpr void addEpsilonEdge(State source, State) {
        ++ns_[source];
    }

    constexpr int64_t getStateCount() const { return ns_.size(); }

    constexpr void setKind(State, TokenId) {}

    constexpr TokenId getKind(State) const { return TokenId(); }

    constexpr const vector<int64_t,StateCount>& getTransitionCounts() const { return ns_; }
private:
    vector<int64_t,StateCount> ns_;
};

template<typename State, typename TokenId, int64_t StateCount, int64_t MaxTransitions>
class ResultingStateCountingGraph {
public:
    using transitions = unordered_map<int64_t,int64_t,MaxTransitions>;

    constexpr State addState() {
        if (ns_.size() > std::numeric_limits<State>::max())
            throw std::range_error("TransitionCountingGraph::addState");
        auto id = static_cast<State>(ns_.size());
        ns_.emplace_back();
        return id;
    }

    constexpr void addEdge(State source, State, int64_t label) {
        ++ns_[source][label];
    }

    constexpr void addEpsilonEdge(State source, State) {
        ++ns_[source][0];
    }

    constexpr int64_t getStateCount() const { return ns_.size(); }

    constexpr void setKind(State, TokenId) {}

    constexpr TokenId getKind(State) const { return TokenId(); }

    constexpr const vector<transitions,StateCount>& getResultingStateCounts() const { return ns_; }

private:
    vector<transitions,StateCount> ns_;
};

template<typename State, typename TokenId, int64_t MaxNodes, int64_t MaxTransitions, int64_t MaxResultStates>
class Graph {
public:
    constexpr State addState() {
        if (ns_.size() > std::numeric_limits<State>::max())
            throw std::range_error("TransitionCountingGraph::addState");
        auto id = static_cast<State>(ns_.size());
        ns_.emplace_back();
        return id;
    }

    constexpr void addEdge(State source, State destination, int64_t label) {
        ns_[source].addTransition(label, destination);
    }

    constexpr void addEpsilonEdge(State source, State destination) {
        ns_[source].addTransition(0, destination);
    }

    constexpr int64_t getStateCount() const { return ns_.size(); }

    constexpr void setKind(State state, TokenId kind) {
        ns_[state].setKind(kind);
    }

    constexpr TokenId getKind(State state) const { return ns_[state].getKind(); }

//private:
    using node = node_t<State,TokenId,MaxTransitions,MaxResultStates>;
    using nodes = vector<node,MaxNodes>;
    nodes ns_;

};

template<typename Symbol, typename State, int64_t MaxSymbols, typename Graph> // TODO: add default argument for Graph as soon as I can compute the counts
struct nfaBuilder {
    using Pair = pair<State,State>;

    State start;
    Symbol eps;
    Graph ns; // TODO: add parameter to constructor
    vector<Symbol,MaxSymbols> idToSymbol;
    unordered_map<Symbol,int64_t,MaxSymbols> symbolToId;

    constexpr nfaBuilder(Symbol _eps = Symbol());

    constexpr Pair getPair();
    constexpr Pair alt(const Pair& nfa1, const Pair& nfa2);
    constexpr Pair closure(const Pair& nfa1);
    constexpr Pair opt(const Pair& nfa1);
    constexpr Pair cat(const Pair& nfa1, const Pair& nfa2);

    constexpr void match(const char*& in, Symbol c) const;
    constexpr Pair singletonLexer(char c);
    constexpr Pair lexH(const char*& in);
    constexpr Pair lexG(const char*& in);
    constexpr Pair lexFR(const char*& in, const Pair& nfa1);
    constexpr Pair lexF(const char*& in);
    constexpr Pair lexTR(const char*& in, const Pair& nfa1);
    constexpr Pair lexT(const char*& in);
    constexpr Pair lexER(const char*& in, const Pair& nfa1);
    constexpr Pair lexE(const char*& in);
    template<typename TokenId>
    constexpr void lexRegex(const char* in, TokenId kind);
};

template<typename Symbol, typename State, int64_t MaxSymbols, typename Graph>
constexpr nfaBuilder<Symbol,State,MaxSymbols,Graph>::nfaBuilder(Symbol _eps) : start(), eps(_eps) {
    ns.addState();
    idToSymbol.push_back(eps);
    symbolToId[eps] = 0;
}

template<typename Symbol, typename State, int64_t MaxSymbols, typename Graph>
constexpr auto nfaBuilder<Symbol,State,MaxSymbols,Graph>::getPair() -> Pair {
    return Pair(ns.addState(), ns.addState());
}

template<typename Symbol, typename State, int64_t MaxSymbols, typename Graph>
constexpr auto nfaBuilder<Symbol,State,MaxSymbols,Graph>::alt(const Pair& nfa1, const Pair& nfa2) -> Pair {
    assert(symbolToId.at(eps) == 0);
    auto out = getPair();
    ns.addEpsilonEdge(out.first, nfa1.first);
    ns.addEpsilonEdge(out.first, nfa2.first);
    ns.addEpsilonEdge(nfa1.second, out.second);
    ns.addEpsilonEdge(nfa2.second, out.second);
    return out;
}

template<typename Symbol, typename State, int64_t MaxSymbols, typename Graph>
constexpr auto nfaBuilder<Symbol,State,MaxSymbols,Graph>::closure(const Pair& nfa1) -> Pair {
    assert(symbolToId.at(eps) == 0);
    auto out = getPair();
    ns.addEpsilonEdge(out.first, nfa1.first);
    ns.addEpsilonEdge(out.first, out.second);
    ns.addEpsilonEdge(nfa1.second, nfa1.first);
    ns.addEpsilonEdge(nfa1.second, out.second);
    return out;
}

template<typename Symbol, typename State, int64_t MaxSymbols, typename Graph>
constexpr auto nfaBuilder<Symbol,State,MaxSymbols,Graph>::opt(const Pair& nfa1) -> Pair {
    assert(symbolToId.at(eps) == 0);
    auto out = getPair();
    ns.addEpsilonEdge(out.first, nfa1.first);
    ns.addEpsilonEdge(out.first, out.second);
    ns.addEpsilonEdge(nfa1.second, out.second);
    return out;
}

template<typename Symbol, typename State, int64_t MaxSymbols, typename Graph>
constexpr auto nfaBuilder<Symbol,State,MaxSymbols,Graph>::cat(const Pair& nfa1, const Pair& nfa2) -> Pair {
    assert(symbolToId.at(eps) == 0);
    ns.addEpsilonEdge(nfa1.second, nfa2.first);
    return Pair(nfa1.first, nfa2.second);
}

template<typename Symbol, typename State, int64_t MaxSymbols, typename Graph>
constexpr void nfaBuilder<Symbol,State,MaxSymbols,Graph>::match(const char*& in, Symbol c) const {
    //std::cout << "match('" << showCharEscaped(c) << "')" << std::endl;
    //static_assert(std::is_same<Symbol,int>::value, "Symbol needs to be int.");
    auto d = Symbol(*in);
    ++in;
    if (d != c) {
        throw std::runtime_error("expected '" + printEscaped2(c) + "', got '" + printEscaped2(d) + "'");
    }
}

// Original, left-recursive grammar
//
// E -> E '|' E | T
// T -> T T | F
// F -> F* | F? | F+ | G
// G -> (E) | \\H | [any char except ...]
// H -> \\ | '|' | * | ? | + | ( | )

// After elimination of left-recursion
//
// E -> T E'
// E' -> '|' T E' | eps
// T -> F T'
// T' -> F T' | eps
// F -> G F'
// F' -> *F' | ?F' | +F' | eps
// G -> ( E ) | \\H | [any char except ...]
// H -> \\ | '|' | * | ? | + | ( | )

//    | FIRST                     | FOLLOW
// -----------------------------------------
// E  | FIRST(T)                  | $, )
// E' | '|', eps                  | FOLLOW(E)
// T  | FIRST(F)                  | FIRST(E') + FOLLOW(E) + FOLLOW(E')
// T' | FIRST(F), eps             | FOLLOW(T)
// F  | FIRST(G)                  | FIRST(T') + FOLLOW(T) + FOLLOW(T')
// F' | *, ?, +, eps              | FOLLOW(F)
// G  | (, \\, ...                | FIRST(F') + FOLLOW(F)
// H  | \\, '|', *, ?, +, (, |, ) | FOLLOW(G)

//    | FIRST                     | FOLLOW
// -----------------------------------------
// E  | (, \\, ...                | $, )
// E' | '|', eps                  | $, )
// T  | (, \\, ...                | '|', $, )
// T' | (, \\, ..., eps           | '|', $, )
// F  | (, \\, ...                | (, \\, ..., '|', $, )
// F' | *, ?, eps                 | (, \\, ..., '|', $, )
// G  | (, \\, ...                | *, ?, +, (, \\, ..., '|', $, )
// H  | \\, '|', *, ?, +, (, |, ) | *, ?, +, (, \\, ..., '|', $, )

// symbols: (, \\, '|', *, ?, ), $, ...

// E -> T E'
// production 1: (, \\, ...
// invalid: '|', *, ?, +, ), $

// E' -> '|' T E' | eps
// production 1: '|'
// production 2: $, )
// invalid: (, \\, *, ?, +, ...

// T -> F T'
// production 1: (, \\, ...
// invalid: '|', *, ?, +, ), $

// T' -> F T' | eps
// production 1: (, \\, ...
// production 2: '|', $, )
// invalid: *, ?, +

// F -> G F'
// production 1: (, \\, ...
// invalid: '|', *, ?, +, ), $

// F' -> *F' | ?F' | +F' | eps
// production 1: *
// production 2: ?
// production 3: +
// production 4: (, \\, '|', $, ), ...
// invalid: (empty)

// G -> ( E ) | \\H | [any char except ...]
// production 1: (
// production 2: \\ [don't end comments with \]
// production 3: ...
// invalid: '|', *, ?, +, ), $

// H -> \\ | '|' | * | ? | ( | ) | n | t | r
// ...
// invalid: $, ...

template<typename Symbol, typename State, int64_t MaxSymbols, typename Graph>
constexpr auto nfaBuilder<Symbol,State,MaxSymbols,Graph>::singletonLexer(char c) -> Pair {
    auto out = getPair();

    auto it = symbolToId.find(c);
    auto id = symbolToId.size(); // default for it == symbolToId.end() branch
    if (it == symbolToId.end()) {
        //std::cout << "new symbol: '" << showCharEscaped(c) << "' (id: " << id << ")" << std::endl;
        symbolToId[c] = id;
        idToSymbol.push_back(c);
    } else
        id = it->second;
    ns.addEdge(out.first, out.second, id);
    return out;
}

template<typename Symbol, typename State, int64_t MaxSymbols, typename Graph>
constexpr auto nfaBuilder<Symbol,State,MaxSymbols,Graph>::lexH(const char*& in) -> Pair {
    auto c = *in;
    //std::cout << "lexH (peek: '" << showCharEscaped(c) << "')" << std::endl;
    switch (c) {
        case '\\':
        case '|':
        case '*':
        case '?':
        case '+':
        case '(':
        case ')': {
            match(in, c);
            auto out = singletonLexer(c);
            //std::cout << "lexH out: " << show(out) << std::endl;
            return out;
        }
        // this case doesn't work if eps == \0 :(
        //case '0': {
        //    match(in, c);
        //    auto out = singletonLexer('\0');
        //    //std::cout << "lexH out: " << show(out) << std::endl;
        //    return out;
        //}
        case 'r': {
            match(in, c);
            auto out = singletonLexer('\r');
            //std::cout << "lexH out: " << show(out) << std::endl;
            return out;
        }
        case 'n': {
            match(in, c);
            auto out = singletonLexer('\n');
            //std::cout << "lexH out: " << show(out) << std::endl;
            return out;
        }
        case 't': {
            match(in, c);
            auto out = singletonLexer('\t');
            //std::cout << "lexH out: " << show(out) << std::endl;
            return out;
        }
        default:
            throw std::runtime_error("lexH Error: '" + printEscaped2(c) + "'");
    }
}

template<typename Symbol, typename State, int64_t MaxSymbols, typename Graph>
constexpr auto nfaBuilder<Symbol,State,MaxSymbols,Graph>::lexG(const char*& in) -> Pair {
    auto c = *in;
    //std::cout << "lexG (peek: '" << showCharEscaped(c) << "')" << std::endl;
    switch (c) {
        case '(':
            {
                match(in, '(');
                auto nfa1 = lexE(in);
                match(in, ')');
                //std::cout << "lexG out: " << show(nfa1) << std::endl;
                return nfa1;
            }
        case '\\':
            {
                match(in, '\\');
                auto out = lexH(in);
                //std::cout << "lexG out: " << show(out) << std::endl;
                return out;
            }
        case '|':
        case '\0':
        case ')':
        case '*':
        case '?':
        case '+':
            throw std::runtime_error("lexG Error: '" + printEscaped2(c) + "'");
        default:
            {
                match(in, c);
                auto out = singletonLexer(c);
                //std::cout << "lexG out: " << show(out) << std::endl;
                return out;
            }
    }
}

template<typename Symbol, typename State, int64_t MaxSymbols, typename Graph>
constexpr auto nfaBuilder<Symbol,State,MaxSymbols,Graph>::lexFR(const char*& in, const Pair& nfa1) -> Pair {
    auto c = *in;
    //std::cout << "lexFR(" << show(nfa1) << ") (peek: '" << showCharEscaped(c) << "')" << std::endl;
    switch (c) {
        case '*':
            {
                match(in, '*');
                auto nfa2 = closure(nfa1);
                auto out = lexFR(in, nfa2);
                //std::cout << "lexFR out: " << show(out) << std::endl;
                return out;
            }
        case '?':
            {
                match(in, '?');
                auto nfa2 = opt(nfa1);
                auto out = lexFR(in, nfa2);
                //std::cout << "lexFR out: " << show(out) << std::endl;
                return out;
            }
        case '+':
            {
                throw std::runtime_error("+: not implemented yet");
                //match(in, '+');
                //auto nfa2 = atleastone(nfa1); // @TODO this is harder than it looks because I need to "copy" nfa1
                //auto out = lexFR(in, nfa2);
                ////std::cout << "lexFR out: " << show(out) << std::endl;
                //return out;
            }
        default:
            //std::cout << "lexFR out: " << show(nfa1) << std::endl;
            return nfa1;
    }
}

template<typename Symbol, typename State, int64_t MaxSymbols, typename Graph>
constexpr auto nfaBuilder<Symbol,State,MaxSymbols,Graph>::lexF(const char*& in) -> Pair {
    auto c = *in;
    //std::cout << "lexF (peek: '" << showCharEscaped(c) << "')" << std::endl;
    switch (c) {
        case '|':
        case '\0':
        case ')':
        case '*':
        case '?':
        case '+':
            throw std::runtime_error("lexF Error: '" + printEscaped2(c) + "'");
        default:
            {
                auto nfa1 = lexG(in);
                auto nfa2 = lexFR(in, nfa1);
                //std::cout << "lexF out: " << show(nfa2) << std::endl;
                return nfa2;
            }
    }
}

template<typename Symbol, typename State, int64_t MaxSymbols, typename Graph>
constexpr auto nfaBuilder<Symbol,State,MaxSymbols,Graph>::lexTR(const char*& in, const Pair& nfa1) -> Pair {
    auto c = *in;
    //std::cout << "lexTR(" << show(nfa1) << ") (peek: '" << showCharEscaped(c) << "')" << std::endl;
    switch (c) {
        case '|':
        case '\0':
        case ')':
            //std::cout << "lexTR out: " << show(nfa1) << std::endl;
            return nfa1;
        case '*':
        case '?':
        case '+':
            throw std::runtime_error("lexTR Error: '" + printEscaped2(c) + "'");
        default:
            {
                auto nfa2 = lexF(in);
                auto nfa3 = cat(nfa1, nfa2);
                auto nfa4 = lexTR(in, nfa3);
                //std::cout << "lexTR out: " << show(nfa4) << std::endl;
                return nfa4;
            }
    }
}

template<typename Symbol, typename State, int64_t MaxSymbols, typename Graph>
constexpr auto nfaBuilder<Symbol,State,MaxSymbols,Graph>::lexT(const char*& in) -> Pair {
    auto c = *in;
    //std::cout << "lexT (peek: '" << showCharEscaped(c) << "')" << std::endl;
    switch (c) {
        case '|':
        case '\0':
        case ')':
        case '*':
        case '?':
        case '+':
            throw std::runtime_error("lexT Error: '" + printEscaped2(c) + "'");
        default:
            {
                auto nfa1 = lexF(in);
                auto nfa2 = lexTR(in, nfa1);
                //std::cout << "lexT out: " << show(nfa2) << std::endl;
                return nfa2;
            }
    }
}

template<typename Symbol, typename State, int64_t MaxSymbols, typename Graph>
constexpr auto nfaBuilder<Symbol,State,MaxSymbols,Graph>::lexER(const char*& in, const Pair& nfa1) -> Pair {
    auto c = *in;
    //std::cout << "lexER(" << show(nfa1) << ") (peek: '" << showCharEscaped(c) << "')" << std::endl;
    switch (c) {
        case '|':
            {
                match(in, '|');
                auto nfa2 = lexT(in);

                auto nfa3 = alt(nfa1, nfa2);

                auto nfa4 = lexER(in, nfa3);
                //std::cout << "lexER out: " << show(nfa4) << std::endl;
                return nfa4;
            }
        case '\0':
        case ')':
            //std::cout << "lexER out: " << show(nfa1) << std::endl;
            return nfa1;
        default:
            throw std::runtime_error("lexER Error: '" + printEscaped2(c) + "'");
    }
}

template<typename Symbol, typename State, int64_t MaxSymbols, typename Graph>
constexpr auto nfaBuilder<Symbol,State,MaxSymbols,Graph>::lexE(const char*& in) -> Pair {
    auto c = *in;
    //std::cout << "lexE (peek: '" << showCharEscaped(c) << "')" << std::endl;
    switch (c) {
        case '|':
        case '\0':
        case ')':
        case '*':
        case '?':
        case '+':
            throw std::runtime_error("lexE Error: '" + printEscaped2(c) + "'");
        default:
            {
                auto nfa1 = lexT(in);
                auto nfa2 = lexER(in, nfa1);
                //std::cout << "lexE out: " << show(nfa2) << std::endl;
                return nfa2;
            }
    }
}

template<typename Symbol, typename State, int64_t MaxSymbols, typename Graph>
template<typename TokenId> // TODO: get TokenId from Graph to ensure compatibility
constexpr void nfaBuilder<Symbol,State,MaxSymbols,Graph>::lexRegex(const char* in, TokenId kind) {
    //std::cout << "entered lexRegex" << std::endl;
    Pair nfa1 = lexE(in);
    assert(symbolToId.at(eps) == 0);
    ns.addEpsilonEdge(start, nfa1.first);
    ns.setKind(nfa1.second, kind);
    //std::cout << "done with lexRegex" << std::endl;
    //std::cout << "out: (" << nfa1.first << "," << nfa1.second << ")" << std::endl;
}
