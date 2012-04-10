#include "BitSet.h"
#include "Regex.h"
#include "Random.h"
#include <vector>
using std::vector;
#include <ctime>
using std::time;
using std::forward;
#include <cmath>

BitSet::BitSet() {
  //cout << "constructed BitSet(): " << *this << endl;
}

BitSet::BitSet(unsigned int _n) : n(_n), w((n + 63) >> 6), wordsInUse(w), p(new long long[w]) {
  //cout << "constructed BitSet(" << _n << "): " << *this << endl;
}

BitSet::BitSet(unsigned int _n, const bool x) : n(_n), w((n + 63) >> 6), wordsInUse(w), p(new long long[w]) {
  if (x)
    memset(p, 0xFF, w * sizeof(long long));
  else
    memset(p, 0x00, w * sizeof(long long));
  //cout << "constructed BitSet(" << _n << ", " << x << "): " << *this << endl;
}

BitSet::BitSet(BitSet&& s) {
  operator=(forward<BitSet>(s));
}

BitSet::BitSet(const BitSet& s) {
  operator=(s);
}

BitSet& BitSet::operator=(const BitSet& s) {
  //cout << "copying BitSet(" << s << "): ";
  if (w < s.wordsInUse && p)
    delete p;
  if (!p)
    p = new long long[s.wordsInUse];
  memmove(p, s.p, s.wordsInUse * sizeof(long long));
  n = s.n;
  w = s.wordsInUse;
  wordsInUse = s.wordsInUse;
  //cout << *this << endl;
  return *this;
}

BitSet& BitSet::operator=(BitSet&& s) {
  //cout << "moving BitSet(" << s << "): ";
  n = s.n;
  w = s.w;
  wordsInUse = s.wordsInUse;
  p = s.p;
  s.n = 0;
  s.w = 0;
  s.wordsInUse = 0;
  s.p = nullptr;
  //cout << *this << endl;
  return *this;
}

void BitSet::resize(unsigned int _n) {
  auto newWordsInUse = (_n + 63) >> 6;
  if (w <= newWordsInUse) {
    n = _n;
    wordsInUse = newWordsInUse;
  } else {
    unsigned int _w = std::max(newWordsInUse, w << 1);
    long long* _p = new long long[_w];
    memmove(_p, p, newWordsInUse * sizeof(long long));
    delete p;
    n = _n;
    w = _w;
    wordsInUse = newWordsInUse;
    p = _p;
  }
}

BitSet::~BitSet() {
  //cout << "deleting BitSet: " << *this << endl;
  if (p) {
    delete p;
  }
}

BitSet::ref BitSet::operator[](unsigned int i) {
  return BitSet::ref(p, i);
}

BitSet::const_ref BitSet::operator[](unsigned int i) const {
  return BitSet::const_ref(p, i);
}

unsigned int BitSet::size() const {
  return n;
}

unsigned int BitSet::count() const {
  unsigned int c = 0;
  for (auto i = begin(); i != end(); ++i)
    c++;
  //cout << "size before return: " << c << endl;
  return c;
//  cerr << "not implemented yet" << endl;
//  throw exception();
//  unsigned int c = 0;
//  for (unsigned int i = 0; i < m-1; i++) {
//    long long v = p[i];
//
//  }
//  return c;
}

unsigned long long BitSet::max() const {
  cerr << "not implemented yet" << endl;
  throw exception();
  if (p[w-1] & (0xffffffffffffffffULL << n));
  long long* word = p + w - 1;
  while (word == 0)
    word--;
  return 0;
}

void BitSet::clear() {
  for (int i = 0; i < wordsInUse; i++)
    p[i] = 0LL;
}

bool BitSet::operator== (const BitSet& rhs) const {
  if (n != rhs.n)
    return false;

  if (n == 0)
    return true;
  int u = n >> 6;
  for (auto i = 0u; i < u; i++)
    if (p[i] != rhs.p[i])
      return false;
  auto mask = 0xffffffffffffffffULL >> -(n + 1);
  return (p[u] & mask) == (rhs.p[u] & mask);
}

BitSet& BitSet::operator|=(const BitSet& rhs) {
  //cout << *this << ".operator|(" << rhs << ")" << endl;
  if (n != rhs.n) {
    cerr << "dimensions don't match: " << n << " != " << rhs.n << endl;
    throw exception();
  }

  for (auto i = 0u; i < wordsInUse; i++)
    p[i] |= rhs.p[i];
  return *this;
}

BitSet& BitSet::operator&=(const BitSet& rhs) {
  //cout << *this << ".operator&(" << rhs << ")" << endl;
  if (n != rhs.n) {
    cerr << "dimensions don't match: " << n << " != " << rhs.n << endl;
    throw exception();
  }

  for (auto i = 0u; i < wordsInUse; i++)
    p[i] &= rhs.p[i];
  return *this;
}

BitSet BitSet::operator~() const {
  //cout << *this << ".operator~()" << endl;
  BitSet s(n);
  for (auto i = 0u; i < wordsInUse; i++)
    s.p[i] = ~p[i];
  return s;
}

BitSet::sparse_iterator BitSet::begin() const {
  return BitSet::sparse_iterator(*this);
}

BitSet::sparse_iterator BitSet::end() const {
  return BitSet::sparse_iterator(*this, n);
}

int BitSet::nextSetBit(unsigned int fromIndex) const {
  while (fromIndex < n && !operator[](fromIndex))
    fromIndex++;
  if (fromIndex < n)
    return int(fromIndex);
  else
    return -1;
}

std::hash<BitSet>::hash() {
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

size_t std::hash<BitSet>::operator()(const BitSet &s) const {
  //cout << "hashing " << s << ": ";
  unsigned long long h = 0xBB40E64DA205B064LL;
  unsigned int u = s.n >> 6;
  int j;
  char* k;
  for (j = 0, k = (char*)s.p; j < u * sizeof(unsigned long long); j++, k++)
    h = (h * 7664345821815920749LL) ^ hashfn_tab[(unsigned char)(*k)];
  long long lastWord = s.p[u] & (0xffffffffffffffffULL >> -(s.n + 1));
  for (j = 0, k = (char*)&lastWord; j < sizeof(unsigned long long); j++)
    h = (h * 7664345821815920749LL) ^ hashfn_tab[(unsigned char)(*k)];
  //cout << h << endl;
  return h;
}

unsigned long long std::hash<BitSet>::hashfn_tab[256];

BitSet operator|(const BitSet& lhs, const BitSet& rhs) {
  //cout << "operator|(" << lhs << ", " << rhs << ")" << endl;
  if (lhs.n != rhs.n) {
    cerr << "dimensions don't match: " << lhs.n << " != " << rhs.n << endl;
    throw exception();
  }

  BitSet s(lhs);
  s |= rhs;

  return s;
}

BitSet operator&(const BitSet& lhs, const BitSet& rhs) {
  //cout << "operator&(" << lhs << ", " << rhs << ")" << endl;
  if (lhs.n != rhs.n) {
    cerr << "dimensions don't match: " << lhs.n << " != " << rhs.n << endl;
    throw exception();
  }

  BitSet s(lhs);
  s &= rhs;

  return s;
}

ostream& operator<<(ostream& s, const BitSet& v) {
  s << "(" << hex << &v << dec << ":" << v.n << ")";
  unsigned int i = 0;

  while (i < v.size() && !v[i])
    i++;

  if (i == v.size()) {
    s << "{}";
    return s;
  }
  s << "{ " << i;
  for (i++; i < v.size(); i++)
    if (v[i])
      s << ", " << i;

  s << " }";
  return s;
}

void testBitSet() {
  Ranq1(time(0));
  BitSet bs;
  vector<bool> v;
}
