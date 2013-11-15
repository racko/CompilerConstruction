#ifndef _BITSET_H_
#define _BITSET_H_

#include "Regex.h"
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

struct BitSetComplement;

struct BitSet {
  struct ref {
    long long *p;
    unsigned int w;
    long long m;
    
    ref(long long* _p, unsigned int i);
    
    operator bool () const;
    
    ref& operator= (const bool x);
    
    ref& operator= (const ref& _x);
  };

  struct const_ref {
    const long long *p;
    unsigned int w;
    long long m;
    
    const_ref(const long long* _p, unsigned int i);
    const_ref(const const_ref&) = default;
    
    operator bool () const;
    
    const_ref& operator= (const const_ref&) = delete;
    const_ref& operator= (const_ref&&) = delete;
  };

  unsigned int n = 0;
  unsigned int w = 0;
  unsigned int wordsInUse = 0;
  long long* p = nullptr;

  BitSet();

  BitSet(unsigned int _n);
  BitSet(unsigned int _n, const bool x);

  BitSet(BitSet&& s);

  BitSet(const BitSet& s);
  
  BitSet& operator=(const BitSet& s);
  
  BitSet& operator=(BitSet&& s);
  
  BitSet& operator=(const BitSetComplement& s);
  
  void resize(unsigned int _n);
  
  ~BitSet();

  ref operator[](unsigned int i);

  const_ref operator[](unsigned int i) const;

  unsigned int size() const;

  unsigned int count() const;

  unsigned long long max() const;

  void clear();
  
  bool operator== (const BitSet& rhs) const;

  BitSet& operator|=(const BitSet& rhs);

  BitSet& operator&=(const BitSet& rhs);

  BitSet operator~() const;

  struct sparse_iterator {
    const BitSet& s;
    unsigned int c;
    unsigned int next;

    sparse_iterator();
    
    sparse_iterator(const BitSet& _s, unsigned int n);

    ~sparse_iterator();

    bool operator!=(const sparse_iterator&) const;

    unsigned int operator*() const;

    sparse_iterator& operator++();
  };

  sparse_iterator begin() const;

  sparse_iterator end() const;

  int nextSetBit(unsigned int fromIndex) const;
};

namespace std {
  template<> class hash<BitSet> {
    static unsigned long long hashfn_tab[256];
    public:

    hash();

    size_t operator()(const BitSet &s) const;
  };
}

inline BitSet::ref BitSet::operator[](unsigned int i) {
  return BitSet::ref(p, i);
}

inline BitSet::const_ref BitSet::operator[](unsigned int i) const {
  return BitSet::const_ref(p, i);
}

inline unsigned int BitSet::size() const {
  return n;
}

inline BitSet::~BitSet() {
  //cout << "deleting BitSet: " << *this << endl;
  if (p) {
    delete[] p;
  }
}

BitSet operator|(const BitSet& lhs, const BitSet& rhs);

BitSet operator&(const BitSet& lhs, const BitSet& rhs);

ostream& operator<<(ostream&, const BitSet&);

void testBitSet();

inline BitSet::ref::ref(long long* _p, unsigned int i) : p(_p), w(i >> 6), m(0x1ULL << i) {}

inline BitSet::ref::operator bool () const {
  return p[w] & m;
}

inline BitSet::const_ref::const_ref(const long long* _p, unsigned int i) : p(_p), w(i >> 6), m(0x1ULL << i) {}

inline BitSet::const_ref::operator bool () const {
  return p[w] & m;
}

inline BitSet::ref& BitSet::ref::operator= (const bool x) {
  p[w] = (p[w] & ~m) | (-x & m);
  return *this;
}

inline BitSet::ref& BitSet::ref::operator= (const BitSet::ref& _x) {
  bool x = _x;
  p[w] = (p[w] & ~m) | (-x & m);
  return *this;
}

inline BitSet::sparse_iterator::sparse_iterator(const BitSet& _s, unsigned int n = 0) : s(_s), c(n), next(n) {
  if (next < s.n)
    operator++();
}

inline BitSet::sparse_iterator::~sparse_iterator() {}

inline bool BitSet::sparse_iterator::operator!=(const BitSet::sparse_iterator& it) const {
  //cout << "next != it.next: " << next << " != " << it.next << endl;
  return next != it.next;
}

inline unsigned int BitSet::sparse_iterator::operator*() const {
  return c;
}

//template<class T>
//function<ostream&(ostream&)> bin(T x);
//
//ostream& operator<<(ostream&, const function<ostream&(ostream&)>&);

inline BitSet::sparse_iterator& BitSet::sparse_iterator::operator++() {
  unsigned int u = next >> 6;
  long long i = s.p[u] & (0xffffffffffffffffLL << next);

  while (true) {
    //cout << "i = " << bin(i) << endl;
    if (i != 0) {
      int _c = 0;
      if (!(i & 0x1)) {
        if ((i & 0xffffffff) == 0) {
          i >>= 32;
          _c += 32;
        }
        if ((i & 0xffff) == 0) {
          i >>= 16;
          _c += 16;
        }
        if ((i & 0xff) == 0) {
          i >>= 8;
          _c += 8;
        }
        if ((i & 0xf) == 0) {
          i >>= 4;
          _c += 4;
        }
        if ((i & 0x3) == 0) {
          i >>= 2;
          _c += 2;
        }
        if ((i & 0x1) == 0) {
          i >>= 1;
          _c += 1;
        }
      }
//      int x, y;
//      int n = 63;
//      y = (int)i; if (y != 0) { n = n -32; x = y; } else x = (int)(i>>32);
//      y = x <<16; if (y != 0) { n = n -16; x = y; }
//      y = x << 8; if (y != 0) { n = n - 8; x = y; }
//      y = x << 4; if (y != 0) { n = n - 4; x = y; }
//      y = x << 2; if (y != 0) { n = n - 2; x = y; }
//      int _c = n - ((x << 1) >> 31);
      //cout << "trailing zeros: " << _c << endl;
      c = (u << 6) + _c;
      //cout << "c: " << c << endl;
      next = c + 1;
      //cout << "next: " << next << endl;
      return *this;
    }
    if (++u == s.wordsInUse) {
      next = s.n + 1;
      //cout << "reached end" << endl;
      return *this;
    }
    i = s.p[u];
    c = u << 6;
  }
  ///////////////////////////////////////////////////////////////////////////////////////
//  cout << "operator++" << endl;
//  cout << *s << endl;
//  cout << "i = " << i << endl;
//  cout << "_c = " << _c << endl;
//
//  while (_c < s->n && s->p[i] == 0) {
//    cout << "p[" << i << "] == 0" << endl;
//    i++;
//    _c = i * 64;
//    cout << *s << endl;
//    cout << "i = " << i << endl;
//    cout << "_c = " << _c << endl;
//  }
//
//  if (_c >= s->n) {
//    cout << "_c >= n: " << _c << " >= " << s->n << endl;
//    return *this;
//  }
//
//  int c = 0;
//  if (!(s->p[i] & 0x1)) {
//    if ((s->p[i] & 0xffffffff) == 0) {
//      s->p[i] >>= 32;
//      c += 32;
//    }
//    if ((s->p[i] & 0xffff) == 0) {
//      s->p[i] >>= 16;
//      c += 16;
//    }
//    if ((s->p[i] & 0xff) == 0) {
//      s->p[i] >>= 8;
//      c += 8;
//    }
//    if ((s->p[i] & 0xf) == 0) {
//      s->p[i] >>= 4;
//      c += 4;
//    }
//    if ((s->p[i] & 0x3) == 0) {
//      s->p[i] >>= 2;
//      c += 2;
//    }
//    if ((s->p[i] & 0x1) == 0) {
//      s->p[i] >>= 1;
//      c += 1;
//    }
//  }
//  cout << c << " clear bits." << endl;
//  _c += c;
//  cout << "_c = " << _c << endl;
//  cout << *s << endl;
//  cout << "i = " << i << endl;
//  s->p[i] &= ~0x1ULL;
//  cout << "cleared lsb" << endl;
//  cout << *s << endl;
//  cout << "i = " << i << endl;
//  return *this;
}

struct BitSetComplement {
  const BitSet& s;
  BitSetComplement(const BitSet&_s) : s(_s) {}
};

inline BitSet::sparse_iterator BitSet::begin() const {
  return BitSet::sparse_iterator(*this);
}

inline BitSet::sparse_iterator BitSet::end() const {
  return BitSet::sparse_iterator(*this, n + 1);
}

#endif
