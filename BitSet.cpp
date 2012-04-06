#include "BitSet.h"
#include "Regex.h"

BitSet::BitSet() {
  //cout << "constructed BitSet(): " << *this << endl;
}

BitSet::BitSet(unsigned int _n) : n(_n), m((n + 63) / 64), p(new long long[m]) {
  //cout << "constructed BitSet(" << _n << "): " << *this << endl;
}
BitSet::BitSet(unsigned int _n, const bool x) : n(_n), m((n + 63) / 64), p(new long long[m]) {
  if (x)
    memset(p, 0xFF, m * sizeof(long long));
  else
    memset(p, 0x00, m * sizeof(long long));
  //cout << "constructed BitSet(" << _n << ", " << x << "): " << *this << endl;
}

BitSet::BitSet(BitSet&& s) : n(s.n), m(s.m), p(s.p) {
  //cout << "moving BitSet(" << s << "): ";
  s.n = 0;
  s.m = 0;
  s.p = nullptr;
  //cout << *this << endl;
}

BitSet::BitSet(const BitSet& s) {
  operator=(s);
}

BitSet& BitSet::operator=(const BitSet& s) {
  //cout << "copying BitSet(" << s << "): ";
  if (m != s.m && p)
    delete p;
  if (!p)
    p = new long long[s.m];
  memmove(p, s.p, s.m * sizeof(long long));
  n = s.n;
  m = s.m;
  //cout << *this << endl;
  return *this;
}

void BitSet::resize(unsigned int _n) {
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

BitSet::~BitSet() {
  //cout << "deleting BitSet: " << *this << endl;
  if (p) {
    delete p;
  }
}

BitSet::ref BitSet::operator[](unsigned int i) {
  return BitSet::ref(p, i);
}

const BitSet::ref BitSet::operator[](unsigned int i) const {
  return BitSet::ref(p, i);
}

unsigned int BitSet::size() const {
  return n;
}

bool BitSet::operator== (const BitSet& rhs) const {
  if (n != rhs.n)
    return false;

  if (n == 0)
    return true;

  for (auto i = 0u; i < m - 1; i++)
    if (p[i] != rhs.p[i])
      return false;
  auto mask = (1 << n) - 1;
  if ((p[m-1] & mask) != (rhs.p[m-1] & mask))
    return false;
  return true;
}

BitSet& BitSet::operator|=(const BitSet& rhs) {
  if (n != rhs.n) {
    cerr << "dimensions don't match: " << n << " != " << rhs.n << endl;
    throw exception();
  }

  for (auto i = 0u; i < m; i++)
    p[i] |= rhs.p[i];
  return *this;
}

BitSet& BitSet::operator&=(const BitSet& rhs) {
  if (n != rhs.n) {
    cerr << "dimensions don't match: " << n << " != " << rhs.n << endl;
    throw exception();
  }

  for (auto i = 0u; i < m; i++)
    p[i] &= rhs.p[i];
  return *this;
}

BitSet BitSet::operator~() const {
  BitSet s(n);
  for (auto i = 0u; i < m; i++)
    s.p[i] = ~p[i];
  return s;
}

BitSet::sparse_iterator BitSet::begin() const {
  return BitSet::sparse_iterator(*this);
}

BitSet::sparse_iterator BitSet::end() const {
  return BitSet::sparse_iterator();
}

int BitSet::nextSetBit(unsigned int fromIndex) const {
  static const unsigned long long limit = 1ULL << 63;
  unsigned int w = fromIndex >> 6;
  unsigned long long m = 1ULL << (fromIndex & 0x3F);
  while (!(p[w] & m) && fromIndex < n) {
    fromIndex++;
    if (m < limit) 
      m <<= 1;
    else {
      w++;
      m = 1;
    }
  }
  if (fromIndex < n)
    return int(fromIndex);
  else
    return -1;
}

BitSet operator|(const BitSet& lhs, const BitSet& rhs) {
  if (lhs.n != rhs.n) {
    cerr << "dimensions don't match: " << lhs.n << " != " << rhs.n << endl;
    throw exception();
  }

  BitSet s(lhs.n);

  for (auto i = 0u; i < lhs.m; i++)
    s.p[i] = lhs.p[i] | rhs.p[i];

  return s;
}

BitSet operator&(const BitSet& lhs, const BitSet& rhs) {
  if (lhs.n != rhs.n) {
    cerr << "dimensions don't match: " << lhs.n << " != " << rhs.n << endl;
    throw exception();
  }

  BitSet s(lhs.n);

  for (auto i = 0u; i < lhs.m; i++)
    s.p[i] = lhs.p[i] & rhs.p[i];

  return s;
}

BitSet::ref::ref(long long* _p, unsigned int i) : p(_p), w(i >> 6), m(1ULL << (i & 0x3F)) {};

BitSet::ref::operator bool () const {
  return p[w] & m;
}

BitSet::ref& BitSet::ref::operator= (const bool x) {
  p[w] = (p[w] & ~m) | (-x & m);
  return *this;
}

BitSet::ref& BitSet::ref::operator= (const BitSet::ref& _x) {
  bool x = _x;
  p[w] = (p[w] & ~m) | (-x & m);
  return *this;
}

BitSet::sparse_iterator::sparse_iterator() {}   

BitSet::sparse_iterator::sparse_iterator(const BitSet& _s) : s(new BitSet(_s)) {
  operator++();
}

BitSet::sparse_iterator::~sparse_iterator() {
  if (s)
    delete(s);
}

bool BitSet::sparse_iterator::operator!=(const BitSet::sparse_iterator&) {
  return _c < s->n;
}

unsigned int BitSet::sparse_iterator::operator*() const {
  return _c;
}

BitSet::sparse_iterator& BitSet::sparse_iterator::operator++() {
  //cout << "operator++" << endl;
  //cout << *s << endl;
  //cout << "i = " << i << endl;
  //cout << "_c = " << _c << endl;

  while (s->p[i] == 0 && _c < s->n) {
    //cout << "p[" << i << "] == 0" << endl;
    i++;
    _c = i * 64;
    //cout << *s << endl;
    //cout << "i = " << i << endl;
    //cout << "_c = " << _c << endl;
  }

  if (_c >= s->n) {
    //cout << "_c >= n: " << _c << " >= " << s->n << endl;
    return *this;
  }

  int c = 0;
  if (!(s->p[i] & 0x1)) {
    if ((s->p[i] & 0xffffffff) == 0) {
      s->p[i] >>= 32;
      c += 32;
    }
    if ((s->p[i] & 0xffff) == 0) {
      s->p[i] >>= 16;
      c += 16;
    }
    if ((s->p[i] & 0xff) == 0) {
      s->p[i] >>= 8;
      c += 8;
    }
    if ((s->p[i] & 0xf) == 0) {
      s->p[i] >>= 4;
      c += 4;
    }
    if ((s->p[i] & 0x3) == 0) {
      s->p[i] >>= 2;
      c += 2;
    }
    if ((s->p[i] & 0x1) == 0) {
      s->p[i] >>= 1;
      c += 1;
    }
  }
  //cout << c << " clear bits." << endl;
  _c += c;
  //cout << "_c = " << _c << endl;
  //cout << *s << endl;
  //cout << "i = " << i << endl;
  s->p[i] &= ~0x1ULL;
  //cout << "cleared lsb" << endl;
  //cout << *s << endl;
  //cout << "i = " << i << endl;
  return *this;
}

ostream& operator<<(ostream& s, const BitSet& v) {
  s << "(" << hex << &v << dec << ")";
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
