#include "Regex/BitSet.h"

#include <algorithm> // for copy_n, fill, copy, fill_n, max
#include <array>     // for array
#include <cstddef>   // for size_t
#include <ostream>   // for operator<<, basic_ostream, basic_ostream<>::__o...
#include <stdexcept> // for runtime_error
#include <string>    // for operator+, to_string, allocator, char_traits
#include <utility>   // for swap

BitSet::BitSet(unsigned int _n) : n(_n), w((n + 63) >> 6), wordsInUse(w), p(new uint64_t[w]) {}

BitSet::BitSet(unsigned int _n, const bool x) : n(_n), w((n + 63) >> 6), wordsInUse(w), p(new uint64_t[w]) {
    std::fill_n(p, w, x ? -1ULL : 0ULL);
}

BitSet::BitSet(BitSet&& s) noexcept : n(s.n), w(s.w), wordsInUse(s.wordsInUse), p(s.p) {
    s.n = 0;
    s.w = 0;
    s.wordsInUse = 0;
    s.p = nullptr;
}

BitSet::BitSet(const BitSet& s) : n(s.n), w(s.w), wordsInUse(s.wordsInUse), p(new uint64_t[s.wordsInUse]) {
    std::copy_n(s.p, s.wordsInUse, p);
}

BitSet& BitSet::operator=(const BitSet& s) {
    if (s.wordsInUse > w) {
        BitSet tmp(s);
        swap(tmp);
    } else {
        n = s.n;
        wordsInUse = s.wordsInUse;
        std::copy_n(s.p, s.wordsInUse, p);
    }
    return *this;
}

BitSet& BitSet::operator=(BitSet&& s) noexcept {
    delete[] p;

    n = s.n;
    w = s.w;
    wordsInUse = s.wordsInUse;
    p = s.p;

    s.n = 0;
    s.w = 0;
    s.wordsInUse = 0;
    s.p = nullptr;

    return *this;
}

void BitSet::swap(BitSet& rhs) noexcept {
    using std::swap;
    swap(n, rhs.n);
    swap(w, rhs.w);
    swap(wordsInUse, rhs.wordsInUse);
    swap(p, rhs.p);
}

BitSet& BitSet::operator=(const BitSetComplement& rhs) {
    // std::cout << "copying BitSet(" << s << "): ";
    if (n != rhs.s.n) {
        throw std::runtime_error("dimensions don't match: " + std::to_string(n) + " != " + std::to_string(rhs.s.n));
    }

    for (auto i = 0u; i < wordsInUse; i++)
        p[i] = ~rhs.s.p[i];
    // std::cout << *this << endl;
    return *this;
}

void BitSet::resize(unsigned int _n, const bool x) {
    auto newWordsInUse = (_n + 63) >> 6;
    if (_n <= n) {
        n = _n;
        wordsInUse = newWordsInUse;
    } else if (newWordsInUse <= w) {
        // _n > n, but number of words remains the same
        // so we need to set the new bits in the last word to x
        auto bits = p + (n >> 6);
        auto number_of_new_bits = _n - n;
        auto mask = ((1ULL << number_of_new_bits) - 1) << n;
        *bits = (*bits & ~mask) | (-static_cast<uint64_t>(x) & mask);

        n = _n;
        wordsInUse = newWordsInUse;
    } else {
        // _n > n && newWordsInUse > w
        unsigned int _w = std::max(newWordsInUse, w << 1);
        uint64_t* _p = new uint64_t[_w];

        // "Critical Line": only the above can throw, so below we can safely modify *this and have strong exceptions
        // safety

        std::copy(p, p + wordsInUse, _p);
        delete[] p;

        auto bits = _p + (n >> 6);
        auto number_of_new_bits = 64 - n;
        auto mask = ((1ULL << number_of_new_bits) - 1) << n;
        *bits = (*bits & ~mask) | (-static_cast<uint64_t>(x) & mask);

        std::fill(_p + wordsInUse, _p + _w, x ? -1ULL : 0ULL);

        n = _n;
        w = _w;
        wordsInUse = newWordsInUse;
        p = _p;
    }
}

unsigned int BitSet::count() const {
    if (wordsInUse == 0)
        return 0;
    unsigned int c = 0;
    // for (auto i = begin(); i != end(); ++i)
    //    c++;
    for (auto i = 0U; i < wordsInUse - 1; ++i) {
        uint64_t v = p[i];
        for (; v; ++c) {
            v &= v - 1;
        }
    }
    const auto handled_bits = (n >> 6) << 6;
    const auto remaining_bits = n - handled_bits;

    if (remaining_bits == 0)
        return c;

    uint64_t lastWord = p[wordsInUse - 1] & ~(-1ULL << remaining_bits);
    for (; lastWord; ++c) {
        lastWord &= lastWord - 1;
    }
    return c;
}

uint64_t BitSet::max() const {
    throw std::runtime_error("not implemented yet");
    // if (p[w-1] & (0xffffffffffffffffULL << n));
    // uint64_t* word = p + w - 1;
    // while (word == 0)
    //  word--;
    // return 0;
}

void BitSet::clear() { std::fill(p, p + wordsInUse, 0ULL); }

bool BitSet::operator==(const BitSet& rhs) const {
    if (n != rhs.n)
        return false;

    if (n == 0)
        return true;
    unsigned int u = n >> 6;
    for (auto i = 0u; i < u; i++)
        if (p[i] != rhs.p[i])
            return false;
    auto mask = -1ULL >> -(n + 1);
    return (p[u] & mask) == (rhs.p[u] & mask);
}

BitSet& BitSet::operator|=(const BitSet& rhs) {
    // std::cout << *this << ".operator|(" << rhs << ")" << endl;
    if (n != rhs.n) {
        throw std::runtime_error("dimensions don't match: " + std::to_string(n) + " != " + std::to_string(rhs.n));
    }

    for (auto i = 0u; i < wordsInUse; i++)
        p[i] |= rhs.p[i];
    return *this;
}

BitSet& BitSet::operator&=(const BitSet& rhs) {
    // std::cout << *this << ".operator&(" << rhs << ")" << endl;
    if (n != rhs.n) {
        throw std::runtime_error("dimensions don't match: " + std::to_string(n) + " != " + std::to_string(rhs.n));
    }

    for (auto i = 0u; i < wordsInUse; i++)
        p[i] &= rhs.p[i];
    return *this;
}

BitSet BitSet::operator~() const {
    // std::cout << *this << ".operator~()" << endl;
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

namespace {
constexpr std::array<uint64_t, 256> make_hashtable() {
    std::array<uint64_t, 256> out{};
    uint64_t h = 0x544B2FBACAAF1684ULL;
    for (auto j = 0U; j < 256U; j++) {
        for (auto i = 0U; i < 31U; i++) {
            h = (h >> 7) ^ h;
            h = (h << 11) ^ h;
            h = (h >> 10) ^ h;
        }
        out[j] = h;
    }
    return out;
}

constexpr std::array<uint64_t, 256> hashfn_tab = make_hashtable();
} // namespace

size_t std::hash<BitSet>::operator()(const BitSet& s) const noexcept {
    // std::cout << "hashing " << s << ": ";
    uint64_t h = 0xBB40E64DA205B064ULL;
    const unsigned char* k = (unsigned char*)s.p;
    const unsigned char* const last = (unsigned char*)(s.p + (s.n >> 6));
    for (; k != last; ++k)
        h = (h * 7664345821815920749ULL) ^ hashfn_tab[*k];

    const auto handled_bits = (s.n >> 6) << 6;
    const auto remaining_bits = s.n - handled_bits;

    if (remaining_bits == 0)
        return h;

    uint64_t lastWord = *(s.p + (s.n >> 6)) & ~(-1ULL << remaining_bits);
    k = (unsigned char*)&lastWord;
    const unsigned char* last2 = (unsigned char*)(&lastWord + 1);
    for (; k != last2; ++k)
        h = (h * 7664345821815920749ULL) ^ hashfn_tab[*k];
    // std::cout << h << endl;
    return h;
}

BitSet operator|(const BitSet& lhs, const BitSet& rhs) {
    // std::cout << "operator|(" << lhs << ", " << rhs << ")" << endl;
    if (lhs.n != rhs.n) {
        throw std::runtime_error("dimensions don't match: " + std::to_string(lhs.n) + " != " + std::to_string(rhs.n));
    }

    BitSet s(lhs);
    s |= rhs;

    return s;
}

BitSet operator&(const BitSet& lhs, const BitSet& rhs) {
    // std::cout << "operator&(" << lhs << ", " << rhs << ")" << endl;
    if (lhs.n != rhs.n) {
        throw std::runtime_error("dimensions don't match: " + std::to_string(lhs.n) + " != " + std::to_string(rhs.n));
    }

    BitSet s(lhs);
    s &= rhs;

    return s;
}

std::ostream& operator<<(std::ostream& s, const BitSet& v) {
    s << "(" << std::hex << &v << std::dec << ":" << v.n << ")";
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

BitSet::sparse_iterator& BitSet::sparse_iterator::operator++() {
    unsigned int u = next >> 6;
    auto i = s.p[u] & (-1ULL << next);

    while (true) {
        // cout << "i = " << bin(i) << endl;
        if (i != 0) {
            auto _c = 0U;
            if (!(i & 0x1)) {
                if ((i & 0xffffffff) == 0) {
                    i >>= 32;
                    _c += 32;
                }
                if ((i & 0xffff) == 0) {
                    i >>= 16;
                    _c += 16;
                }
                if ((i & 0xff) == 0) {
                    i >>= 8;
                    _c += 8;
                }
                if ((i & 0xf) == 0) {
                    i >>= 4;
                    _c += 4;
                }
                if ((i & 0x3) == 0) {
                    i >>= 2;
                    _c += 2;
                }
                if ((i & 0x1) == 0) {
                    i >>= 1;
                    _c += 1;
                }
            }
            //      int x, y;
            //      int n = 63;
            //      y = (int)i; if (y != 0) { n = n -32; x = y; } else x = (int)(i>>32);
            //      y = x <<16; if (y != 0) { n = n -16; x = y; }
            //      y = x << 8; if (y != 0) { n = n - 8; x = y; }
            //      y = x << 4; if (y != 0) { n = n - 4; x = y; }
            //      y = x << 2; if (y != 0) { n = n - 2; x = y; }
            //      int _c = n - ((x << 1) >> 31);
            // cout << "trailing zeros: " << _c << endl;
            c = (u << 6) + _c;
            // cout << "c: " << c << endl;
            next = c + 1;
            // cout << "next: " << next << endl;
            return *this;
        }
        if (++u == s.wordsInUse) {
            next = s.n + 1;
            // cout << "reached end" << endl;
            return *this;
        }
        i = s.p[u];
        c = u << 6;
    }
    ///////////////////////////////////////////////////////////////////////////////////////
    //  cout << "operator++" << endl;
    //  cout << *s << endl;
    //  cout << "i = " << i << endl;
    //  cout << "_c = " << _c << endl;
    //
    //  while (_c < s->n && s->p[i] == 0) {
    //    cout << "p[" << i << "] == 0" << endl;
    //    i++;
    //    _c = i * 64;
    //    cout << *s << endl;
    //    cout << "i = " << i << endl;
    //    cout << "_c = " << _c << endl;
    //  }
    //
    //  if (_c >= s->n) {
    //    cout << "_c >= n: " << _c << " >= " << s->n << endl;
    //    return *this;
    //  }
    //
    //  int c = 0;
    //  if (!(s->p[i] & 0x1)) {
    //    if ((s->p[i] & 0xffffffff) == 0) {
    //      s->p[i] >>= 32;
    //      c += 32;
    //    }
    //    if ((s->p[i] & 0xffff) == 0) {
    //      s->p[i] >>= 16;
    //      c += 16;
    //    }
    //    if ((s->p[i] & 0xff) == 0) {
    //      s->p[i] >>= 8;
    //      c += 8;
    //    }
    //    if ((s->p[i] & 0xf) == 0) {
    //      s->p[i] >>= 4;
    //      c += 4;
    //    }
    //    if ((s->p[i] & 0x3) == 0) {
    //      s->p[i] >>= 2;
    //      c += 2;
    //    }
    //    if ((s->p[i] & 0x1) == 0) {
    //      s->p[i] >>= 1;
    //      c += 1;
    //    }
    //  }
    //  cout << c << " clear bits." << endl;
    //  _c += c;
    //  cout << "_c = " << _c << endl;
    //  cout << *s << endl;
    //  cout << "i = " << i << endl;
    //  s->p[i] &= ~0x1ULL;
    //  cout << "cleared lsb" << endl;
    //  cout << *s << endl;
    //  cout << "i = " << i << endl;
    //  return *this;
}
