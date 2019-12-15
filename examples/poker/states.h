#pragma once

#include "Regex/nfaBuilder.h"
#include "types.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <ostream>
#include <unordered_map>
#include <utility>
#include <vector>

#define NOINLINE __attribute__((noinline))

template <typename StateRepr>
class States {
  public:
    States() {
        // nfaBuilder internally adds an initial state. We need to prepare idToState and stateToId accordingly
        idToState.emplace_back();
        stateToId[idToState[0]] = 0;

        // symbol 0 is eps
        for (Symbol i = 1U; i < 53U; ++i) {
            const auto id = builder.idToSymbol.size();
            builder.idToSymbol.push_back(i);
            builder.symbolToId[i] = id;
        }
        // for (Symbol i = 1U; i < 14U; ++i) {
        //    const auto id = builder.idToSymbol.size();
        //    builder.idToSymbol.push_back(i);
        //    builder.symbolToId[i] = id;
        //}
    }

    class State {
      public:
        State() = default;
        State(const StateId ii, States& states) : ii_(ii), states_(&states) {}
        StateRepr GetState() const { return states_->GetState(ii_); }
        StateId GetStateId() const { return ii_; }

        void SetHandType(const TerminalId handTypeId) { states_->SetHandType(ii_, handTypeId); }

        void AddTransitions(const std::uint32_t r, const State kk) { states_->AddTransitions(ii_, r, kk.GetStateId()); }

      private:
        StateId ii_{};
        States* states_{};
    };

    StateId GetStateCount() const {
        assert(idToState.size() < static_cast<std::size_t>(std::numeric_limits<StateId>::max()));
        return static_cast<StateId>(idToState.size());
    }

    const nfaBuilder<Symbol, StateId, TerminalId>& GetBuilder() const { return builder; }

    std::pair<State, bool> NOINLINE Insert(const StateRepr k) {
        if (const auto state = getStateId(k); state != nullptr) {
            // logger << "already known. id: " << *state << '\n';
            return {State(*state, *this), false};
        }
        const auto stateId = addState(k);
        // logger << "it's new: id " << stateId << '\n';
        return {State(stateId, *this), true};
    }

    template <typename Iterator>
    std::pair<State, bool>
        NOINLINE Insert(const StateRepr i, const TerminalId handTypeId, const Iterator start, const Iterator stop) {
        auto [ii, is_new] = Insert(i);
        // if (!is_new) {
        //    return {ii, false};
        //}
        // std::cout << "visiting " << i << " (" << ii.GetStateId() << ")\n";
        if (countCards(i) == 7) {
            ii.SetHandType(handTypeId);
            return {ii, true};
        }
        std::for_each(
            start, stop, [ii = ii](const std::pair<std::uint32_t, typename States<StateRepr>::State>& x) mutable {
                // std::cout << "  adding transition " << ii.GetStateId() << " -> " << kk.GetStateId() << " via "
                //          << labelled_child.first << '\n';
                ii.AddTransitions(x.first, x.second);
            });
        return {ii, true};
    }

    // if you need a const version, add one that returns a ConstState
    State Find(const StateRepr k) {
        if (const auto it = stateToId.find(k); it != stateToId.end()) {
            return State(it->second, *this);
        }
        return State(GetStateCount(), *this);
    }

    StateRepr GetState(const StateId ii) const { return idToState[ii]; }
    State GetFoo(const StateId ii) { return State(ii, *this); }

  private:
    const StateId* getStateId(const StateRepr k) const {
        if (const auto it = stateToId.find(k); it != stateToId.end()) {
            return &it->second;
        }
        return nullptr;
    };

    StateId addState(const StateRepr k) {
        assert(getStateId(k) == nullptr);
        const auto stateId = GetStateCount();
        builder.ns.emplace_back();
        idToState.push_back(k);
        stateToId[k] = stateId;
        return stateId;
    };

    void NOINLINE SetHandType(const StateId ii, const TerminalId handTypeId) {
        assert(builder.ns[ii].kind == 0 || builder.ns[ii].kind == handTypeId);
        builder.ns[ii].kind = handTypeId;
        // logger << "set hand type index to " << builder.ns[ii].kind << '\n';
    }

    void NOINLINE AddTransitions(const StateId ii, const std::uint32_t r, const StateId kk) {
        builder.ns[ii].ns[builder.symbolToId[1 + r]].push_back(kk);
        // builder.ns[ii].ns[builder.symbolToId[1 + 13 + r]].push_back(kk);
        // builder.ns[ii].ns[builder.symbolToId[1 + 26 + r]].push_back(kk);
        // builder.ns[ii].ns[builder.symbolToId[1 + 39 + r]].push_back(kk);
    }

    nfaBuilder<Symbol, StateId, TerminalId> builder;
    std::unordered_map<StateRepr, StateId> stateToId;
    std::vector<StateRepr> idToState;
};

template <typename StateRepr>
std::ostream& operator<<(std::ostream& stream, const typename States<StateRepr>::State& state) {
    return stream << state.GetStateId();
}

#undef NOINLINE
