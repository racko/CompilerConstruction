#include "ranks.h"

#include <cstddef>
#include <cstdint>

std::size_t addRank(const std::size_t i, const std::uint32_t r) {
    auto bit = 1ULL << r;
    auto k = i | bit;
    int j = 0;
    while (k == i && j < 3) {
        bit <<= 13;
        k |= bit;
        ++j;
    }
    return k;
}
