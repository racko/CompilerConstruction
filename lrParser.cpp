#include "lrParser.h"

#include <boost/dynamic_bitset.hpp>
#include <iostream>
#include <map>

#undef NDEBUG
#include <cassert>

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
//  print(std::cout, lhs.G, lhs);
//  std::cout << " < ";
//  print(std::cout, rhs.G, rhs);
//  std::cout.flush();
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
//  std::cout << " = " << std::boolalpha << result << std::endl;
  return result;
}

using SetOfItems = std::set<item>;


SetOfItems closure(const Grammar& G, SetOfItems&& I) {
  bool changed;
  do {
    changed = false;
    for (const auto& i : I) {
      if (i.r.empty() || G.kindOf(i.r.front()) != kind::NONTERMINAL)
        continue;
//      std::cout << "Considering item";
//      print(std::cout, G, i);
      const auto& B = G.nonterminalIdOf(i.r.front());
//      std::cout << "B = " << NT(G.idOfNonterminal(B)) << std::endl;
      for (const auto& gamma : G.getProductions(B)) {
//        std::cout << "gamma = ";
//        print(std::cout, G, gamma);
//        std::cout << std::endl;
        auto betaStart = i.r.begin();
        if (betaStart != i.r.end())
          std::advance(betaStart, 1);
        Grammar::String rest(betaStart, i.r.end());
        rest.push_back(G.idOfTerminal(i.a));
//        std::cout << "rest = ";
//        print(std::cout, G, rest);
//        std::cout << std::endl;
        auto firstRest = first(G, rest);
        for (auto b = 0u; b < firstRest.size(); ++b) {
          if (!firstRest[b])
            continue;
//          std::cout << "b = " << T(b) << std::endl;
          auto res = I.emplace(G, B, item::String{}, item::String{gamma.front() != G.idOfEps() ? gamma.begin() : gamma.end(), gamma.end()}, b);

//          if (res.second) {
//            std::cout << "Adding ";
//            print(std::cout, G, *res.first);
//          }

          changed |= res.second;
        }
      }
    }
  } while(changed);
//  std::cout << "After closure: ";
//  print(std::cout, G, I);
//  std::cout << std::endl;
  return I;
}

SetOfItems GOTO(const Grammar& G, const SetOfItems& I, Grammar::GrammarElement X) {
//  std::cout << "In GOTO" << std::endl;
//  print(std::cout, G, I);
//  print(std::cout, G, X);
//  std::cout << std::endl;
  SetOfItems J;
  for (const auto& i : I) {
//    std::cout << "considering item ";
//    print(std::cout, G, i);
//    std::cout << std::endl;
    if (i.r.empty() || i.r.front() != X) {
//      std::cout << "not relevant" << std::endl;
      continue;
    }
    auto l = i.l;
    l.push_back(X);
    auto r = i.r;
    r.pop_front();
//    item j(G, i.A, l, r, i.a);
//    std::cout << "adding item ";
//    print(std::cout, G, j);
//    std::cout << std::endl;
    J.emplace(G, i.A, move(l), move(r), i.a);

  }
//  std::cout << "Before closure:" << std::endl;
//  print(std::cout, G, J);
  return closure(G, move(J));
}

LRParser::LRParser(const Grammar& G) : G(G) {
  auto init = SetOfItems{item(G, G.getStartOfExtendedGrammar(), item::String(), item::String{G.idOfNonterminal(G.getStart())}, G.getEOF())};
  std::cout << "Starting with ";
  print(std::cout, G, init);
  std::cout << std::endl;
  auto C = std::vector<SetOfItems>{closure(G, std::move(init))};
  std::map<SetOfItems, state> ids{{C[0], 0}};
  std::map<state,std::map<Grammar::NonterminalID,state>> _goto_table;
  std::map<state,std::map<Grammar::TerminalID,action>> _action_table;
  std::vector<uint32_t> stack;
  stack.push_back(0);

  while(!stack.empty()) {
    auto i = stack.back();
    stack.pop_back();
    auto I = C[i]; // copying here to avoid invalid reference when C changes
//    std::cout << "Considering item set " << i << std::endl;
//    print(std::cout, G, I);
    for (auto X = 0u; X < G.getNumberOfGrammarElements(); ++X) {

      if (G.kindOf(X) == kind::TERMINAL) {
        for (const auto& item : I) {
          if (item.r.empty() && item.a == G.terminalIdOf(X)) {
            action a(type::FAIL);
            if (item.A == G.getStartOfExtendedGrammar())
              a = action(type::ACCEPT);
            else
              a = action(type::REDUCE, item.A, item.l.size() + item.r.size());
            if (_action_table[i].find(G.terminalIdOf(X)) != _action_table[i].end()) {
              std::cout << "conflict: ACTION(" << i << ", ";
              print(std::cout, G, X);
              std::cout << "): " << _action_table[i].find(G.terminalIdOf(X))->second << " or " << a << std::endl;
              throw std::runtime_error("conflict");
            }
            _action_table[i].insert({G.terminalIdOf(X), a});
          }
        }
      }

//      std::cout << "GOTO ";
//      print(std::cout, G, X);
//      std::cout << ":" << std::endl;

      auto J = GOTO(G, I, X);
      if (!J.empty()) {
//        print(std::cout, G, J);
        auto it = ids.find(J);
        uint32_t j;
        if (it == ids.end()) {

          auto id = C.size();
          auto result = ids.insert({J, id});
          assert(result.second);
          C.emplace_back(J);
          j = result.first->second;
          stack.push_back(j);
          std::cout << "new item set: " << j << std::endl;
        } else {
          j = it->second;
//          std::cout << "saw it before: " << j << std::endl;
        }

//        auto& actions_i = _action_table[i];
//        auto& goto_i = _goto_table[i];

        if (G.kindOf(X) == kind::NONTERMINAL)
          _goto_table[i][G.nonterminalIdOf(X)] = j;
        else if (G.kindOf(X) == kind::TERMINAL) {
          assert(_action_table[i].find(G.terminalIdOf(X)) == _action_table[i].end());
          if (_action_table[i].find(G.terminalIdOf(X)) != _action_table[i].end()) {
            std::cout << "conflict: ACTION(" << i << ", ";
            print(std::cout, G, X);
            std::cout << "): " << _action_table[i].find(G.terminalIdOf(X))->second << " or " << action(type::SHIFT, i) << std::endl;
            throw std::runtime_error("conflict");
          }
          _action_table[i].insert({G.terminalIdOf(X), action(type::SHIFT, j)});
        } else
          throw std::logic_error("eps occured");
      } else {
//        std::cout << "empty" << std::endl;
      }
    }
  }

  print(std::cout, G, C);

  goto_table.resize(C.size(), std::vector<state>(G.getNumberOfNonterminals(), C.size()));

  for (const auto& goto_i : _goto_table) {
    auto i = goto_i.first;
    for (auto it : goto_i.second) {
      auto A = it.first;
      auto j = it.second;
      goto_table[i][A] = j;
    }
  }

  action_table.resize(C.size(), std::vector<action>(G.getNumberOfTerminals(), action(type::FAIL)));
  for (const auto& action_i : _action_table) {
    auto i = action_i.first;
    for (auto it : action_i.second) {
      auto a = it.first;
      auto j = it.second;
      action_table[i][a] = j;
    }
  }
}

void LRParser::parse(const Grammar::String& w) const {
  std::vector<state> stack{0};
  auto a = w.begin();
  while(!stack.empty() && a != w.end()) {
    auto s = stack.back();
//    stack.pop_back();
    const action& _action = action_table[s][*a];
    if (_action.getType() == type::SHIFT) {
      stack.push_back(_action.getState());
      std::advance(a, 1);
      std::cout << "Shift" << std::endl;
    } else if (_action.getType() == type::REDUCE) {
      // action: reduce A -> beta
      auto length = _action.getLength();
      auto head = _action.getHead();
      assert(stack.size() > length);
      stack.erase(stack.end() - length, stack.end());
      auto t = goto_table[stack.back()][head];
      assert(t < goto_table.size());
      stack.push_back(t);
      std::cout << "Reduce to ";
      print(std::cout, G, G.idOfNonterminal(_action.getHead()));
      std::cout << " (length " << _action.getLength() << ")" << std::endl;
    } else if (_action.getType() == type::ACCEPT) {
      std::cout << "Accept" << std::endl;
      break;
    } else {
      std::cout << "Fail." << std::endl;
      break;
    }
  }

  if (stack.empty()) {
    std::cout << "empty stack" << std::endl;
  }

  if (a == w.end()) {
    std::cout << "reached end of input" << std::endl;
  }
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

std::ostream& operator<<(std::ostream& s, action a) {
  switch(a.getType()) {
  case type::ACCEPT:
    return s << "acc";
  case type::FAIL:
    return s << "fail";
  case type::REDUCE:
    return s << "r" << a.getHead() << "," << a.getLength();
  case type::SHIFT:
    return s << "s" << a.getState();
  default:
    throw std::logic_error("unhandled action type");
  }
}

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

void print(std::ostream& s, const Grammar& G, const std::vector<std::set<item>>& C) {
  auto i = 0u;
  for (const auto& I : C) {
    std::cout << "Set " << i++ << ":" << std::endl;
    print(s, G, I);
    std::cout << std::endl;

  }
}
