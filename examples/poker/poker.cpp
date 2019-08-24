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
    for (auto i = 0U; i < dfa.symbols_.count(); ++i) {
        if (dfa.states_.GetTransition(s, i) != dfa.states_.GetDeadState()) {
            stream << int(dfa.symbols_.idToSymbol(i)) << ' ';
        }
    }
    stream << '\n';
}
} // namespace

HandRank rank(const Symbol* const cards) {
    static auto x = loadRanker(logger);
    const auto& [dfa, terminals] = x;

    auto s = dfa.states_.GetStart();
    logger << "starting in state " << s << ", type " << dfa.states_.GetTokenId(s) << '\n';
    const auto scount = dfa.symbols_.count();
    const auto dstate = dfa.states_.GetDeadState();
    printValidCards(logger, dfa, s);
    assert(s != dstate);
    for (auto i = 0; i < 6; ++i) {
        logger << "symbol: " << cards[i] << '\n';
        auto sid = dfa.symbols_.symbolToId(cards[i]);
        if (sid == scount) {
            throw std::runtime_error("invalid card");
        }
        s = dfa.states_.GetTransition(s, sid);
        printValidCards(logger, dfa, s);
        assert(dfa.states_.GetTokenId(s) == 0);
        if (s == dstate) { // Not an assert because this is input validation.
            throw std::runtime_error("not a valid hand");
        }
    }
    s = dfa.states_.GetTransition(s, dfa.symbols_.symbolToId(cards[6]));
    if (s == dstate) { // Not an assert because this is input validation.
        throw std::runtime_error("not a valid hand");
    }
    assert(dfa.states_.GetTokenId(s) > 0);
    return getHandRank(terminals.getHandType(dfa.states_.GetTokenId(s)));
}
