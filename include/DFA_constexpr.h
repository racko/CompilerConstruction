#pragma once

#include "NumRange.h"  // for NumRange
#include "Print.h"     // for showCharEscaped
#include "constexpr.h" // for vector, all_of, find_if, pair
#include <cstdint>     // for int64_t
#include <ostream>     // for operator<<, ostream, basic_ostream, basic_ost...
#include <stdexcept>   // for runtime_error

using Position = int64_t;
using PositionRange = const_expr::pair<Position, Position>;

template <typename State, typename TokenId, int64_t Size1, int64_t Size2>
constexpr State determineDeadState(int64_t stateCount,
                                   int64_t symbolCount,
                                   const const_expr::vector<State, Size1>& T,
                                   const const_expr::vector<TokenId, Size2>& finals) {
    //  std::cout << "determineDeadState" << std::endl;
    auto idempotent = [&](State q) {
        const auto ptr = T.data() + q * symbolCount;
        return const_expr::all_of(ptr, ptr + symbolCount, [&](State q_) { return q_ == q; });
    };
    auto isDeadState = [&](State q) { return !finals[q] && idempotent(q); };
    NumRange<State> r(0, stateCount);
    auto it = const_expr::find_if(r.begin(), r.end(), isDeadState);
    if (it != r.end()) {
        return *it;
        // std::cout << deadState << " is the dead state" << std::endl;
    } else {
        // std::cout << "there is no dead state" << std::endl;
        throw std::runtime_error("there is no dead state");
    }
}

template <typename Symbol, typename State, typename TokenId, int64_t MaxNodes, int64_t MaxSymbols>
struct DFA {
    int64_t stateCount;  // TODO change to State lastState for safe comparisions
    int64_t symbolCount; // TODO change to Symbol lastSymbol ...
    State start;
    State deadState;
    const_expr::vector<TokenId, MaxNodes> finals;
    const_expr::vector<State, MaxNodes * MaxSymbols> T;
    const_expr::vector<int64_t, MaxSymbols> symbolToId;
    const_expr::vector<Symbol, MaxSymbols> idToSymbol;

    // constexpr DFA() = default;

    // constexpr DFA(
    //    int64_t stateCount,
    //    int64_t symbolCount,
    //    State start,
    //    State deadState,
    //    vector<TokenId,MaxNodes> finals,
    //    vector<State,MaxNodes*MaxSymbols> T,
    //    vector<int64_t,MaxSymbols> symbolToId,
    //    vector<Symbol,MaxSymbols> idToSymbol);
    constexpr DFA(int64_t stateCount,
                  int64_t symbolCount,
                  State start,
                  State deadState,
                  const const_expr::vector<TokenId, MaxNodes>& finals,
                  const const_expr::vector<State, MaxNodes * MaxSymbols>& T,
                  const const_expr::vector<int64_t, MaxSymbols>& symbolToId,
                  const const_expr::vector<Symbol, MaxSymbols>& idToSymbol);
};

// template<typename Symbol, typename State, typename TokenId, int64_t MaxNodes, int64_t MaxSymbols>
// constexpr DFA<Symbol,State,TokenId,MaxNodes,MaxSymbols>::DFA(
//        int64_t stateCount_,
//        int64_t symbolCount_,
//        State start_,
//        State deadState_,
//        vector<TokenId,MaxNodes> finals_,
//        vector<State,MaxNodes*MaxSymbols> T_,
//        vector<int64_t,MaxSymbols> symbolToId_,
//        vector<Symbol,MaxSymbols> idToSymbol_)
//    : stateCount(stateCount_),
//      symbolCount(symbolCount_),
//      start(start_),
//      deadState(deadState_),
//      finals(std::move(finals_)),
//      T(std::move(T_)),
//      symbolToId(std::move(symbolToId_)),
//      idToSymbol(std::move(idToSymbol_))
//{
//}

template <typename Symbol, typename State, typename TokenId, int64_t MaxNodes, int64_t MaxSymbols>
constexpr DFA<Symbol, State, TokenId, MaxNodes, MaxSymbols>::DFA(int64_t stateCount_,
                                                                 int64_t symbolCount_,
                                                                 State start_,
                                                                 State deadState_,
                                                                 const const_expr::vector<TokenId, MaxNodes>& finals_,
                                                                 const const_expr::vector<State, MaxNodes * MaxSymbols>& T_,
                                                                 const const_expr::vector<int64_t, MaxSymbols>& symbolToId_,
                                                                 const const_expr::vector<Symbol, MaxSymbols>& idToSymbol_)
    : stateCount(stateCount_),
      symbolCount(symbolCount_),
      start(start_),
      deadState(deadState_),
      finals(finals_),
      T(T_),
      symbolToId(symbolToId_),
      idToSymbol(idToSymbol_) {}

template <typename Symbol, typename State, typename TokenId, int64_t MaxNodes, int64_t MaxSymbols>
std::ostream& operator<<(std::ostream& s, const DFA<Symbol, State, TokenId, MaxNodes, MaxSymbols>& dfa) {
    s << "digraph G {\n";
    for (unsigned int p = 0; p < dfa.stateCount; p++) {
        if (p == dfa.deadState)
            continue;

        if (dfa.finals[p])
            s << "  " << p << "[shape = doublecircle];\n";
        const auto ptr = dfa.T.data() + p * dfa.symbolCount;
        for (auto a = 0u; a < dfa.symbolCount; ++a)
            if (ptr[a] != dfa.deadState)
                s << "  " << p << " -> " << int(ptr[a]) << " [label = \"" << showCharEscaped(dfa.idToSymbol[a])
                  << "\"];\n";
    }
    s << "}\n";
    return s;
}
