#ifndef _BITSET_H_
#define _BITSET_H_

#include <cmath>
using std::ceil;
#include <cstring>
using std::memmove;
#include <iostream>
using std::cout;
#include <iomanip>
using std::endl;

struct BitSet {
  struct ref {
    long long *p;
    unsigned int w;
    unsigned long long m;
    
    ref(long long* _p, unsigned int i) : p(_p), w(i >> 6), m(1ULL << (i & 0x3F)) {}; // cout << i << ": (" << w << "," << m << ")" << endl; 
    
    operator bool () const {
      return p[w] & m;
    }
    
    ref& operator= (const bool x) {
      p[w] = (p[w] & ~m) | (-x & m);
      return *this;
    }
    
    ref& operator= (const ref& _x) {
      bool x = _x;
      p[w] = (p[w] & ~m) | (-x & m);
      return *this;
    }
  };

  unsigned int n = 0;
  unsigned int m = 0;
  long long* p = nullptr;

  BitSet() {}

  BitSet(unsigned int _n) : n(_n), m((n + 63) / 64), p(new long long[m]) {}
  
  void resize(unsigned int _n) {
    if (n >= _n)
      n = _n;
    else {
      unsigned int _m = (_n + 63) / 64;
      long long* _p = new long long[_m];
      memmove(_p, p, m * sizeof(long long));
      delete p;
      n = _n;
      m = _m;
      p = _p;
    }
  }
  
  ~BitSet() {
    if (p != nullptr)
      delete p;
  }

  ref operator[](unsigned int i) {
    return ref(p, i);
  }

  const ref operator[](unsigned int i) const {
    return ref(p, i);
  }

  unsigned int size() const {
    return n;
  }
  
  bool operator== (const BitSet& rhs) const {
    if (n != rhs.n)
      return false;

    for (auto i = 0u; i < m; i++)
      if (p[i] != rhs.p[i])
        return false;
    return true;
  }
};

#endif
