#ifndef _DFA_H_
#define _DFA_H_

#include <vector>
using std::vector;

struct NFA;
struct DFA {
  unsigned int stateCount, symbolCount;
  vector<vector<unsigned int>> T;
  DFA(const NFA& nfa);
};

#endif
