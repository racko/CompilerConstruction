#pragma once

#include <functional> // for function, _1, bind, placeholders
#include <ostream>    // for ostream, operator<<, basic_ostream::operator<<
#include <string>     // for operator<<, string
#include <vector>     // for vector

std::string print(int c);
std::string printEscaped(int c); // this should be given a set of chars to be replaced
struct showChar {
    int c;
};

std::ostream& operator<<(std::ostream& s, showChar c);

inline std::function<std::ostream&(std::ostream&)> showChar(int c) {
    return [c](std::ostream& s) -> std::ostream& { return s << print(c); };
}

inline std::function<std::ostream&(std::ostream&)> showCharEscaped(int c) {
    return [c](std::ostream& s) -> std::ostream& { return s << printEscaped(c); };
}

inline std::ostream& operator<<(std::ostream& s, const std::function<std::ostream&(std::ostream&)>& f) { return f(s); }

template <class T>
std::ostream& showVector(const std::vector<T>& v, std::ostream& s) {
    if (v.empty()) {
        s << "[]";
        return s;
    }
    s << "[ " << v[0];
    for (unsigned int i = 1; i < v.size(); i++)
        s << ", " << v[i];

    s << " ]";
    return s;
}

template <class T1, class T2>
std::ostream& showPair(const std::pair<T1, T2>& p, std::ostream& s) {
    s << "(" << p.first << "," << p.second << ")";
    return s;
}

template <class T>
std::function<std::ostream&(std::ostream&)> show(const std::vector<T>& v) {
    using namespace std::placeholders;
    return bind(showVector<T>, v, _1);
}

template <class T1, class T2>
std::function<std::ostream&(std::ostream&)> show(const std::pair<T1, T2>& p) {
    using namespace std::placeholders;
    return bind(showPair<T1, T2>, p, _1);
}

template <class T>
std::function<std::ostream&(std::ostream&)> bin(T x) {
    return [x](std::ostream& s) -> std::ostream& {
        int n = 8 * sizeof(T);
        int i = 0;
        while (i < n && !(x & (T(1) << i)))
            i++;
        if (i == n) {
            s << "{}";
            return s;
        }
        s << "{ " << i;
        for (i++; i < n; i++)
            if (x & (T(1) << i))
                s << ", " << i;
        s << " }";
        return s;
    };
}

template <>
std::ostream& showVector(const std::vector<bool>& v, std::ostream& s);
template <>
std::ostream& showVector(const std::vector<std::pair<unsigned int, unsigned int>>& v, std::ostream& s);
template <>
std::ostream& showVector(const std::vector<char>& v, std::ostream& s);
