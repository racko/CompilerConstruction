#ifndef _HASHSET_H_
#define _HASHSET_H_

#include <iostream>
using std::ostream;
using std::cerr;
#include <iomanip>
using std::endl;
using std::hex;
using std::dec;
#include <unordered_set>
using std::unordered_set;
#include <exception>
using std::exception;
using std::forward;
using std::size_t;

struct HashSet {
  struct ref {
    unordered_set<unsigned int>& s;
    unsigned int i;
    
    ref(unordered_set<unsigned int>& _p, unsigned int i);
    
    operator bool () const;
    
    ref& operator= (const bool x);
    
    ref& operator= (const ref& _x);
  };

  struct const_ref {
    const unordered_set<unsigned int>& s;
    unsigned int i;
    
    const_ref(const unordered_set<unsigned int>& _p, unsigned int i);
    const_ref(const const_ref&) = default;
    
    operator bool () const;
    
    const_ref& operator= (const const_ref&) = delete;
    const_ref& operator= (const_ref&&) = delete;
  };

  unordered_set<unsigned int> s;
  unsigned int n;

  HashSet();

  HashSet(unsigned int _n);
  HashSet(unsigned int _n, const bool x);

  HashSet(HashSet&& s);

  HashSet(const HashSet& s);
  
  HashSet& operator=(const HashSet& s);
  
  HashSet& operator=(HashSet&& s);
  
  void resize(unsigned int _n);
  
  ~HashSet();

  ref operator[](unsigned int i);

  const_ref operator[](unsigned int i) const;

  unsigned int size() const;

  unsigned int count() const;

  unsigned long long max() const;

  void clear();
  
  bool operator== (const HashSet& rhs) const;

  HashSet& operator|=(const HashSet& rhs);

  HashSet& operator&=(const HashSet& rhs);

  HashSet operator~() const;

  unordered_set<unsigned int>::const_iterator begin() const;

  unordered_set<unsigned int>::const_iterator end() const;
};

namespace std {
  template<> class hash<HashSet> {
    static unsigned long long hashfn_tab[256];
    public:

    hash();

    size_t operator()(const HashSet &s) const;
  };
}

HashSet operator|(const HashSet& lhs, const HashSet& rhs);

HashSet operator&(const HashSet& lhs, const HashSet& rhs);

ostream& operator<<(ostream&, const HashSet&);

void testHashSet();

inline HashSet::ref::ref(unordered_set<unsigned int>& _s, unsigned int _i) : s(_s), i(_i) {}
inline HashSet::const_ref::const_ref(const unordered_set<unsigned int>& _s, unsigned int _i) : s(_s), i(_i) {}

inline HashSet::ref::operator bool () const {
  return s.count(i) > 0;
}

inline HashSet::ref& HashSet::ref::operator= (const bool x) {
  if (x)
    s.insert(i);
  else
    s.erase(i);
  return *this;
}

inline HashSet::ref& HashSet::ref::operator= (const HashSet::ref& x) {
  if (x)
    s.insert(i);
  else
    s.erase(i);
  return *this;
}

inline unordered_set<unsigned int>::const_iterator HashSet::begin() const {
  return s.begin();
}

inline unordered_set<unsigned int>::const_iterator HashSet::end() const {
  return s.end();
}

#endif
