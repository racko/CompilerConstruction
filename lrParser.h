#ifndef LRPARSER_H_
#define LRPARSER_H_

#include <vector>
#include <set>
#include <list>
#include <sstream>
#include <boost/dynamic_bitset_fwd.hpp>
#include <cassert>

enum class kind { EPS, TERMINAL, NONTERMINAL };

class Grammar;

std::vector<boost::dynamic_bitset<>> allFirsts(const Grammar& G);

class Grammar {
public:
  using TerminalID = uint32_t;
  using NonterminalID = uint32_t;
  using GrammarElement = uint32_t;
  using String = std::vector<GrammarElement>;

  kind kindOf(GrammarElement X) const {
    if (X < numberOfTerminals)
      return kind::TERMINAL;
    else if (X > numberOfTerminals)
      return kind::NONTERMINAL;
    else return kind::EPS;
  }

  TerminalID terminalIdOf(GrammarElement a) const {
    if (a >= numberOfTerminals) {
      std::stringstream s;
      s << "terminalIdOf: Not a terminal (" << a << " >= " << numberOfTerminals << ")";
      throw std::runtime_error(s.str());
    }
    return a;
  }

  NonterminalID nonterminalIdOf(GrammarElement A) const {
    if (A <= numberOfTerminals) {
      std::stringstream s;
      s << "nonterminalIdOf: Not a nonterminal (" << A << " <= " << numberOfTerminals << ")";
      throw std::runtime_error(s.str());
    } else if (A >= getNumberOfGrammarElements()) {
      std::stringstream s;
      s << "nonterminalIdOf: Not a grammar element (" << A << " >= " << getNumberOfGrammarElements() << ")";
      throw std::runtime_error(s.str());
    }
    return A - numberOfTerminals - 1;
  }

  GrammarElement idOfTerminal(TerminalID a) const {
    if (a >= numberOfTerminals) {
      std::stringstream s;
      s << "terminalIdOf: Not a terminal (" << a << " >= " << numberOfTerminals << ")";
      throw std::runtime_error(s.str());
    }
    return a;
  }

  GrammarElement idOfNonterminal(NonterminalID A) const {
    if (A == getStartOfExtendedGrammar())
      return getNumberOfGrammarElements() - 1;
    if (A >= getNumberOfNonterminals()) {
      std::stringstream s;
      s << "nonterminalIdOf: Not a nonterminal (" << A << " >= " << numberOfNonterminals << ")";
      throw std::runtime_error(s.str());
    }
    return A + numberOfTerminals + 1;
  }

  size_t idOfEps() const {
    return numberOfTerminals;
  }

  size_t getNumberOfTerminals() const {
    return numberOfTerminals;
  }

  size_t getNumberOfNonterminals() const {
    return numberOfNonterminals + 1;
  }

  size_t getNumberOfGrammarElements() const {
    return numberOfTerminals + numberOfNonterminals + 2;
  }

  const std::vector<std::vector<String>>& getProductions() const {
    return productions;
  }

  const std::vector<String>& getProductions(NonterminalID A) const {
    if (A == getStartOfExtendedGrammar()) {
      static const std::vector<String> extProd{String{idOfNonterminal(getStart())}}; // not correct ... wrong constructor chosen ... I guess
      return extProd;
    }

    return productions[A];
  }

  const std::vector<boost::dynamic_bitset<>>& getFirsts() const {
    return firsts;
  }

  const boost::dynamic_bitset<>& getFirsts(GrammarElement X) const {
    return firsts[X];
  }

  NonterminalID getStart() const {
    return start;
  }

  NonterminalID getStartOfExtendedGrammar() const {
    return getNumberOfNonterminals() - 1;
  }

  TerminalID getEOF() const {
    return eof;
  }

  template<class T>
  Grammar(T&& _productions, NonterminalID _start, TerminalID _eof, size_t _numberOfTerminals, size_t _numberOfNonterminals)
    : productions(std::forward<T>(_productions)), start(_start), eof(_eof),
      numberOfTerminals(_numberOfTerminals), numberOfNonterminals(_numberOfNonterminals) {
    firsts = allFirsts(*this);
  }
  Grammar(const Grammar&) = default;
  Grammar(Grammar&&) = default;
  Grammar& operator=(const Grammar&) = default;
  Grammar& operator=(Grammar&&) = default;
  ~Grammar() = default;
private:
  std::vector<std::vector<String>> productions;
  std::vector<boost::dynamic_bitset<>> firsts;
  NonterminalID start;
  TerminalID eof;
  size_t numberOfTerminals;
  size_t numberOfNonterminals;
};

enum class type : uint32_t {
  SHIFT,
  REDUCE,
  ACCEPT,
  FAIL
};

inline uint32_t bits(type t) { return uint32_t(t) << 30; }

struct action {
  uint32_t x;
  static const uint32_t mask = 3u << 30;

  action(type t, uint32_t s) : x(bits(t) | s) { assert(s < (1u << 31) && t == type::SHIFT); }
  action(type t) : x(bits(t)) { assert(t == type::ACCEPT || t == type::FAIL); }
  action(type t, uint32_t A, uint32_t length) : x(bits(t) | A << 15 | length) {
    assert(A < (1u << 16) && length < (1u << 16) && t == type::REDUCE);
    auto a = getHead();
    auto l = getLength();
    assert(a == A && l == length);}
  type getType() const { return type(x >> 30); }
  uint32_t getState() const { auto t = getType(); assert(t == type::SHIFT); return x & ~mask; }
  uint32_t getHead() const {
    assert(getType() == type::REDUCE);
    auto shifted = x >> 15;
    auto mask = (1 << 15) - 1;
    return shifted & mask;
  }
  uint32_t getLength() const { assert(getType() == type::REDUCE); return x & ((1 << 15) - 1); }
};

struct LRParser {
  using state = uint32_t;
  Grammar G;
  std::vector<std::vector<state>> goto_table;
  std::vector<std::vector<action>> action_table;

  LRParser(const Grammar& G);
  void parse(const Grammar::String& s) const;
//  void parse(std::initializer_list<Grammar::GrammarElement> s) const { parse(Grammar::String(s)); }
};

boost::dynamic_bitset<> first(const Grammar&, Grammar::GrammarElement);

struct item {
  using String = std::list<Grammar::GrammarElement>;
  const Grammar& G; // only for debugging purposes
  Grammar::NonterminalID A;
  String l;
  String r;
  Grammar::TerminalID a;

  item(const Grammar& _G, Grammar::NonterminalID _A, String const& _l, String const& _r, Grammar::TerminalID _a) : G(_G), A(_A), l(_l), r(_r), a(_a) {}
};

bool operator<(const item& lhs, const item& rhs);

std::set<std::set<item>> items(const Grammar&);


enum class T : uint32_t {
  LEFTBR, RIGHTBR, ID, PERIOD, ASSIGN, OR, AND, EQ, NEQ, LT, LE, GE, GT, PLUS, MINUS, TIMES, DIV, NOT, LEFTPAR, RIGHTPAR, NUM, REAL, TRUE, FALSE, STRING, COMMA, EOI,
  EPS
};

enum class NT : uint32_t {
  LOC = uint32_t(T::EPS) + 1, ASSIGN, BOOL, JOIN, EQ, REL, EXPR, TERM, UNARY, FACTOR, FUNCALL, OPTARGS, ARGS,
  max
};

//enum class T : uint32_t {
//  C, D, EOI,
//  EPS
//};
//
//enum class NT : uint32_t {
//  S = uint32_t(T::EPS) + 1, C,
//  max
//};

std::ostream& operator<<(std::ostream& s, action a);

std::ostream& operator<<(std::ostream& s, T a);

std::ostream& operator<<(std::ostream& s, NT A);

void print(std::ostream& s, const Grammar& G, const Grammar::GrammarElement& X);

void print(std::ostream& s, const Grammar& G, const Grammar::String& alpha);

void print(std::ostream& s, const Grammar& G, const item::String& alpha);

void print(std::ostream& s, const Grammar& G, const item& i);

void print(std::ostream& s, const Grammar& G, const std::set<item>& I);

void print(std::ostream& s, const Grammar& G, const std::set<std::set<item>>& C);

void print(std::ostream& s, const Grammar& G, const std::vector<std::set<item>>& C);

#endif /* LRPARSER_H_ */
