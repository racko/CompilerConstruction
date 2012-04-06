#include "NFA.h"
#include "Regex.h"
#include <iostream>
using std::cout;
#include <iomanip>
using std::endl;
using std::dec;


void NFA::getClosure(BitSet& S) const {
  cout << "getClosure(" << S << ")" << endl;
  vector<unsigned int> stack;
  for (unsigned int i = 0; i < S.size(); i++)
    if (S[i])
      stack.push_back(i);
  
  while (!stack.empty()) {
    cout << "stack: " << show(stack) << endl;
    unsigned int q = stack.back();
    stack.pop_back();
    cout << "considering state " << q << endl;
    const BitSet& T = table[q][eps];
    cout << "delta(" << q << ",eps) = " << T << endl;
    BitSet newStates = T & ~S;
    S |= newStates;
    cout << "newStates: " << newStates << endl;
    for (auto i = newStates.begin(); i != newStates.end(); ++i) {
      cout << "adding state " << *i << endl;
      stack.push_back(*i);
    }
  }
}

NFA::NFA(nfaBuilder&& nfa) : eps(0), symbolCount(nfa.symbolToId.size()), stateCount(nfa.ns.size()), start(nfa.start), final(stateCount), table(stateCount, vector<BitSet>(symbolCount, BitSet(stateCount, false))), symbols(nfa.idToSymbol) {
  cout << "constructing NFA from nfaBuilder" << endl;
  cout << "stateCount: " << stateCount << endl;
  cout << "start: " << nfa.start << endl;
  cout << "symbolCount: " << symbolCount << endl;
  cout << "symbols: " << show(nfa.idToSymbol) << endl;
  
  for (unsigned int p = 0; p < stateCount; p++) {
    if (!nfa.ns[p].deleted) {
      for (auto& t : nfa.ns[p].ns) {
        cout << "d(" << p << "," << nfa.idToSymbol[t.first] << ") = " << show(t.second) << endl;
        for (auto& q : t.second) {
          table[p][t.first][q] = true;
        }
      }
      final[p] = nfa.ns[p].kind;
    } else {
      cout << "not considering state " << p << ": deleted" << endl;
    }
  }
}
