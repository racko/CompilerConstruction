#ifndef _DFA_H_
#define _DFA_H_

#include <vector>
using std::vector;
#include <ostream>

struct NFA;
struct DFA {
  unsigned int stateCount, symbolCount, start, deadState;
  vector<unsigned int> final;
  vector<vector<unsigned int>> T;
  vector<unsigned int> symbolToId;
  vector<char> idToSymbol;
  DFA(const NFA& nfa);
  void minimize();
  void determineDeadState();
};

std::ostream& operator<<(std::ostream&, const DFA&);

#endif
