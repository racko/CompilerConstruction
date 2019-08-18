#pragma once

#include "DFA_fwd.h"

#include <BitSet.h> // for BitSet
#include <cstddef>  // for NULL
#include <iostream> // for operator<<, ostream, size_t, basic_ostream, basi...
#include <iterator> // for random_access_iterator_tag
#include <utility>  // for pair
#include <vector>   // for vector

#ifndef NULL
#define NULL 0
#endif
#include <boost/serialization/vector.hpp>
#undef NULL

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
    std::vector<TokenId> final;
    std::vector<State> T;
    std::vector<std::size_t> symbolToId;
    std::vector<Symbol> idToSymbol;

    DFA() = default;

    DFA(State stateCount,
        Symbol symbolCount,
        State start,
        State deadState,
        const std::vector<TokenId>& final,
        const std::vector<State>& T,
        const std::vector<std::size_t>& symbolToId,
        const std::vector<Symbol>& idToSymbol);
};

template <typename Symbol, typename State, typename TokenId>
DFA<Symbol, State, TokenId>::DFA(State stateCount,
                                 Symbol symbolCount,
                                 State start,
                                 State deadState,
                                 const std::vector<TokenId>& final,
                                 const std::vector<State>& T,
                                 const std::vector<std::size_t>& symbolToId,
                                 const std::vector<Symbol>& idToSymbol)
    : stateCount{stateCount},
      symbolCount{symbolCount},
      start{start},
      deadState{deadState},
      final{final},
      T{T},
      symbolToId{symbolToId},
      idToSymbol{idToSymbol} {}

template <typename Symbol, typename State, typename TokenId>
std::ostream& operator<<(std::ostream&, const DFA<Symbol, State, TokenId>&);

template <typename T>
struct NumIterator {
    using difference_type = std::ptrdiff_t;
    using value_type = const T;
    using pointer = const T*;
    using reference = const T&;
    using iterator_category = std::random_access_iterator_tag;

    T x_;

    NumIterator() = default;
    NumIterator(T x) : x_(x) {}

    T operator*() { return x_; }
    NumIterator& operator++() {
        ++x_;
        return *this;
    }
    bool operator!=(const NumIterator& other) { return x_ != other.x_; }
    difference_type operator-(const NumIterator& other) { return x_ - other.x_; }
};

template <typename T>
struct NumRange {
    T a_, b_;
    NumRange(T a, T b) : a_(a), b_(b) {}

    NumIterator<T> begin() const { return a_; }
    NumIterator<T> end() const { return b_; }
};

template <typename Symbol, typename State, typename TokenId>
State determineDeadState(const State stateCount,
                         const Symbol symbolCount,
                         const std::vector<State>& T,
                         const std::vector<TokenId>& final) {
    //  std::cout << "determineDeadState" << std::endl;
    auto idempotent = [&](State q) {
        const auto ptr = T.data() + q * symbolCount;
        return std::all_of(ptr, ptr + symbolCount, [q](State q_) { return q_ == q; });
    };
    auto isDeadState = [&](State q) { return !final[q] && idempotent(q); };
    auto stop = NumIterator<State>(stateCount);
    auto it = std::find_if(NumIterator<State>(0u), stop, isDeadState);
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
           lhs.deadState == rhs.deadState && lhs.final == rhs.final && lhs.T == rhs.T &&
           lhs.symbolToId == rhs.symbolToId && lhs.idToSymbol == rhs.idToSymbol;
}

template <typename Symbol, typename State, typename TokenId>
std::ostream& operator<<(std::ostream& s, const DFA<Symbol, State, TokenId>& dfa) {
    s << "digraph G {\n";
    for (unsigned int p = 0; p < dfa.stateCount; p++) {
        if (p == dfa.deadState) {
            continue;
        }

        if (dfa.final[p]) {
            s << "  " << p << "[label = \"" << p << '|' << dfa.final[p] << "\" shape = doublecircle];\n";
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
    ar& dfa.final;
    ar& dfa.T;
    ar& dfa.symbolToId;
    ar& dfa.idToSymbol;
}

} // namespace serialization
} // namespace boost
