#pragma once

#include <array>
#include <type_traits>

enum class HandType {
    HIGH_CARD,
    PAIR,
    TWO_PAIR,
    THREE_OF_A_KIND,
    STRAIGHT,
    FLUSH,
    FULL_HOUSE,
    FOUR_OF_A_KIND,
    STRAIGHT_FLUSH,
    /* ROYAL_FLUSHes are just the highest ranked straight flushes. No need to complicate the code further */
};

using high_card_t = std::integral_constant<HandType, HandType::HIGH_CARD>;
using pair_t = std::integral_constant<HandType, HandType::PAIR>;
using two_pair_t = std::integral_constant<HandType, HandType::TWO_PAIR>;
using three_k_t = std::integral_constant<HandType, HandType::THREE_OF_A_KIND>;
using straight_t = std::integral_constant<HandType, HandType::STRAIGHT>;
using flush_t = std::integral_constant<HandType, HandType::FLUSH>;
using full_house_t = std::integral_constant<HandType, HandType::FULL_HOUSE>;
using four_k_t = std::integral_constant<HandType, HandType::FOUR_OF_A_KIND>;
using straight_flush_t = std::integral_constant<HandType, HandType::STRAIGHT_FLUSH>;

class HandRank {
    HandType t_;
    // always exactly 5 cards count. Most of the time we need to compare fewer cards because of some constraints:
    // PAIR has 1 pair (of the same rank) and 3 kickers, so only 4 ranks
    // TWO_PAIRS: 2 pairs, 1 kicker = 3 ranks
    // THREE_OF_A_KIND: 1 triplet, 2 kickers = 3 ranks
    // STRAIGHT: The highest rank is enough
    // FLUSH: 5 ranks
    // FULL_HOUSE: 1 triplet, 1 pair = 2 ranks
    // FOUR_OF_A_KIND: 1 four-tuple, 1 kicker = 2 ranks
    // STRAIGHT_FLUSH: The highest rank is enough
    //
    // THIS RELATION BETWEEN t_ AND ranks_ IS A CLASS INVARIANT!
    // So don't make the members public.
    using rank = uint8_t;
    std::array<rank, 5> ranks_;

    // TODO: fancy, but currently not worth it:
    //       Could just do HandRank(...) : t_(...), ranks_{...} everywhere below
    //       without it being less maintainable.
    // NOTE: if sizeof...(ranks) < 5, the remaining ranks are value-initialized, which means zero-initialization for
    // uint8_t.
    template <typename... T, std::enable_if_t<(std::is_same_v<rank, T> && ...), int> = 0>
    HandRank(HandType t, T... ranks) : t_(t), ranks_{ranks...} {}

  public:
    // NOTE: if we provide less than 5 ranks, the remaining entries in ranks_ get value-initialized (set to 0).
    // (See http://en.cppreference.com/w/cpp/language/aggregate_initialization)
    // 0s are okay because they don't influence the comparison operators. Any fixed value would do.

    HandRank(high_card_t, rank a, rank b, rank c, rank d, rank e) : HandRank(HandType::HIGH_CARD, a, b, c, d, e) {}

    HandRank(pair_t, rank pair_rank, rank a, rank b, rank c) : HandRank(HandType::PAIR, pair_rank, a, b, c) {}

    HandRank(two_pair_t, rank pair_rank1, rank pair_rank2, rank a)
        : HandRank(HandType::TWO_PAIR, pair_rank1, pair_rank2, a) {}

    HandRank(three_k_t, rank three_k_rank, rank a, rank b) : HandRank(HandType::THREE_OF_A_KIND, three_k_rank, a, b) {}

    HandRank(straight_t, rank a) : HandRank(HandType::STRAIGHT, a) {}
    HandRank(flush_t, rank a, rank b, rank c, rank d, rank e) : HandRank(HandType::FLUSH, a, b, c, d, e) {}

    HandRank(full_house_t, rank three_k_rank, rank pair_rank)
        : HandRank(HandType::FULL_HOUSE, three_k_rank, pair_rank) {}

    HandRank(four_k_t, rank four_k_rank, rank a) : HandRank(HandType::FOUR_OF_A_KIND, four_k_rank, a) {}

    HandRank(straight_flush_t, rank a) : HandRank(HandType::STRAIGHT_FLUSH, a) {}

    HandType type() const { return t_; }
    const std::array<rank, 5>& ranks() const { return ranks_; }
};

inline bool operator==(const HandRank& a, const HandRank& b) {
    return a.type() == b.type() && a.ranks() == b.ranks();
}

inline bool operator<(const HandRank& a, const HandRank& b) {
    if (a.type() < b.type())
        return true;
    if (a.type() == b.type())
        return a.ranks() < b.ranks();
    return false;
}

std::ostream& operator<<(std::ostream& s, const HandRank& a);

HandRank rank(const int* const cards);
