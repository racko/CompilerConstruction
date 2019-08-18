#pragma once

#include "rank_count.h"
#include "types.h"
#include <unordered_map>
#include <vector>

#ifndef NULL
#define NULL 0
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/vector.hpp>
#endif
#undef NULL

class Terminals {
    // We insert dummy entries because terminalID 0 is reserved for
    // non-terminals :)
    // We know that we will never call getHandTypeIndex with a handType of 0
    // because we only call it with integers with at least one set bit, so
    // inserting 0 as the dummy handType will not cause it to be found and
    // returned in getHandTypeIndex.
    std::vector<RankCount> idToTerminal{RankCount{}};
    std::unordered_map<RankCount, TerminalId> terminalToId{{RankCount{}, 0}};

  public:
    Terminals();

    TerminalId getHandTypeIndex(const RankCount handType);

    RankCount getHandType(const TerminalId id) const { return idToTerminal.at(id); }

    std::size_t size() const { return idToTerminal.size(); }

    friend bool operator==(const Terminals& lhs, const Terminals& rhs);

    template <typename Archive>
    void serialize(Archive& ar, unsigned int) {
        ar& idToTerminal;
        ar& terminalToId;
    }
};
