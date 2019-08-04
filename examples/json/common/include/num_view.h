#pragma once

#include <string_view>

struct num_view {
    std::string_view x;

    num_view(const std::string_view& x_) : x(x_) {}
    num_view(const char* s, size_t n) : x(s, n) {}
};

inline bool operator==(const num_view& a, const num_view& b) { return a.x == b.x; }
