#pragma once

#include <cstdint>

// https://graphics.stanford.edu/~seander/bithacks.html

template <typename T>
T clearLowestBit(const T v) {
    return v & v - 1;
}

template <typename T>
T clearNLowestBits(T v, const int n) {
    for (int j = 0; j < n; j++)
        v = clearLowestBit(v);
    return v;
}

template <typename T>
T getHighestBit(T v) {
    T result{v};
    while ((v = clearLowestBit(v)) != 0)
        result = v;
    return result;
}

template <typename T>
T getLowestBit(T v) {
    return -v & v;
}

template <typename R = int>
R countTrailingZeros(std::uint64_t v) {
    R c; // c will be the number of zero bits on the right,
         // so if v is 1101000 (base 2), then c will be 3
    // NOTE: if 0 == v, then c = 31.
    if (v & 0x1) {
        // special case for odd v (assumed to happen half of the time)
        c = 0;
    } else {
        c = 1;
        if ((v & 0xffffffff) == 0) {
            v >>= 32;
            c += 32;
        }
        if ((v & 0xffff) == 0) {
            v >>= 16;
            c += 16;
        }
        if ((v & 0xff) == 0) {
            v >>= 8;
            c += 8;
        }
        if ((v & 0xf) == 0) {
            v >>= 4;
            c += 4;
        }
        if ((v & 0x3) == 0) {
            v >>= 2;
            c += 2;
        }
        c -= v & 0x1;
    }
    return c;
}

template <typename R = int>
R countTrailingZeros(std::uint32_t v) {
    R c; // c will be the number of zero bits on the right,
         // so if v is 1101000 (base 2), then c will be 3
    // NOTE: if 0 == v, then c = 31.
    if (v & 0x1) {
        // special case for odd v (assumed to happen half of the time)
        c = 0;
    } else {
        c = 1;
        if ((v & 0xffff) == 0) {
            v >>= 16;
            c += 16;
        }
        if ((v & 0xff) == 0) {
            v >>= 8;
            c += 8;
        }
        if ((v & 0xf) == 0) {
            v >>= 4;
            c += 4;
        }
        if ((v & 0x3) == 0) {
            v >>= 2;
            c += 2;
        }
        c -= v & 0x1;
    }
    return c;
}

template <typename R = int, typename T>
R countBits(T v) {
    R c = 0; // c accumulates the total bits set in v
    for (; v; ++c)
        v = clearLowestBit(v); // clear the least significant bit set
    return c;
}

