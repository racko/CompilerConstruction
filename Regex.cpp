#include "Regex.h"
using std::bind;
#include <tuple>
using std::pair;
#include <istream>
using std::istream;
#include <iostream>
#include <iomanip>
using std::cerr;
using std::endl;
#include <unordered_map>
using std::unordered_map;
#include <cstdlib>
using std::exit;
#include "NFA.h"
using std::move;

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
template function<ostream&(ostream&)> show<pair<unsigned int,unsigned int>>(const vector<pair<unsigned int,unsigned int>>& v);

enum struct NT : unsigned int {
  E = 0,
  ER,
  T,
  TR,
  F,
  FR,
  G
};

void match(istream& in, char c) {
  char d = in.get();
  if (d != c) {
    cerr << "expected '" << c << "'" << ", got '" << d << "'" << endl;
    exit (1);
  }
}

pair<node*,node*> lexE(istream& in, nfaBuilder& nfa);

pair<node*,node*> lexG(istream& in, nfaBuilder& nfa) {
  switch (char c = in.peek()) {
    case '(':
      {
        match(in, '(');
        pair<node*,node*> nfa1 = lexE(in, nfa);
        match(in, ')');
        return nfa1;
      }
    case '|':
    case EOF:
    case ')':
    case '*':
      cerr << "Error: '" << c << "'" << endl;
      exit(1);
    default:
      {
        match(in, c);
        pair<node*,node*> out;

        nfa.ns.emplace_back();
        out.first = &nfa.ns.back();

        nfa.ns.emplace_back();
        out.second = &nfa.ns.back();

        out.first->ns[c].push_back(out.second);
        return out;
      }
  }
}

pair<node*,node*> lexFR(istream& in, nfaBuilder& nfa, pair<node*,node*> nfa1) {
  switch (char c = in.peek()) {
    case '*':
      {
        match(in, '*');
        pair<node*,node*> out;

        nfa.ns.emplace_back();
        out.first = &nfa.ns.back();

        nfa.ns.emplace_back();
        out.second = &nfa.ns.back();

        out.first->ns[-1].push_back(nfa1.first);
        out.first->ns[-1].push_back(out.second);
        nfa1.second->ns[-1].push_back(nfa1.first);
        nfa1.second->ns[-1].push_back(out.second);
        return out;
      }
    default:
      return nfa1;
  }
}

pair<node*,node*> lexF(istream& in, nfaBuilder& nfa) {
  switch (char c = in.peek()) {
    case '|':
    case EOF:
    case ')':
    case '*':
      cerr << "Error: '" << c << "'" << endl;
      exit(1);
    default:
      {
        pair<node*,node*> nfa1 = lexG(in, nfa);
        pair<node*,node*> nfa2 = lexFR(in, nfa, nfa1);
        return nfa2;
      }
  }
}

pair<node*,node*> lexTR(istream& in, nfaBuilder& nfa, pair<node*,node*> nfa1) {
  switch (char c = in.peek()) {
    case '|':
    case EOF:
    case ')':
      return nfa1;
    case '*':
      cerr << "Error: '" << c << "'" << endl;
      exit(1);
    default:
      {
        pair<node*,node*> nfa2 = lexF(in, nfa);
        *nfa1.second = *nfa2.first;
        pair<node*,node*> nfa3 = lexTR(in, nfa, nfa1);
        return nfa3;
      }
  }
}

pair<node*,node*> lexT(istream& in, nfaBuilder& nfa) {
  switch (char c = in.peek()) {
    case '|':
    case EOF:
    case ')':
    case '*':
      cerr << "Error: '" << c << "'" << endl;
      exit(1);
    default:
      {
        pair<node*,node*> nfa1 = lexF(in, nfa);
        pair<node*,node*> nfa2 = lexTR(in, nfa, nfa1);
        return nfa2;
      }
  }
}

pair<node*,node*> lexER(istream& in, nfaBuilder& nfa, pair<node*,node*> nfa1) {
  switch (char c = in.peek()) {
    case '|':
      {
        match(in, '|');
        pair<node*,node*> nfa2 = lexT(in, nfa);

        pair<node*,node*> nfa3;

        nfa.ns.emplace_back();
        nfa3.first = &nfa.ns.back();

        nfa.ns.emplace_back();
        nfa3.second = &nfa.ns.back();
        
        nfa3.first->ns[-1].push_back(nfa1.first);
        nfa3.first->ns[-1].push_back(nfa2.first);
        nfa1.second->ns[-1].push_back(nfa3.second);
        nfa2.second->ns[-1].push_back(nfa3.second);
        
        pair<node*,node*> nfa4 = lexER(in, nfa, nfa3);
        return nfa4;
      }
    case EOF:
    case ')':
      return nfa1;
    default:
      cerr << "Error: '" << c << "'" << endl;
      exit(1);
  }
}

pair<node*,node*> lexE(istream& in, nfaBuilder& nfa) {
  switch (char c = in.peek()) {
    case '|':
    case EOF:
    case ')':
    case '*':
      cerr << "Error: '" << c << "'" << endl;
      exit(1);
    default:
      {
        pair<node*,node*> nfa1 = lexT(in, nfa);
        pair<node*,node*> nfa2 = lexER(in, nfa, nfa1);
        return nfa2;
      }
  }
}

NFA lexRegex(istream& in) {
  nfaBuilder nfa;
  lexE(in, nfa);
  return NFA(move(nfa));
}
