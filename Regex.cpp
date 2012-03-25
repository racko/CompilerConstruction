#include "Regex.h"
using std::bind;

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

using namespace std::placeholders;

template<class T>
function<ostream&(ostream&)> show(const vector<T>& v) {
  return bind(showVector, v, _1);
}

template<class T>
ostream& operator<<(ostream& s, const function<T>& f) {
  return f(s);
}

template <>
ostream& showVector<bool>(const vector<bool>& v, ostream& s) {
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

