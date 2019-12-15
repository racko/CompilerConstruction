#pragma once

#include "rank_set.h"
#include <cstdint>

// Returns 0 if there is no straight
std::uint8_t ComputeStraightHighCard(const RankSet kickerRanks);

// Returns an empty RankSet if there is no straight
RankSet ComputeStraightRanks(const RankSet kickerRanks);
