#ifndef _NFA_H_
#define _NFA_H_

#include <NFA_fwd.h>
#include <nfaBuilder_fwd.h>
#include "Print.h"
//#include <BitSet.h>
#include <HashSet.h>
#include <unordered_map>
using std::unordered_map;
#include <vector>
using std::vector;
#include <tuple>
using std::pair;
#include <iostream>
using std::cout;
#include <numeric>

template<typename Symbol, typename State, typename TokenId>
struct NFA {
    using BitSet = HashSet;
    Symbol eps;
    size_t symbolCount, stateCount;
    State start;
    vector<TokenId> final;
    vector<vector<BitSet>> table;
    vector<Symbol> symbols;
    mutable BitSet newStates;
    mutable vector<unsigned int> stack;
    NFA(size_t _symbolCount, size_t _stateCount, Symbol _eps, State _start, vector<TokenId> _final) : eps(_eps), symbolCount(_symbolCount), stateCount(_stateCount), start(_start), final(std::move(_final)), symbols(symbolCount), newStates(stateCount) {
        for (auto i = Symbol(); i < symbolCount; i++)
            symbols[i] = i;
    };
    NFA(const nfaBuilder<Symbol,State,TokenId>& nfa);
    void getClosure(BitSet& s) const;
};

template<typename Symbol, typename State, typename TokenId>
std::ostream& operator<<(std::ostream&, const NFA<Symbol,State,TokenId>&);

template<typename Symbol, typename State, typename TokenId>
void NFA<Symbol,State,TokenId>::getClosure(BitSet& S) const {
    //cout << "getClosure(" << S << ")" << endl;
    stack.insert(stack.end(), S.begin(), S.end());

    while (!stack.empty()) {
        //cout << "stack: " << show(stack) << endl;
        unsigned int q = stack.back();
        stack.pop_back();
        //cout << "considering state " << q << endl;
        const BitSet& T = table[q][eps];
        //cout << "delta(" << q << ",eps) = " << T << endl;

        //newStates = BitSetComplement(S);
        //newStates &= T;
        a_and_not_b(T, S, newStates);

        //BitSet newStates = T & ~S;
        S |= newStates;
        //cout << "newStates: " << newStates << endl;
        stack.insert(stack.end(), newStates.begin(), newStates.end());
    }
}

template<typename Symbol, typename State, typename TokenId>
NFA<Symbol,State,TokenId>::NFA(const nfaBuilder<Symbol,State,TokenId>& nfa) : eps(nfa.eps), symbolCount(nfa.symbolToId.size()), stateCount(nfa.ns.size()), start(nfa.start), final(stateCount), table(stateCount, vector<BitSet>(symbolCount, BitSet(stateCount, false))), symbols(nfa.idToSymbol), newStates(nfa.ns.size()) {
    cout << "constructing NFA from nfaBuilder" << endl;
    cout << "stateCount: " << stateCount << endl;
    cout << "start: " << nfa.start << endl;
    cout << "symbolCount: " << symbolCount << endl;
    //  cout << "symbols: " << show(nfa.idToSymbol) << endl;

    for (unsigned int p = 0; p < stateCount; p++) {
        for (auto& t : nfa.ns[p].ns) {
            //cout << "d(" << p << "," << nfa.idToSymbol[t.first] << ") = " << show(t.second) << endl;
            for (auto& q : t.second) {
                table[p][t.first][q] = true;
            }
            //cout << "T[" << p << "][" << t.first << "] = " << table[p][t.first] << endl;
        }
        //cout << "kind(" << p << ") = " << nfa.ns[p].kind << endl;
        final[p] = nfa.ns[p].kind;
    }
}



template<typename Symbol, typename State, typename TokenId>
std::ostream& operator<<(std::ostream& s, const NFA<Symbol,State,TokenId>& nfa) {
    s << "digraph G {\n";
    for (unsigned int p = 0; p < nfa.stateCount; p++) {
        if (nfa.final[p])
            s << "  " << p << "[shape = doublecircle];\n";
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
                s << " } [label = \"" << showCharEscaped(nfa.symbols[a]) << "\"];\n";
            }
        }
    }
    s << "}\n";
    return s;
}
#endif
