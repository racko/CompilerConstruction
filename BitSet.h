#ifndef _BITSET_H_
#define _BITSET_H_

#include <cmath>
using std::ceil;
#include <cstring>
using std::memmove;
#include <iostream>
using std::cerr;
using std::cout;
#include <iomanip>
using std::endl;
using std::hex;
using std::dec;
using std::ostream;
#include <exception>
using std::exception;

struct BitSet {
  struct ref {
    long long *p;
    unsigned int w;
    unsigned long long m;
    
    ref(long long* _p, unsigned int i);
    
    operator bool () const;
    
    ref& operator= (const bool x);
    
    ref& operator= (const ref& _x);
  };

  unsigned int n = 0;
  unsigned int m = 0;
  long long* p = nullptr;

  BitSet();

  BitSet(unsigned int _n);
  BitSet(unsigned int _n, const bool x);

  BitSet(BitSet&& s);

  BitSet(const BitSet& s);
  
  BitSet& operator=(const BitSet& s);
  
  void resize(unsigned int _n);
  
  ~BitSet();

  ref operator[](unsigned int i);

  const ref operator[](unsigned int i) const;

  unsigned int size() const;
  
  bool operator== (const BitSet& rhs) const;

  BitSet& operator|=(const BitSet& rhs);

  BitSet& operator&=(const BitSet& rhs);

  BitSet operator~() const;

  struct sparse_iterator {
    BitSet* s = nullptr;
    unsigned int i = 0;
    unsigned int _c = 0;

    sparse_iterator();
    
    sparse_iterator(const BitSet& _s);

    ~sparse_iterator();

    bool operator!=(const sparse_iterator&);

    unsigned int operator*() const;

    sparse_iterator& operator++();
  };

  sparse_iterator begin() const;

  sparse_iterator end() const;

  int nextSetBit(unsigned int fromIndex) const;
};

BitSet operator|(const BitSet& lhs, const BitSet& rhs);

BitSet operator&(const BitSet& lhs, const BitSet& rhs);

ostream& operator<<(ostream&, const BitSet&);

#endif
