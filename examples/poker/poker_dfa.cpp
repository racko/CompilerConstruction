#include "poker_dfa.h"

#include "DFA.h"
#include "DFA_minimization.h"
#include "NFA.h"
#include "NFA_to_DFA.h"
#include "iterative_fold.h"
#include "nfaBuilder.h"
#include "rank_count.h"
#include "recursive_fold.h"
#include "significant_ranks.h"
#include "sorted_cards.h"
#include "sorted_cards_evaluation.h"
#include "states.h"
#include "terminals.h"
#include "types.h"
#include <chrono>
#include <ostream>
#include <utility>

#define NOINLINE __attribute__((noinline))

namespace {
template <typename StateRepr>
class PokerNfaBuilder {
    Terminals terminals;
    States<StateRepr> states;

    // lambda would be simpler but doesn't compile:
    // We don't know the iterator type here and we don't know the return value in the tree-fold.
    // To get the return value we do a declval with nullptrs. With this struct it works fine, but
    // with a lambda with auto parameters the compiler somehow compiles more and sees that we don't provide the correct
    // type (std::pair<std::uint32_t, ...::State>), which we can't provide because we are trying to determine it ...
    //
    // So it might work in the future with templated lambda expressions (the auto parameters are the only difference I
    // see to the struct), but I actually don't understand why the compiler looks deeper in one case and not in the
    // other ...
    struct MakeState {
        template <typename Iterator>
        typename States<StateRepr>::State NOINLINE operator()(const StateRepr i, Iterator start, const Iterator stop) {
            const auto hand_type =
                countCards(i) == 7 ? context_->terminals.getHandTypeIndex(getSignificantRanks(i)) : TerminalId{};
            return context_->states.Insert(i, hand_type, start, stop).first;
        }
        PokerNfaBuilder* context_;
    };

    void iterative(const std::size_t initial_memory_size) {
        foldIterative(MakeState{this}, StateRepr{}, initial_memory_size);
    }

    void recursive(const std::size_t initial_memory_size) {
        foldRecursive(MakeState{this}, StateRepr{}, initial_memory_size);
    }

  public:
    PokerNfaBuilder(std::ostream& logger, const std::size_t initial_memory_size) {
        const auto start = std::chrono::system_clock::now();
        recursive(initial_memory_size);
        const auto d = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(
                           std::chrono::system_clock::now() - start)
                           .count();
        logger << d << "ms\n";
    }

    std::pair<nfaBuilder<Symbol, StateId, TerminalId>, Terminals> GetRanker() const {
        return {states.GetBuilder(), terminals};
    }
};

[[maybe_unused]] std::pair<nfaBuilder<Symbol, StateId, TerminalId>, Terminals>
make_RankCount_nfaBuilder(std::ostream& logger) {
    const std::size_t initial_memory_size{80000};
    return PokerNfaBuilder<RankCount>(logger, initial_memory_size).GetRanker();
}

[[maybe_unused]] std::pair<nfaBuilder<Symbol, StateId, TerminalId>, Terminals>
make_SortedCards_nfaBuilder(std::ostream& logger) {
    const std::size_t initial_memory_size{80000}; // TODO
    return PokerNfaBuilder<SortedCards>(logger, initial_memory_size).GetRanker();
}

std::pair<NFA<Symbol, StateId, TerminalId>, Terminals> make_NFA(std::ostream& logger) {
    const auto builder = make_RankCount_nfaBuilder(logger);
    // const auto builder = make_SortedCards_nfaBuilder(logger);
    logger << "nfa builder done\n";
    logger << "state count: " << builder.first.ns.size() << '\n';
    return {builder};
}

std::pair<DFA<Symbol, StateId, TerminalId>, Terminals> make_DFA(std::ostream& logger) {
    const auto nfa = make_NFA(logger);
    logger << "nfa done\n";
    // logger << nfa.first << '\n';
    return {toDFA(nfa.first), std::move(nfa.second)};
}
} // namespace

std::pair<DFA<Symbol, StateId, TerminalId>, Terminals> make_minimal_DFA(std::ostream& logger) {
    auto dfa = make_DFA(logger);
    logger << "dfa done\n";
    logger << dfa.first << '\n';
    dfa.first = minimize(dfa.first);
    logger << "minimization done\n";
    logger << dfa.first << '\n';
    return dfa;
}
