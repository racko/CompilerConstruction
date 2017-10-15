#include <catch.hpp>
#include <iostream>

#include "BitSet.h"

TEST_CASE("BitSet_TestHash0", "[BitSet]") {
    BitSet a(10);
    BitSet b(10);
    a.p[0] = -1ULL;
    b.p[0] = 0;

    a[0] = 0;
    a[1] = 1;
    a[2] = 0;
    a[3] = 1;
    a[4] = 0;
    a[5] = 1;
    a[6] = 0;
    a[7] = 1;
    a[8] = 0;
    a[9] = 1;

    b[0] = 0;
    b[1] = 1;
    b[2] = 0;
    b[3] = 1;
    b[4] = 0;
    b[5] = 1;
    b[6] = 0;
    b[7] = 1;
    b[8] = 0;
    b[9] = 1;

    std::hash<BitSet> h;
    REQUIRE(h(a) == h(b));
}

TEST_CASE("BitSet_TestHash1", "[BitSet]") {
    BitSet a(10);
    BitSet b(10);

    a.p[0] = 0;
    b.p[0] = 0;

    a[0] = 0;
    a[1] = 1;
    a[2] = 0;
    a[3] = 1;
    a[4] = 0;
    a[5] = 1;
    a[6] = 0;
    a[7] = 1;
    a[8] = 0;
    a[9] = 1;

    b[0] = 0;
    b[1] = 1;
    b[2] = 0;
    b[3] = 1;
    b[4] = 0;
    b[5] = 1;
    b[6] = 0;
    b[7] = 1;
    b[8] = 0;
    b[9] = 0; // here's a difference, so the hashes will be different

    std::hash<BitSet> h;
    REQUIRE(h(a) != h(b));
}

TEST_CASE("BitSet::resize_bigger", "[BitSet]") {
    BitSet a(5);
    a[0] = 0;
    a[1] = 1;
    a[2] = 0;
    a[3] = 1;
    a[4] = 0;

    a.resize(10);
    REQUIRE(a.w == 1);
    REQUIRE(a.wordsInUse == 1);
    REQUIRE(a.size() == 10);
    REQUIRE(a[0] == 0);
    REQUIRE(a[1] == 1);
    REQUIRE(a[2] == 0);
    REQUIRE(a[3] == 1);
    REQUIRE(a[4] == 0);

    REQUIRE(a[5] == 0);
    REQUIRE(a[6] == 0);
    REQUIRE(a[7] == 0);
    REQUIRE(a[8] == 0);
    REQUIRE(a[9] == 0);
}

TEST_CASE("BitSet::resize_bigger1", "[BitSet]") {
    BitSet a(5);
    a[0] = 0;
    a[1] = 1;
    a[2] = 0;
    a[3] = 1;
    a[4] = 0;

    a.resize(10, true);
    REQUIRE(a.w == 1);
    REQUIRE(a.wordsInUse == 1);
    REQUIRE(a.size() == 10);
    REQUIRE(a[0] == 0);
    REQUIRE(a[1] == 1);
    REQUIRE(a[2] == 0);
    REQUIRE(a[3] == 1);
    REQUIRE(a[4] == 0);

    REQUIRE(a[5] == 1);
    REQUIRE(a[6] == 1);
    REQUIRE(a[7] == 1);
    REQUIRE(a[8] == 1);
    REQUIRE(a[9] == 1);
}

TEST_CASE("BitSet::resize_smaller", "[BitSet]") {
    BitSet a(10);
    a[0] = 0;
    a[1] = 1;
    a[2] = 0;
    a[3] = 1;
    a[4] = 0;
    a[5] = 1;
    a[6] = 0;
    a[7] = 1;
    a[8] = 0;
    a[9] = 1;

    a.resize(5);
    REQUIRE(a.w == 1);
    REQUIRE(a.wordsInUse == 1);
    REQUIRE(a.size() == 5);
    REQUIRE(a[0] == 0);
    REQUIRE(a[1] == 1);
    REQUIRE(a[2] == 0);
    REQUIRE(a[3] == 1);
    REQUIRE(a[4] == 0);
}

TEST_CASE("BitSet::resize_bigger2", "[BitSet]") {
    BitSet a(10, true);

    a.resize(65);
    REQUIRE(a.size() == 65);
    REQUIRE(a.w == 2); // a.w == 1
    REQUIRE(a.wordsInUse == 2);
    for (auto i = 0U; i < 10U; ++i)
        REQUIRE(a[i] == 1);
    for (auto i = 10U; i < 65U; ++i)
        REQUIRE(a[i] == 0);
}

TEST_CASE("BitSet::resize_bigger3", "[BitSet]") {
    BitSet a(10, true);

    a.resize(65, true);
    REQUIRE(a.size() == 65);
    REQUIRE(a.w == 2); // a.w == 1
    REQUIRE(a.wordsInUse == 2);
    for (auto i = 0U; i < 10U; ++i)
        REQUIRE(a[i] == 1);
    for (auto i = 10U; i < 65U; ++i)
        REQUIRE(a[i] == 1);
}

TEST_CASE("BitSet::resize_smaller2", "[BitSet]") {
    BitSet a(65, true);

    a.resize(10);
    REQUIRE(a.size() == 10);
    REQUIRE(a.w == 2); // a.w == 4
    REQUIRE(a.wordsInUse == 1);
    for (auto i = 0U; i < 10U; ++i)
        REQUIRE(a[i] == 1);
}

TEST_CASE("BitSet::count1", "[BitSet]") {
    BitSet a(10);
    a[0] = 0;
    a[1] = 1;
    a[2] = 0;
    a[3] = 1;
    a[4] = 0;
    a[5] = 1;
    a[6] = 0;
    a[7] = 1;
    a[8] = 0;
    a[9] = 1;

    REQUIRE(a.count() == 5);
}

TEST_CASE("BitSet::count2", "[BitSet]") {
    BitSet a(65, true);
    REQUIRE(a.count() == 65);
}
