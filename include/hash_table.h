#pragma once

#include <array>

namespace {
constexpr std::array<uint64_t,256> make_hashtable() {
    std::array<uint64_t,256> out{};
    uint64_t h = 0x544B2FBACAAF1684ULL;
    for (auto j = 0U; j < 256U; j++) {
        for (auto i = 0U; i < 31U; i++) {
            h = (h >> 7) ^ h;
            h = (h << 11) ^ h;
            h = (h >> 10) ^ h;
        }
        out[j] = h;
    }
    return out;
}

constexpr std::array<uint64_t,256> hashfn_tab = make_hashtable();
}

