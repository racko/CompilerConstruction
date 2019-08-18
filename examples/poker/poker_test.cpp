#include "poker.h"

#include "hand_rank.h"
#include <catch.hpp>

//TEST_CASE("HighCard", "[poker]") {
//    std::uint32_t cards[7] = {1, 3, 5, 7, 9, 11, 2};
//    REQUIRE(rank(cards) == HandRank(high_card_t{}, 10, 8, 6, 4, 2));
//}
//
//TEST_CASE("OnePair", "[poker]") {
//    std::uint32_t cards[7] = {1, 2, 13 + 1, 4, 5, 6, 7};
//    REQUIRE(rank(cards) == HandRank(pair_t{}, 0, 6, 5, 4));
//}
//
//TEST_CASE("TwoPair", "[poker]") {
//    std::uint32_t cards[7] = {1, 2, 13+1, 4, 13+2, 6, 7};
//    REQUIRE(rank(cards) == HandRank(two_pair_t{}, 1, 0, 6));
//}
//
//TEST_CASE("TwoPair with three pairs", "[poker]") {
//    std::uint32_t cards[7] = {1, 2, 13+1, 4, 13+2, 6, 13+4};
//    REQUIRE(rank(cards) == HandRank(two_pair_t{}, 3, 1, 5));
//}
//
//TEST_CASE("ThreeOfAKind", "[poker]") {
//    std::uint32_t cards[7] = {1, 2, 13+1, 4, 5, 6, 2*13+1};
//    REQUIRE(rank(cards) == HandRank(three_k_t{}, 0, 5, 4));
//}
//
//TEST_CASE("Straight", "[poker]") {
//    std::uint32_t cards[7] = {1, 2, 3, 4, 5, 6, 7};
//    REQUIRE(rank(cards) == HandRank(straight_t{}, 6));
//}
//
//TEST_CASE("Straight Ace to Five", "[poker]") {
//    std::uint32_t cards[7] = {1, 2, 3, 4, 8, 10, 13};
//    REQUIRE(rank(cards) == HandRank(straight_t{}, 3));
//}
//
//TEST_CASE("FullHouse", "[poker]") {
//    std::uint32_t cards[7] = {1, 2, 13+1, 4, 13+2, 6, 2*13+2};
//    REQUIRE(rank(cards) == HandRank(full_house_t{}, 1, 0));
//}
//
//TEST_CASE("FullHouse with two ThreeOfAKind", "[poker]") {
//    std::uint32_t cards[7] = {1, 2, 13+1, 2*13+1, 13+2, 6, 2*13+2};
//    REQUIRE(rank(cards) == HandRank(full_house_t{}, 1, 0));
//}
//
//TEST_CASE("FourOfAKind", "[poker]") {
//    std::uint32_t cards[7] = {1, 2, 13+1, 4, 3*13+1, 6, 2*13+1};
//    REQUIRE(rank(cards) == HandRank(four_k_t{}, 0, 5));
//}
//
//TEST_CASE("FourOfAKind with Pair", "[poker]") {
//    std::uint32_t cards[7] = {1, 2, 13+1, 13+6, 3*13+1, 6, 2*13+1};
//    REQUIRE(rank(cards) == HandRank(four_k_t{}, 0, 5));
//}
//
//TEST_CASE("FourOfAKind with ThreeOfAKind", "[poker]") {
//    std::uint32_t cards[7] = {1, 2*13+6, 13+1, 13+6, 3*13+1, 6, 2*13+1};
//    REQUIRE(rank(cards) == HandRank(four_k_t{}, 0, 5));
//}

///////////////////////////////////////////////////////

//TEST_CASE("HighCard", "[poker]") {
//    std::uint32_t cards[7] = {1, 3, 5, 7, 9, 11, 2};
//    REQUIRE(rank(cards) == HandRank(high_card_t{}, 10, 8, 6, 4, 2));
//}
//
//TEST_CASE("OnePair", "[poker]") {
//    std::uint32_t cards[7] = {1, 2, 1, 4, 5, 6, 7};
//    REQUIRE(rank(cards) == HandRank(pair_t{}, 0, 6, 5, 4));
//}
//
//TEST_CASE("TwoPair", "[poker]") {
//    std::uint32_t cards[7] = {1, 2, 1, 4, 2, 6, 7};
//    REQUIRE(rank(cards) == HandRank(two_pair_t{}, 1, 0, 6));
//}
//
//TEST_CASE("TwoPair with three pairs", "[poker]") {
//    std::uint32_t cards[7] = {1, 2, 1, 4, 2, 6, 4};
//    REQUIRE(rank(cards) == HandRank(two_pair_t{}, 3, 1, 5));
//}
//
//TEST_CASE("ThreeOfAKind", "[poker]") {
//    std::uint32_t cards[7] = {1, 2, 1, 4, 5, 6, 1};
//    REQUIRE(rank(cards) == HandRank(three_k_t{}, 0, 5, 4));
//}
//
//TEST_CASE("Straight", "[poker]") {
//    std::uint32_t cards[7] = {1, 2, 3, 4, 5, 6, 7};
//    REQUIRE(rank(cards) == HandRank(straight_t{}, 6));
//}
//
//TEST_CASE("Straight Ace to Five", "[poker]") {
//    std::uint32_t cards[7] = {1, 2, 3, 4, 8, 10, 13};
//    REQUIRE(rank(cards) == HandRank(straight_t{}, 3));
//}
//
//TEST_CASE("FullHouse", "[poker]") {
//    std::uint32_t cards[7] = {1, 2, 1, 4, 2, 6, 2};
//    REQUIRE(rank(cards) == HandRank(full_house_t{}, 1, 0));
//}
//
//TEST_CASE("FullHouse with two ThreeOfAKind", "[poker]") {
//    std::uint32_t cards[7] = {1, 2, 1, 1, 2, 6, 2};
//    REQUIRE(rank(cards) == HandRank(full_house_t{}, 1, 0));
//}
//
//TEST_CASE("FourOfAKind", "[poker]") {
//    std::uint32_t cards[7] = {1, 2, 1, 4, 1, 6, 1};
//    REQUIRE(rank(cards) == HandRank(four_k_t{}, 0, 5));
//}
//
//TEST_CASE("FourOfAKind with Pair", "[poker]") {
//    std::uint32_t cards[7] = {1, 2, 1, 6, 1, 6, 1};
//    REQUIRE(rank(cards) == HandRank(four_k_t{}, 0, 5));
//}
//
//TEST_CASE("FourOfAKind with ThreeOfAKind", "[poker]") {
//    std::uint32_t cards[7] = {1, 6, 1, 6, 1, 6, 1};
//    REQUIRE(rank(cards) == HandRank(four_k_t{}, 0, 5));
//}

////////////////////////////////////////////////////////////////

TEST_CASE("HighCard", "[poker]") {
    std::uint32_t cards[7] = {11, 9, 7, 5, 3, 2, 1};
    REQUIRE(rank(cards) == HandRank(high_card_t{}, 10, 8, 6, 4, 2));
}

TEST_CASE("OnePair", "[poker]") {
    std::uint32_t cards[7] = {13+1, 7, 6, 5, 4, 2, 1};
    REQUIRE(rank(cards) == HandRank(pair_t{}, 0, 6, 5, 4));
}

TEST_CASE("TwoPair", "[poker]") {
    std::uint32_t cards[7] = {13+2, 13+1, 7, 6, 4, 2, 1};
    REQUIRE(rank(cards) == HandRank(two_pair_t{}, 1, 0, 6));
}

TEST_CASE("TwoPair with three pairs", "[poker]") {
    std::uint32_t cards[7] = {13+4, 13+2, 13+1, 6, 4, 2, 1};
    REQUIRE(rank(cards) == HandRank(two_pair_t{}, 3, 1, 5));
}

TEST_CASE("ThreeOfAKind", "[poker]") {
    std::uint32_t cards[7] = {2*13+1, 13+1, 6, 5, 4, 2, 1};
    REQUIRE(rank(cards) == HandRank(three_k_t{}, 0, 5, 4));
}

TEST_CASE("Straight", "[poker]") {
    std::uint32_t cards[7] = {7, 6, 5, 4, 3, 2, 1};
    REQUIRE(rank(cards) == HandRank(straight_t{}, 6));
}

TEST_CASE("Straight Ace to Five", "[poker]") {
    std::uint32_t cards[7] = {13, 10, 8, 4, 3, 2, 1};
    REQUIRE(rank(cards) == HandRank(straight_t{}, 3));
}

TEST_CASE("FullHouse", "[poker]") {
    std::uint32_t cards[7] = {2*13+2, 13+2, 13+1, 6, 4, 2, 1};
    REQUIRE(rank(cards) == HandRank(full_house_t{}, 1, 0));
}

TEST_CASE("FullHouse with two ThreeOfAKind", "[poker]") {
    std::uint32_t cards[7] = {2*13+2, 2*13+1, 13+2, 13+1, 6, 2, 1};
    REQUIRE(rank(cards) == HandRank(full_house_t{}, 1, 0));
}

TEST_CASE("FourOfAKind", "[poker]") {
    std::uint32_t cards[7] = {3*13+1, 2*13+1, 13+1, 6, 4, 2, 1};
    REQUIRE(rank(cards) == HandRank(four_k_t{}, 0, 5));
}

TEST_CASE("FourOfAKind with Pair", "[poker]") {
    std::uint32_t cards[7] = {3*13+1, 2*13+1, 13+6, 13+1, 6, 2, 1};
    REQUIRE(rank(cards) == HandRank(four_k_t{}, 0, 5));
}

TEST_CASE("FourOfAKind with ThreeOfAKind", "[poker]") {
    std::uint32_t cards[7] = {3*13+1, 2*13+6, 2*13+1, 13+6, 13+1, 6, 1};
    REQUIRE(rank(cards) == HandRank(four_k_t{}, 0, 5));
}
