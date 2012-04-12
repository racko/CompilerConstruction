#ifndef _DFA_H_
#define _DFA_H_

#include <vector>
using std::vector;

struct NFA;
struct DFA {
  unsigned int stateCount, symbolCount, start;
  vector<unsigned int> final;
  vector<vector<unsigned int>> T;
  vector<unsigned int> symbolToId;
  DFA(NFA& nfa);
  void minimize();
};

#endif
