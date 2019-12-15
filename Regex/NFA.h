#pragma once

#include "Regex/NFA_fwd.h"
#include "Regex/Print.h"
#include "Regex/nfaBuilder.h"
//#include "Regex/BitSet.h"
#include "Regex/HashSet.h"
#include <iostream>
#include <numeric>
#include <tuple>
#include <unordered_map>
#include <vector>

template <typename Symbol, typename State, typename TokenId>
struct NFA {
    using BitSet = HashSet;
    Symbol eps;
    Symbol symbolCount;
    State stateCount;
    State start;
    std::vector<TokenId> finals;
    std::vector<std::vector<BitSet>> table;
    std::vector<Symbol> symbols;
    mutable BitSet newStates;
    mutable std::vector<unsigned int> stack;

    NFA(size_t _symbolCount, size_t _stateCount, Symbol _eps, State _start, std::vector<TokenId> _final);

    NFA(const nfaBuilder<Symbol, State, TokenId>& nfa);

    void getClosure(BitSet& s) const;
};

template <typename Symbol, typename State, typename TokenId>
NFA<Symbol, State, TokenId>::NFA(
    size_t _symbolCount, size_t _stateCount, Symbol _eps, State _start, std::vector<TokenId> _final)
    : eps(_eps),
      symbolCount(_symbolCount),
      stateCount(_stateCount),
      start(_start),
      finals(std::move(_final)),
      symbols(symbolCount),
      newStates(stateCount) {
    for (auto i = Symbol(); i < symbolCount; i++)
        symbols[i] = i;
};

template <typename Symbol, typename State, typename TokenId>
std::ostream& operator<<(std::ostream&, const NFA<Symbol, State, TokenId>&);

template <typename Symbol, typename State, typename TokenId>
void NFA<Symbol, State, TokenId>::getClosure(BitSet& S) const {
    // std::cout << "getClosure(" << S << ")" << std::endl;
    stack.insert(stack.end(), S.begin(), S.end());

    while (!stack.empty()) {
        // std::cout << "stack: " << show(stack) << std::endl;
        unsigned int q = stack.back();
        stack.pop_back();
        // std::cout << "considering state " << q << std::endl;
        const BitSet& T = table[q][eps];
        // std::cout << "delta(" << q << ",eps) = " << T << std::endl;

        // newStates = BitSetComplement(S);
        // newStates &= T;
        a_and_not_b(T, S, newStates);

        // BitSet newStates = T & ~S;
        S |= newStates;
        // std::cout << "newStates: " << newStates << std::endl;
        stack.insert(stack.end(), newStates.begin(), newStates.end());
    }
}

template <typename Symbol, typename State, typename TokenId>
NFA<Symbol, State, TokenId>::NFA(const nfaBuilder<Symbol, State, TokenId>& nfa)
    : eps(nfa.eps),
      symbolCount(static_cast<Symbol>(nfa.symbolToId.size())),
      stateCount(static_cast<State>(nfa.ns.size())),
      start(nfa.start),
      finals(stateCount),
      table(stateCount, std::vector<BitSet>(symbolCount, BitSet(stateCount, false))),
      symbols(nfa.idToSymbol),
      newStates(nfa.ns.size()) {
    assert(nfa.symbolToId.size() < static_cast<std::size_t>(std::numeric_limits<Symbol>::max()));
    assert(nfa.ns.size() < static_cast<std::size_t>(std::numeric_limits<State>::max()));
    std::cout << "constructing NFA from nfaBuilder" << std::endl;
    std::cout << "stateCount: " << stateCount << std::endl;
    std::cout << "start: " << nfa.start << std::endl;
    std::cout << "symbolCount: " << symbolCount << std::endl;
    assert(symbols.size() == symbolCount);
    //  std::cout << "symbols: " << show(nfa.idToSymbol) << std::endl;

    for (unsigned int p = 0; p < stateCount; p++) {
        for (auto& t : nfa.ns[p].ns) {
            // std::cout << "d(" << p << "," << nfa.idToSymbol[t.first] << ") = " << show(t.second) << std::endl;
            for (auto& q : t.second) {
                table[p][t.first][q] = true;
            }
            // std::cout << "T[" << p << "][" << t.first << "] = " << table[p][t.first] << std::endl;
        }
        // std::cout << "kind(" << p << ") = " << nfa.ns[p].kind << std::endl;
        finals[p] = nfa.ns[p].kind;
    }
}

template <typename Symbol, typename State, typename TokenId>
std::ostream& operator<<(std::ostream& s, const NFA<Symbol, State, TokenId>& nfa) {
    s << "digraph G {\n";
    for (unsigned int p = 0; p < nfa.stateCount; p++) {
        if (nfa.finals[p])
            s << "  " << p << "[label = \"" << p << '|' << nfa.finals[p] << "\" shape = doublecircle];\n";
        for (auto a = 0u; a < nfa.symbolCount; ++a) {
            if (nfa.table[p][a].count() > 0) {
                s << "  " << p << " -> { ";
                auto q = nfa.table[p][a].begin();
                auto q_end = nfa.table[p][a].end();
                if (q != q_end) {
                    s << *q;
                    ++q;
                    for (; q != q_end; ++q) {
                        s << ", " << *q;
                    }
                }
                // s << " } [label = \"" << showCharEscaped(nfa.symbols[a]) << "\"];\n";
                s << " } [label = \"" << int(nfa.symbols[a]) << "\"];\n";
            }
        }
    }
    s << "}\n";
    return s;
}
