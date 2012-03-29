#include "NFA.h"
#include "Regex.h"
#include <iostream>
using std::cout;
#include <iomanip>
using std::endl;
using std::dec;


void NFA::getClosure(vector<bool>& S) const {
  cout << "getClosure(" << show(S) << ")" << endl;
  vector<unsigned int> stack;
  for (unsigned int i = 0; i < S.size(); i++)
    if (S[i])
      stack.push_back(i);
  
  while (!stack.empty()) {
    cout << "stack: " << show(stack) << endl;
    unsigned int q = stack.back();
    stack.pop_back();
    cout << "considering state " << q << endl;
    const vector<bool>& T = table[q][eps];
    cout << "delta(" << q << ",eps) = " << show(T) << endl;
    for (unsigned int i = 0; i < T.size(); i++)
      if (T[i] && !S[i]) {
        cout << "adding state " << i << endl;
        stack.push_back(i);
        S[i] = true;
      }
  }
}

NFA::NFA(nfaBuilder&& nfa) : eps(0), symbolCount(nfa.symbolToId.size()), stateCount(nfa.ns.size()), start(nfa.start), final(stateCount), table(stateCount, vector<vector<bool>>(symbolCount, vector<bool>(stateCount, false))), symbols(nfa.idToSymbol) {
  cout << "constructing NFA from nfaBuilder" << endl;
  cout << "stateCount: " << stateCount << endl;
  cout << "start: " << nfa.start << endl;
  cout << "end: " << nfa.end << endl;
  cout << "symbolCount: " << symbolCount << endl;
  cout << "symbols: " << show(nfa.idToSymbol) << endl;
  
  final[nfa.end] = true;
  for (unsigned int p = 0; p < stateCount; p++) {
    if (!nfa.ns[p].deleted) {
      for (auto& t : nfa.ns[p].ns) {
        cout << "d(" << p << "," << nfa.idToSymbol[t.first] << ") = " << show(t.second) << endl;
        for (auto& q : t.second) {
          table[p][t.first][q] = true;
        }
      }
    } else {
      cout << "not considering state " << p << ": deleted" << endl;
    }
  }
}
