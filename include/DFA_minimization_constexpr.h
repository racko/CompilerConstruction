#pragma once
#include "DFA_constexpr.h"
#include "partition.h"
#include "HashSet_constexpr.h"
#include "BitSet_constexpr.h"

template<typename T>
constexpr void assign(T& lhs, const T& rhs) {
    // TODO: Check sizes?
    lhs = rhs;
}

template<int64_t Size1, int64_t Size2>
constexpr HashSet<Size1>& operator|=(HashSet<Size1>& lhs, const BitSet<Size2>& rhs) {
    // TODO: Check sizes?
    for (auto i : rhs)
        lhs.set(i);
    return lhs;
}

template<int64_t Size1, int64_t Size2>
constexpr void assign(HashSet<Size1>& lhs, const BitSet<Size2>& rhs) {
    // TODO: Check sizes?
    lhs.clear();
    lhs.s.data_.insert(lhs.s.data_.end(), rhs.begin(), rhs.end());
}

template<int64_t Size1, int64_t Size2>
constexpr BitSet<Size1>& operator|=(BitSet<Size1>& lhs, const HashSet<Size2>& rhs) {
    // TODO: Check sizes?
    const auto stop = rhs.s.stop_;
    for (auto it = rhs.s.values.data; it != stop; ++it)
        lhs.set(*it);
    //for (auto i : rhs)
    //    lhs.set(i);
    return lhs;
}

template<int64_t Size1, int64_t Size2>
constexpr void assign(BitSet<Size1>& lhs, const HashSet<Size2>& rhs) {
    // TODO: Check sizes?
    lhs.clear();
    lhs |= rhs;
}

template<int64_t Size1, typename T, int64_t Size2, int64_t Size3>
constexpr void a_and_not_b(const HashSet<Size1,T>& a, const BitSet<Size2>& b, HashSet<Size3,T>& c) {
    c.clear();
    for (auto i : a) {
        if (!b.get(i)) {
            // since we insert unique values into an empty set, we can bypass the "is v already in c.s check" that insert does by directly pushing into c.s.data_ ... knowing fully well that the implementation might change :)
            c.s.insert(i);
            //c.s.data_.push_back(i);
        }
    }
    //const auto stop = a.stop_;
    //for (auto it = a.values; it != stop; ++it) {
    //    if (!b.get(*i)) {
    //        // since we insert unique values into an empty set, we can bypass the "is v already in c.s check" that insert does by directly pushing into c.s.data_ ... knowing fully well that the implementation might change :)
    //        c.s.insert(*i);
    //        //c.s.data_.push_back(i);
    //    }
    //}
}

namespace detail {
template<typename State, typename Set, typename InverseTransitionTable>
// requires InverseTransitionTable = vector<Set2<Size1>,Size2>
constexpr void splitterSet(const State* s, const State* stop, Set& tmp, const InverseTransitionTable& tIa) {
    //std::cout << "collecting tI[a][" << *s << "]: " << tIa[*s] << std::endl;
    assign(tmp, tIa[*s]);
    ++s;
    for (; s != stop; ++s) {
        //std::cout << "collecting tI[a][" << *s << "]: " << tIa[*s] << std::endl;
        tmp |= tIa[*s];
    }
}
}

template<typename Symbol, typename Set, typename Partition, typename InverseTransitionTable>
// requires Partition = partition<State,Class,Size1>
// requires InverseTransitionTable = vector<vector<Set2<Size2>,Size3>,Size4>
constexpr void splitterSet(const PositionRange& t1, Symbol a, Set& tmp, const Partition& part, const InverseTransitionTable& tI) {
    const auto p = part.p.data();
    //std::cout << "collecting with a = " << int(a) << std::endl;
    detail::splitterSet(p + t1.first, p + t1.second, tmp, tI[a]);
}

template<int64_t MaxNodes, int64_t MaxSymbols, typename Symbol, typename State, int64_t Size>
constexpr auto inverseTransitionTable(const vector<State,Size>& T, int64_t stateCount, int64_t symbolCount) {
    //using TSet = HashSet<MaxNodes>;
    using TSet = BitSet<MaxNodes>;
    if (stateCount > MaxNodes)
        throw std::runtime_error("stateCount > MaxNodes");
    if (symbolCount > MaxSymbols)
        throw std::runtime_error("symbolCount > MaxSymbols");
    vector<vector<TSet,MaxNodes>,MaxSymbols> tI(symbolCount, vector<TSet,MaxNodes>(stateCount, TSet(stateCount, false)));
    auto tIptr = tI.data();

    for (State i = 0; i < stateCount; i++) {
        const auto ptr = T.data() + i * symbolCount;
        for (Symbol a = 0; a < symbolCount; a++) {
            tIptr[a][ptr[a]].set(i);
        }
    }
    return tI;
}

template<typename Symbol, typename State, typename Class, typename TokenId, int64_t MaxNodes, int64_t MaxSymbols>
constexpr DFA<Symbol,Class,TokenId,MaxNodes,MaxSymbols>
generateFromMinimizationResults(
        const partition<State,Class,MaxNodes>& part,
        State start,
        int64_t symbolCount,
        const vector<State,MaxNodes*MaxSymbols>& T,
        const vector<TokenId,MaxNodes>& finals,
        const vector<int64_t,MaxSymbols>& symbolToId,
        const vector<Symbol,MaxSymbols>& idToSymbol) {
    auto newStateCount = part.c_i.size();
    //std::cout << "new state count: " << newStateCount << std::endl;
    vector<Class,MaxNodes*MaxSymbols> newT(newStateCount * symbolCount);
    vector<TokenId,MaxNodes> newFinals(newStateCount);
    for(Class q = 0; q < newStateCount; q++) {
        auto t1 = part.c_i[q];
        auto s = t1.first;
        const auto ptr1 = T.data() + part.p[s] * symbolCount;
        const auto ptr2 = newT.data() + q * symbolCount;
        for (Symbol a = 0; a < symbolCount; a++)
            ptr2[a] = part.c[part.pI[ptr1[a]]];
        newFinals[q] = finals[part.p[s]];
    }
    return DFA<Symbol,Class,TokenId,MaxNodes,MaxSymbols>(
            newStateCount,
            symbolCount,
            part.c[part.pI[start]],
            determineDeadState(newStateCount, symbolCount, newT, newFinals),
            std::move(newFinals),
            std::move(newT),
            symbolToId,
            idToSymbol);
}

template<typename Class, typename Symbol, typename State, typename TokenId, int64_t MaxNodes, int64_t MaxSymbols>
constexpr DFA<Symbol,Class,TokenId,MaxNodes,MaxSymbols> minimize(const DFA<Symbol,State,TokenId,MaxNodes,MaxSymbols>& dfa) {
    //std::cout << "minimize" << std::endl;

    const auto& T = dfa.T;
    const auto start = dfa.start;
    const auto stateCount = dfa.stateCount;
    const auto symbolCount = dfa.symbolCount;
    const auto& finals = dfa.finals;
    const auto& symbolToId = dfa.symbolToId;
    const auto& idToSymbol = dfa.idToSymbol;

    auto tI = inverseTransitionTable<MaxNodes,MaxSymbols,Symbol>(T, stateCount, symbolCount);

    partition<State,Class,MaxNodes> part(finals);

    vector<Class,MaxNodes> stack;
    stack.reserve(part.c_i.size());
    //std::generate_n(std::back_inserter(stack), kinds.size(), [i = kinds.size()-1] () mutable { return i--;});
    generate_n(back_inserter(stack), part.c_i.size(), [i = 0] () mutable { return i++;});

    BitSet<MaxNodes> tmp(stateCount, false);
    while (!stack.empty()) {
        //std::cout << "stack: " << show(stack) << std::endl;
        auto splitter = stack.back();
        stack.pop_back();
        //auto splitter = stack.front();
        //stack.erase(stack.begin());
        auto t1 = part.c_i[splitter];
        //std::cout << "splitter: " << splitter << " (" << t1.first << "-" << t1.second << ")" << std::endl;
        for (Symbol a = 0; a < symbolCount; a++) {
            //std::cout << "considering symbol " << int(a) << std::endl;

            splitterSet(t1, a, tmp, part, tI);

            if (tmp.count() == 0)
                continue;
            //std::cout << "splitter set: " << tmp << '\n';
            const auto initial_number_of_groups = part.c_i.size();
            part.split(tmp);
            const auto new_number_of_groups = part.c_i.size();
            NumRange r(initial_number_of_groups, new_number_of_groups);
            stack.insert(stack.end(), r.begin(), r.end());
        }
    }

    //std::cout << "Done. Generating new table." << std::endl;
    //std::cout << "c_i: " << show(part.c_i) << std::endl;
    //std::cout << "p: " << show(part.p) << std::endl;
    //std::cout << "pI: " << show(part.pI) << std::endl;
    //std::cout << "c: " << show(part.c) << std::endl;

    return generateFromMinimizationResults(part, start, symbolCount, T, finals, symbolToId, idToSymbol);
}
