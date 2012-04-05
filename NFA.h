#ifndef _NFA_H_
#define _NFA_H_

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
  pair<unsigned int,unsigned int> p;
  vector<node> ns;
  vector<char> idToSymbol;
  unordered_map<char, unsigned int> symbolToId;
  nfaBuilder() {
    p = getPair();
    idToSymbol.push_back(0);
    symbolToId[0] = 0;
  }

  pair<unsigned int,unsigned int> getPair() {
    pair<unsigned int,unsigned int> out;
    out.first = ns.size();
    ns.emplace_back();
    out.second = ns.size();
    ns.emplace_back();
    return out;
  }

  pair<unsigned int,unsigned int> alt(pair<unsigned int,unsigned int> nfa1, pair<unsigned int,unsigned int> nfa2) {
    auto nfa3 = getPair();

    ns[nfa3.first].ns[0].push_back(nfa1.first);
    ns[nfa3.first].ns[0].push_back(nfa2.first);
    ns[nfa1.second].ns[0].push_back(nfa3.second);
    ns[nfa2.second].ns[0].push_back(nfa3.second);
    return nfa3;
  }

  pair<unsigned int,unsigned int> closure(pair<unsigned int,unsigned int> nfa1) {
    auto out = getPair();
    ns[out.first].ns[0].push_back(nfa1.first);
    ns[out.first].ns[0].push_back(out.second);
    ns[nfa1.second].ns[0].push_back(nfa1.first);
    ns[nfa1.second].ns[0].push_back(out.second);
    return out;
  }

  pair<unsigned int,unsigned int> cat(pair<unsigned int,unsigned int> nfa1, pair<unsigned int,unsigned int> nfa2) {
    ns[nfa1.second] = ns[nfa2.first];
    ns[nfa2.first].deleted = true;
    nfa1.second = nfa2.second;
    return nfa1;
  }
};

struct NFA {
  int eps;
  unsigned int symbolCount, stateCount, start;
  vector<unsigned int> final;
  vector<vector<vector<bool>>> table;
  vector<char> symbols;
  NFA(unsigned int _symbolCount, unsigned int _stateCount, int _eps, unsigned int _start, vector<unsigned int>&& _final) : eps(_eps), symbolCount(_symbolCount), stateCount(_stateCount), start(_start), final(std::forward<vector<unsigned int>>(_final)), symbols(symbolCount) {
    for (unsigned int i = 0; i < symbolCount; i++)
      symbols[i] = i;
  };
  NFA(nfaBuilder&& nfa);
  void getClosure(vector<bool>& s) const;
};

#endif
