#pragma once

#include "BitSet_constexpr.h"
#include "HashSet_constexpr.h"
#include "Print.h"
#include "constexpr.h"
#include "nfaBuilder_constexpr.h"

#include <iostream>

template <typename Symbol, typename State, typename TokenId, int64_t MaxNodes, int64_t MaxSymbols,
          int64_t MaxTransitions, int64_t MaxResultStates>
struct NFA {
    // using Set = HashSet<MaxResultStates>;
    using Set = BitSet<MaxNodes>;
    using internal_set = HashSet<MaxResultStates>;

    Symbol eps;
    int64_t symbolCount, stateCount;
    State start;
    vector<TokenId, MaxNodes> finals;
    vector<vector<internal_set, MaxNodes>, MaxSymbols> table; // MaxTransitions instead of MaxSymbols?
    vector<Symbol, MaxSymbols> symbols;
    vector<Set, MaxNodes> closures;

    // constexpr NFA(int64_t _symbolCount, int64_t _stateCount, Symbol _eps, State _start, vector<TokenId,MaxNodes>
    // _final) : eps(_eps), symbolCount(_symbolCount), stateCount(_stateCount), start(_start), finals(std::move(_final)),
    // symbols(symbolCount) {
    //    for (auto i = Symbol(); i < symbolCount; i++)
    //        symbols[i] = i;
    //};

    constexpr NFA(const nfaBuilder<Symbol, State, MaxSymbols,
                                   Graph<State, TokenId, MaxNodes, MaxTransitions, MaxResultStates>>& nfa);
    template <typename SetIn, typename SetOut>
    constexpr void getClosure(const SetIn& s, SetOut& out) const;

    // constexpr void getClosure(Set& s) const;
  private: // only made private to find remaing call sites
    constexpr void getClosure(Set& s, HashSet<MaxTransitions>& newStates,
                              vector<typename Set::value_type, MaxNodes>& stack) const;
};

template <typename Symbol, typename State, typename TokenId, int64_t MaxNodes, int64_t MaxSymbols,
          int64_t MaxTransitions, int64_t MaxResultStates>
constexpr NFA<Symbol, State, TokenId, MaxNodes, MaxSymbols, MaxTransitions, MaxResultStates>::NFA(
    const nfaBuilder<Symbol, State, MaxSymbols, Graph<State, TokenId, MaxNodes, MaxTransitions, MaxResultStates>>& nfa)
    : eps(nfa.eps), symbolCount(nfa.symbolToId.size()), stateCount(nfa.ns.getStateCount()), start(nfa.start),
      finals(stateCount),
      table(symbolCount, vector<internal_set, MaxNodes>(stateCount, internal_set(stateCount, false))),
      symbols(nfa.idToSymbol) {
    //  cout << "symbols: " << show(nfa.idToSymbol) << std::endl;

    // workaround for clang bug in vector initialization ... could not assign to const Set in v[0] -.-*
    closures.reserve(stateCount);

    for (State p = 0; p < stateCount; p++) {
        for (auto& t : nfa.ns.ns_[p]) {
            // std::cout << "d(" << p << "," << showCharEscaped(nfa.idToSymbol[t.first]) << ") = " << show(t.second) <<
            // std::endl;
            auto& tptr = table[t.first][p];
            for (auto& q : t.second) {
                tptr.set(q);
            }
            // std::cout << "T[" << p << "][" << t.first << "] = " << table[t.first][p] << std::endl;
        }
        // cout << "kind(" << p << ") = " << nfa.ns[p].kind << std::endl;
        finals[p] = nfa.ns.getKind(p);
    }

    Set S(stateCount);
    HashSet<MaxTransitions> newStates(stateCount);
    vector<typename Set::value_type, MaxNodes> stack;
    for (State p = 0; p < stateCount; p++) {
        S.set(p);
        getClosure(S, newStates, stack);
        closures.push_back(S);
        // std::cout << "closure[" << p << "] = " << closures[p] << std::endl;
        S.clear();
    }
}

template <typename Symbol, typename State, typename TokenId, int64_t MaxNodes, int64_t MaxSymbols,
          int64_t MaxTransitions, int64_t MaxResultStates>
template <typename SetIn, typename SetOut>
constexpr void
NFA<Symbol, State, TokenId, MaxNodes, MaxSymbols, MaxTransitions, MaxResultStates>::getClosure(const SetIn& S,
                                                                                               SetOut& out) const {
    out.clear();
    // std::cout << "getClosure(" << S << ")" << std::endl;
    for (auto p : S) {
        // std::cout << "closure[" << p << "] = " << closures[p] << std::endl;
        out |= closures[p];
        // std::cout << "intermediate result: " << out << std::endl;
    }
    // std::cout << "final result: " << out << std::endl;
}

template <typename Symbol, typename State, typename TokenId, int64_t MaxNodes, int64_t MaxSymbols,
          int64_t MaxTransitions, int64_t MaxResultStates>
constexpr void NFA<Symbol, State, TokenId, MaxNodes, MaxSymbols, MaxTransitions, MaxResultStates>::getClosure(
    Set& S, HashSet<MaxTransitions>& newStates, vector<typename Set::value_type, MaxNodes>& stack) const {
    // cout << "getClosure(" << S << ")" << std::endl;
    // Set newStates(stateCount);
    // HashSet<MaxNodes> newStates(stateCount);
    // vector<typename Set::value_type,MaxNodes> stack;

    stack.clear();
    stack.insert(stack.end(), S.begin(), S.end());

    const auto& eps_table = table[eps];
    while (!stack.empty()) {
        // cout << "stack: " << show(stack) << std::endl;
        unsigned int q = stack.back();
        stack.pop_back();
        // cout << "considering state " << q << std::endl;
        const auto& T = eps_table[q];
        if (T.empty())
            continue;
        // cout << "delta(" << q << ",eps) = " << T << std::endl;

        // newStates = BitSetComplement(S);
        // newStates &= T;
        a_and_not_b(T, S, newStates);

        // Set newStates = T & ~S;
        S |= newStates;
        // cout << "newStates: " << newStates << std::endl;
        stack.insert(stack.end(), newStates.begin(), newStates.end());
    }

    // const auto first = stack.begin(); // we do not reallocate ...
    // while (!stack.empty()) {
    //    //cout << "stack: " << show(stack) << std::endl;
    //    auto it = first;
    //    const auto stop = stack.end();
    //    assign(newStates, eps_table[*it++]);
    //    for (; it != stop; ++it) {
    //        //cout << "considering state " << q << std::endl;
    //        //cout << "delta(" << q << ",eps) = " << eps_table[q] << std::endl;
    //        newStates |= eps_table[*it];
    //    }
    //    stack.clear();
    //    newStates -= S;

    //    S |= newStates;
    //    //cout << "newStates: " << newStates << std::endl;
    //    stack.insert(stack.end(), newStates.begin(), newStates.end());
    //}
}

template <typename Symbol, typename State, typename TokenId, int64_t MaxNodes, int64_t MaxSymbols,
          int64_t MaxTransitions, int64_t MaxResultStates>
std::ostream&
operator<<(std::ostream& s,
           const NFA<Symbol, State, TokenId, MaxNodes, MaxSymbols, MaxTransitions, MaxResultStates>& nfa) {
    s << "digraph G {\n";
    for (unsigned int p = 0; p < nfa.stateCount; p++) {
        if (nfa.finals[p])
            s << "  " << p << "[shape = doublecircle];\n";
        for (auto a = 0u; a < nfa.symbolCount; ++a) {
            if (nfa.table[a][p].count() > 0) {
                s << "  " << p << " -> { ";
                auto q = nfa.table[a][p].begin();
                auto q_end = nfa.table[a][p].end();
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
