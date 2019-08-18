#include "determine_hand_rank.h"

#include "hand_rank.h"
#include "rank_count.h"
#include "straight.h"
#include <cstdint>

HandRank getHandRank(const RankCount k) {
    const auto kickerRanks = k.kickerRanks();
    const auto pairRanks = k.pairRanks();
    const auto threeKRanks = k.threeKRanks();
    const auto fourKRanks = k.fourKRanks();

    // logger << "kickerRanks: " << kickerRanks << '\n';
    // logger << "pairRanks: " << pairRanks << '\n';
    // logger << "threeKRanks: " << threeKRanks << '\n';
    // logger << "fourKRanks: " << fourKRanks << '\n';

    if (!fourKRanks.empty()) {
        return {four_k_t{}, fourKRanks.getLowestRank<std::uint8_t>(), kickerRanks.getLowestRank<std::uint8_t>()};
    }

    if (!threeKRanks.empty() && !pairRanks.empty()) {
        return {full_house_t{}, threeKRanks.getLowestRank<std::uint8_t>(), pairRanks.getLowestRank<std::uint8_t>()};
    }

    if (const auto straight_high_card = ComputeStraightHighCard(kickerRanks); straight_high_card != 0) {
        return {straight_t{}, straight_high_card};
    }

    if (!threeKRanks.empty()) {
        const auto lower_kicker = kickerRanks.getLowestRank<std::uint8_t>();
        const auto higher_kicker = kickerRanks.removeLowestRank().getLowestRank<std::uint8_t>();
        return {three_k_t{}, threeKRanks.getLowestRank<std::uint8_t>(), higher_kicker, lower_kicker};
    } else if (pairRanks.countRanks() > 1) {
        const auto lower_pair = pairRanks.getLowestRank<std::uint8_t>();
        const auto higher_pair = pairRanks.removeLowestRank().getLowestRank<std::uint8_t>();
        return {two_pair_t{}, higher_pair, lower_pair, kickerRanks.getLowestRank<std::uint8_t>()};
    } else if (!pairRanks.empty()) {
        const auto kicker3 = kickerRanks.getLowestRank<std::uint8_t>();
        const auto kicker2 = kickerRanks.removeLowestRank().getLowestRank<std::uint8_t>();
        const auto kicker1 = kickerRanks.removeNLowestRanks(2).getLowestRank<std::uint8_t>();
        return {pair_t{}, pairRanks.getLowestRank<std::uint8_t>(), kicker1, kicker2, kicker3};
    } else {
        const std::uint8_t kicker5 = kickerRanks.getLowestRank<std::uint8_t>();
        const std::uint8_t kicker4 = kickerRanks.removeLowestRank().getLowestRank<std::uint8_t>();
        const std::uint8_t kicker3 = kickerRanks.removeNLowestRanks(2).getLowestRank<std::uint8_t>();
        const std::uint8_t kicker2 = kickerRanks.removeNLowestRanks(3).getLowestRank<std::uint8_t>();
        const std::uint8_t kicker1 = kickerRanks.removeNLowestRanks(4).getLowestRank<std::uint8_t>();
        return {high_card_t{}, kicker1, kicker2, kicker3, kicker4, kicker5};
    }
}
