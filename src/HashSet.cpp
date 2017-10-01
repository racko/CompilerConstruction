#include "HashSet.h"

HashSet& HashSet::operator|=(const HashSet& rhs) {
    //cout << *this << ".operator|(" << rhs << ")" << endl;
    if (n != rhs.n) {
        cerr << "dimensions don't match: " << n << " != " << rhs.n << endl;
        throw exception();
    }

    s.insert(rhs.s.begin(), rhs.s.end());
    return *this;
}

HashSet& HashSet::operator&=(const HashSet& rhs) {
    //cout << *this << ".operator&(" << rhs << ")" << endl;
    if (n != rhs.n) {
        cerr << "dimensions don't match: " << n << " != " << rhs.n << endl;
        throw exception();
    }

    for (auto i = s.begin(); i != s.end(); ++i)
        if (rhs.s.count(*i) == 0)
            s.erase(i);
    return *this;
}

HashSet HashSet::operator~() const {
    //cout << *this << ".operator~()" << endl;
    HashSet _s(n);
    for (unsigned int i = 0; i < n; i++)
        if (s.count(i) == 0)
            _s.s.insert(i);
    return _s;
}

ostream& operator<<(ostream& s, const HashSet& v) {
    s << "(" << hex << &v << dec << ":" << v.n << ")";

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

unsigned long long std::hash<HashSet>::hashfn_tab[256];

std::hash<HashSet>::hash() {
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

size_t std::hash<HashSet>::operator()(const HashSet &s) const {
    //cout << "hashing " << s << ": ";
    unsigned long long h = 0xBB40E64DA205B064LL;
    for (auto it : s) {
        unsigned int j;
        char* k;
        for (j = 0, k = (char*)&(it); j < sizeof(it); j++, k++)
            h = (h * 7664345821815920749LL) ^ hashfn_tab[(unsigned char)(*k)];
    }
    //cout << h << endl;
    return h;
}

