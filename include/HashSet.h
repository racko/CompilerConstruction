#pragma once

#include <cstddef>       // for size_t
#include <functional>     // for hash
#include <iosfwd>         // for ostream, size_t
#include <stdexcept>      // for runtime_error
#include <unordered_set>  // for unordered_set, unordered_set<>::const_iterator

struct HashSet {
    struct ref {
        std::unordered_set<unsigned int>& s;
        unsigned int i;

        ref(std::unordered_set<unsigned int>& _p, unsigned int i);
        ref(ref&&) = default;

        operator bool() const;

        ref& operator=(const bool x);

        ref(const ref&) = delete;
        ref& operator=(const ref&) = delete;
        ref& operator=(ref&&) = delete;
    };

    struct const_ref {
        const std::unordered_set<unsigned int>& s;
        unsigned int i;

        const_ref(const std::unordered_set<unsigned int>& _p, unsigned int i);
        const_ref(const_ref&&) = default;

        operator bool() const;

        const_ref(const const_ref&) = delete;
        const_ref& operator=(const const_ref&) = delete;
        const_ref& operator=(const_ref&&) = delete;
    };

    std::unordered_set<unsigned int> s;
    unsigned int n = 0;

    HashSet() = default;
    ~HashSet(); // ignore rule of five here. It's defaulted in the cpp. I only want the compiler to not generate the unordered_set destructor everywhere ...

    HashSet(unsigned int _n);
    HashSet(unsigned int _n, const bool x);

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

    std::unordered_set<unsigned int>::const_iterator begin() const;

    std::unordered_set<unsigned int>::const_iterator end() const;
};

namespace std {
template<>
class hash<HashSet> {
    public:
    size_t operator()(const HashSet &s) const;
};
}

inline HashSet::HashSet(unsigned int _n) : n(_n) {
    //cout << "constructed HashSet(" << _n << "): " << *this << endl;
}

inline HashSet::HashSet(unsigned int _n, const bool x) : n(_n) {
    if (x) {
        throw std::runtime_error("You don't want to construct a HashSet with all values in it ... you'd want to use a BitSet for this");
    }
    //cout << "constructed HashSet(" << _n << ", " << x << "): " << *this << endl;
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
}

HashSet operator|(const HashSet& lhs, const HashSet& rhs);

HashSet operator&(const HashSet& lhs, const HashSet& rhs);

std::ostream& operator<<(std::ostream&, const HashSet&);

inline HashSet::ref::ref(std::unordered_set<unsigned int>& _s, unsigned int _i) : s(_s), i(_i) {}
inline HashSet::const_ref::const_ref(const std::unordered_set<unsigned int>& _s, unsigned int _i) : s(_s), i(_i) {}

inline std::unordered_set<unsigned int>::const_iterator HashSet::begin() const {
    return s.begin();
}

inline std::unordered_set<unsigned int>::const_iterator HashSet::end() const {
    return s.end();
}

void a_and_not_b(const HashSet& a, const HashSet& b, HashSet& c);
