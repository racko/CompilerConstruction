#include "Regex.h"
#include "NFA.h"
#include "DFA.h"
#include <iostream>
#include <iomanip>
using std::cout;
using std::endl;

enum struct Symbol : unsigned int {
  a = 0,
  b = 1,
  last = b,
  eps = last + 1
};

int main(int argc, char** args) {
  const unsigned int stateCount = 11, symbolCount = (unsigned int)(Symbol::last) + 1, a = (unsigned int)(Symbol::a), b = (unsigned int)(Symbol::b), eps = (unsigned int)(Symbol::eps);
  vector<bool> final(stateCount, false);
  final[10] = true;
  NFA nfa(symbolCount, stateCount, eps, 0, std::move(final));
  nfa.table.resize(stateCount, vector<vector<bool>>(symbolCount + 1, vector<bool>(stateCount)));
  nfa.table[0][eps][1] = true;
  nfa.table[0][eps][7] = true;
  nfa.table[1][eps][2] = true;
  nfa.table[1][eps][4] = true;
  nfa.table[2][a][3] = true;
  nfa.table[4][b][5] = true;
  nfa.table[3][eps][6] = true;
  nfa.table[5][eps][6] = true;
  nfa.table[6][eps][1] = true;
  nfa.table[6][eps][7] = true;
  nfa.table[7][a][8] = true;
  nfa.table[8][b][9] = true;
  nfa.table[9][b][10] = true;
  DFA dfa(nfa);
  cout << "start: " << dfa.start << endl;
  cout << "final: " << show(dfa.final) << endl;
  for (unsigned int q = 0; q < dfa.stateCount; q++) {
    for (unsigned int a = 0; a < dfa.symbolCount; a++)
      cout << "(" << q << "," << a << ") -> " << dfa.T[q][a] << endl;
  }
  dfa.minimize();
  cout << "start: " << dfa.start << endl;
  cout << "final: " << show(dfa.final) << endl;
  for (unsigned int q = 0; q < dfa.stateCount; q++) {
    for (unsigned int a = 0; a < dfa.symbolCount; a++)
      cout << "(" << q << "," << a << ") -> " << dfa.T[q][a] << endl;
  }
  return 0;
}
