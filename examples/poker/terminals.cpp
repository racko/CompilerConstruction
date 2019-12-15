#include "terminals.h"

#include "rank_count.h"
#include <cassert>
#include <cstddef>
#include <limits>

#include <iostream>

Terminals::Terminals() = default;

TerminalId Terminals::getHandTypeIndex(const RankCount handType) {
    assert(2 <= handType.countRanks());
    assert(handType.countRanks() <= 5);
    // std::cout << "Terminals::getHandTypeIndex(" << handType << ")\n";
    if (const auto it = terminalToId.find(handType); it != terminalToId.end()) {
        // std::cout << "already seen. id: " << it->second << '\n';
        return it->second;
    }
    assert(idToTerminal.size() < static_cast<std::size_t>(std::numeric_limits<TerminalId>::max()));
    const auto terminalID = static_cast<TerminalId>(idToTerminal.size());
    idToTerminal.push_back(handType);
    terminalToId[handType] = terminalID;
    // std::cout << "new. id: " << terminalID << '\n';
    return terminalID;
}

bool operator==(const Terminals& lhs, const Terminals& rhs) {
    return lhs.idToTerminal == rhs.idToTerminal && lhs.terminalToId == rhs.terminalToId;
}
