#ifndef _PRINT_H_
#define _PRINT_H_

#include <ostream>
using std::ostream;
#include <vector>
using std::vector;
#include <functional>
using std::function;
#include <tuple>
using std::pair;

std::string print(char c);
std::string printEscaped(char c); // this should be given a set of chars to be replaced
inline function<ostream&(ostream&)> showChar(char c) {
    return [c] (ostream& s) -> ostream& {
        return s << print(c);
    };
}

inline function<ostream&(ostream&)> showCharEscaped(char c) {
    return [c] (ostream& s) -> ostream& {
        return s << printEscaped(c);
    };
}

inline ostream& operator<<(ostream& s, const function<ostream&(ostream&)>& f) {
    return f(s);
}

template <class T>
ostream& showVector(const vector<T>& v, ostream& s) {
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

template<class T1, class T2>
ostream& showPair(const pair<T1,T2>& p, ostream& s) {
  s << "(" << p.first << "," << p.second << ")";
  return s;
}

using namespace std::placeholders;

template<class T>
function<ostream&(ostream&)> show(const vector<T>& v) {
  return bind(showVector<T>, v, _1);
}

template<class T1, class T2>
function<ostream&(ostream&)> show(const pair<T1,T2>& p) {
  return bind(showPair<T1,T2>, p, _1);
}

template<class T>
function<ostream&(ostream&)> bin(T x) {
  return [x] (ostream& s) -> ostream& {
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
ostream& showVector(const vector<bool>& v, ostream& s);
template <>
ostream& showVector(const vector<pair<unsigned int,unsigned int>>& v, ostream& s);
template <>
ostream& showVector(const vector<char>& v, ostream& s);

#endif
