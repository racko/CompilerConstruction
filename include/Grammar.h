#ifndef GRAMMAR_H_
#define GRAMMAR_H_

#include <boost/dynamic_bitset.hpp>
#include <vector>
#include <list>
#include <iostream>
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
  explicit GrammarElement(TerminalID<G> _x) : x(_x.x) {}
  explicit GrammarElement(NonterminalID<G> _x) : x(_x.x + G::numberOfTerminals + 1) {}

  explicit operator TerminalID<G>() const {
    return TerminalID<G>(x);
  }

  explicit operator NonterminalID<G>() const {
    return NonterminalID<G>(x - G::numberOfTerminals - 1);
  }
};

template<class G>
struct TerminalID {
  uint32_t x;

//  constexpr TerminalID(T _x) : x(uint32_t(_x)) {}

  explicit constexpr TerminalID(uint32_t _x) : x(_x) {}

  explicit operator GrammarElement<G>() const {
    return GrammarElement<G>(x);
  }
};

template<class G>
struct NonterminalID {
  uint32_t x;

//  constexpr NonterminalID(NT _x) : x(uint32_t(_x)) {}

  explicit constexpr NonterminalID(uint32_t _x) : x(_x) {}

  explicit operator GrammarElement<G>() const {
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

template <class G>
std::ostream& operator<<(std::ostream& s, const std::list<GrammarElement<G>>& alpha) {
    for (const auto& X : alpha) {
        s << X << " ";
    }
    return s;
}

template <class G>
std::ostream& operator<<(std::ostream& s, const std::unordered_set<TerminalID<G>>& alpha) {
    for (const auto& X : alpha) {
        s << X << " ";
    }
    return s;
}

namespace std {
  template<class G>
  struct hash<TerminalID<G>> {
    public:

    size_t operator()(const TerminalID<G> &a) const {
      return a.x;
    }
  };

  template<class G>
  struct hash<NonterminalID<G>> {
    public:

    size_t operator()(const NonterminalID<G> &A) const {
      return A.x;
    }
  };

  template<class G>
  struct hash<GrammarElement<G>> {
    public:

    size_t operator()(const GrammarElement<G> &X) const {
      return X.x;
    }
  };
}

template<class G>
void first(const std::vector<GrammarElement<G>>& alpha, const std::unordered_map<GrammarElement<G>, std::unordered_set<TerminalID<G>>>& fs, std::unordered_set<TerminalID<G>>& out) {
  //std::cout << "alpha: " << alpha << std::endl;
  for (auto Y : alpha) {
    //std::cout << "Y: " << Y << std::endl;
    const auto& firstY = fs.at(Y);
    //std::cout << "firstY: " << firstY << std::endl;
    out.insert(firstY.begin(), firstY.end());
    //std::cout << "out: " << out << std::endl;

    auto it = out.find(G::eps);
    if (it == out.end()) {
      //std::cout << "out does not contain eps. Done" << std::endl;
      return;
    }

    //std::cout << "out does contain eps. Removing it before continuing" << std::endl;
    out.erase(it);
  }
  //std::cout << "production can produce eps. Adding it to out again before returning." << std::endl;

  out.insert(G::eps);
}

template<class G>
bool updateFirsts(std::unordered_map<GrammarElement<G>, std::unordered_set<TerminalID<G>>>& fs) {
  bool change = false;
  std::unordered_set<TerminalID<G>> buffer;
  for (auto i = 0u; i < G::getNumberOfNonterminals(); ++i) {
    NonterminalID<G> A(i);
    auto& fsA = fs[A];
    //std::cout << A << ": " << fsA << std::endl;
    auto oldSize = fsA.size();
    const auto& productions = G::getProductions(A);
    for (const auto& alpha : productions) {
      buffer.clear();
      first(alpha, fs, buffer);
      fsA.insert(buffer.begin(), buffer.end());
    }
    //std::cout << A << ": " << fsA << std::endl;

    if (fsA.size() > oldSize)
      change = true;
  }
  //std::cout << std::endl;
  return change;
}

// TODO: topological order
// 1. create initial sets as below
// 2. add eps to first[A] if A -> eps is a production
// 3. create dependency graph using usual rules
// 4. find topological order
// 5. fill first sets in topological order

template<class G>
std::unordered_map<GrammarElement<G>, std::unordered_set<TerminalID<G>>> allFirsts() {
  std::unordered_map<GrammarElement<G>, std::unordered_set<TerminalID<G>>> fs(G::getNumberOfGrammarElements());
  fs[G::eps].insert(G::eps);

  for (auto i = 0u; i < G::getNumberOfTerminals(); ++i) {
    auto a = TerminalID<G>(i);
    fs[a].insert(a);
  }

  for (auto i = 0u; i < G::getNumberOfNonterminals(); ++i) {
    auto a = NonterminalID<G>(i);
    fs[a]; // this intializes an empty set
  }

  bool change;
  do change = updateFirsts<G>(fs);
  while(change);

  return fs;
}

template<class G>
std::unordered_set<TerminalID<G>> first(const std::vector<GrammarElement<G>>& alpha) {
  std::unordered_set<TerminalID<G>> out(G::getNumberOfTerminals() + 1);
  first(alpha, G::getFirsts(), out);
  return out;
}

#endif /* GRAMMAR_H_ */
