#pragma once
#include <DFA_constexpr.h>
#include <NFA_constexpr.h>
//#include <HashSet_constexpr.h>
#include <BitSet_constexpr.h>
#include <iostream>

template<typename DfaState, typename Symbol, typename NfaState, typename TokenId, int64_t MaxNodes, int64_t MaxSymbols, int64_t MaxTransitions, int64_t MaxResultStates>
constexpr DFA<Symbol,DfaState,TokenId,MaxNodes,MaxSymbols> toDFA(const NFA<Symbol,NfaState,TokenId,MaxNodes,MaxSymbols,MaxTransitions,MaxResultStates>& nfa)
{
    using SymbolIndex = int64_t;
    auto symbolCount(nfa.symbolCount - 1);
    vector<SymbolIndex,MaxSymbols> symbolToId(128, symbolCount);
    if (symbolCount > std::numeric_limits<Symbol>::max())
        throw std::range_error("toDFA: symbolCount");
    vector<Symbol,MaxSymbols> idToSymbol(128, static_cast<Symbol>(symbolCount));

    vector<DfaState,MaxNodes*MaxSymbols> T;

    using States = HashSet<MaxNodes>;
    //std::cout << "DFA constructor" << std::endl;
    //std::cout << "stateCount = " << nfa.stateCount << std::endl;
    //std::cout << "symbolCount = " << symbolCount << std::endl;
    vector<SymbolIndex,MaxSymbols> symbolMap(nfa.symbolCount);
    SymbolIndex nonEpsSymbol = 0;
    for (auto i = SymbolIndex(); i < nfa.symbolCount; i++) {
        if (i != nfa.eps) { // @FIXME: this implies that NFA::eps is actually a SymbolIndex
            symbolMap[i] = nonEpsSymbol;
            symbolToId[nfa.symbols[i]] = nonEpsSymbol;
            idToSymbol[nonEpsSymbol] = nfa.symbols[i];
            nonEpsSymbol++;
        }
    }

    States getClosure_output(nfa.stateCount);
    States U(nfa.stateCount);

    auto n = nfa.stateCount;
    DfaState id = 0;
    vector<DfaState,MaxNodes> stack; /// @FIXME: (non-minimized) DFA MaxNodes can be exponentially larger than NFA MaxNodes
    vector<States,MaxNodes> idToState;
    //unordered_map<States,DfaState,MaxNodes> stateToId;
    HashMap<States,DfaState,MaxNodes> stateToId;
    idToState.emplace_back(n, false);
    States& S = idToState.back();
    S.set(nfa.start);
    //  std::cout << "nfa.start: " << nfa.start << std::endl;
    //  std::cout << "nfa.finals" << show(nfa.finals) << std::endl;
    //  std::cout << "s0: " << S << std::endl;
    //nfa.getClosure(S, getClosure_newStates, getClosure_stack);
    nfa.getClosure(S, getClosure_output);
    S = getClosure_output;
    //  std::cout << "closure(s0): " << S << std::endl;
    stateToId[getClosure_output] = id;
    stack.push_back(id);
    auto start = id;
    ++id;

    using nfa_internal_set = typename NFA<Symbol,NfaState,TokenId,MaxNodes,MaxSymbols,MaxTransitions,MaxResultStates>::internal_set;

    while(!stack.empty()) {
        //std::cout << "stack: " << show(stack) << std::endl;
        auto q = stack.back();
        stack.pop_back();
        //auto& _p = idToState[q];
        //std::cout << "state: " << _p << std::endl;
        if (T.size() < (q + 1) * symbolCount)
            T.resize((q + 1) * symbolCount);
        for (SymbolIndex a = 0; a < nfa.symbolCount; a++) {
            if (a != nfa.eps) {
                const nfa_internal_set* t_ptr = nfa.table[a].data();
                const auto& p = idToState[q];
                //std::cout << "Constructing delta(" << p << "," << a << ")" << std::endl;
                U.clear();
                for (auto s : p) {
                    //std::cout << "collecting T[" << s << "][" << a << "] = " << t_ptr[s] << std::endl;
                    U |= t_ptr[s];
                }
                //const auto stop = p.end().c;
                //for (auto it = p.begin(); it.c != stop; ++it) {
                //    //std::cout << "collecting T[" << s << "][" << a << "] = " << t_ptr[s] << std::endl;
                //    U |= t_ptr[it.c];
                //}
                //std::cout << "targets: " << U << std::endl;
                //        if (U.count() > 1)
                //          cin.get();
                //nfa.getClosure(U, getClosure_newStates, getClosure_stack);
                nfa.getClosure(U, getClosure_output);
                //std::cout << "closure: " << getClosure_output << std::endl;
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
                //std::cout << "stateToId.find(U);" << std::endl;
                auto it = stateToId.find(getClosure_output);
                if (it == stateToId.end()) {
                    //std::cout << "idToState.emplace_back(std::move(U));" << std::endl;
                    auto& UU = idToState.emplace_back(std::move(getClosure_output));
                    //std::cout << "auto& UU = idToState.back();" << std::endl;
                    //std::cout << "stateToId[UU] = id;" << std::endl;
                    stateToId[UU] = id;
                    //std::cout << "stack.push_back(id);" << std::endl;
                    stack.push_back(id);
                    //std::cout << "T[q][symbolMap[a]] = id;" << std::endl;
                    T[q * symbolCount + symbolMap[a]] = id;
                    //std::cout << "new state; id = " << id << std::endl;
                    id++;
                } else {
                    //std::cout << "seen before; id = " << it->second << std::endl;
                    T[q * symbolCount + symbolMap[a]] = it->second;
                }
            }
        }
    }
    auto stateCount = id;
    //std::cout << "final state count: " << stateCount << std::endl;
    //  std::cout << "checking for terminal states" << std::endl;
    const auto not_final = TokenId{};
    vector<TokenId,MaxNodes> finals(stateCount, not_final);
    for (DfaState q = 0; q < stateCount; q++) {
        const States& UU = idToState[q];
        //    std::cout << "checking dfa state " << q << ": " << UU << std::endl;
        //auto first_nfa_state = std::numeric_limits<NfaState>::max();
        for (NfaState s : UU) {
            //std::cout << "checking nfa state " << *s << std::endl;

            if (nfa.finals[s] != not_final) {
                //if (finals[q] == not_final) {
                //    first_nfa_state = s;
                //    finals[q] = nfa.finals[s];
                //    //std::cout << "dfa.finals[" << q << "] = " << "nfa.finals[" << s << "] = " << int(nfa.finals[s]) << std::endl;
                //} else if (s < first_nfa_state) { // required because UU can be an unordered set
                //    first_nfa_state = s;
                //    finals[q] = nfa.finals[s];
                //    //std::cout << "dfa.finals[" << q << "] is ambiguous. Preferring nfa.finals[" << s << "] = " << int(nfa.finals[s]) << std::endl;
                //} else {
                //    //std::cout << "dfa.finals[" << q << "] is ambiguous. Ignoring nfa.finals[" << s << "] = " << int(nfa.finals[s]) << std::endl;
                //} // are there more cases?
                if (finals[q] == not_final) {
                    finals[q] = nfa.finals[s];
                    //std::cout << "dfa.finals[" << q << "] = " << "nfa.finals[" << s << "] = " << int(nfa.finals[s]) << std::endl;
                } else {
                    finals[q] = std::min(finals[q], nfa.finals[s]);
                }
            }
        }
    }
    auto deadState = determineDeadState(stateCount, symbolCount, T, finals);
    //return DFA<Symbol,DfaState,TokenId,MaxNodes,MaxSymbols>(
    //        stateCount,
    //        symbolCount,
    //        start,
    //        deadState,
    //        std::move(finals),
    //        std::move(T),
    //        symbolToId,
    //        idToSymbol);
    return DFA<Symbol,DfaState,TokenId,MaxNodes,MaxSymbols>(
            stateCount,
            symbolCount,
            start,
            deadState,
            finals,
            T,
            symbolToId,
            idToSymbol);
}

