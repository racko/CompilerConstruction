#ifndef _NFA_H_
#define _NFA_H_

#include <unordered_set>
using std::unordered_set;
#include <vector>
using std::vector;


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
//  vector<bool> data;
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
  vector<bool> final;
  vector<vector<vector<bool>>> table;
  NFA(unsigned int _symbolCount, unsigned int _stateCount, int _eps, unsigned int _start, vector<bool>&& _final) : eps(_eps), symbolCount(_symbolCount), stateCount(_stateCount), start(_start), final(std::forward<vector<bool>>(_final)) {};
  void getClosure(vector<bool>& s) const;
};

#endif
