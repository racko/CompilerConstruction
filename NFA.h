#ifndef _NFA_H_
#define _NFA_H_

#include "nfaBuilder.h"
#include "BitSet.h"
#include <unordered_map>
using std::unordered_map;
#include <vector>
using std::vector;
#include <tuple>
using std::pair;
#include <ostream>

struct NFA {
  unsigned int eps;
  unsigned int symbolCount, stateCount, start;
  vector<unsigned int> final;
  vector<vector<BitSet>> table;
  vector<char> symbols;
  mutable BitSet newStates;
  NFA(unsigned int _symbolCount, unsigned int _stateCount, int _eps, unsigned int _start, vector<unsigned int>&& _final) : eps(_eps), symbolCount(_symbolCount), stateCount(_stateCount), start(_start), final(std::forward<vector<unsigned int>>(_final)), symbols(symbolCount), newStates(stateCount) {
    for (unsigned int i = 0; i < symbolCount; i++)
      symbols[i] = i;
  };
  NFA(const nfaBuilder& nfa);
  void getClosure(BitSet& s) const;
};

std::ostream& operator<<(std::ostream&, const NFA&);

#endif
