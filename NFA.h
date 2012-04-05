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


//template <class T>
//struct set {
//  virtual void join(const set<T>& s) = 0;
//  virtual void intersect(const set<T>& s) = 0;
//  virtual void remove(const set<T>& s) = 0;
//  
//  virtual bool contains(const T& x) const = 0;
//  virtual void add(const T& x) = 0;
//  virtual void remove(const T& x) = 0;
//  
//  virtual bool isEmpty() const = 0;
//  virtual int size() const = 0;
//  
//  virtual bool intersects(const set<T>& s) const = 0;
//  virtual bool contains(const set<T>& s) const = 0;
//  virtual bool isSubsetOf(const set<T>& s) const = 0;
//  virtual ~set() = 0;
//};
//
//struct bitset : public set<int> {
//  BitSet data;
//  
//  bitset();
//  
//  virtual void join(const bitset& s);
//  virtual void intersect(const bitset& s);
//  virtual void remove(const bitset& s);
//  
//  virtual bool contains(const int& x) const;
//  virtual void add(const int& x);
//  virtual void remove(const int& x);
//  
//  virtual bool isEmpty() const;
//  virtual int size() const;
//  
//  virtual bool intersects(const bitset& s) const;
//  virtual bool contains(const bitset& s) const;
//  virtual bool isSubsetOf(const bitset& s) const;
//};

struct NFA {
  int eps;
  unsigned int symbolCount, stateCount, start;
  vector<unsigned int> final;
  vector<vector<BitSet>> table;
  vector<char> symbols;
  NFA(unsigned int _symbolCount, unsigned int _stateCount, int _eps, unsigned int _start, vector<unsigned int>&& _final) : eps(_eps), symbolCount(_symbolCount), stateCount(_stateCount), start(_start), final(std::forward<vector<unsigned int>>(_final)), symbols(symbolCount) {
    for (unsigned int i = 0; i < symbolCount; i++)
      symbols[i] = i;
  };
  NFA(nfaBuilder&& nfa);
  void getClosure(BitSet& s) const;
};

#endif
