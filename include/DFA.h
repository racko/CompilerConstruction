#pragma once

#include "DFA_fwd.h"

#include "Print.h"
#include <BitSet.h>
#include <HashSet.h>

#include <ostream>
#include <vector>

#include <algorithm>
#include <exception>
#include <functional>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <tuple>
#include <unordered_map>
#include <utility>

#ifndef NULL
#define NULL 0
#endif
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
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
  private:
    using Set = detail::Set;

  public:
    using Class = State;
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

    void determineDeadState();

    void minimize();
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

// template <typename Symbol, typename State, typename TokenId>
// DFA<Symbol, detail::Position, TokenId> minimize(const DFA<Symbol, State, TokenId>& dfa);

template <typename Symbol, typename State, typename TokenId>
std::ostream& operator<<(std::ostream&, const DFA<Symbol, State, TokenId>&);

template <typename State>
struct partition {
  private:
    using Set = detail::Set;
    using Position = detail::Position;
    using PositionRange = detail::PositionRange;

  public:
    using Class = State;
    std::vector<State> p;
    std::vector<Position> pI;
    std::vector<PositionRange> c_i;
    std::vector<Class> c;

    template <typename TokenId>
    partition(const std::vector<TokenId>& final);

    bool swapToFront(Position l, Position h, const Set& tmp);

    bool swapToBack(Position l, Position h, const Set& tmp);

    Position swapRest(Position l, Position h, const Set& tmp);

    void update(Position l, Position h, Position j, Class b, std::vector<Class>& stack);

    void split(const Set& tmp, std::vector<Class>& stack);
};

template <typename State>
template <typename TokenId>
partition<State>::partition(const std::vector<TokenId>& final) : p(final.size()), pI(final.size()), c(final.size()) {
    auto stateCount = final.size();
    std::vector<std::vector<State>>
        kinds; // given TokenId k, kinds[k] is std::vector<State> containing states of kind k

    for (State i = 0; i < stateCount; i++) {
        if (kinds.size() <= final[i])
            kinds.resize(final[i] + 1);
        kinds[final[i]].push_back(i);
    }

    //std::cout << "kinds: ";
    //int i = 0;
    //for (auto& it : kinds) {
    //    std::cout << i++ << ' ' << show(it) << '\n';
    //}

    std::sort(kinds.begin(), kinds.end(), [](const auto& a, const auto& b) { return a.size() < b.size(); });

    c_i.reserve(kinds.size());

    Class j = 0;
    Position lastState = 0;
    for (const auto& k : kinds) {
        auto n = k.size();
        if (n > 0) {
            c_i.emplace_back(lastState, lastState + n);
            for (auto s : k) {
                p[lastState] = s;
                pI[s] = lastState;
                c[lastState] = j;
                lastState++;
            }
            j++;
        } else
            throw std::runtime_error("Empty kind sets are invalid: " + std::to_string(j));
    }

    //  std::cout << "c: " << show(c) << std::endl;
    //  std::cout << "c_i: " << show(c_i) << std::endl;
    //  std::cout << "p: " << show(p) << std::endl;
    //  std::cout << "pI: " << show(pI) << std::endl;
}

template <typename State>
bool partition<State>::swapToFront(Position l, Position h, const Set& tmp) {
    // std::cout << "swapping in-splitter to front" << std::endl;
    // for (auto i = l; i <= h; i++) {
    //    if (tmp[p[i]]) {
    //        std::swap(p[l], p[i]);
    //        //std::cout << "swapping " << l << " and " << i << ": " << show(p) << std::endl;
    //        pI[p[l]] = l;
    //        pI[p[i]] = i;
    //        return true;
    //    }
    //}

    // the below is more efficient when the compiler reloads p.begin() and tmp.p from memory in every iteration in the
    // implementation above
    const auto stop = p.begin() + static_cast<std::ptrdiff_t>(h);
    const auto _p = tmp.p;
    for (auto i = p.begin() + static_cast<std::ptrdiff_t>(l); i != stop; ++i) {
        if (*(_p + (*i >> 6)) & (0x1LL << *i)) {
            std::swap(p[l], *i);
            // std::cout << "swapping " << l << " and " << i << ": " << show(p) << std::endl;
            pI[p[l]] = l;
            pI[*i] = static_cast<std::size_t>(std::distance(p.begin(), i));
            return true;
        }
    }
    return false;
}

template <typename State>
bool partition<State>::swapToBack(Position l, Position h, const Set& tmp) {
    // std::cout << "swapping not-in-splitter to end" << std::endl;
    auto last = h - 1;
    for (auto i = static_cast<int>(last); i >= int(l); i--) {
        if (!tmp[p[i]]) {
            std::swap(p[i], p[last]);
            // std::cout << "swapping " << i << " and " << h << ": " << show(p) << std::endl;
            pI[p[last]] = last;
            pI[p[i]] = i;
            return true;
        }
    }
    return false;
}

template <typename State>
auto partition<State>::swapRest(Position l, Position h, const Set& tmp) -> Position {
    // std::cout << "swapping all others" << std::endl;
    auto j = l;
    auto k = h - 1;
    for (;;) {
        do
            j++;
        while (tmp[p[j]]);
        do
            k--;
        while (!tmp[p[k]]);
        if (k < j)
            break;
        std::swap(p[j], p[k]);
        // std::cout << "swapping " << j << " and " << k << ": " << show(p) << std::endl;
        pI[p[j]] = j;
        pI[p[k]] = k;
    }
    return j;
}

template <typename State>
void partition<State>::update(Position l, Position h, Position j, Class b, std::vector<State>& stack) {
    auto A = std::make_pair(l, j);
    auto B = std::make_pair(j, h);

    // std::cout << "B': (" << A.first << "-" << A.second << ")" << std::endl;
    // std::cout << "B'': (" << B.first << "-" << B.second << ")" << std::endl;

    auto A_size = A.second - A.first;
    auto B_size = B.second - B.first;
    auto newIx = c_i.size();

    // auto newSize = std::min(A_size, B_size);
    // auto it = std::upper_bound(stack.begin(), stack.end(), newSize, [&] (auto n, const auto& i) { const auto& p =
    // c_i[i]; return n < p.second - p.first + 1; }); stack.insert(it, newIx);

    stack.push_back(newIx);
    if (A_size > B_size) {
        c_i[b] = A;
        c_i.push_back(B);
        std::fill_n(c.begin() + B.first, B_size, newIx);
        // std::cout << "Pushing B'' on the stack. Index: " << newIx << std::endl;
    } else {
        c_i[b] = B;
        c_i.push_back(A);
        std::fill_n(c.begin() + A.first, A_size, newIx);
        // std::cout << "Pushing B' on the stack. Index: " << newIx << std::endl;
    }
}

template <typename State>
void partition<State>::split(const Set& tmp, std::vector<Class>& stack) {
    const auto groupCount = c_i.size();
    for (Class b = 0; b < groupCount; b++) {
        const auto indices = c_i[b];
        const auto l = indices.first;
        const auto h = indices.second;
        // We check whether the set is a singleton because in this case we cannot split it further.
        // Much cheaper than figuring it out later in the loop.
        // Especially useful if we have lots of these.
        if (l + 1 == h)
            continue;
        // std::cout << "B: " << b << " (" << l << "-" << h << ")\n";
        if (!swapToFront(l, h, tmp))
            continue;
        if (!swapToBack(l, h, tmp))
            continue;
        auto s = swapRest(l, h, tmp);
        update(l, h, s, b, stack);
    }
}

template <typename T>
void assign(T& lhs, const T& rhs) {
    lhs = rhs;
}

inline BitSet& operator|=(BitSet& lhs, const HashSet& rhs) {
    for (auto i : rhs)
        lhs[i] = true;
    return lhs;
}

inline void assign(BitSet& lhs, const HashSet& rhs) {
    lhs.clear();
    lhs |= rhs;
}

namespace detail {
template <typename State>
void splitterSet(const State* s, const State* stop, BitSet& tmp, const std::vector<HashSet>& tIa) {
    assign(tmp, tIa[*s]);
    ++s;
    for (; s != stop; ++s) {
        // std::cout << "collecting tI[" << a << "][" << p[q] << "]: " << tI[a][p[q]] << std::endl;
        tmp |= tIa[*s];
    }
}
} // namespace detail

template <typename Symbol, typename State>
void splitterSet(const detail::PositionRange& t1,
                 Symbol a,
                 BitSet& tmp,
                 const partition<State>& part,
                 const std::vector<std::vector<HashSet>>& tI) {
    const auto p = part.p.data();
    detail::splitterSet(p + t1.first, p + t1.second, tmp, tI[a]);
}

template <typename Symbol, typename State>
auto inverseTransitionTable(const std::vector<State>& T, std::size_t stateCount, std::size_t symbolCount) {
    using TSet = HashSet;
    std::vector<std::vector<TSet>> tI(symbolCount, std::vector<TSet>(stateCount, TSet(stateCount, false)));

    for (State i = 0; i < stateCount; i++) {
        const auto ptr = T.data() + i * symbolCount;
        for (Symbol a = 0; a < symbolCount; a++) {
            tI[a][ptr[a]][i] = true;
        }
    }
    return tI;
}

template <typename Symbol, typename State, typename TokenId>
State determineDeadState(const State stateCount,
                         const Symbol symbolCount,
                         const std::vector<State>& T,
                         const std::vector<TokenId>& final);

template <typename Symbol, typename State, typename TokenId>
DFA<Symbol, State, TokenId> generateFromMinimizationResults(const partition<State>& part,
                                                            const State start,
                                                            const Symbol symbolCount,
                                                            const std::vector<State>& T,
                                                            const std::vector<TokenId>& final,
                                                            const std::vector<std::size_t>& symbolToId,
                                                            const std::vector<Symbol>& idToSymbol) {
    State newStateCount = part.c_i.size();
    std::cout << "new state count: " << newStateCount << std::endl;
    std::vector<State> newT(newStateCount * symbolCount);
    std::vector<TokenId> newFinal(newStateCount);
    for (State q = 0; q < newStateCount; q++) {
        auto t1 = part.c_i[q];
        auto s = t1.first;
        const auto ptr1 = T.data() + part.p[s] * symbolCount;
        const auto ptr2 = newT.data() + q * symbolCount;
        for (Symbol a = 0; a < symbolCount; a++)
            ptr2[a] = part.c[part.pI[ptr1[a]]];
        newFinal[q] = final[part.p[s]];
    }
    return DFA<Symbol, State, TokenId>(newStateCount,
                                       symbolCount,
                                       part.c[part.pI[start]],
                                       determineDeadState(newStateCount, symbolCount, newT, newFinal),
                                       std::move(newFinal),
                                       std::move(newT),
                                       symbolToId,
                                       idToSymbol);
}

template <typename Symbol, typename State, typename TokenId>
void DFA<Symbol, State, TokenId>::minimize() {
    std::cout << "minimize" << std::endl;

    auto tI = inverseTransitionTable<Symbol>(T, stateCount, symbolCount);

    partition<State> part(final);

    std::vector<Class> stack;
    stack.reserve(part.c_i.size());
    // std::generate_n(std::back_inserter(stack), kinds.size(), [i = kinds.size()-1] () mutable { return i--;});
    std::generate_n(std::back_inserter(stack), part.c_i.size(), [i = 0]() mutable { return i++; });

    Set tmp(stateCount, false);
    while (!stack.empty()) {
        // std::cout << "stack: " << show(stack) << std::endl;
        auto splitter = stack.back();
        stack.pop_back();
        // auto splitter = stack.front();
        // stack.erase(stack.begin());
        auto t1 = part.c_i[splitter];
        // std::cout << "splitter: " << splitter << " (" << t1.first << "-" << t1.second << ")" << std::endl;
        for (Symbol a = 0; a < symbolCount; a++) {
            // std::cout << "considering symbol " << showChar(a) << std::endl;

            splitterSet(t1, a, tmp, part, tI);

            if (tmp.count() == 0)
                continue;
            // std::cout << "splitter set: " << tmp << '\n';
            part.split(tmp, stack);
        }
    }

    std::cout << "Done. Generating new table." << std::endl;
    //  std::cout << "c_i: " << show(c_i) << std::endl;
    //  std::cout << "p: " << show(p) << std::endl;
    //  std::cout << "pI: " << show(pI) << std::endl;
    //  std::cout << "c: " << show(c) << std::endl;

    *this = ::generateFromMinimizationResults<Symbol, State, TokenId>(
        part, start, symbolCount, T, final, symbolToId, idToSymbol);
}

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
void DFA<Symbol, State, TokenId>::determineDeadState() {
    deadState = ::determineDeadState(stateCount, symbolCount, T, final);
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
