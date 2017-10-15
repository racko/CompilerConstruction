#pragma once

#include <iosfwd> // for std::ostream&
#include <functional> // for std::hash

struct BitSetComplement;

struct BitSet {
    struct ref {
        uint64_t *p;
        uint64_t m;

        ref(uint64_t* _p, unsigned int i);
        ref(ref&& _x) = default;

        operator bool() const;

        ref& operator=(const bool x);

        ref(const ref& _x) = delete;
        ref& operator=(const ref& _x) = delete;
        ref& operator=(ref&& _x) = delete;
    };

    struct const_ref {
        const uint64_t *p;
        uint64_t m;

        const_ref(const uint64_t* _p, unsigned int i);
        const_ref(const_ref&&) = default;

        operator bool() const;

        const_ref(const const_ref&) = delete;
        const_ref& operator=(const const_ref&) = delete;
        const_ref& operator=(const_ref&&) = delete;
    };

    unsigned int n = 0;
    unsigned int w = 0;
    unsigned int wordsInUse = 0;
    uint64_t* p = nullptr;

    BitSet() noexcept = default;

    BitSet(unsigned int _n);
    BitSet(unsigned int _n, const bool x);

    BitSet(BitSet&& s) noexcept;

    BitSet(const BitSet& s);

    BitSet& operator=(const BitSet& s);

    BitSet& operator=(BitSet&& s) noexcept;

    BitSet& operator=(const BitSetComplement& s);

    void swap(BitSet& rhs) noexcept;

    void resize(unsigned int _n, bool x = false);

    ~BitSet();

    ref operator[](unsigned int i);

    const_ref operator[](unsigned int i) const;

    unsigned int size() const;

    unsigned int count() const;

    uint64_t max() const;

    void clear();

    bool operator==(const BitSet& rhs) const;

    BitSet& operator|=(const BitSet& rhs);

    BitSet& operator&=(const BitSet& rhs);

    BitSet operator~() const;

    struct sparse_iterator {
        const BitSet& s;
        unsigned int c;
        unsigned int next;

        sparse_iterator(const BitSet& _s, unsigned int n = 0);

        bool operator!=(const sparse_iterator&) const;

        unsigned int operator*() const;

        sparse_iterator& operator++();
    };

    sparse_iterator begin() const;

    sparse_iterator end() const;

    int nextSetBit(unsigned int fromIndex) const;
};

inline void swap(BitSet& lhs, BitSet& rhs) noexcept {
    lhs.swap(rhs);
}

namespace std {
template<>
class hash<BitSet> {
public:
    size_t operator()(const BitSet &s) const noexcept;
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
    delete[] p;
}

BitSet operator|(const BitSet& lhs, const BitSet& rhs);

BitSet operator&(const BitSet& lhs, const BitSet& rhs);

std::ostream& operator<<(std::ostream&, const BitSet&);

//inline BitSet::ref::ref(uint64_t* _p, unsigned int i) : p(_p), w(i >> 6), m(0x1ULL << i) {}
inline BitSet::ref::ref(uint64_t* _p, unsigned int i) : p(_p + (i >> 6)), m(0x1ULL << (i & 63)) {}

inline BitSet::ref::operator bool() const {
    return *p & m;
}

//inline BitSet::const_ref::const_ref(const uint64_t* _p, unsigned int i) : p(_p), w(i >> 6), m(0x1ULL << i) {}
inline BitSet::const_ref::const_ref(const uint64_t* _p, unsigned int i) : p(_p + (i >> 6)), m(0x1ULL << (i & 63)) {}

inline BitSet::const_ref::operator bool() const {
    return *p & m;
}

inline BitSet::ref& BitSet::ref::operator=(const bool x) {
    *p = (*p & ~m) | (-static_cast<uint64_t>(x) & m);
    return *this;
}

//inline BitSet::ref& BitSet::ref::operator= (const BitSet::ref& _x) {
//    bool x = _x;
//    //p[w] = (p[w] & ~m) | (-x & m);
//    *p = (*p & ~m) | (-x & m);
//    return *this;
//}

inline BitSet::sparse_iterator::sparse_iterator(const BitSet& _s, unsigned int n) : s(_s), c(n), next(n) {
    if (next < s.n)
        operator++();
}

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

inline void a_and_not_b(const BitSet& a, const BitSet& b, BitSet& c) {
    c = BitSetComplement(b);
    c &= a;
}
