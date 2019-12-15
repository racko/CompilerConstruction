#pragma once

#include "Regex/constexpr.h"
#include "Regex/hash_table.h"

//#include <iosfwd> // for std::ostream&
#include <functional> // for std::hash

template<int64_t max_n>
struct BitSetComplement;

struct ref {
    uint64_t *p;
    uint64_t m;

    constexpr ref(uint64_t* _p, int64_t i);
    constexpr ref(ref&& _x) = default;

    constexpr operator bool() const;

    constexpr ref& operator=(const bool x);

    constexpr ref(const ref& _x) = delete;
    constexpr ref& operator=(const ref& _x) = delete;
    constexpr ref& operator=(ref&& _x) = delete;
};

struct const_ref {
    const uint64_t *p;
    uint64_t m;

    constexpr const_ref(const uint64_t* _p, int64_t i);
    constexpr const_ref(const_ref&&) = default;

    constexpr operator bool() const;

    constexpr const_ref(const const_ref&) = delete;
    constexpr const_ref& operator=(const const_ref&) = delete;
    constexpr const_ref& operator=(const_ref&&) = delete;
};

template<int64_t max_n>
struct BitSet {
    using value_type = int64_t;
    int64_t n = 0;
    uint64_t p[(max_n+63)>>6]{};

    constexpr BitSet() = default;
    constexpr BitSet(int64_t _n);
    constexpr BitSet(int64_t _n, const bool x);

    template<int64_t w2>
    constexpr BitSet& operator=(const BitSetComplement<w2>& s);

    constexpr void resize(int64_t _n, bool x = false);

    //constexpr ref operator[](int64_t i);

    //constexpr const_ref operator[](int64_t i) const;

    constexpr bool INLINE get(int64_t i) const;
    constexpr void INLINE set(int64_t i);
    constexpr void INLINE clear(int64_t i);

    constexpr int64_t INLINE wordsInUse() const;

    constexpr int64_t INLINE size() const;

    constexpr int64_t count() const;

    constexpr void clear();

    template<int64_t w2>
    constexpr bool operator==(const BitSet<w2>& rhs) const;

    template<int64_t w2>
    constexpr BitSet& operator|=(const BitSet<w2>& rhs);

    template<int64_t w2>
    constexpr BitSet& operator&=(const BitSet<w2>& rhs);

    template<int64_t w2>
    constexpr BitSet& operator-=(const BitSet<w2>& rhs);

    constexpr BitSet operator~() const;

    struct sparse_iterator {
        const BitSet& s;
        int64_t c;

        constexpr INLINE sparse_iterator(const BitSet& _s, int64_t n = 0);

        constexpr bool INLINE operator!=(const sparse_iterator&) const;

        constexpr int64_t INLINE operator*() const;

        constexpr sparse_iterator& operator++();
        constexpr sparse_iterator INLINE operator++(int) {
            sparse_iterator tmp(*this);
            ++(*this);
            return tmp;
        }
    };

    constexpr sparse_iterator INLINE begin() const;

    constexpr sparse_iterator INLINE end() const;

    constexpr int64_t nextSetBit(int64_t fromIndex) const;
};

template<int64_t w1, int64_t w2>
constexpr void INLINE swap(BitSet<w1>& lhs, BitSet<w2>& rhs) noexcept {
    lhs.swap(rhs);
}

namespace std {
template<int64_t max_n>
class hash<BitSet<max_n>> {
public:
    constexpr size_t operator()(const BitSet<max_n> &s) const noexcept;
};
}

//template<int64_t max_n>
//constexpr ref BitSet<max_n>::operator[](int64_t i) {
//    return ref(p, i);
//}
//
//template<int64_t max_n>
//constexpr const_ref BitSet<max_n>::operator[](int64_t i) const {
//    return const_ref(p, i);
//}

template<int64_t max_n>
constexpr bool BitSet<max_n>::get(int64_t i) const {
    return *(p + (i >> 6)) & (0x1ULL << (i & 63));
}

template<int64_t max_n>
constexpr void BitSet<max_n>::set(int64_t i) {
    *(p + (i >> 6)) |= (0x1ULL << (i & 63));
}

template<int64_t max_n>
constexpr void BitSet<max_n>::clear(int64_t i) {
    *(p + (i >> 6)) &= ~(0x1ULL << (i & 63));
}

template<int64_t max_n>
constexpr int64_t BitSet<max_n>::wordsInUse() const {
    return (n + 63) >> 6;
}

template<int64_t max_n>
constexpr int64_t BitSet<max_n>::size() const {
    return n;
}

template<int64_t w1, int64_t w2>
constexpr BitSet<std::min(w1,w2)> INLINE operator|(const BitSet<w1>& lhs, const BitSet<w2>& rhs);

template<int64_t w1, int64_t w2>
constexpr BitSet<std::min(w1,w2)> INLINE operator&(const BitSet<w1>& lhs, const BitSet<w2>& rhs);

template<int64_t w1, int64_t w2>
constexpr BitSet<std::min(w1,w2)> INLINE operator-(const BitSet<w1>& lhs, const BitSet<w2>& rhs);

template<int64_t max_n>
std::ostream& operator<<(std::ostream&, const BitSet<max_n>&);

constexpr ref::ref(uint64_t* _p, int64_t i) : p(_p + (i >> 6)), m(0x1ULL << (i & 63)) {}

constexpr ref::operator bool() const {
    return *p & m;
}

constexpr const_ref::const_ref(const uint64_t* _p, int64_t i) : p(_p + (i >> 6)), m(0x1ULL << (i & 63)) {}

constexpr const_ref::operator bool() const {
    return *p & m;
}

constexpr ref& ref::operator=(const bool x) {
    *p = (*p & ~m) | (-static_cast<uint64_t>(x) & m);
    return *this;
}

template<int64_t max_n>
constexpr BitSet<max_n>::sparse_iterator::sparse_iterator(const BitSet<max_n>& _s, int64_t n) : s(_s), c(n - 1) {
    if (c + 1 < s.n)
        operator++();
}

template<int64_t max_n>
constexpr bool BitSet<max_n>::sparse_iterator::operator!=(const BitSet<max_n>::sparse_iterator& it) const {
    //cout << "c != it.c: " << c << " != " << it.c << endl;
    return c != it.c;
}

template<int64_t max_n>
constexpr int64_t BitSet<max_n>::sparse_iterator::operator*() const {
    return c;
}

template<int64_t max_n>
struct BitSetComplement {
    const BitSet<max_n>& s;
    constexpr INLINE BitSetComplement(const BitSet<max_n>&_s) : s(_s) {}
};

template<int64_t max_n>
constexpr typename BitSet<max_n>::sparse_iterator BitSet<max_n>::begin() const {
    return typename BitSet<max_n>::sparse_iterator(*this);
}

template<int64_t max_n>
constexpr typename BitSet<max_n>::sparse_iterator BitSet<max_n>::end() const {
    return typename BitSet<max_n>::sparse_iterator(*this, n + 1);
}

template<int64_t w1, int64_t w2, int64_t w3>
constexpr void INLINE a_and_not_b(const BitSet<w1>& a, const BitSet<w2>& b, BitSet<w3>& c) {
    c = BitSetComplement<w2>(b);
    c &= a;
}

// cpp

//#include <iomanip>
//#include <ostream>

template<int64_t max_n>
constexpr BitSet<max_n>::BitSet(int64_t _n) : n(_n) {}

template<int64_t max_n>
constexpr BitSet<max_n>::BitSet(int64_t _n, const bool x) : n(_n) {
    const_expr::fill_n(p, wordsInUse(), x ? -1ULL : 0ULL);
}

template<int64_t max_n>
template<int64_t w2>
constexpr BitSet<max_n>& BitSet<max_n>::operator=(const BitSetComplement<w2>& rhs) {
    //std::cout << "copying BitSet(" << s << "): ";
    if (n != rhs.s.n) {
        throw std::runtime_error("dimensions don't match: " + std::to_string(n) + " != " + std::to_string(rhs.s.n));
    }

    auto out = p;
    const auto stop = out + wordsInUse();
    auto in = rhs.s.p;
    for (; out != stop; ++out, ++in)
        *out = ~(*in);
    //std::cout << *this << endl;
    return *this;
}

template<int64_t max_n>
constexpr void BitSet<max_n>::resize(int64_t _n, const bool x) {
    auto newWordsInUse = (_n + 63) >> 6;
    if (_n <= n) {
        n = _n;
    } else if (newWordsInUse <= ((max_n+63)>>6)) {
        // _n > n, but number of words remains the same
        // so we need to set the new bits in the last word to x
        auto bits = p + (n >> 6);
        auto number_of_new_bits = _n - n;
        auto mask = ((1ULL << number_of_new_bits) - 1) << n;
        *bits = (*bits & ~mask) | (-static_cast<uint64_t>(x) & mask);

        n = _n;
    } else {
        throw std::runtime_error("BitSet::resize: fixed sized allocator");
    }
}

template<int64_t max_n>
constexpr int64_t BitSet<max_n>::count() const {
    if (wordsInUse() == 0)
        return 0;
    int64_t c = 0;
    //for (auto i = begin(); i != end(); ++i)
    //    c++;
    for (auto i = 0U; i < wordsInUse() - 1; ++i) {
        uint64_t v = p[i];
        for (; v; ++c) {
            v &= v-1;
        }
    }
    const auto handled_bits = (n >> 6) << 6;
    const auto remaining_bits = n - handled_bits;

    if (remaining_bits == 0)
        return c;

    uint64_t lastWord = p[wordsInUse() - 1] & ~(-1ULL << remaining_bits);
    for (; lastWord; ++c) {
        lastWord &= lastWord - 1;
    }
    return c;
}

template<int64_t max_n>
constexpr void BitSet<max_n>::clear() {
    const_expr::fill_n(p, wordsInUse(), 0ULL);
}

template<int64_t max_n>
template<int64_t w2>
constexpr bool BitSet<max_n>::operator==(const BitSet<w2>& rhs) const {
    if (n != rhs.n)
        return false;

    if (n == 0)
        return true;
    int64_t u = n >> 6;
    for (auto i = 0; i < u; i++)
        if (p[i] != rhs.p[i])
            return false;
    //auto mask = -1ULL >> -(n + 1);
    auto mask = -1ULL >> (64 - (n & 63));
    return (p[u] & mask) == (rhs.p[u] & mask);
}

template<int64_t max_n>
template<int64_t w2>
constexpr BitSet<max_n>& BitSet<max_n>::operator|=(const BitSet<w2>& rhs) {
    //std::cout << *this << ".operator|(" << rhs << ")" << endl;
    if (n != rhs.n) {
        throw std::runtime_error("dimensions don't match: " + std::to_string(n) + " != " + std::to_string(rhs.n));
    }

    auto out = p;
    const auto stop = out + wordsInUse();
    auto in = rhs.p;
    for (; out != stop; ++out, ++in)
        *out |= *in;
    return *this;
}

template<int64_t max_n>
template<int64_t w2>
constexpr BitSet<max_n>& BitSet<max_n>::operator&=(const BitSet<w2>& rhs) {
    //std::cout << *this << ".operator&(" << rhs << ")" << endl;
    if (n != rhs.n) {
        throw std::runtime_error("dimensions don't match: " + std::to_string(n) + " != " + std::to_string(rhs.n));
    }

    auto out = p;
    const auto stop = out + wordsInUse();
    auto in = rhs.p;
    for (; out != stop; ++out, ++in)
        *out &= *in;
    return *this;
}

template<int64_t max_n>
template<int64_t w2>
constexpr BitSet<max_n>& BitSet<max_n>::operator-=(const BitSet<w2>& rhs) {
    //std::cout << *this << ".operator-(" << rhs << ")" << endl;
    if (n != rhs.n) {
        throw std::runtime_error("dimensions don't match: " + std::to_string(n) + " != " + std::to_string(rhs.n));
    }

    auto out = p;
    const auto stop = out + wordsInUse();
    auto in = rhs.p;
    for (; out != stop; ++out, ++in)
        *out &= ~(*in);
    return *this;
}

template<int64_t max_n>
constexpr BitSet<max_n> BitSet<max_n>::operator~() const {
    //std::cout << *this << ".operator~()" << endl;
    BitSet<max_n> s(n);
    for (auto i = 0u; i < wordsInUse(); i++)
        s.p[i] = ~p[i];
    return s;
}

template<int64_t max_n>
constexpr int64_t BitSet<max_n>::nextSetBit(int64_t fromIndex) const {
    while (fromIndex < n && !get(fromIndex))
        fromIndex++;
    return fromIndex;
}

//template<int64_t max_n>
//constexpr size_t std::hash<BitSet<max_n>>::operator()(const BitSet<max_n> &s) const noexcept {
//    //std::cout << "hashing " << s << ": ";
//    
//    auto tab = ::hashfn_tab.data();
//
//    uint64_t h = 0xBB40E64DA205B064ULL;
//    const unsigned char* k = (unsigned char*)s.p;
//    const unsigned char* const last = (unsigned char*)(s.p + (s.n >> 6));
//    for (; k != last; ++k)
//        h = (h * 7664345821815920749ULL) ^ tab[*k];
//
//    const auto remaining_bits = s.n & 63;
//
//    if (remaining_bits == 0)
//        return h;
//
//    uint64_t lastWord = *(s.p + (s.n >> 6)) & ~(-1ULL << remaining_bits);
//    k = (unsigned char*)&lastWord;
//    const unsigned char* last2 = (unsigned char*)(&lastWord + 1);
//    for (; k != last2; ++k)
//        h = (h * 7664345821815920749ULL) ^ tab[*k];
//    //std::cout << h << endl;
//    return h;
//}

template<int64_t max_n>
constexpr size_t std::hash<BitSet<max_n>>::operator()(const BitSet<max_n> &s) const noexcept {
    //std::cout << "hashing " << s << ": ";
    
    auto tab = ::hashfn_tab.data();

    uint64_t h = 0xBB40E64DA205B064ULL;
    const auto* w = s.p;
    const auto* const last = s.p + (s.n >> 6);
    for (; w != last; ++w) {
        h = (h * 7664345821815920749ULL) ^ tab[*w & 0xff];
        h = (h * 7664345821815920749ULL) ^ tab[(*w >> 8) & 0xff];
        h = (h * 7664345821815920749ULL) ^ tab[(*w >> 16) & 0xff];
        h = (h * 7664345821815920749ULL) ^ tab[(*w >> 24) & 0xff];

        h = (h * 7664345821815920749ULL) ^ tab[(*w >> 32) & 0xff];
        h = (h * 7664345821815920749ULL) ^ tab[(*w >> 40) & 0xff];
        h = (h * 7664345821815920749ULL) ^ tab[(*w >> 48) & 0xff];
        h = (h * 7664345821815920749ULL) ^ tab[(*w >> 56) & 0xff];
    }

    const auto remaining_bits = s.n & 63;

    if (remaining_bits == 0)
        return h;

    const uint64_t lastWord = *(s.p + (s.n >> 6)) & ~(-1ULL << remaining_bits);

    h = (h * 7664345821815920749ULL) ^ tab[lastWord & 0xff];
    h = (h * 7664345821815920749ULL) ^ tab[(lastWord >> 8) & 0xff];
    h = (h * 7664345821815920749ULL) ^ tab[(lastWord >> 16) & 0xff];
    h = (h * 7664345821815920749ULL) ^ tab[(lastWord >> 24) & 0xff];

    h = (h * 7664345821815920749ULL) ^ tab[(lastWord >> 32) & 0xff];
    h = (h * 7664345821815920749ULL) ^ tab[(lastWord >> 40) & 0xff];
    h = (h * 7664345821815920749ULL) ^ tab[(lastWord >> 48) & 0xff];
    h = (h * 7664345821815920749ULL) ^ tab[(lastWord >> 56) & 0xff];

    //std::cout << h << endl;
    return h;
}

template<int64_t w1, int64_t w2>
constexpr auto operator|(const BitSet<w1>& lhs, const BitSet<w2>& rhs) {
    //std::cout << "operator|(" << lhs << ", " << rhs << ")" << endl;
    if (lhs.n != rhs.n) {
        throw std::runtime_error("dimensions don't match: " + std::to_string(lhs.n) + " != " + std::to_string(rhs.n));
    }

    BitSet<std::min(w1,w2)> s(w1 <= w2 ? lhs : rhs);
    s |= (w1 <= w2 ? rhs : lhs);

    return s;
}

template<int64_t w1, int64_t w2>
constexpr auto operator&(const BitSet<w1>& lhs, const BitSet<w2>& rhs) {
    //std::cout << "operator&(" << lhs << ", " << rhs << ")" << endl;
    if (lhs.n != rhs.n) {
        throw std::runtime_error("dimensions don't match: " + std::to_string(lhs.n) + " != " + std::to_string(rhs.n));
    }

    BitSet<std::min(w1,w2)> s(w1 <= w2 ? lhs : rhs);
    s &= (w1 <= w2 ? rhs : lhs);

    return s;
}

template<int64_t w1, int64_t w2>
constexpr auto operator-(const BitSet<w1>& lhs, const BitSet<w2>& rhs) {
    //std::cout << "operator-(" << lhs << ", " << rhs << ")" << endl;
    if (lhs.n != rhs.n) {
        throw std::runtime_error("dimensions don't match: " + std::to_string(lhs.n) + " != " + std::to_string(rhs.n));
    }

    auto s{lhs};
    s -= rhs;

    return s;
}

//template<int64_t max_n>
//std::ostream& operator<<(std::ostream& s, const BitSet<max_n>& v) {
//    s << "(" << std::hex << &v << std::dec << ":" << v.n << ")";
//    int64_t i = 0;
//
//    while (i < v.size() && !v.get(i))
//        i++;
//
//    if (i == v.size()) {
//        s << "{}";
//        return s;
//    }
//    s << "{ " << i;
//    for (i++; i < v.size(); i++)
//        if (v.get(i))
//            s << ", " << i;
//    s << " }";
//    return s;
//}

//template<int64_t max_n>
//constexpr typename BitSet<max_n>::sparse_iterator& BitSet<max_n>::sparse_iterator::operator++() {
//    const auto p_ = s.p;
//    const auto next = c + 1;
//    auto pp = p_ + (next >> 6);
//    if (const auto i = *pp & (-1ULL << (next & 63)); i != 0) {
//        c = ((pp - p_) << 6) + count_trailing_zeros(i);
//        return *this;
//    }
//    ++pp;
//    const auto stop = p_ + s.wordsInUse();
//
//    for (; pp != stop; ++pp) {
//        if (const auto i = *pp; i != 0) {
//            c = ((pp - p_) << 6) + count_trailing_zeros(i);
//            return *this;
//        }
//    }
//    c = s.n;
//    return *this;
//}
template<int64_t max_n>
constexpr typename BitSet<max_n>::sparse_iterator& BitSet<max_n>::sparse_iterator::operator++() {
    if (const auto i = *(s.p + ((c + 1) >> 6)) & (-1ULL << ((c + 1) & 63)); i != 0) {
        //c = ((c + 1) & ~63) + count_trailing_zeros(i);
        c = ((c + 1) & ~63) + const_expr::Mod67BitPosition[(-i & i) % 67];
        return *this;
    }
    auto pp = s.p + ((c + 1) >> 6) + 1;
    const auto stop = s.p + s.wordsInUse();

    for (; pp != stop; ++pp) {
        if (const auto i = *pp; i != 0) {
            //c = ((pp - s.p) << 6) + count_trailing_zeros(i);
            c = ((pp - s.p) << 6) + const_expr::Mod67BitPosition[(-i & i) % 67];
            return *this;
        }
    }
    c = s.n;
    return *this;
}
