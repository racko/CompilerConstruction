#include "sorted_cards_evaluation.h"

#include "rank_count.h"
#include "significant_ranks.h"
#include "sorted_cards.h"
#include <cassert>

namespace {
RankCount countRanks(const SortedCards cards) {
    assert(countCards(cards) == 7);

    RankCount k;
    for (auto c = 0U; c < 4U; ++c) {
        const auto base = c * 13U;
        for (auto r = 0U; r < 13U; ++r) {
            if (cards.HasCard(base + r)) {
                k = k.addRank(r);
            }
        }
    }
    assert(2 <= k.countRanks());
    assert(k.countRanks() <= 7);
    return k;
}
} // namespace

RankCount getSignificantRanks(const SortedCards cards) { return getSignificantRanks(countRanks(cards)); }
