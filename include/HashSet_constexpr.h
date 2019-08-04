#pragma once

#include "constexpr.h"
#include "hash_table.h"
#include <ostream>

template<typename T>
struct integer_hash {
    constexpr size_t operator()(T n) const { return static_cast<size_t>(n); }
};

template<int64_t Size, typename T = unsigned int>
struct HashSet {
    using internal_set = unordered_set<T,Size, integer_hash<T>, int16_t>;
    using value_type = T;
    struct ref {
        internal_set& s;
        T i;

        constexpr ref(internal_set& _p, T i);
        constexpr ref(ref&&) = default;

        constexpr operator bool() const {
            return s.find(i) != s.end();
        }

        constexpr ref& operator=(const bool x) {
            if (x)
                s.insert(i);
            else
                s.erase(i);
            return *this;
        }

        ref(const ref&) = delete;
        ref& operator=(const ref&) = delete;
        ref& operator=(ref&&) = delete;
    };

    struct const_ref {
        const internal_set& s;
        T i;

        constexpr const_ref(const internal_set& _p, T i);
        constexpr const_ref(const_ref&&) = default;

        constexpr operator bool() const {
            return s.find(i) != s.end();
        }

        const_ref(const const_ref&) = delete;
        const_ref& operator=(const const_ref&) = delete;
        const_ref& operator=(const_ref&&) = delete;
    };

    internal_set s;
    int64_t n = 0;

    constexpr HashSet() = default;

    constexpr HashSet(int64_t _n);
    constexpr HashSet(int64_t _n, const bool x);

    void INLINE resize(int64_t _n);

    //constexpr ref operator[](T i);

    //constexpr const_ref operator[](T i) const;

    constexpr bool INLINE get(T i) const;
    constexpr void INLINE set(T i);
    constexpr void INLINE clear(T i);

    int64_t INLINE size() const;

    constexpr bool INLINE empty() const;

    constexpr int64_t INLINE count() const;

    unsigned long long max() const;

    constexpr void INLINE clear();

    template<int64_t Size2>
    constexpr bool operator==(const HashSet<Size2,T>& rhs) const;

    template<int64_t Size2>
    constexpr HashSet& operator|=(const HashSet<Size2,T>& rhs);

    template<int64_t Size2>
    constexpr HashSet& operator&=(const HashSet<Size2,T>& rhs);

    constexpr HashSet operator~() const;

    constexpr typename internal_set::const_iterator INLINE begin() const;

    constexpr typename internal_set::const_iterator INLINE end() const;
};

namespace std {

template<int64_t Size, typename T>
class hash<HashSet<Size,T>> {
public:
    constexpr size_t operator()(const HashSet<Size,T>& s) const;
};

template<int64_t Size, typename T>
constexpr size_t hash<HashSet<Size,T>>::operator()(const HashSet<Size,T> &s) const {
    //cout << "hashing " << s << ": ";
    const uint64_t* const tab = ::hashfn_tab.data();
    unsigned long long h = 0xBB40E64DA205B064LL;
    for (auto v : s) {
        const unsigned char* k = reinterpret_cast<const unsigned char*>(&v);
        const unsigned char* const last = reinterpret_cast<const unsigned char*>(&v + 1);
        for (; k != last; ++k)
            h = (h * 7664345821815920749LL) ^ tab[*k];
    }
    //cout << h << std::endl;
    return h;
}

} // namespace std

template<int64_t Size, typename T>
constexpr HashSet<Size,T>::HashSet(int64_t _n) : n(_n) {
    //cout << "constructed HashSet(" << _n << "): " << *this << endl;
}

template<int64_t Size, typename T>
constexpr HashSet<Size,T>::HashSet(int64_t _n, const bool x) : n(_n) {
    if (x) {
        throw std::runtime_error("You don't want to construct a HashSet with all values in it ... you'd want to use a BitSet for this");
    }
    //cout << "constructed HashSet(" << _n << ", " << x << "): " << *this << endl;
}

//template<int64_t Size, typename T>
//constexpr typename HashSet<Size,T>::ref HashSet<Size,T>::operator[](T i) {
//    return typename HashSet<Size,T>::ref(s, i);
//}
//
//template<int64_t Size, typename T>
//constexpr typename HashSet<Size,T>::const_ref HashSet<Size,T>::operator[](T i) const {
//    return typename HashSet<Size,T>::const_ref(s, i);
//}

template<int64_t Size, typename T>
constexpr bool HashSet<Size,T>::get(T i) const {
    return s.find(i) != s.end();
}

template<int64_t Size, typename T>
constexpr void HashSet<Size,T>::set(T i) {
    s.insert(i);
}

template<int64_t Size, typename T>
constexpr void HashSet<Size,T>::clear(T i) {
    s.erase(i);
}

template<int64_t Size, typename T>
int64_t HashSet<Size,T>::size() const {
    return n;
}

template<int64_t Size, typename T>
constexpr bool HashSet<Size,T>::empty() const {
    return s.empty();
}

template<int64_t Size, typename T>
constexpr int64_t HashSet<Size,T>::count() const {
    return s.size();
}

template<int64_t Size, typename T>
constexpr void HashSet<Size,T>::clear() {
    s.clear();
}

template<int64_t Size1, int64_t Size2, typename T>
constexpr HashSet<Size1,T> operator|(const HashSet<Size1,T>& lhs, const HashSet<Size2,T>& rhs) {
    //cout << "operator|(" << lhs << ", " << rhs << ")" << endl;
    if (lhs.n != rhs.n) {
        throw std::runtime_error("dimensions don't match: " + std::to_string(lhs.n) + " != " + std::to_string(rhs.n));
    }

    HashSet<Size1,T> s(lhs);
    s |= rhs;

    return s;
}

template<int64_t Size1, int64_t Size2, typename T>
constexpr HashSet<Size1,T> operator&(const HashSet<Size1,T>& lhs, const HashSet<Size2,T>& rhs) {
    //cout << "operator&(" << lhs << ", " << rhs << ")" << endl;
    if (lhs.n != rhs.n) {
        throw std::runtime_error("dimensions don't match: " + std::to_string(lhs.n) + " != " + std::to_string(rhs.n));
    }

    HashSet<Size1,T> s(lhs);
    s &= rhs;

    return s;
}

template<int64_t Size, typename T>
template<int64_t Size2>
constexpr HashSet<Size,T>& HashSet<Size,T>::operator|=(const HashSet<Size2,T>& rhs) {
    //cout << *this << ".operator|(" << rhs << ")" << std::endl;
    if (n != rhs.n) {
        throw std::runtime_error("dimensions don't match: " + std::to_string(n) + " != " + std::to_string(rhs.n));
    }

    s.insert(rhs.s.begin(), rhs.s.end());
    return *this;
}

template<int64_t Size, typename T>
template<int64_t Size2>
constexpr HashSet<Size,T>& HashSet<Size,T>::operator&=(const HashSet<Size2,T>& rhs) {
    //cout << *this << ".operator&(" << rhs << ")" << std::endl;
    if (n != rhs.n) {
        throw std::runtime_error("dimensions don't match: " + std::to_string(n) + " != " + std::to_string(rhs.n));
    }

    for (auto i = s.begin(); i != s.end(); ++i)
        if (rhs.s.find(*i) == rhs.s.end())
            s.erase(i);
    return *this;
}

template<int64_t Size, typename T>
constexpr HashSet<Size,T> HashSet<Size,T>::operator~() const {
    //cout << *this << ".operator~()" << std::endl;
    HashSet<Size,T> _s(n);
    for (T i = 0; i < n; i++)
        if (s.find(i) == s.end())
            _s.s.insert(i);
    return _s;
}
template<int64_t Size, typename T>
template<int64_t Size2>
constexpr bool HashSet<Size,T>::operator==(const HashSet<Size2,T>& rhs) const {
    if (n != rhs.n)
        return false;

    return s == rhs.s;
}


template<int64_t Size, typename T>
std::ostream& operator<<(std::ostream&, const HashSet<Size,T>&);

template<int64_t Size, typename T>
constexpr HashSet<Size,T>::ref::ref(internal_set& _s, T _i) : s(_s), i(_i) {}

template<int64_t Size, typename T>
constexpr HashSet<Size,T>::const_ref::const_ref(const internal_set& _s, T _i) : s(_s), i(_i) {}

template<int64_t Size, typename T>
constexpr auto HashSet<Size,T>::begin() const -> typename internal_set::const_iterator {
    return s.values.data;
    //return s.begin();
}

template<int64_t Size, typename T>
constexpr auto HashSet<Size,T>::end() const -> typename internal_set::const_iterator {
    //return s.end();
    return s.stop_;
}

template<int64_t Size, typename T>
constexpr void a_and_not_b(const HashSet<Size,T>& a, const HashSet<Size,T>& b, HashSet<Size,T>& c) {
    c.clear();
    for (auto i : a) {
        //c[i] = !b[i];
        if (b.s.find(i) == b.s.end())
            c.s.insert(i);
    }
}

template<int64_t Size, typename T>
std::ostream& operator<<(std::ostream& s, const HashSet<Size,T>& v) {
    s << "(" << std::hex << &v << std::dec << ":" << v.n << ")";

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
