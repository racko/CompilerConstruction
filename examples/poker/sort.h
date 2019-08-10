#pragma once

#include <array>

// TODO: Use sort5 before giving the cards to the dfa.
//       Check assembly to make sure that sort5 works on registers only. Then it should be extremely efficient and using
//       it should be better than having a lot more states in the dfa to deal with unsorted cards.
namespace merge {
inline constexpr void swap(int& a, int& b) {
    int tmp = a;
    a = b;
    b = tmp;
}
inline constexpr void merge11(int& a0, int& a1) {
    if (a0 >= a1)
        swap(a0, a1);
}

inline constexpr void merge12(int& x0, int& x1, int& x2) {
    if (x0 < x1) {
        return;
    } else {
        swap(x0, x1);
        merge11(x1, x2);
    }
}

inline constexpr void rotate_right3(int& x0, int& x1, int& x2) {
    swap(x0, x1); // b, a, c
    swap(x0, x2); // c, a, b
}

inline constexpr void merge21(int& x0, int& x1, int& x2) {
    if (x0 < x2) {
        merge11(x1, x2);
    } else {
        rotate_right3(x0, x1, x2);
        merge11(x1, x2);
    }
}

inline constexpr void merge13(int& x0, int& x1, int& x2, int& x3) {
    if (x0 < x1) {
        return;
    } else {
        swap(x0, x1);
        merge12(x1, x2, x3);
    }
}

inline constexpr void merge22(int& x0, int& x1, int& x2, int& x3) {
    if (x0 < x2) {
        merge12(x1, x2, x3);
    } else {
        rotate_right3(x0, x1, x2);
        merge21(x1, x2, x3);
    }
}

inline constexpr void merge23(int& x0, int& x1, int& x2, int& x3, int& x4) {
    if (x0 < x2) {
        merge13(x1, x2, x3, x4);
    } else {
        rotate_right3(x0, x1, x2);
        merge22(x1, x2, x3, x4);
    }
}

constexpr std::array<int, 5> sort5(std::array<int, 5> x) {
    auto a = x[0];
    auto b = x[1];
    auto c = x[2];
    auto d = x[3];
    auto e = x[4];
    merge11(a, b);
    merge11(d, e);
    merge12(c, d, e);
    merge23(a, b, c, d, e);
    return {a, b, c, d, e};
}
} // namespace merge
