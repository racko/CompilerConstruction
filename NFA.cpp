#include "NFA.h"
#include "Regex.h"
#include <iostream>
using std::cout;
using std::cin;
#include <iomanip>
using std::endl;
using std::dec;


void NFA::getClosure(BitSet& S) const {
  //cout << "getClosure(" << S << ")" << endl;
  vector<unsigned int> stack;
  for (auto i = S.begin(); i != S.end(); ++i)
    stack.push_back(*i);
  
  while (!stack.empty()) {
    //cout << "stack: " << show(stack) << endl;
    unsigned int q = stack.back();
    stack.pop_back();
    //cout << "considering state " << q << endl;
    const BitSet& T = table[q][eps];
    //cout << "delta(" << q << ",eps) = " << T << endl;
    newStates = BitSetComplement(S);
    newStates &= T;
    //BitSet newStates = T & ~S;
    S |= newStates;
    //cout << "newStates: " << newStates << endl;
    for (auto i = newStates.begin(); i != newStates.end(); ++i) {
      //cout << "adding state " << *i << endl;
      stack.push_back(*i);
    }
  }
}

NFA::NFA(const nfaBuilder& nfa) : eps(nfa.eps), symbolCount(nfa.symbolToId.size()), stateCount(nfa.ns.size()), start(nfa.start), final(stateCount), table(stateCount, vector<BitSet>(symbolCount, BitSet(stateCount, false))), symbols(nfa.idToSymbol), newStates(nfa.ns.size()) {
  cout << "constructing NFA from nfaBuilder" << endl;
  cout << "stateCount: " << stateCount << endl;
  cout << "start: " << nfa.start << endl;
  cout << "symbolCount: " << symbolCount << endl;
  cout << "symbols: " << show(nfa.idToSymbol) << endl;
  
  for (unsigned int p = 0; p < stateCount; p++) {
    if (!nfa.ns[p].deleted) {
      for (auto& t : nfa.ns[p].ns) {
        //cout << "d(" << p << "," << nfa.idToSymbol[t.first] << ") = " << show(t.second) << endl;
        for (auto& q : t.second) {
          table[p][t.first][q] = true;
        }
      //cout << "T[" << p << "][" << t.first << "] = " << table[p][t.first] << endl;
      }
      //cout << "kind(" << p << ") = " << nfa.ns[p].kind << endl;
      final[p] = nfa.ns[p].kind;
    } else {
      //cout << "not considering state " << p << ": deleted" << endl;
    }
  }
}



std::ostream& operator<<(std::ostream& s, const NFA& nfa) {
  s << "digraph G {\n";
  for (unsigned int p = 0; p < nfa.stateCount; p++) {
    if (nfa.final[p])
      s << "  " << p << "[shape = doublecircle];\n";
    for (auto a = 0u; a < nfa.symbolCount; ++a) {
      if (nfa.table[p][a].count() > 0) {
        s << "  " << p << " -> { ";
        auto q = nfa.table[p][a].begin();
        auto q_end = nfa.table[p][a].end();
        if (q != q_end) {
          s << *q;
        }
        ++q;
        for (; q != q_end; ++q) {
          s << ", " << *q;
        }
        s << " } [label = \"" << showCharEscaped(nfa.symbols[a]) << "\"];\n";
      }
    }
  }
  s << "}\n";
  return s;
}
