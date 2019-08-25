#pragma once

#include "DFA_fwd.h"

#include <BitSet.h> // for BitSet
#include <boost/serialization/vector.hpp>
#include <cstddef>  // for NULL
#include <iostream> // for operator<<, ostream, size_t, basic_ostream, basi...
#include <iterator> // for random_access_iterator_tag
#include <ranges.h>
#include <utility> // for pair
#include <vector>  // for vector

template <typename Symbol>
class Symbols {
  public:
    Symbols() = default;
    Symbols(const Symbol symbolCount, std::vector<std::size_t> symbolToId, std::vector<Symbol> idToSymbol)
        : symbolCount_{symbolCount}, symbolToId_{std::move(symbolToId)}, idToSymbol_{std::move(idToSymbol)} {}
    Symbol count() const { return symbolCount_; }
    std::size_t symbolToId(const std::size_t symbol) const { return symbolToId_[symbol]; }
    Symbol idToSymbol(const std::size_t id) const { return idToSymbol_[id]; }

    bool operator==(const Symbols& rhs) const {
        return symbolCount_ == rhs.symbolCount_ && symbolToId_ == rhs.symbolToId_ && idToSymbol_ == rhs.idToSymbol_;
    }

    template <class Archive>
    void serialize(Archive& ar, unsigned int) {
        ar& symbolCount_;
        ar& symbolToId_;
        ar& idToSymbol_;
    }

  private:
    Symbol symbolCount_{};
    std::vector<std::size_t> symbolToId_;
    std::vector<Symbol> idToSymbol_;
};

template <typename State>
class Transitions {
  public:
    Transitions() = default;

    Transitions(State stateCount, std::size_t symbolCount, const std::vector<State>& T)
        : stateCount_{stateCount}, symbolCount_{symbolCount}, T_{T} {}

    State GetStateCount() const { return stateCount_; }
    std::size_t GetSymbolCount() const { return symbolCount_; }
    State GetTransition(const State s, const std::size_t c) const { return T_[s * symbolCount_ + c]; }

    bool operator==(const Transitions& rhs) const {
        return stateCount_ == rhs.stateCount_ && symbolCount_ == rhs.symbolCount_ && T_ == rhs.T_;
    }

    template <class Archive>
    void serialize(Archive& ar, unsigned int) {
        ar& stateCount_;
        ar& symbolCount_;
        ar& T_;
    }

  private:
    State stateCount_{};
    std::size_t symbolCount_{};
    std::vector<State> T_;
};

template <typename TokenId>
class TokenIds {
  public:
    TokenIds() = default;

    TokenIds(const std::vector<TokenId>& finals) : finals_{finals} {}

    std::size_t GetStateCount() const { return finals_.size(); }
    TokenId GetTokenId(const std::size_t s) const { return finals_[s]; }

    bool operator==(const TokenIds& rhs) const { return finals_ == rhs.finals_; }

    template <class Archive>
    void serialize(Archive& ar, unsigned int) {
        ar& finals_;
    }

  private:
    std::vector<TokenId> finals_;
};

template <typename State, typename TokenId>
class DfaStates {
  public:
    DfaStates() = default;

    DfaStates(
        State stateCount, State start, State deadState, const TokenIds<TokenId>& finals, const Transitions<State>& T)
        : stateCount_{stateCount}, start_{start}, deadState_{deadState}, finals_{finals}, T_{T} {}

    State GetCount() const { return stateCount_; }
    State GetStart() const { return start_; }
    State GetDeadState() const { return deadState_; }
    TokenId GetTokenId(const State s) const { return finals_.GetTokenId(s); }
    State GetTransition(const State s, const std::size_t c) const { return T_.GetTransition(s, c); }
    const Transitions<State>& GetTransitions() const { return T_; }
    const TokenIds<TokenId>& GetTokenIds() const { return finals_; }

    bool operator==(const DfaStates& rhs) const {
        return stateCount_ == rhs.stateCount_ && start_ == rhs.start_ && deadState_ == rhs.deadState_ &&
               finals_ == rhs.finals_ && T_ == rhs.T_;
    }

    template <class Archive>
    void serialize(Archive& ar, unsigned int) {
        ar& stateCount_;
        ar& start_;
        ar& deadState_;
        ar& finals_;
        ar& T_;
    }

  private:
    State stateCount_{};
    State start_{};
    State deadState_{};
    TokenIds<TokenId> finals_;
    Transitions<State> T_;
};

template <typename Symbol, typename State, typename TokenId>
class DFA {
  public:
    DFA() = default;

    DFA(const DfaStates<State, TokenId>& states, const Symbols<Symbol>& symbols);

    State GetStateCount() const { return states_.GetCount(); }
    std::size_t GetSymbolCount() const { return symbols_.count(); }
    State GetStart() const { return states_.GetStart(); }
    State GetDeadState() const { return states_.GetDeadState(); }
    TokenId GetTokenId(const State s) const { return states_.GetTokenId(s); }
    State GetTransition(const State s, const std::size_t c) const { return states_.GetTransition(s, c); }
    const Transitions<State>& GetTransitions() const { return states_.GetTransitions(); }
    const TokenIds<TokenId>& GetTokenIds() const { return states_.GetTokenIds(); }
    const Symbols<Symbol>& GetSymbols() const { return symbols_; }
    std::size_t symbolToId(const std::size_t symbol) const { return symbols_.symbolToId(symbol); }
    Symbol idToSymbol(const std::size_t id) const { return symbols_.idToSymbol(id); }

    bool operator==(const DFA& rhs) const { return states_ == rhs.states_ && symbols_ == rhs.symbols_; }

    template <class Archive>
    void serialize(Archive& ar, unsigned int) {
        ar& states_;
        ar& symbols_;
    }

  private:
    DfaStates<State, TokenId> states_;
    Symbols<Symbol> symbols_;
};

template <typename Symbol, typename State, typename TokenId>
DFA<Symbol, State, TokenId>::DFA(const DfaStates<State, TokenId>& states, const Symbols<Symbol>& symbols)
    : states_{states}, symbols_{symbols} {}

template <typename Symbol, typename State, typename TokenId>
std::ostream& operator<<(std::ostream&, const DFA<Symbol, State, TokenId>&);

template <typename Symbol, typename State, typename TokenId>
State determineDeadState(const State stateCount,
                         const Symbol symbolCount,
                         const std::vector<State>& T,
                         const std::vector<TokenId>& finals) {
    //  std::cout << "determineDeadState" << std::endl;
    auto idempotent = [&](State q) {
        const auto ptr = T.data() + q * symbolCount;
        return std::all_of(ptr, ptr + symbolCount, [q](State q_) { return q_ == q; });
    };
    auto isDeadState = [&](State q) { return !finals[q] && idempotent(q); };
    auto stop = counting_iterator<State>(stateCount);
    auto it = std::find_if(counting_iterator<State>(0u), stop, isDeadState);
    if (it != stop) {
        std::cout << *it << " is the dead state" << std::endl;
        return *it;
    } else {
        std::cout << "there is no dead state" << std::endl;
        return stateCount;
    }
}

template <typename Symbol, typename State, typename TokenId>
std::ostream& operator<<(std::ostream& s, const DFA<Symbol, State, TokenId>& dfa) {
    s << "digraph G {\n";
    for (unsigned int p = 0; p < dfa.GetStateCount(); p++) {
        if (p == dfa.GetDeadState()) {
            continue;
        }

        if (dfa.GetTokenId(p)) {
            s << "  " << p << "[label = \"" << p << '|' << dfa.GetDeadState() << "\" shape = doublecircle];\n";
        }
        for (auto a = 0u; a < dfa.GetSymbolCount(); ++a) {
            if (dfa.GetTransition(p, a) != dfa.GetDeadState()) {
                s << "  " << p << " -> " << dfa.GetTransition(p, a) << " [label = \"";
                s << int(dfa.idToSymbol(a)) << "\"];\n";
            }
        }
    }
    s << "}\n";
    return s;
}
