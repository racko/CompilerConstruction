#include "poker.h"

#include "Print.h"
#include "determine_hand_rank.h"
#include "hand_rank.h"
#include "poker_dfa_serialization.h"
#include <iostream>

namespace {
struct NullLogger : std::ostream {};
NullLogger null_logger;
[[maybe_unused]] std::ostream& logger = std::cout;

// TODO: Also add a class for the rank count encoding uint64_t we use everywhere

[[maybe_unused]] void
printValidCards(std::ostream& stream, const DFA<Symbol, StateId, TerminalId>& dfa, const StateId s) {
    const auto& transitions = dfa.T.data() + dfa.symbolCount * s;
    for (auto i = 0U; i < dfa.symbolCount; ++i) {
        if (transitions[i] != dfa.deadState) {
            stream << int(dfa.idToSymbol[i]) << ' ';
        }
    }
    stream << '\n';
}
} // namespace

HandRank rank(const Symbol* const cards) {
    static auto x = loadRanker(logger);
    const auto& [dfa, terminals] = x;

    auto s = dfa.start;
    const auto fptr = dfa.finals.data();
    logger << "starting in state " << s << ", type " << fptr[s] << '\n';
    logger << "dfa.symbolToId.size(): " << dfa.symbolToId.size() << '\n';
    const auto Tptr = dfa.T.data();
    const auto scount = dfa.symbolCount;
    const auto dstate = dfa.deadState;
    printValidCards(logger, dfa, s);
    assert(s != dstate);
    for (auto i = 0; i < 6; ++i) {
        logger << "symbol: " << cards[i] << '\n';
        auto sid = dfa.symbolToId[cards[i]];
        if (sid == scount) {
            throw std::runtime_error("invalid card");
        }
        s = Tptr[s * scount + sid];
        printValidCards(logger, dfa, s);
        assert(fptr[s] == 0);
        if (s == dstate) { // Not an assert because this is input validation.
            throw std::runtime_error("not a valid hand");
        }
    }
    s = Tptr[s * scount + dfa.symbolToId[cards[6]]];
    if (s == dstate) { // Not an assert because this is input validation.
        throw std::runtime_error("not a valid hand");
    }
    assert(fptr[s] > 0);
    return getHandRank(terminals.getHandType(fptr[s]));
}
