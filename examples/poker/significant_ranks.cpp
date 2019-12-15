#include "significant_ranks.h"

#include "rank_count.h"
#include "straight.h"
#include <cassert>

#include <iostream>

RankCount getSignificantRanks(const RankCount k) {
    // auto kickerMask = 0x1FFFLL;
    // auto pairMask = 0x1FFFLL << 13;
    // auto threeKMask = 0x1FFFLL << 26;
    // auto fourKMask = 0x1FFFLL << 39;

    // const auto kickerRanks = k & 0x1FFF;
    // const auto pairRanks = (k >> 13) & 0x1FFF;
    // const auto threeKRanks = (k >> 26) & 0x1FFF;
    // const auto fourKRanks = (k >> 39) & 0x1FFF;

    assert(2 <= k.countRanks());
    assert(k.countRanks() <= 7);

    const auto kickerRanks = k.kickerRanks();
    const auto pairRanks = k.pairRanks();
    const auto threeKRanks = k.threeKRanks();
    const auto fourKRanks = k.fourKRanks();

    assert(fourKRanks.countRanks() <= 1);
    assert(threeKRanks.countRanks() <= 2);
    assert(pairRanks.countRanks() <= 3);
    assert(kickerRanks.countRanks() <= 7);

    // logger << "kickerRanks: " << kickerRanks << '\n';
    // logger << "pairRanks: " << pairRanks << '\n';
    // logger << "threeKRanks: " << threeKRanks << '\n';
    // logger << "fourKRanks: " << fourKRanks << '\n';

    if (!fourKRanks.empty()) {
        // logger << "got 4K\n";

        // The other three cards may be a 3K, a pair + kicker or three kickers.
        // We don't care, we can just select the highest rank from among the
        // kickerRanks.
        const auto kicker = kickerRanks.removeRanks(fourKRanks).getHighestRank();
        // there can only be a single 4K because we only have seven cards. So we
        // don't need to check for the highest ranked 4K.
        return RankCount{fourKRanks, RankSet{}, RankSet{}, kicker};
    }

    const auto pairCount = pairRanks.countRanks();
    // logger << "pairCount: " << pairCount << '\n';

    // To recognize a FH, we need 1 3K and at least TWO pairs because the 3K
    // is also counted as a pair.
    if (!threeKRanks.empty() && pairCount > 1) {
        // logger << "got FH\n";

        const auto threeKRank = threeKRanks.getHighestRank();
        const auto pairRank = pairRanks.removeRanks(threeKRank).getHighestRank();
        return RankCount{RankSet{}, threeKRank, pairRank, RankSet{}};
    } else if (const auto straight_ranks = ComputeStraightRanks(kickerRanks); !straight_ranks.empty()) {
        // std::cout << "got straight: " << straight_ranks << "\n";
        return RankCount{RankSet{}, RankSet{}, RankSet{}, straight_ranks};
    } else if (!threeKRanks.empty()) {
        // logger << "got 3K\n";

        // We know that there are neither 4K nor other pairs (other than the 3K
        // which can be counted as a pair). That's why we know that the
        // remaining FOUR cards all have DIFFERENT RANKS. Since we need two
        // kickers, we can just remove the lower two ranks.
        const auto kickers = kickerRanks.removeRanks(threeKRanks).removeNLowestRanks(2);
        // A second 3K would be counted as another pair in which case we would
        // have entered the FH branch instead. That's why we don't need to check
        // for the highest ranked 3K.
        return RankCount{RankSet{}, threeKRanks, RankSet{}, kickers};
    } else if (pairCount > 1) {
        // logger << "got 2P\n";

        // Since there are no 4K or 3K, we know that only pairs or kickers
        // remain. Also there are at least two pairs. What about the remaining 3
        // cards? There could be another pair or three kickers. All remaining
        // cards have ranks different from the two highest pairs of highest
        // rank. Otherwise they wouldn't be pairs.
        const auto two_highest_ranked_pairs = pairCount == 2 ? pairRanks : pairRanks.removeLowestRank();

        const auto kicker = kickerRanks.removeRanks(two_highest_ranked_pairs).getHighestRank();
        return RankCount{RankSet{}, RankSet{}, two_highest_ranked_pairs, kicker};
    } else if (!pairRanks.empty()) {
        // logger << "got 1P\n";

        // We have exactly one pair and 5 kickers, all having ranks different
        // from the pair's rank (or it wouldn't be a pair) and from each other.
        // That's why we don't need to check for the highest ranked pair. From
        // among the 5 kickers we have to select the three highest ranked.
        const auto kickers = kickerRanks.removeRanks(pairRanks).removeNLowestRanks(2);
        return RankCount{RankSet{}, RankSet{}, pairRanks, kickers};
    } else {
        // logger << "got HC\n";

        // There are no duplicate ranks whatsoever, so we have 7 kickers and
        // have to remove the two lowest ranked.
        return RankCount{RankSet{}, RankSet{}, RankSet{}, kickerRanks.removeNLowestRanks(2)};
    }
}
