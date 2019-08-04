#pragma once
#include "constexpr.h"

template <typename State, typename Class, int64_t MaxNodes>
struct partition {
    using Position = int64_t;
    using PositionRange = pair<Position, Position>;

    vector<State,MaxNodes> p;
    vector<Position,MaxNodes> pI;
    vector<PositionRange,MaxNodes> c_i;
    vector<Class,MaxNodes> c;

    template<typename TokenId>
    constexpr partition(const vector<TokenId,MaxNodes>& finals);

    template <typename Set>
    constexpr bool swapToFront(Position l, Position h, const Set& tmp);

    template <typename Set>
    constexpr bool swapToBack(Position l, Position h, const Set& tmp);

    template <typename Set>
    constexpr Position swapRest(Position l, Position h, const Set& tmp);

    constexpr void update(Position l, Position h, Position j, Class b);

    template <typename Set>
    constexpr void split(const Set& tmp);
};

template<typename State, typename Class, int64_t MaxNodes>
template<typename TokenId>
constexpr partition<State,Class,MaxNodes>::partition(const vector<TokenId,MaxNodes>& finals) : p(finals.size()), pI(finals.size()), c(finals.size()) {
    auto stateCount = finals.size();
    vector<vector<State,MaxNodes>,MaxNodes> kinds; // given TokenId k, kinds[k] is vector<State> containing states of kind k

    for (State i = 0; i < stateCount; i++) {
        if (kinds.size() <= finals[i])
            kinds.resize(finals[i] + 1);
        kinds[finals[i]].push_back(i);
    }

    // std::cout << "count of kinds: ";
    // for (auto& it: kinds)
    //    std::cout << show(it);
    // std::cout << std::endl;

    /// @TODO: implement constexpr sort
    // sort(kinds.begin(), kinds.end(), [] (const auto& a, const auto& b) { return a.size() < b.size(); });

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

    // std::cout << "c: " << show(c) << std::endl;
    // std::cout << "c_i: " << show(c_i) << std::endl;
    // std::cout << "p: " << show(p) << std::endl;
    // std::cout << "pI: " << show(pI) << std::endl;
}

template <typename State, typename Class, int64_t MaxNodes>
template <typename Set>
constexpr bool partition<State, Class, MaxNodes>::swapToFront(Position l, Position h, const Set& tmp) {
    // std::cout << "swapping in-splitter to front" << std::endl;
    for (auto i = l; i < h; i++) {
        if (tmp.get(p[i])) {
            swap(p[l], p[i]);
            // std::cout << "swapping " << l << " and " << i << ": " << show(p) << std::endl;
            pI[p[l]] = l;
            pI[p[i]] = i;
            return true;
        }
    }
    return false;
}

template <typename State, typename Class, int64_t MaxNodes>
template <typename Set>
constexpr bool partition<State, Class, MaxNodes>::swapToBack(Position l, Position h, const Set& tmp) {
    // std::cout << "swapping not-in-splitter to end" << std::endl;
    auto last = h - 1;
    for (auto i = static_cast<int>(last); i >= int(l); i--) {
        if (!tmp.get(p[i])) {
            swap(p[i], p[last]);
            // std::cout << "swapping " << i << " and " << last << ": " << show(p) << std::endl;
            pI[p[last]] = last;
            pI[p[i]] = i;
            return true;
        }
    }
    return false;
}

template <typename State, typename Class, int64_t MaxNodes>
template <typename Set>
constexpr auto partition<State, Class, MaxNodes>::swapRest(Position l, Position h, const Set& tmp) -> Position {
    // std::cout << "swapping all others" << std::endl;
    auto j = l;
    auto k = h - 1;
    for (;;) {
        do
            j++;
        while (tmp.get(p[j]));
        do
            k--;
        while (!tmp.get(p[k]));
        if (k < j)
            break;
        swap(p[j], p[k]);
        // std::cout << "swapping " << j << " and " << k << ": " << show(p) << std::endl;
        pI[p[j]] = j;
        pI[p[k]] = k;
    }
    return j;
}

template <typename State, typename Class, int64_t MaxNodes>
constexpr void partition<State, Class, MaxNodes>::update(Position l, Position h, Position j, Class b) {
    auto A = make_pair(l, j);
    auto B = make_pair(j, h);

    // std::cout << "B': (" << A.first << "-" << A.second << ")" << std::endl;
    // std::cout << "B'': (" << B.first << "-" << B.second << ")" << std::endl;

    auto A_size = A.second - A.first;
    auto B_size = B.second - B.first;
    auto newIx = c_i.size();

    if (A_size > B_size) {
        c_i[b] = A;
        c_i.push_back(B);
        fill_n(c.begin() + B.first, B_size, newIx);
        // std::cout << "Pushing B'' on the stack. Index: " << newIx << std::endl;
    } else {
        c_i[b] = B;
        c_i.push_back(A);
        fill_n(c.begin() + A.first, A_size, newIx);
        // std::cout << "Pushing B' on the stack. Index: " << newIx << std::endl;
    }
}

template <typename State, typename Class, int64_t MaxNodes>
template <typename Set>
constexpr void partition<State, Class, MaxNodes>::split(const Set& tmp) {
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
        update(l, h, s, b);
    }
}

