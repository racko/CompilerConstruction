#include "BitSet.h"
#include "Print.h"
#include <vector>
using std::vector;
#include <ctime>
using std::time;
using std::forward;
using std::move;
#include <cmath>
#include <iomanip>
using std::dec;
using std::hex;

BitSet::BitSet(unsigned int _n) : n(_n), w((n + 63) >> 6), wordsInUse(w), p(new long long[w]) {
  //std::cout << "constructed BitSet(" << _n << "): " << *this << endl;
}

BitSet::BitSet(unsigned int _n, const bool x) : n(_n), w((n + 63) >> 6), wordsInUse(w), p(new long long[w]) {
  if (x)
    memset(p, 0xFF, w * sizeof(long long));
  else
    memset(p, 0x00, w * sizeof(long long));
  //std::cout << "constructed BitSet(" << _n << ", " << x << "): " << *this << endl;
}

BitSet::BitSet(BitSet&& s) {
  operator=(move(s));
}

BitSet::BitSet(const BitSet& s) {
  operator=(s);
}

BitSet& BitSet::operator=(const BitSet& s) {
  //std::cout << "copying BitSet(" << s << "): ";
  if (w < s.wordsInUse && p)
    delete[] p;
  if (!p)
    p = new long long[s.wordsInUse];
  memmove(p, s.p, s.wordsInUse * sizeof(long long));
  n = s.n;
  w = s.wordsInUse;
  wordsInUse = s.wordsInUse;
  //std::cout << *this << endl;
  return *this;
}

BitSet& BitSet::operator=(BitSet&& s) {
  //std::cout << "moving BitSet(" << s << "): ";
  if (p)
    delete[] p;
  n = s.n;
  w = s.w;
  wordsInUse = s.wordsInUse;
  p = s.p;
  s.n = 0;
  s.w = 0;
  s.wordsInUse = 0;
  s.p = nullptr;
  //std::cout << *this << endl;
  return *this;
}

BitSet& BitSet::operator=(const BitSetComplement& rhs) {
  //std::cout << "copying BitSet(" << s << "): ";
  if (n != rhs.s.n) {
    throw std::runtime_error("dimensions don't match: " + std::to_string(n) + " != " + std::to_string(rhs.s.n));
  }

  for (auto i = 0u; i < wordsInUse; i++)
    p[i] = ~rhs.s.p[i];
  //std::cout << *this << endl;
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
    delete[] p;
    n = _n;
    w = _w;
    wordsInUse = newWordsInUse;
    p = _p;
  }
}

unsigned int BitSet::count() const {
  unsigned int c = 0;
  for (auto i = begin(); i != end(); ++i)
    c++;
  //std::cout << "size before return: " << c << endl;
  return c;
//  std::cerr << "not implemented yet" << endl;
//  throw exception();
//  unsigned int c = 0;
//  for (unsigned int i = 0; i < m-1; i++) {
//    long long v = p[i];
//
//  }
//  return c;
}

unsigned long long BitSet::max() const {
  throw std::runtime_error("not implemented yet");
  //if (p[w-1] & (0xffffffffffffffffULL << n));
  //long long* word = p + w - 1;
  //while (word == 0)
  //  word--;
  //return 0;
}

void BitSet::clear() {
  for (unsigned int i = 0; i < wordsInUse; i++)
    p[i] = 0LL;
}

bool BitSet::operator== (const BitSet& rhs) const {
  if (n != rhs.n)
    return false;

  if (n == 0)
    return true;
  unsigned int u = n >> 6;
  for (auto i = 0u; i < u; i++)
    if (p[i] != rhs.p[i])
      return false;
  auto mask = -1LL >> -(n + 1);
  return (p[u] & mask) == (rhs.p[u] & mask);
}

BitSet& BitSet::operator|=(const BitSet& rhs) {
  //std::cout << *this << ".operator|(" << rhs << ")" << endl;
  if (n != rhs.n) {
    throw std::runtime_error("dimensions don't match: " + std::to_string(n) + " != " + std::to_string(rhs.n));
  }

  for (auto i = 0u; i < wordsInUse; i++)
    p[i] |= rhs.p[i];
  return *this;
}

BitSet& BitSet::operator&=(const BitSet& rhs) {
  //std::cout << *this << ".operator&(" << rhs << ")" << endl;
  if (n != rhs.n) {
    throw std::runtime_error("dimensions don't match: " + std::to_string(n) + " != " + std::to_string(rhs.n));
  }

  for (auto i = 0u; i < wordsInUse; i++)
    p[i] &= rhs.p[i];
  return *this;
}

BitSet BitSet::operator~() const {
  //std::cout << *this << ".operator~()" << endl;
  BitSet s(n);
  for (auto i = 0u; i < wordsInUse; i++)
    s.p[i] = ~p[i];
  return s;
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
  //std::cout << "hashing " << s << ": ";
  unsigned long long h = 0xBB40E64DA205B064LL;
  unsigned int u = s.n >> 6;
  unsigned int j;
  char* k;
  for (j = 0, k = (char*)s.p; j < u * sizeof(unsigned long long); j++, k++)
    h = (h * 7664345821815920749LL) ^ hashfn_tab[(unsigned char)(*k)];
  long long lastWord = s.p[u] & (-1LL >> -(s.n + 1));
  for (j = 0, k = (char*)&lastWord; j < sizeof(unsigned long long); j++)
    h = (h * 7664345821815920749LL) ^ hashfn_tab[(unsigned char)(*k)];
  //std::cout << h << endl;
  return h;
}

unsigned long long std::hash<BitSet>::hashfn_tab[256];

BitSet operator|(const BitSet& lhs, const BitSet& rhs) {
  //std::cout << "operator|(" << lhs << ", " << rhs << ")" << endl;
  if (lhs.n != rhs.n) {
    throw std::runtime_error("dimensions don't match: " + std::to_string(lhs.n) + " != " + std::to_string(rhs.n));
  }

  BitSet s(lhs);
  s |= rhs;

  return s;
}

BitSet operator&(const BitSet& lhs, const BitSet& rhs) {
  //std::cout << "operator&(" << lhs << ", " << rhs << ")" << endl;
  if (lhs.n != rhs.n) {
    throw std::runtime_error("dimensions don't match: " + std::to_string(lhs.n) + " != " + std::to_string(rhs.n));
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
