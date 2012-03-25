#ifndef _DFA_H_
#define _DFA_H_

#include <vector>
using std::vector;

struct NFA;
struct DFA {
  unsigned int stateCount, symbolCount, start;
  vector<bool> final;
  vector<vector<unsigned int>> T;
  DFA(const NFA& nfa);
  void minimize();
};

#endif
