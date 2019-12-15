#include "straight.h"

#include "Regex/Print.h"
#include "ranges.h"
#include "rank_set.h"
#include <algorithm>
#include <cstdint>
#include <iostream>

constexpr auto ace_to_five_mask = 0b1000000001111ULL;
constexpr std::uint64_t straight_mask(const std::uint64_t r) { return 0b11111ULL << (r - 4); };

// Returns 0 if there is no straight
std::uint8_t ComputeStraightHighCard(const RankSet kickerRanks) {
    // std::cout << "ComputeStraightHighCard(" << kickerRanks << ")\n";
    const auto k = kickerRanks.bits();
    const auto straight_bits = [k](const auto r) { return k & straight_mask(r); };
    const auto is_straight = [straight_bits](const auto r) {
        // std::cout << "straight_mask(" << int(r) << "): " << bin(straight_mask(r)) << '\n';
        // std::cout << "straight_bits(" << int(r) << "): " << bin(straight_bits(r)) << '\n';
        return straight_bits(r) == straight_mask(r);
    };
    // 6 to Ace (+1)-> 4 to 12 (+1)
    const auto rev_ranks = reverse(range(std::uint8_t{4}, std::uint8_t{13}));
    const auto foo = std::find_if(rev_ranks.begin(), rev_ranks.end(), is_straight);
    const auto is_straight_ace_to_five = [k, mask = ace_to_five_mask] { return (k & mask) == mask; };
    return foo != rev_ranks.end() ? *foo : is_straight_ace_to_five() ? 3 : 0;
}

// Returns an empty RankSet if there is no straight
RankSet ComputeStraightRanks(const RankSet kickerRanks) {
    switch (const auto r = ComputeStraightHighCard(kickerRanks)) {
    case 0:
        // std::cout << "not a straight\n";
        return RankSet{};
    case 3:
        // std::cout << "straight ace to five\n";
        return RankSet{ace_to_five_mask};
    default:
        // std::cout << "straight " << int(r - 4) << " to " << int(r) << '\n';
        return RankSet{straight_mask(r)};
    }
}
