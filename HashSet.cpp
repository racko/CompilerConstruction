#include "HashSet.h"

HashSet::HashSet() {
  //cout << "constructed HashSet(): " << *this << endl;
}

HashSet::HashSet(unsigned int _n) : n(_n) {
  //cout << "constructed HashSet(" << _n << "): " << *this << endl;
}

HashSet::HashSet(unsigned int _n, const bool x) : n(_n) {
  if (x) {
    cerr << "You don't want to construct a HashSet with all values in it ... you'd want to use a BitSet for this." << endl;
    throw exception();
  }
  //cout << "constructed HashSet(" << _n << ", " << x << "): " << *this << endl;
}

HashSet::HashSet(HashSet&& s) {
  operator=(forward<HashSet>(s));
}

HashSet::HashSet(const HashSet& s) {
  operator=(s);
}

HashSet& HashSet::operator=(const HashSet& _s) {
  //cout << "copying HashSet(" << s << "): ";
  s = _s.s;
  n = _s.n;
  //cout << *this << endl;
  return *this;
}

HashSet& HashSet::operator=(HashSet&& _s) {
  //cout << "moving HashSet(" << s << "): ";
  s = move(_s.s);
  n = _s.n;
  _s.n = 0;
  //cout << *this << endl;
  return *this;
}

void HashSet::resize(unsigned int _n) {
  if (n <= _n) {
    n = _n;
  } else {
    cerr << "HashSet::resize() with newN < N: not implemented yet." << endl;
    throw exception();
  }
}

HashSet::~HashSet() {
  //cout << "deleting HashSet: " << *this << endl;
}

HashSet::ref HashSet::operator[](unsigned int i) {
  return HashSet::ref(s, i);
}

HashSet::const_ref HashSet::operator[](unsigned int i) const {
  return HashSet::const_ref(s, i);
}

unsigned int HashSet::size() const {
  return n;
}

unsigned int HashSet::count() const {
  return s.size();
//  cerr << "not implemented yet" << endl;
//  throw exception();
//  unsigned int c = 0;
//  for (unsigned int i = 0; i < m-1; i++) {
//    long long v = p[i];
//
//  }
//  return c;
}

unsigned long long HashSet::max() const {
  cerr << "not implemented yet" << endl;
  throw exception();
  return 0;
}

void HashSet::clear() {
  s.clear();
}

bool HashSet::operator== (const HashSet& rhs) const {
  if (n != rhs.n)
    return false;

  return s == rhs.s;
}

HashSet& HashSet::operator|=(const HashSet& rhs) {
  //cout << *this << ".operator|(" << rhs << ")" << endl;
  if (n != rhs.n) {
    cerr << "dimensions don't match: " << n << " != " << rhs.n << endl;
    throw exception();
  }

  for (auto i = rhs.s.begin(); i != rhs.s.end(); ++i)
    s.insert(*i);
  return *this;
}

HashSet& HashSet::operator&=(const HashSet& rhs) {
  //cout << *this << ".operator&(" << rhs << ")" << endl;
  if (n != rhs.n) {
    cerr << "dimensions don't match: " << n << " != " << rhs.n << endl;
    throw exception();
  }

  for (auto i = s.begin(); i != s.end(); ++i)
    if (rhs.s.count(*i) == 0)
      s.erase(i);
  return *this;
}

HashSet HashSet::operator~() const {
  //cout << *this << ".operator~()" << endl;
  HashSet _s(n);
  for (unsigned int i = 0; i < n; i++)
    if (s.count(i) == 0)
      _s.s.insert(i);
  return _s;
}

HashSet operator|(const HashSet& lhs, const HashSet& rhs) {
  //cout << "operator|(" << lhs << ", " << rhs << ")" << endl;
  if (lhs.n != rhs.n) {
    cerr << "dimensions don't match: " << lhs.n << " != " << rhs.n << endl;
    throw exception();
  }

  HashSet s(lhs);
  s |= rhs;

  return s;
}

HashSet operator&(const HashSet& lhs, const HashSet& rhs) {
  //cout << "operator&(" << lhs << ", " << rhs << ")" << endl;
  if (lhs.n != rhs.n) {
    cerr << "dimensions don't match: " << lhs.n << " != " << rhs.n << endl;
    throw exception();
  }

  HashSet s(lhs);
  s &= rhs;

  return s;
}

ostream& operator<<(ostream& s, const HashSet& v) {
  s << "(" << hex << &v << dec << ":" << v.n << ")";

  if (v.s.empty()) {
    s << "{}";
    return s;
  }
  auto i = v.s.begin();
  s << "{ " << *i;

  for (++i; i != v.s.end(); ++i)
    s << ", " << *i;

  s << " }";
  return s;
}

unsigned long long std::hash<HashSet>::hashfn_tab[256];

std::hash<HashSet>::hash() {
  unsigned long long h;
  h = 0x544B2FBACAAF1684LL;
  for (int j = 0; j < 256; j++) {
    for (int i = 0; i < 31; i++) {
      h = (h >> 7) ^ h;
      h = (h << 11) ^ h;
      h = (h >> 10) ^ h;
    }
    hashfn_tab[j] = h;
  }
}

size_t std::hash<HashSet>::operator()(const HashSet &s) const {
  //cout << "hashing " << s << ": ";
  unsigned long long h = 0xBB40E64DA205B064LL;
  for (auto it = s.begin(); it != s.end(); ++it) {
    int j;
    char* k;
    for (j = 0, k = (char*)&(*it); j < sizeof(*it); j++, k++)
      h = (h * 7664345821815920749LL) ^ hashfn_tab[(unsigned char)(*k)];
  }
  //cout << h << endl;
  return h;
}

