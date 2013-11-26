#ifndef GRAMMAR_H_
#define GRAMMAR_H_

#include <boost/dynamic_bitset.hpp>
#include <vector>
#include <ostream>
#include <unordered_set>
#include <unordered_map>

enum class kind { TERMINAL, NONTERMINAL, EPS, EOI };

template<class G>
struct GrammarElement;

template<class G>
struct TerminalID;

template<class G>
struct NonterminalID;

template<class G>
struct GrammarElement {
  uint32_t x;
  explicit constexpr GrammarElement(uint32_t _x) : x(_x) {}
  GrammarElement(TerminalID<G> _x) : x(_x.x) {}
  GrammarElement(NonterminalID<G> _x) : x(_x.x + G::numberOfTerminals + 1) {}

  operator TerminalID<G>() const {
    return TerminalID<G>(x);
  }

  operator NonterminalID<G>() const {
    return NonterminalID<G>(x - G::numberOfTerminals - 1);
  }
};

template<class G>
struct TerminalID {
  uint32_t x;

//  constexpr TerminalID(T _x) : x(uint32_t(_x)) {}

  explicit constexpr TerminalID(uint32_t _x) : x(_x) {}

  operator GrammarElement<G>() const {
    return GrammarElement<G>(x);
  }
};

template<class G>
struct NonterminalID {
  uint32_t x;

//  constexpr NonterminalID(NT _x) : x(uint32_t(_x)) {}

  explicit constexpr NonterminalID(uint32_t _x) : x(_x) {}

  operator GrammarElement<G>() const {
    return GrammarElement<G>(x + G::numberOfTerminals + 1);
  }
};

template<class G>
bool operator==(const TerminalID<G>& lhs, const TerminalID<G>& rhs) {
  return lhs.x == rhs.x;
}

template<class G>
bool operator!=(const TerminalID<G>& lhs, const TerminalID<G>& rhs) {
  return !operator==(lhs, rhs);
}

template<class G>
bool operator<(const TerminalID<G>& lhs, const TerminalID<G>& rhs) {
  return lhs.x < rhs.x;
}

template<class G>
bool operator==(const GrammarElement<G>& lhs, const GrammarElement<G>& rhs) {
  return lhs.x == rhs.x;
}

template<class G>
bool operator!=(const GrammarElement<G>& lhs, const GrammarElement<G>& rhs) {
  return !operator==(lhs, rhs);
}

template<class G>
bool operator<(const GrammarElement<G>& lhs, const GrammarElement<G>& rhs) {
  return lhs.x < rhs.x;
}

template<class G>
bool operator==(const NonterminalID<G>& lhs, const NonterminalID<G>& rhs) {
  return lhs.x == rhs.x;
}

template<class G>
bool operator!=(const NonterminalID<G>& lhs, const NonterminalID<G>& rhs) {
  return !operator==(lhs, rhs);
}

template<class G>
bool operator<(const NonterminalID<G>& lhs, const NonterminalID<G>& rhs) {
  return lhs.x < rhs.x;
}

namespace std {
  template<class G>
  class hash<TerminalID<G>> {
    public:

    size_t operator()(const TerminalID<G> &a) const {
      return a.x;
    }
  };

  template<class G>
  class hash<NonterminalID<G>> {
    public:

    size_t operator()(const NonterminalID<G> &A) const {
      return A.x;
    }
  };

  template<class G>
  class hash<GrammarElement<G>> {
    public:

    size_t operator()(const GrammarElement<G> &X) const {
      return X.x;
    }
  };
}

template<class G>
bool updateFirsts(std::unordered_map<GrammarElement<G>, std::unordered_set<TerminalID<G>>>& fs) {
  bool change = false;
  for (auto i = 0u; i < G::getNumberOfNonterminals(); ++i) {
    NonterminalID<G> A(i);
    auto& fsA = fs[A];
    auto oldFirsts = fs[A]; // we copy here because we modify fs[A] in the loop
    const auto& productions = G::getProductions(A);
    for (const auto& alpha : productions) {
      auto eps = true;
      for (auto Y : alpha) {
        const auto& fsY = fs[Y];

        fsA.insert(fsY.begin(), fsY.end());
        auto it = fsA.find(G::eps);
        if (it == fsA.end()) {
          eps = false;
          break;
        }
        fsA.erase(it);
      }

      if (eps)
        fsA.insert(G::eps);
    }

    if (fsA.size() > oldFirsts.size())
      change = true;
  }
  return change;
}

template<class G>
std::unordered_map<GrammarElement<G>, std::unordered_set<TerminalID<G>>> allFirsts() {
  std::unordered_map<GrammarElement<G>, std::unordered_set<TerminalID<G>>> fs(G::getNumberOfGrammarElements());
  fs[G::eps].insert(G::eps);

  for (auto i = 0u; i < G::getNumberOfTerminals(); ++i) {
    auto a = TerminalID<G>(i);
    fs[a].insert(a);
  }

  bool change;
  do {
    change = updateFirsts<G>(fs);
  } while(change);

  return fs;
}

template<class G>
std::unordered_set<TerminalID<G>> first(const std::vector<GrammarElement<G>>& alpha) {
  std::unordered_set<TerminalID<G>> out(G::getNumberOfTerminals() + 1);
  for (auto Y : alpha) {
    const auto& firstY = G::getFirsts(Y);
    out.insert(firstY.begin(), firstY.end());
//    out |= firstY;

    auto it = out.find(G::eps);
    if (it == out.end())
      return out;

    out.erase(it);
  }

  out.insert(G::eps);

  return out;
}

template<class G>
std::ostream& operator<<(std::ostream& s, GrammarElement<G> const& X) {
  switch(G::kindOf(X)) {
  case kind::TERMINAL:
    return s << TerminalID<G>(X);
  case kind::EPS:
    return s << "EPS";
  case kind::EOI:
    return s << "EOF";
  case kind::NONTERMINAL:
    return s << NonterminalID<G>(X);
  default:
    throw std::logic_error("unhandled case in operator<<(std::ostream& s, GrammarElement<G> const& X)");
  }
}

template<class G>
std::ostream& operator<<(std::ostream& s, const std::vector<GrammarElement<G>>& alpha) {
  for (const auto& X : alpha) {
    s << X << " ";
  }
  return s;
}

#endif /* GRAMMAR_H_ */
