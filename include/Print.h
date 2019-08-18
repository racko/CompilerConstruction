#pragma once

#include <ostream> // for ostream, operator<<, basic_ostream::operator<<
#include <string>  // for operator<<, string
#include <vector>  // for vector

std::string print(int c);
std::string printEscaped(int c); // this should be given a set of chars to be replaced

struct showChar {
    showChar(const int c) : c_{c} {}
    int c_;
};

struct showCharEscaped {
    showCharEscaped(const int c) : c_{c} {}
    int c_;
};

std::ostream& operator<<(std::ostream& s, showChar c);
std::ostream& operator<<(std::ostream& s, showCharEscaped c);

template <typename T>
struct show {
    show(const T& x) : x_{x} {}
    const T& x_;
};

template <class T>
struct showImpl {
    static std::ostream& Show(std::ostream& s, const T& x) { return s << x; }
};

template <typename T>
std::ostream& operator<<(std::ostream& s, show<T> x) {
    return showImpl<T>::Show(s, x.x_);
}

template <class... Ts>
struct showImpl<std::vector<Ts...>> {
    static std::ostream& Show(std::ostream& s, const std::vector<Ts...>& v) {
        if (v.empty()) {
            s << "[]";
            return s;
        }
        s << "[ " << show(v[0]);
        for (unsigned int i = 1; i < v.size(); i++)
            s << ", " << show(v[i]);

        s << " ]";
        return s;
    }
};

template <class T1, class T2>
struct showImpl<std::pair<T1, T2>> {
    static std::ostream& Show(std::ostream& s, const std::pair<T1, T2>& p) {
        s << "(" << show(p.first) << "," << show(p.second) << ")";
        return s;
    }
};

template <class T>
struct bin {
    bin(const T x) : x_{x} {}
    T x_;
};

template <class T>
std::ostream& operator<<(std::ostream& s, const bin<T> x) {
    int n = 8 * sizeof(T);
    int i = 0;
    while (i < n && !(x.x_ & (T(1) << i)))
        i++;
    if (i == n) {
        s << "{}";
        return s;
    }
    s << "{ " << i;
    for (i++; i < n; i++)
        if (x.x_ & (T(1) << i))
            s << ", " << i;
    s << " }";
    return s;
}

template <>
struct showImpl<std::vector<bool>> {
    static std::ostream& Show(std::ostream& s, const std::vector<bool>& v);
};

template <>
struct showImpl<std::vector<char>> {
    static std::ostream& Show(std::ostream& s, const std::vector<char>& v);
};
