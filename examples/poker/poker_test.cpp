#include "poker.h"

#include <catch.hpp>

TEST_CASE("HighCard", "[poker]") {
    int cards[7] = {1, 2, 3, 4, 5, 6, 7};
    REQUIRE(rank(cards) == HandRank(high_card_t{}, 6, 5, 4, 3, 2));
}

TEST_CASE("OnePair", "[poker]") {
    int cards[7] = {1, 2, 14, 4, 5, 6, 7};
    REQUIRE(rank(cards) == HandRank(pair_t{}, 0, 6, 5, 4));
}

TEST_CASE("TwoPair", "[poker]") {
    int cards[7] = {1, 2, 14, 4, 15, 6, 7};
    REQUIRE(rank(cards) == HandRank(two_pair_t{}, 1, 0, 6));
}

TEST_CASE("TwoPair with three pairs", "[poker]") {
    int cards[7] = {1, 2, 14, 4, 15, 6, 17};
    REQUIRE(rank(cards) == HandRank(two_pair_t{}, 3, 1, 5));
}

TEST_CASE("ThreeOfAKind", "[poker]") {
    int cards[7] = {1, 2, 14, 4, 5, 6, 27};
    REQUIRE(rank(cards) == HandRank(three_k_t{}, 0, 5, 4));
}

TEST_CASE("FullHouse", "[poker]") {
    int cards[7] = {1, 2, 14, 4, 15, 6, 28};
    REQUIRE(rank(cards) == HandRank(full_house_t{}, 1, 0));
}

TEST_CASE("FullHouse with two ThreeOfAKind", "[poker]") {
    int cards[7] = {1, 2, 14, 27, 15, 6, 28};
    REQUIRE(rank(cards) == HandRank(full_house_t{}, 1, 0));
}

TEST_CASE("FourOfAKind", "[poker]") {
    int cards[7] = {1, 2, 14, 4, 40, 6, 27};
    REQUIRE(rank(cards) == HandRank(four_k_t{}, 0, 5));
}

TEST_CASE("FourOfAKind with Pair", "[poker]") {
    int cards[7] = {1, 2, 14, 19, 40, 6, 27};
    REQUIRE(rank(cards) == HandRank(four_k_t{}, 0, 5));
}

TEST_CASE("FourOfAKind with ThreeOfAKind", "[poker]") {
    int cards[7] = {1, 32, 14, 19, 40, 6, 27};
    REQUIRE(rank(cards) == HandRank(four_k_t{}, 0, 5));
}
