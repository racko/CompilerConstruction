#include "lrParser.h"

#include <boost/dynamic_bitset.hpp>
#include <iostream>

struct LRParser {

};

bool updateFirsts(const Grammar& G, std::vector<boost::dynamic_bitset<>>& fs) {
  bool change = false;
  for (Grammar::NonterminalID A = 0u; A < G.getNumberOfNonterminals(); ++A) {
    auto oldFirsts = fs[G.idOfNonterminal(A)];
    auto productions = G.getProductions(A);
    for (const auto& alpha : productions) {
      auto eps = true;
      for (auto Y : alpha) {
        fs[G.idOfNonterminal(A)] |= (fs[Y] & ~fs[G.idOfEps()]);

        if (!fs[Y][G.idOfEps()]) {
          eps = false;
          break;
        }
      }
      if (eps)
        fs[G.idOfNonterminal(A)][G.idOfEps()] = 1;
    }
    if (oldFirsts != fs[G.idOfNonterminal(A)])
      change = true;
  }
  return change;
}

std::vector<boost::dynamic_bitset<>> allFirsts(const Grammar& G) {
  std::vector<boost::dynamic_bitset<>> fs(G.getNumberOfGrammarElements(), boost::dynamic_bitset<>(G.getNumberOfTerminals() + 1));
  fs[G.idOfEps()][G.idOfEps()] = 1;

  for (Grammar::TerminalID a = 0u; a < G.getNumberOfTerminals(); ++a)
    fs[G.idOfTerminal(a)][G.idOfTerminal(a)] = 1;

  bool change;
  do {
    change = updateFirsts(G, fs);
  } while(change);

  return fs;
}

boost::dynamic_bitset<> first(const Grammar& G, const Grammar::String& alpha) {
  boost::dynamic_bitset<> out(G.getNumberOfTerminals() + 1);
  for (auto Y : alpha) {
    auto firstY = G.getFirsts(Y);
    out |= firstY;

    if (!firstY[G.idOfEps()])
      return out;

    out[G.idOfEps()] = 0;
  }

  out[G.idOfEps()] = 1;

  return out;
}

bool operator<(const item& lhs, const item& rhs) {
  print(std::cout, lhs.G, lhs);
  std::cout << " < ";
  print(std::cout, rhs.G, rhs);
  std::cout.flush();
  bool result;
  if (lhs.A < rhs.A)
    result = true;
  else if (lhs.A > rhs.A)
    result = false;
  else if (lhs.l < rhs.l)
    result = true;
  else if (lhs.l > rhs.l)
    result = false;
  else if (lhs.r < rhs.r)
    result = true;
  else if (lhs.r > rhs.r)
    result = false;
  else if (lhs.a < rhs.a)
    result = true;
  else
    result = false;
  std::cout << " = " << std::boolalpha << result << std::endl;
  return result;
}

using SetOfItems = std::set<item>;


SetOfItems closure(const Grammar& G, SetOfItems I) {
  bool changed;
  do {
    changed = false;
    for (const auto& i : I) {
      if (i.r.empty() || G.kindOf(i.r.front()) != kind::NONTERMINAL)
        continue;
      std::cout << "Considering item";
      print(std::cout, G, i);
      const auto& B = G.nonterminalIdOf(i.r.front());
      std::cout << "B = " << NT(G.idOfNonterminal(B)) << std::endl;
      for (const auto& gamma : G.getProductions(B)) {
        std::cout << "gamma = ";
        print(std::cout, G, gamma);
        std::cout << std::endl;
        auto betaStart = i.r.begin();
        if (betaStart != i.r.end())
          std::advance(betaStart, 1);
        Grammar::String rest(betaStart, i.r.end());
        rest.push_back(G.idOfTerminal(i.a));
        std::cout << "rest = ";
        print(std::cout, G, rest);
        std::cout << std::endl;
        auto firstRest = first(G, rest);
        for (auto b = 0u; b < firstRest.size(); ++b) {
          if (!firstRest[b])
            continue;
          std::cout << "b = " << T(b) << std::endl;
          auto res = I.emplace(G, B, item::String{}, item::String{gamma.front() != G.idOfEps() ? gamma.begin() : gamma.end(), gamma.end()}, b);

          if (res.second) {
            std::cout << "Adding ";
            print(std::cout, G, *res.first);
          }

          changed |= res.second;
        }
      }
    }
  } while(changed);
  std::cout << "After closure: ";
  print(std::cout, G, I);
  return I;
}

SetOfItems GOTO(const Grammar& G, const SetOfItems& I, Grammar::GrammarElement X) {
  SetOfItems J;
  for (const auto& i : I) {
    if (i.r.empty() || i.r.front() != X)
      continue;
    auto l = i.l;
    l.push_back(X);
    auto r = i.r;
    r.pop_front();
    J.emplace(G, i.A, move(l), move(r), i.a);
  }
  std::cout << "Before closure:";
  print(std::cout, G, J);
  return closure(G, move(J));
}

std::set<SetOfItems> items(const Grammar& G) {
  auto init = SetOfItems{item(G, G.getStartOfExtendedGrammar(), item::String(), item::String{G.idOfNonterminal(G.getStart())}, G.getEOF())};
  std::cout << "Starting with";
  print(std::cout, G, init);
  auto C = std::set<SetOfItems>{closure(G, std::move(init))};
  bool changed;
  do {
    changed = false;
    for (const auto& I : C) {
      for (auto X = 0u; X < G.getNumberOfGrammarElements(); ++X) {
        std::cout << "GOTO(...,";
        print(std::cout, G, X);
        std::cout << ") of";
        print(std::cout, G, I);
        auto J = GOTO(G, I, X);
        if (!J.empty())
          C.emplace(move(J));
      }
    }
  } while(changed);
  return C;
}

const char* NT_strings[] = {
    "LOC", "ASSIGN", "BOOL", "JOIN", "EQ", "REL", "EXPR", "TERM", "UNARY", "FACTOR", "FUNCALL", "OPTARGS", "ARGS", "S'"
};

const char* T_strings[] = {
    "LEFTBR", "RIGHTBR", "ID", "PERIOD", "ASSIGN", "OR", "AND", "EQ", "NEQ", "LT", "LE", "GE", "GT", "PLUS", "MINUS", "TIMES", "DIV", "NOT", "LEFTPAR", "RIGHTPAR", "NUM", "REAL", "TRUE", "FALSE", "STRING", "COMMA", "EOF",
    "EPS"
};

//const char* NT_strings[] = {
//    "S", "C", "S'"
//};
//
//const char* T_strings[] = {
//    "c", "d", "EOF",
//    "EPS"
//};

std::ostream& operator<<(std::ostream& s, T a) {
  return s << T_strings[uint32_t(a)];
}

std::ostream& operator<<(std::ostream& s, NT A) {
  return s << NT_strings[uint32_t(A) - uint32_t(T::EPS) - 1];
}

void print(std::ostream& s, const Grammar& G, const Grammar::GrammarElement& X) {
  switch(G.kindOf(X)) {
  case kind::TERMINAL:
    s << T(G.terminalIdOf(X));
    break;
  case kind::EPS:
    s << "EPS";
    break;
  case kind::NONTERMINAL:
    s << NT(X) ;
    break;
  }
}

void print(std::ostream& s, const Grammar& G, const Grammar::String& alpha) {
  for (const auto& X : alpha) {
    print(s, G, X);
    s << " ";
  }
}

void print(std::ostream& s, const Grammar& G, const item::String& alpha) {
  for (const auto& X : alpha) {
    print(s, G, X);
    s << " ";
  }
}

void print(std::ostream& s, const Grammar& G, const item& i) {
  s << "[" << NT(G.idOfNonterminal(i.A)) << " -> ";
  print(s, G, i.l);
  s << " \t";
  print(s, G, i.r);
  s << "\t" << T(i.a) << "]";
}

void print(std::ostream& s, const Grammar& G, const std::set<item>& I) {
  for (const auto& i : I) {
    print(s, G, i);
    std::cout << std::endl;
  }
}

void print(std::ostream& s, const Grammar& G, const std::set<std::set<item>>& C) {
  auto i = 0u;
  for (const auto& I : C) {
    std::cout << "Set" << i++ << ":" << std::endl;
    print(s, G, I);
    std::cout << std::endl;

  }
}
