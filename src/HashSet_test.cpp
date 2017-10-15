#include <catch.hpp>
#include <iostream>

#include "HashSet.h"

TEST_CASE("HashSet_TestHash0", "[HashSet]") {
    HashSet a(10);
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
    HashSet b(10);
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
    std::hash<HashSet> h;
    REQUIRE(h(a) == h(b));
}

TEST_CASE("HashSet_TestHash1", "[HashSet]") {
    HashSet a(10);
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
    HashSet b(10);
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
    std::hash<HashSet> h;
    REQUIRE(h(a) != h(b));
}

TEST_CASE("", "") {
    HashSet s(10);
    s[9] = true;
    s.resize(5);
    REQUIRE(s.count() == 0);
}
