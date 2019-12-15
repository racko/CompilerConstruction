#include "Regex/HashSet.h"

#include <algorithm> // for max_element
#include <array>     // for array
#include <ostream>   // for operator<<, basic_ostream, basic_ostream<>::__o...
#include <string>    // for operator+, to_string, allocator, char_traits

HashSet& HashSet::operator|=(const HashSet& rhs) {
    // cout << *this << ".operator|(" << rhs << ")" << std::endl;
    if (n != rhs.n) {
        throw std::runtime_error("dimensions don't match: " + std::to_string(n) + " != " + std::to_string(rhs.n));
    }

    s.insert(rhs.s.begin(), rhs.s.end());
    return *this;
}

HashSet& HashSet::operator&=(const HashSet& rhs) {
    // cout << *this << ".operator&(" << rhs << ")" << std::endl;
    if (n != rhs.n) {
        throw std::runtime_error("dimensions don't match: " + std::to_string(n) + " != " + std::to_string(rhs.n));
    }

    for (auto i = s.begin(); i != s.end(); ++i)
        if (rhs.s.count(*i) == 0)
            s.erase(i);
    return *this;
}

HashSet HashSet::operator~() const {
    // cout << *this << ".operator~()" << std::endl;
    HashSet _s(n);
    for (unsigned int i = 0; i < n; i++)
        if (s.count(i) == 0)
            _s.s.insert(i);
    return _s;
}

std::ostream& operator<<(std::ostream& s, const HashSet& v) {
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

namespace {
constexpr std::array<unsigned long long, 256> make_hashtable() {
    std::array<unsigned long long, 256> out{};
    unsigned long long h = 0x544B2FBACAAF1684LL;
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
} // namespace

// @FIXME: this one is not static or in the anonymous namespace so it can be used by other code ...
constexpr std::array<unsigned long long, 256> hashfn_tab = make_hashtable();

size_t std::hash<HashSet>::operator()(const HashSet& s) const {
    // cout << "hashing " << s << ": ";
    unsigned long long h = 0xBB40E64DA205B064LL;
    for (auto v : s) {
        const unsigned char* k = (const unsigned char*)&v;
        const unsigned char* const last = (const unsigned char*)(&v + 1);
        for (; k != last; ++k)
            h = (h * 7664345821815920749LL) ^ hashfn_tab[*k];
    }
    // cout << h << std::endl;
    return h;
}

void HashSet::clear() { s.clear(); }

void a_and_not_b(const HashSet& a, const HashSet& b, HashSet& c) {
    c.clear();
    for (auto i : a) {
        c[i] = !b[i];
    }
}

HashSet::ref& HashSet::ref::operator=(const bool x) {
    if (x)
        s.insert(i);
    else
        s.erase(i);
    return *this;
}

HashSet operator|(const HashSet& lhs, const HashSet& rhs) {
    // cout << "operator|(" << lhs << ", " << rhs << ")" << endl;
    if (lhs.n != rhs.n) {
        throw std::runtime_error("dimensions don't match: " + std::to_string(lhs.n) + " != " + std::to_string(rhs.n));
    }

    HashSet s(lhs);
    s |= rhs;

    return s;
}

HashSet operator&(const HashSet& lhs, const HashSet& rhs) {
    // cout << "operator&(" << lhs << ", " << rhs << ")" << endl;
    if (lhs.n != rhs.n) {
        throw std::runtime_error("dimensions don't match: " + std::to_string(lhs.n) + " != " + std::to_string(rhs.n));
    }

    HashSet s(lhs);
    s &= rhs;

    return s;
}

HashSet::ref::operator bool() const { return s.count(i) > 0; }

HashSet::const_ref::operator bool() const { return s.count(i) > 0; }

bool HashSet::operator==(const HashSet& rhs) const {
    if (n != rhs.n)
        return false;

    return s == rhs.s;
}

unsigned long long HashSet::max() const {
    if (count() == 0)
        throw std::runtime_error("HashSet::max(): empty");
    return *std::max_element(s.begin(), s.end());
}

void HashSet::resize(unsigned int _n) {
    if (n > _n) {
        for (auto it = s.begin(); it != s.end();) {
            if (*it >= _n) {
                it = s.erase(it);
            } else
                ++it;
        }
    }
    n = _n;
}

HashSet::~HashSet() = default;

