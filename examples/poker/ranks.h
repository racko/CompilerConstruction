#pragma once

#include "bits.h"
#include <cstddef>
#include <cstdint>

template <typename T>
T removeLowestRank(const T v) {
    return clearLowestBit(v);
}

template <typename T>
T removeNLowestRanks(const T v, int n) {
    return clearNLowestBits(v, n);
}

template <typename T>
T getHighestRank(const T v) {
    return getHighestBit(v);
}

template <typename R = int, typename T>
R getLowestRank(const T v) {
    return countTrailingZeros<R>(v);
}

template <typename R = int, typename T>
R countCards(const T v) {
    return countBits<R>(v);
}

// Add another card of rank r. Bit (r + j*13) encodes the existence of (j+1) cards of rank r. (0 <= j < 4)
// If we already have 4 cards of rank r in i, return i.
std::size_t addRank(const std::size_t i, const std::uint32_t r);
