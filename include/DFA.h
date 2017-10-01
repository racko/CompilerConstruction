#ifndef _DFA_H_
#define _DFA_H_

#include "Print.h"
#include <NFA.h>
#include <BitSet.h>
#include <HashSet.h>

#include <vector>
using std::vector;
#include <ostream>

#include <tuple>
#include <utility>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <iomanip>
#include <exception>
#include <algorithm>
#include <numeric>
using std::unordered_map;
using std::hash;
using std::pair;
using std::make_pair;
using std::sort;
using std::move;
using std::tie;

using Set = BitSet;

using Position = size_t;
using PositionRange = std::pair<Position,Position>;

template<typename State>
struct partition;

template<typename Symbol, typename State, typename TokenId>
struct DFA {
    using Class = State;
    size_t stateCount; //TODO change to State lastState for safe comparisions
    size_t symbolCount; //TODO change to Symbol lastSymbol ...
    State start, deadState;
    vector<TokenId> final;
    vector<State> T;
    vector<size_t> symbolToId;
    vector<Symbol> idToSymbol;
    DFA() = default;
    DFA(const NFA<Symbol,State,TokenId>& nfa);
    void generateFromMinimizationResults(const partition<State>& part);
    void determineDeadState();
    void minimize();
};

template<typename Symbol, typename State, typename TokenId>
DFA<Symbol,Position,TokenId> minimize(const DFA<Symbol,State,TokenId>& dfa);

template<typename Symbol, typename State, typename TokenId>
std::ostream& operator<<(std::ostream&, const DFA<Symbol,State,TokenId>&);

template<typename Symbol, typename State, typename TokenId>
DFA<Symbol,State,TokenId>::DFA(const NFA<Symbol,State,TokenId>& nfa) : symbolCount(nfa.symbolCount - 1), symbolToId(128,symbolCount), idToSymbol(128,symbolCount) {
    using BitSet = HashSet;
    cout << "DFA constructor" << endl;
    cout << "stateCount = " << nfa.stateCount << endl;
    cout << "symbolCount = " << symbolCount << endl;
    vector<size_t> symbolMap(nfa.symbolCount);
    size_t nonEpsSymbol = 0;
    for (auto i = Symbol(); i < nfa.symbolCount; i++)
        if (i != nfa.eps) {
            symbolMap[i] = nonEpsSymbol;
            symbolToId[nfa.symbols[i]] = nonEpsSymbol;
            idToSymbol[nonEpsSymbol] = nfa.symbols[i];
            nonEpsSymbol++;
        }

    unsigned int n = nfa.stateCount;
    unsigned int id = 0;
    vector<unsigned int> stack;
    vector<BitSet> idToState;
    unordered_map<BitSet,unsigned int> stateToId;
    idToState.emplace_back(n, false);
    BitSet& S = idToState.back();
    S[nfa.start] = true;
    //  cout << "nfa.start: " << nfa.start << endl;
    //  cout << "nfa.final" << show(nfa.final) << endl;
    //  cout << "s0: " << S << endl;
    nfa.getClosure(S);
    //  cout << "closure(s0): " << S << endl;
    stateToId[S] = id;
    stack.push_back(id);
    start = id;
    id++;

    while(!stack.empty()) {
        //cout << "stack: " << show(stack) << endl;
        unsigned int q = stack.back();
        stack.pop_back();
        //auto& _p = idToState[q];
        //cout << "state: " << _p << endl;
        if (T.size() < (q + 1) * symbolCount)
            T.resize((q + 1) * symbolCount);
        for (unsigned int a = 0; a < nfa.symbolCount; a++) {
            if (a != nfa.eps) {
                const auto& p = idToState[q];
                //cout << "Constructing delta(" << p << "," << a << ")" << endl;
                BitSet U(nfa.stateCount, false);
                for (auto s : p) {
                    //cout << "collecting T[" << *s << "][" << a << "] = " << nfa.table[*s][a] << endl;
                    U |= nfa.table[s][a];
                }
                //cout << "targets: " << U << endl;
                //        if (U.count() > 1)
                //          cin.get();
                nfa.getClosure(U);
                //cout << "closure: " << U << endl;
                //        if (U.count() > 1)
                //          cin.get();
                //        cout << "idToState: " << endl;
                //        for (auto& i: idToState) {
                //          auto it = stateToId.find(i);
                //          cout << i << " (";
                //          if (it != stateToId.end())
                //            cout << it->first << ": " << it->second;
                //          else
                //            cout << "not found";
                //          cout << ")" << endl;
                //        }
                //        cout << "stateToId: " << endl;
                //        for (auto& i: stateToId) {
                //          BitSet localCopy(i.first);
                //          cout
                //            << i.second << ": "
                //            << localCopy << endl;
                //        }
                //cout << "stateToId.find(U);" << endl;
                auto it = stateToId.find(U);
                if (it == stateToId.end()) {
                    //cout << "idToState.emplace_back(std::move(U));" << endl;
                    idToState.emplace_back(std::move(U));
                    //cout << "auto& UU = idToState.back();" << endl;
                    auto& UU = idToState.back();
                    //cout << "stateToId[UU] = id;" << endl;
                    stateToId[UU] = id;
                    //cout << "stack.push_back(id);" << endl;
                    stack.push_back(id);
                    //cout << "T[q][symbolMap[a]] = id;" << endl;
                    T[q * symbolCount + symbolMap[a]] = id;
                    //cout << "new state; id = " << id << endl;
                    id++;
                } else {
                    //cout << "seen before; id = " << it->second << endl;
                    T[q * symbolCount + symbolMap[a]] = it->second;
                }
            }
        }
    }
    stateCount = id;
    cout << "final state count: " << stateCount << endl;
    //  cout << "checking for terminal states" << endl;
    final.resize(stateCount, 0);
    for (unsigned int q = 0; q < stateCount; q++) {
        const BitSet& U = idToState[q];
        //    cout << "checking dfa state " << q << ": " << U << endl;
        size_t first_nfa_state = SIZE_MAX;
        for (auto s : U) {
            //cout << "checking nfa state " << *s << endl;
            if (nfa.final[s] != 0 && final[q] == 0) {
                first_nfa_state = s;
                final[q] = nfa.final[s];
                //cout << "dfa.final[" << q << "] = " << "nfa.final[" << s << "] = " << int(nfa.final[s]) << endl;
            } else if (nfa.final[s] != 0 && s < first_nfa_state) {
                first_nfa_state = s;
                final[q] = nfa.final[s];
                //cout << "dfa.final[" << q << "] is ambiguous. Preferring nfa.final[" << s << "] = " << int(nfa.final[s]) << endl;
            } else if (nfa.final[s] != 0) {
                //cout << "dfa.final[" << q << "] is ambiguous. Ignoring nfa.final[" << s << "] = " << int(nfa.final[s]) << endl;
            }
        }
    }

    determineDeadState();
}

template<typename State>
struct partition {
    using Class = State;
    vector<State> p;
    vector<Position> pI;
    vector<PositionRange> c_i;
    vector<Class> c;

    template<typename TokenId>
    partition(const vector<TokenId>& final);

    bool swapToFront(Position l, Position h, const Set& tmp);

    bool swapToBack(Position l, Position h, const Set& tmp);

    Position swapRest(Position l, Position h, const Set& tmp);

    void update(Position l, Position h, Position j, Class b, vector<Class>& stack);

    void split(const Set& tmp, vector<Class>& stack);
};

template<typename State>
template<typename TokenId>
partition<State>::partition(const vector<TokenId>& final) : p(final.size()), pI(final.size()), c(final.size()) {
    auto stateCount = final.size();
    vector<vector<State>> kinds; // given TokenId k, kinds[k] is vector<State> containing states of kind k

    for (State i = 0; i < stateCount; i++) {
        if (kinds.size() <= final[i])
            kinds.resize(final[i] + 1);
        kinds[final[i]].push_back(i);
    }

    //  cout << "count of kinds: ";
    //  for (auto& it: kinds)
    //    cout << show(it);
    //  cout << endl;

    sort(kinds.begin(), kinds.end(), [] (const auto& a, const auto& b) { return a.size() < b.size(); });

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
            throw std::runtime_error("Empty kind sets are invalid.");
    }

    //  cout << "c: " << show(c) << endl;
    //  cout << "c_i: " << show(c_i) << endl;
    //  cout << "p: " << show(p) << endl;
    //  cout << "pI: " << show(pI) << endl;
}

template<typename State>
bool partition<State>::swapToFront(Position l, Position h, const Set& tmp) {
    //cout << "swapping in-splitter to front" << endl;
    //for (auto i = l; i <= h; i++) {
    //    if (tmp[p[i]]) {
    //        std::swap(p[l], p[i]);
    //        //cout << "swapping " << l << " and " << i << ": " << show(p) << endl;
    //        pI[p[l]] = l;
    //        pI[p[i]] = i;
    //        return true;
    //    }
    //}

    // the below is more efficient when the compiler reloads p.begin() and tmp.p from memory in every iteration in the implementation above
    const auto stop = p.begin() + h;
    const auto _p = tmp.p;
    for (auto i = p.begin() + l; i != stop; ++i) {
        if (*(_p + (*i >> 6)) & (0x1LL << *i)) {
            std::swap(p[l], *i);
            //cout << "swapping " << l << " and " << i << ": " << show(p) << endl;
            pI[p[l]] = l;
            pI[*i] = std::distance(p.begin(), i);
            return true;
        }
    }
    return false;
}

template<typename State>
bool partition<State>::swapToBack(Position l, Position h, const Set& tmp) {
    //cout << "swapping not-in-splitter to end" << endl;
    auto last = h-1;
    for (auto i = static_cast<int>(last); i >= int(l); i--) {
        if (!tmp[p[i]]) {
            std::swap(p[i], p[last]);
            //cout << "swapping " << i << " and " << h << ": " << show(p) << endl;
            pI[p[last]] = last;
            pI[p[i]] = i;
            return true;
        }
    }
    return false;
}

template<typename State>
Position partition<State>::swapRest(Position l, Position h, const Set& tmp) {
    //cout << "swapping all others" << endl;
    auto j = l;
    auto k = h-1;
    for (;;) {
        do j++; while (tmp[p[j]]);
        do k--; while (!tmp[p[k]]);
        if (k < j) break;
        std::swap(p[j], p[k]);
        //cout << "swapping " << j << " and " << k << ": " << show(p) << endl;
        pI[p[j]] = j;
        pI[p[k]] = k;
    }
    return j;
}

template<typename State>
void partition<State>::update(Position l, Position h, Position j, Class b, vector<State>& stack) {
    auto A = make_pair(l,j);
    auto B = make_pair(j,h);

    //cout << "B': (" << A.first << "-" << A.second << ")" << endl;
    //cout << "B'': (" << B.first << "-" << B.second << ")" << endl;

    auto A_size = A.second - A.first;
    auto B_size = B.second - B.first;
    auto newIx = c_i.size();

    //auto newSize = std::min(A_size, B_size);
    //auto it = std::upper_bound(stack.begin(), stack.end(), newSize, [&] (auto n, const auto& i) { const auto& p = c_i[i]; return n < p.second - p.first + 1; });
    //stack.insert(it, newIx);

    stack.push_back(newIx);
    if (A_size > B_size) {
        c_i[b] = A;
        c_i.push_back(B);
        std::fill_n(c.begin() + B.first, B_size, newIx);
        //cout << "Pushing B'' on the stack. Index: " << newIx << endl;
    } else {
        c_i[b] = B;
        c_i.push_back(A);
        std::fill_n(c.begin() + A.first, A_size, newIx);
        //cout << "Pushing B' on the stack. Index: " << newIx << endl;
    }
}

template<typename State>
void partition<State>::split(const Set& tmp, vector<Class>& stack) {
    const auto groupCount = c_i.size();
    for (Class b = 0; b < groupCount; b++) {
        const auto indices = c_i[b];
        const auto l = indices.first;
        const auto h = indices.second;
        // We check whether the set is a singleton because in this case we cannot split it further.
        // Much cheaper than figuring it out later in the loop.
        // Especially useful if we have lots of these.
        if (l+1 == h)
            continue;
        //cout << "B: " << b << " (" << l << "-" << h << ")\n";
        if (!swapToFront(l, h, tmp))
            continue;
        if (!swapToBack(l, h, tmp))
            continue;
        auto s = swapRest(l, h, tmp);
        update(l, h, s, b, stack);
    }
}

template<typename T>
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
template<typename State>
void splitterSet(const State* s, const State* stop, BitSet& tmp, const vector<HashSet>& tIa) {
    assign(tmp, tIa[*s]);
    ++s;
    for (; s != stop; ++s) {
        //cout << "collecting tI[" << a << "][" << p[q] << "]: " << tI[a][p[q]] << endl;
        tmp |= tIa[*s];
    }
}
}

template<typename Symbol, typename State>
void splitterSet(const PositionRange& t1, Symbol a, BitSet& tmp, const partition<State>& part, const vector<vector<HashSet>>& tI) {
    const auto p = part.p.data();
    detail::splitterSet(p + t1.first, p + t1.second, tmp, tI[a]);
}

template<typename Symbol, typename State>
auto inverseTransitionTable(const vector<State>& T, size_t stateCount, size_t symbolCount) {
    using TSet = HashSet;
    vector<vector<TSet>> tI(symbolCount, vector<TSet>(stateCount, TSet(stateCount, false)));

    for (State i = 0; i < stateCount; i++) {
        const auto ptr = T.data() + i * symbolCount;
        for (Symbol a = 0; a < symbolCount; a++) {
            tI[a][ptr[a]][i] = true;
        }
    }
    return tI;
}

template<typename Symbol, typename State, typename TokenId>
void DFA<Symbol,State,TokenId>::generateFromMinimizationResults(const partition<State>& part) {
    auto newStateCount = part.c_i.size();
    cout << "new state count: " << newStateCount << endl;
    vector<Class> newT(newStateCount * symbolCount);
    vector<TokenId> newFinal(newStateCount);
    for(Class q = 0; q < newStateCount; q++) {
        auto t1 = part.c_i[q];
        auto s = t1.first;
        const auto ptr1 = T.data() + part.p[s] * symbolCount;
        const auto ptr2 = newT.data() + q * symbolCount;
        for (Symbol a = 0; a < symbolCount; a++)
            ptr2[a] = part.c[part.pI[ptr1[a]]];
        newFinal[q] = final[part.p[s]];
    }
    start = part.c[part.pI[start]]; // bad: State start gets assigned a Class => This should be a free function that generates a DFA<Class> from DFA<State>
    final = move(newFinal);
    stateCount = newStateCount;
    T = move(newT);
    determineDeadState();
}

template<typename Symbol, typename State, typename TokenId>
void DFA<Symbol,State,TokenId>::minimize() {
    cout << "minimize" << endl;

    auto tI = inverseTransitionTable<Symbol>(T, stateCount, symbolCount);

    partition<State> part(final);

    vector<Class> stack;
    stack.reserve(part.c_i.size());
    //std::generate_n(std::back_inserter(stack), kinds.size(), [i = kinds.size()-1] () mutable { return i--;});
    std::generate_n(std::back_inserter(stack), part.c_i.size(), [i = 0] () mutable { return i++;});

    Set tmp(stateCount, false);
    while (!stack.empty()) {
        //cout << "stack: " << show(stack) << endl;
        auto splitter = stack.back();
        stack.pop_back();
        //auto splitter = stack.front();
        //stack.erase(stack.begin());
        auto t1 = part.c_i[splitter];
        //cout << "splitter: " << splitter << " (" << t1.first << "-" << t1.second << ")" << endl;
        for (Symbol a = 0; a < symbolCount; a++) {
            //cout << "considering symbol " << showChar(a) << endl;

            splitterSet(t1, a, tmp, part, tI);

            if (tmp.count() == 0)
                continue;
            //cout << "splitter set: " << tmp << '\n';
            part.split(tmp, stack);
        }
    }

    cout << "Done. Generating new table." << endl;
    //  cout << "c_i: " << show(c_i) << endl;
    //  cout << "p: " << show(p) << endl;
    //  cout << "pI: " << show(pI) << endl;
    //  cout << "c: " << show(c) << endl;

    generateFromMinimizationResults(part);
}

template<typename T>
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
    NumIterator& operator++() { ++x_; return *this; }
    bool operator!=(const NumIterator& other) { return x_ != other.x_; }
    difference_type operator-(const NumIterator& other) { return x_ - other.x_; }
};

template<typename T>
struct NumRange {
    T a_, b_;
    NumRange(T a, T b) : a_(a), b_(b) {}

    NumIterator<T> begin() const { return a_; }
    NumIterator<T> end() const { return b_; }
};

template<typename Symbol, typename State, typename TokenId>
void DFA<Symbol,State,TokenId>::determineDeadState() {
    //  cout << "determineDeadState" << endl;
    auto idempotent = [&] (State q) {
        const auto ptr = T.data() + q * symbolCount;
        return std::all_of(ptr, ptr + symbolCount, [&] (State q_) { return q_ == q; });
    };
    auto isDeadState = [&] (State q) {
        return !final[q] && idempotent(q);
    };
    auto stop = NumIterator<State>(stateCount);
    auto it = std::find_if(NumIterator<State>(0u), stop, isDeadState);
    if (it != stop) {
        deadState = *it;
        cout << deadState << " is the dead state" << endl;
    } else {
        cout << "there is no dead state" << endl;
        deadState = stateCount;
    }
}

template<typename Symbol, typename State, typename TokenId>
std::ostream& operator<<(std::ostream& s, const DFA<Symbol,State,TokenId>& dfa) {
    s << "digraph G {\n";
    for (unsigned int p = 0; p < dfa.stateCount; p++) {
        if (p == dfa.deadState)
            continue;

        if (dfa.final[p])
            s << "  " << p << "[shape = doublecircle];\n";
        const auto ptr = dfa.T.data() + p * dfa.symbolCount;
        for (auto a = 0u; a < dfa.symbolCount; ++a)
            if (ptr[a] != dfa.deadState)
                s << "  " << p << " -> " << ptr[a] << " [label = \"" << showCharEscaped(dfa.idToSymbol[a]) << "\"];\n";
    }
    s << "}\n";
    return s;
}

#endif
