#include "Regex.h"
using std::bind;
#include <tuple>
using std::pair;
#include <iostream>

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

ostream& operator<<(ostream& s, const function<ostream&(ostream&)>& f) {
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

ostream& operator<<(ostream& s, const BitSet& v) {
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

template <>
ostream& showVector(const vector<pair<unsigned int,unsigned int>>& v, ostream& s) {
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

//template ostream& showVector<unsigned int>(const vector<unsigned int>& v, ostream& s);
//template ostream& showVector<bool>(const vector<bool>& v, ostream& s);
//template ostream& showVector<pair<unsigned int,unsigned int>>(const vector<pair<unsigned int,unsigned int>>& v, ostream& s);
template function<ostream&(ostream&)> show<unsigned int>(const vector<unsigned int>& v);
template function<ostream&(ostream&)> show<bool>(const vector<bool>& v);
template function<ostream&(ostream&)> show<char>(const vector<char>& v);
template function<ostream&(ostream&)> show<unsigned int, unsigned int>(const pair<unsigned int, unsigned int>& p);
template function<ostream&(ostream&)> show<pair<unsigned int,unsigned int>>(const vector<pair<unsigned int,unsigned int>>& v);

/*
template<class T>
T getToken(istream& in, DFA& dfa, function<T(const char*,unsigned int,unsigned int)>& f) {
  auto s = dfa.start;
  auto c = in.get();
  int lastFinal = dfa.final[s] ? s : -1;
  auto startPos = in.tellg();
  auto lastPos = startPos;
  while (c != EOF) {
    s = dfa.T[s][dfa.symbolToId[c]];
    c = in.get();
    
    if (dfa.final[s]) {
      lastFinal = s;
      lastPos = in.tellg();
    }
  }
  
  if (lastPos != startPos) {
    in.seekg(startPos);
    //
  } else {
    cerr << "Lexer failure." << endl;
    throw exception();
  }
}
*/
