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
        ref(ref&&) = default;

        operator bool() const;

        ref& operator=(const bool x);

        ref(const ref&) = delete;
        ref& operator=(const ref&) = delete;
        ref& operator=(ref&&) = delete;
    };

    struct const_ref {
        const unordered_set<unsigned int>& s;
        unsigned int i;

        const_ref(const unordered_set<unsigned int>& _p, unsigned int i);
        const_ref(const_ref&&) = default;

        operator bool() const;

        const_ref(const const_ref&) = delete;
        const_ref& operator=(const const_ref&) = delete;
        const_ref& operator=(const_ref&&) = delete;
    };

    unordered_set<unsigned int> s;
    unsigned int n = 0;

    HashSet() = default;

    HashSet(unsigned int _n);
    HashSet(unsigned int _n, const bool x);

    HashSet(HashSet&& s);

    HashSet(const HashSet& s);

    HashSet& operator=(const HashSet& s);

    HashSet& operator=(HashSet&& s);

    void resize(unsigned int _n);

    ref operator[](unsigned int i);

    const_ref operator[](unsigned int i) const;

    unsigned int size() const;

    size_t count() const;

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

inline HashSet::HashSet(unsigned int _n) : n(_n) {
    //cout << "constructed HashSet(" << _n << "): " << *this << endl;
}

inline HashSet::HashSet(unsigned int _n, const bool x) : n(_n) {
    if (x) {
        cerr << "You don't want to construct a HashSet with all values in it ... you'd want to use a BitSet for this." << endl;
        throw exception();
    }
    //cout << "constructed HashSet(" << _n << ", " << x << "): " << *this << endl;
}

inline HashSet::HashSet(HashSet&& s) {
    operator=(forward<HashSet>(s));
}

inline HashSet::HashSet(const HashSet& s) {
    operator=(s);
}

inline HashSet& HashSet::operator=(const HashSet& _s) {
    //cout << "copying HashSet(" << s << "): ";
    s = _s.s;
    n = _s.n;
    //cout << *this << endl;
    return *this;
}

inline HashSet& HashSet::operator=(HashSet&& _s) {
    //cout << "moving HashSet(" << s << "): ";
    s = move(_s.s);
    n = _s.n;
    _s.n = 0;
    //cout << *this << endl;
    return *this;
}

inline void HashSet::resize(unsigned int _n) {
    if (n <= _n) {
        n = _n;
    } else {
        cerr << "HashSet::resize() with newN < N: not implemented yet." << endl;
        throw exception();
    }
}

inline HashSet::ref HashSet::operator[](unsigned int i) {
    return HashSet::ref(s, i);
}

inline HashSet::const_ref HashSet::operator[](unsigned int i) const {
    return HashSet::const_ref(s, i);
}

inline unsigned int HashSet::size() const {
    return n;
}

inline size_t HashSet::count() const {
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

inline unsigned long long HashSet::max() const {
    cerr << "not implemented yet" << endl;
    throw exception();
    return 0;
}

inline void HashSet::clear() {
    s.clear();
}

inline bool HashSet::operator==(const HashSet& rhs) const {
    if (n != rhs.n)
        return false;

    return s == rhs.s;
}

inline HashSet operator|(const HashSet& lhs, const HashSet& rhs) {
    //cout << "operator|(" << lhs << ", " << rhs << ")" << endl;
    if (lhs.n != rhs.n) {
        cerr << "dimensions don't match: " << lhs.n << " != " << rhs.n << endl;
        throw exception();
    }

    HashSet s(lhs);
    s |= rhs;

    return s;
}

inline HashSet operator&(const HashSet& lhs, const HashSet& rhs) {
    //cout << "operator&(" << lhs << ", " << rhs << ")" << endl;
    if (lhs.n != rhs.n) {
        cerr << "dimensions don't match: " << lhs.n << " != " << rhs.n << endl;
        throw exception();
    }

    HashSet s(lhs);
    s &= rhs;

    return s;
}

HashSet operator|(const HashSet& lhs, const HashSet& rhs);

HashSet operator&(const HashSet& lhs, const HashSet& rhs);

ostream& operator<<(ostream&, const HashSet&);

inline HashSet::ref::ref(unordered_set<unsigned int>& _s, unsigned int _i) : s(_s), i(_i) {}
inline HashSet::const_ref::const_ref(const unordered_set<unsigned int>& _s, unsigned int _i) : s(_s), i(_i) {}

inline HashSet::ref::operator bool() const {
    return s.count(i) > 0;
}

inline HashSet::const_ref::operator bool() const {
    return s.count(i) > 0;
}

inline HashSet::ref& HashSet::ref::operator=(const bool x) {
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

inline void a_and_not_b(const HashSet& a, const HashSet& b, HashSet& c) {
    c.clear();
    for (auto i : a) {
        c[i] = !b[i];
    }
}

#endif
