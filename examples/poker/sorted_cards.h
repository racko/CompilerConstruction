#pragma once

#include "bits.h"
#include "ranges.h"
#include "transition.h"
#include <algorithm>
#include <cstdint>
#include <functional>
#include <iosfwd>
#include <iostream>

// bits 0 to 51 represent the 52 cards
struct SortedCards {
    bool HasCard(const std::uint32_t card) const { return bits & (1ULL << card); }
    bool HasCard(const std::uint32_t c, const std::uint32_t r) const { return HasCard(c * 13 + r); }
    std::uint64_t bits;
};

inline bool operator==(const SortedCards lhs, const SortedCards rhs) { return lhs.bits == rhs.bits; }
inline bool operator!=(const SortedCards lhs, const SortedCards rhs) { return lhs.bits != rhs.bits; }

std::ostream& operator<<(std::ostream& s, const SortedCards cards);

namespace std {
template <>
class hash<SortedCards> {
  public:
    std::size_t operator()(const SortedCards cards) const noexcept { return std::hash<std::uint64_t>()(cards.bits); }
};
} // namespace std

inline SortedCards addCard(const SortedCards cards, const std::uint32_t r) {
    SortedCards out{cards.bits};
    out.bits |= 1ULL << r;
    return out;
}

int countCards(const SortedCards cards);

inline auto children(const SortedCards cards) {
    // std::cout << "children(" << cards << ")\n";
    auto make_transition = [cards](const std::uint32_t card) {
        return Transition<SortedCards>{cards, addCard(cards, card), card};
    };
    auto is_valid = [](const Transition<SortedCards> t) { return t.child != t.parent; };
    const auto max_new_rank = std::min(countTrailingZeros<std::uint32_t>(cards.bits), 52U);
    const auto cardCount = countCards(cards);
    const auto min_new_rank = std::max(6U - static_cast<std::uint32_t>(cardCount), 0U);
    // std::cout << "max_new_rank: " << max_new_rank << '\n';
    return filter(
        is_valid,
        transform(make_transition, range(cardCount != 7 ? min_new_rank : max_new_rank, max_new_rank)));
}
