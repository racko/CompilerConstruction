#pragma once

#include "DFA_fwd.h"

#include <BitSet.h> // for BitSet
#include <boost/serialization/vector.hpp>
#include <cstddef>  // for NULL
#include <iostream> // for operator<<, ostream, size_t, basic_ostream, basi...
#include <iterator> // for random_access_iterator_tag
#include <ranges.h>
#include <utility> // for pair
#include <vector>  // for vector

namespace detail {
using Set = BitSet;

using Position = std::size_t;
using PositionRange = std::pair<Position, Position>;
} // namespace detail

template <typename State>
struct partition;

template <typename Symbol, typename State, typename TokenId>
struct DFA {
    State stateCount{};
    Symbol symbolCount{};
    State start{};
    State deadState{};
    std::vector<TokenId> finals;
    std::vector<State> T;
    std::vector<std::size_t> symbolToId;
    std::vector<Symbol> idToSymbol;

    DFA() = default;

    DFA(State stateCount,
        Symbol symbolCount,
        State start,
        State deadState,
        const std::vector<TokenId>& finals,
        const std::vector<State>& T,
        const std::vector<std::size_t>& symbolToId,
        const std::vector<Symbol>& idToSymbol);
};

template <typename Symbol, typename State, typename TokenId>
DFA<Symbol, State, TokenId>::DFA(State stateCount,
                                 Symbol symbolCount,
                                 State start,
                                 State deadState,
                                 const std::vector<TokenId>& finals,
                                 const std::vector<State>& T,
                                 const std::vector<std::size_t>& symbolToId,
                                 const std::vector<Symbol>& idToSymbol)
    : stateCount{stateCount},
      symbolCount{symbolCount},
      start{start},
      deadState{deadState},
      finals{finals},
      T{T},
      symbolToId{symbolToId},
      idToSymbol{idToSymbol} {}

template <typename Symbol, typename State, typename TokenId>
std::ostream& operator<<(std::ostream&, const DFA<Symbol, State, TokenId>&);

template <typename Symbol, typename State, typename TokenId>
State determineDeadState(const State stateCount,
                         const Symbol symbolCount,
                         const std::vector<State>& T,
                         const std::vector<TokenId>& finals) {
    //  std::cout << "determineDeadState" << std::endl;
    auto idempotent = [&](State q) {
        const auto ptr = T.data() + q * symbolCount;
        return std::all_of(ptr, ptr + symbolCount, [q](State q_) { return q_ == q; });
    };
    auto isDeadState = [&](State q) { return !finals[q] && idempotent(q); };
    auto stop = counting_iterator<State>(stateCount);
    auto it = std::find_if(counting_iterator<State>(0u), stop, isDeadState);
    if (it != stop) {
        std::cout << *it << " is the dead state" << std::endl;
        return *it;
    } else {
        std::cout << "there is no dead state" << std::endl;
        return stateCount;
    }
}

template <typename Symbol, typename State, typename TokenId>
bool operator==(const DFA<Symbol, State, TokenId>& lhs, const DFA<Symbol, State, TokenId>& rhs) {
    return lhs.stateCount == rhs.stateCount && lhs.symbolCount == rhs.symbolCount && lhs.start == rhs.start &&
           lhs.deadState == rhs.deadState && lhs.finals == rhs.finals && lhs.T == rhs.T &&
           lhs.symbolToId == rhs.symbolToId && lhs.idToSymbol == rhs.idToSymbol;
}

template <typename Symbol, typename State, typename TokenId>
std::ostream& operator<<(std::ostream& s, const DFA<Symbol, State, TokenId>& dfa) {
    s << "digraph G {\n";
    for (unsigned int p = 0; p < dfa.stateCount; p++) {
        if (p == dfa.deadState) {
            continue;
        }

        if (dfa.finals[p]) {
            s << "  " << p << "[label = \"" << p << '|' << dfa.finals[p] << "\" shape = doublecircle];\n";
        }
        const auto ptr = dfa.T.data() + p * dfa.symbolCount;
        for (auto a = 0u; a < dfa.symbolCount; ++a) {
            if (ptr[a] != dfa.deadState) {
                // s << "  " << p << " -> " << ptr[a] << " [label = \"" << showCharEscaped(dfa.idToSymbol[a]) <<
                // "\"];\n";
                s << "  " << p << " -> " << ptr[a] << " [label = \"" << int(dfa.idToSymbol[a]) << "\"];\n";
            }
        }
    }
    s << "}\n";
    return s;
}

namespace boost {
namespace serialization {

template <class Archive, typename Symbol, typename State, typename TokenId>
void serialize(Archive& ar, DFA<Symbol, State, TokenId>& dfa, unsigned int) {
    ar& dfa.stateCount;
    ar& dfa.symbolCount;
    ar& dfa.start;
    ar& dfa.deadState;
    ar& dfa.finals;
    ar& dfa.T;
    ar& dfa.symbolToId;
    ar& dfa.idToSymbol;
}

} // namespace serialization
} // namespace boost
