#include "constexpr.h"

namespace const_expr {
static_assert(count_trailing_zeros(0x0ULL) == 64);
static_assert(count_trailing_zeros(0x80000000ULL) == 31);
static_assert(count_trailing_zeros(0x800000000000ULL) == 47);
static_assert(count_trailing_zeros(0x80000000000000ULL) == 55);
static_assert(count_trailing_zeros(0x800000000000000ULL) == 59);
static_assert(count_trailing_zeros(0x2000000000000000ULL) == 61);
static_assert(count_trailing_zeros(0x4000000000000000ULL) == 62);
static_assert(count_trailing_zeros(0x8000000000000000ULL) == 63);
} // namespace const_expr
