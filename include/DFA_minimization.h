#pragma once

#include "BitSet.h"  // for BitSet, BitSet::ref
#include "DFA.h"     // for PositionRange, DFA, Set, Position, determineDea...
#include "HashSet.h" // for HashSet
#include <iostream>  // for operator<<, endl, size_t, cout, ostream, basic_...
#include <iterator>  // for back_inserter, distance
#include <stdexcept> // for runtime_error
#include <string>    // for to_string
#include <utility>   // for make_pair, pair
#include <vector>    // for vector

namespace detail {
using Set = BitSet;

using Position = std::size_t;
using PositionRange = std::pair<Position, Position>;
} // namespace detail

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
    partition(const TokenIds<TokenId>& finals);

    bool swapToFront(Position l, Position h, const Set& tmp);

    bool swapToBack(Position l, Position h, const Set& tmp);

    Position swapRest(Position l, Position h, const Set& tmp);

    void update(Position l, Position h, Position j, Class b, std::vector<Class>& stack);

    void split(const Set& tmp, std::vector<Class>& stack);
};

template <typename State>
template <typename TokenId>
partition<State>::partition(const TokenIds<TokenId>& finals)
    : p(finals.GetStateCount()), pI(finals.GetStateCount()), c(finals.GetStateCount()) {
    auto stateCount = finals.GetStateCount();
    // given TokenId k, kinds[k] is std::vector<State> containing states of kind k
    std::vector<std::vector<State>> kinds;

    for (State i = 0; i < stateCount; i++) {
        if (kinds.size() <= finals.GetTokenId(i))
            kinds.resize(finals.GetTokenId(i) + 1);
        kinds[finals.GetTokenId(i)].push_back(i);
    }

    // std::cout << "kinds: ";
    // int i = 0;
    // for (auto& it : kinds) {
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
    if (tmp.count() == 0) {
        return;
    }
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
auto inverseTransitionTable(const Transitions<State>& T) {
    using TSet = HashSet;
    const auto stateCount = T.GetStateCount();
    const auto symbolCount = T.GetSymbolCount();
    std::vector<std::vector<TSet>> tI(symbolCount, std::vector<TSet>(stateCount, TSet(stateCount, false)));

    for (State i = 0; i < stateCount; i++) {
        for (Symbol a = 0; a < symbolCount; a++) {
            tI[a][T.GetTransition(i, a)][i] = true;
        }
    }
    return tI;
}

template <typename Symbol, typename State, typename TokenId>
DFA<Symbol, State, TokenId> generateFromMinimizationResults(const partition<State>& part,
                                                            const State start,
                                                            const Transitions<State>& T,
                                                            const TokenIds<TokenId>& finals,
                                                            const Symbols<Symbol>& symbols) {
    const auto symbolCount = symbols.count();
    const State newStateCount = part.c_i.size();
    std::cout << "new state count: " << newStateCount << std::endl;
    std::vector<State> newT(newStateCount * symbolCount);
    std::vector<TokenId> newFinal(newStateCount);
    for (State q = 0; q < newStateCount; q++) {
        auto t1 = part.c_i[q];
        auto s = t1.first;
        const auto ptr2 = newT.data() + q * symbolCount;
        for (Symbol a = 0; a < symbolCount; a++)
            ptr2[a] = part.c[part.pI[T.GetTransition(part.p[s], a)]];
        newFinal[q] = finals.GetTokenId(part.p[s]);
    }
    return DFA<Symbol, State, TokenId>(
        DfaStates<State, TokenId>(newStateCount,
                                  part.c[part.pI[start]],
                                  determineDeadState(newStateCount, symbolCount, newT, newFinal),
                                  TokenIds<TokenId>(std::move(newFinal)),
                                  Transitions<State>(newStateCount, symbolCount, std::move(newT))),
        symbols);
}

template <typename Symbol, typename State, typename TokenId>
DFA<Symbol, State, TokenId> minimize(const DFA<Symbol, State, TokenId>& dfa) {
    std::cout << "minimize" << std::endl;

    const auto& T = dfa.GetTransitions();
    const auto start = dfa.GetStart();
    const auto stateCount = dfa.GetStateCount();
    const auto symbolCount = dfa.GetSymbolCount();
    const auto& finals = dfa.GetTokenIds();

    auto tI = inverseTransitionTable<Symbol>(T);

    partition<State> part(finals);

    std::vector<State> stack;
    stack.reserve(part.c_i.size());
    // std::generate_n(std::back_inserter(stack), kinds.size(), [i = kinds.size()-1] () mutable { return i--;});
    std::generate_n(std::back_inserter(stack), part.c_i.size(), [i = 0]() mutable { return i++; });

    detail::Set tmp(stateCount, false);
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

            // std::cout << "splitter set: " << tmp << '\n';
            part.split(tmp, stack);
        }
    }

    std::cout << "Done. Generating new table." << std::endl;
    //  std::cout << "c_i: " << show(c_i) << std::endl;
    //  std::cout << "p: " << show(p) << std::endl;
    //  std::cout << "pI: " << show(pI) << std::endl;
    //  std::cout << "c: " << show(c) << std::endl;

    return generateFromMinimizationResults<Symbol, State, TokenId>(part, start, T, finals, dfa.GetSymbols());
}

