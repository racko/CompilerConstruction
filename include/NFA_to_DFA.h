#pragma once

#include "HashSet.h"     // for HashSet, hash
#include <DFA.h>         // for DFA, determineDeadState
#include <cstdint>       // for SIZE_MAX
#include <iostream>      // for operator<<, size_t, endl, cout, ostream
#include <unordered_map> // for unordered_map, operator==, _Node_iterator
#include <utility>       // for move, pair
#include <vector>        // for vector
template <typename Symbol, typename State, typename TokenId>
struct NFA;

template <typename Symbol, typename State, typename TokenId>
DFA<Symbol, State, TokenId> toDFA(const NFA<Symbol, State, TokenId>& nfa) {
    std::vector<State> T;
    const Symbol symbolCount(nfa.symbolCount - 1);
    std::vector<std::size_t> symbolToId(128, symbolCount);
    std::vector<Symbol> idToSymbol(128, symbolCount);
    using BitSet = HashSet;
    std::cout << "DFA constructor" << std::endl;
    std::cout << "stateCount = " << nfa.stateCount << std::endl;
    std::cout << "symbolCount = " << symbolCount << std::endl;
    std::vector<std::size_t> symbolMap(nfa.symbolCount);
    std::size_t nonEpsSymbol = 0;
    for (auto i = Symbol(); i < nfa.symbolCount; i++)
        if (i != nfa.eps) {
            symbolMap[i] = nonEpsSymbol;
            symbolToId[nfa.symbols[i]] = nonEpsSymbol;
            idToSymbol[nonEpsSymbol] = nfa.symbols[i];
            nonEpsSymbol++;
        }

    unsigned int n = nfa.stateCount;
    unsigned int id = 0;
    std::vector<unsigned int> stack;
    std::vector<BitSet> idToState;
    std::unordered_map<BitSet, unsigned int> stateToId;
    idToState.emplace_back(n, false);
    BitSet& S = idToState.back();
    S[nfa.start] = true;
    //  std::cout << "nfa.start: " << nfa.start << std::endl;
    //  std::cout << "nfa.finals" << show(nfa.finals) << std::endl;
    //  std::cout << "s0: " << S << std::endl;
    nfa.getClosure(S);
    //  std::cout << "closure(s0): " << S << std::endl;
    stateToId[S] = id;
    stack.push_back(id);
    const State start = id;
    id++;

    while (!stack.empty()) {
        // std::cout << "stack: " << show(stack) << std::endl;
        unsigned int q = stack.back();
        stack.pop_back();
        // auto& _p = idToState[q];
        // std::cout << "state: " << _p << std::endl;
        if (T.size() < (q + 1) * symbolCount)
            T.resize((q + 1) * symbolCount);
        for (unsigned int a = 0; a < nfa.symbolCount; a++) {
            if (a != nfa.eps) {
                const auto& p = idToState[q];
                // std::cout << "Constructing delta(" << p << "," << a << ")" << std::endl;
                BitSet U(nfa.stateCount, false);
                for (auto s : p) {
                    // std::cout << "collecting T[" << *s << "][" << a << "] = " << nfa.table[*s][a] << std::endl;
                    U |= nfa.table[s][a];
                }
                // std::cout << "targets: " << U << std::endl;
                //        if (U.count() > 1)
                //          cin.get();
                nfa.getClosure(U);
                // std::cout << "closure: " << U << std::endl;
                //        if (U.count() > 1)
                //          cin.get();
                //        std::cout << "idToState: " << std::endl;
                //        for (auto& i: idToState) {
                //          auto it = stateToId.find(i);
                //          std::cout << i << " (";
                //          if (it != stateToId.end())
                //            std::cout << it->first << ": " << it->second;
                //          else
                //            std::cout << "not found";
                //          std::cout << ")" << std::endl;
                //        }
                //        std::cout << "stateToId: " << std::endl;
                //        for (auto& i: stateToId) {
                //          BitSet localCopy(i.first);
                //          std::cout
                //            << i.second << ": "
                //            << localCopy << std::endl;
                //        }
                // std::cout << "stateToId.find(U);" << std::endl;
                auto it = stateToId.find(U);
                if (it == stateToId.end()) {
                    // std::cout << "idToState.emplace_back(std::move(U));" << std::endl;
                    idToState.emplace_back(std::move(U));
                    // std::cout << "auto& UU = idToState.back();" << std::endl;
                    auto& UU = idToState.back();
                    // std::cout << "stateToId[UU] = id;" << std::endl;
                    stateToId[UU] = id;
                    // std::cout << "stack.push_back(id);" << std::endl;
                    stack.push_back(id);
                    // std::cout << "T[q][symbolMap[a]] = id;" << std::endl;
                    T[q * symbolCount + symbolMap[a]] = id;
                    // std::cout << "new state; id = " << id << std::endl;
                    id++;
                } else {
                    // std::cout << "seen before; id = " << it->second << std::endl;
                    T[q * symbolCount + symbolMap[a]] = it->second;
                }
            }
        }
    }
    const State stateCount = id;
    std::cout << "final state count: " << stateCount << std::endl;
    //  std::cout << "checking for terminal states" << std::endl;
    std::vector<TokenId> finals(stateCount, 0);
    for (unsigned int q = 0; q < stateCount; q++) {
        const BitSet& U = idToState[q];
        //    std::cout << "checking dfa state " << q << ": " << U << std::endl;
        std::size_t first_nfa_state = SIZE_MAX;
        for (auto s : U) {
            // std::cout << "checking nfa state " << *s << std::endl;
            if (nfa.finals[s] != 0 && finals[q] == 0) {
                first_nfa_state = s;
                finals[q] = nfa.finals[s];
                // std::cout << "dfa.finals[" << q << "] = " << "nfa.finals[" << s << "] = " << int(nfa.finals[s]) <<
                // std::endl;
            } else if (nfa.finals[s] != 0 && s < first_nfa_state) {
                first_nfa_state = s;
                finals[q] = nfa.finals[s];
                // std::cout << "dfa.finals[" << q << "] is ambiguous. Preferring nfa.finals[" << s << "] = " <<
                // int(nfa.finals[s]) << std::endl;
            } else if (nfa.finals[s] != 0) {
                // std::cout << "dfa.finals[" << q << "] is ambiguous. Ignoring nfa.finals[" << s << "] = " <<
                // int(nfa.finals[s]) << std::endl;
            }
        }
    }

    State deadState = ::determineDeadState(stateCount, symbolCount, T, finals);
    return DFA<Symbol, State, TokenId>(
        DfaStates<State, TokenId>(
            stateCount, start, deadState, TokenIds<TokenId>(finals), Transitions<State>(stateCount, symbolCount, T)),
        Symbols<Symbol>{symbolCount, symbolToId, idToSymbol});
}

