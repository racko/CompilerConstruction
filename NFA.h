#ifndef _NFA_H_
#define _NFA_H_

#include <unordered_map>
using std::unordered_map;
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


struct node {
  bool deleted;
  unordered_map<unsigned int,vector<unsigned int>> ns;
};

struct nfaBuilder {
  unsigned int start, end;
  vector<node> ns;
  vector<char> idToSymbol;
  unordered_map<char, unsigned int> symbolToId;
  nfaBuilder() {
    ns.emplace_back();
    start = 0;
    ns.emplace_back();
    end = 1;
    idToSymbol.push_back(0);
    symbolToId[0] = 0;
  }
};

struct NFA {
  int eps;
  unsigned int symbolCount, stateCount, start;
  vector<bool> final;
  vector<vector<vector<bool>>> table;
  NFA(unsigned int _symbolCount, unsigned int _stateCount, int _eps, unsigned int _start, vector<bool>&& _final) : eps(_eps), symbolCount(_symbolCount), stateCount(_stateCount), start(_start), final(std::forward<vector<bool>>(_final)) {};
  NFA(nfaBuilder&& nfa);
  void getClosure(vector<bool>& s) const;
};

#endif
