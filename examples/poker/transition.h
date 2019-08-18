#pragma once

#include <cstdint>

template <typename StateRepr>
struct Transition {
    StateRepr parent;
    StateRepr child;
    std::uint32_t r;
};
