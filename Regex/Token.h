#pragma once

#include <cstddef>

template <typename TokenId>
struct Token {
    const char* start;
    std::size_t length;
    TokenId tokenId;

    constexpr Token(const char* s, std::size_t l, TokenId t) : start(s), length(l), tokenId(t) {}
};
