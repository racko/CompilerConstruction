#include "Regex.h"
using std::bind;
#include <tuple>
using std::pair;
#include <iostream>
#include <iomanip>
using std::cerr;
using std::cout;
using std::endl;
#include <unordered_map>
using std::unordered_map;
#include <cstdlib>
using std::exit;
using std::move;
using std::exception;

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
template function<ostream&(ostream&)> show<char>(const vector<char>& v);
template function<ostream&(ostream&)> show<pair<unsigned int,unsigned int>>(const vector<pair<unsigned int,unsigned int>>& v);

void match(istream& in, char c) {
  cout << "match('" << c << "')" << endl;
  char d = in.get();
  if (d != c) {
    cerr << "expected '" << c << "'" << ", got '" << d << "'" << endl;
    exit (1);
  }
}

pair<unsigned int,unsigned int> lexE(istream& in, nfaBuilder& nfa);

pair<unsigned int,unsigned int> lexG(istream& in, nfaBuilder& nfa) {
  char c = in.peek();
  cout << "lexG (peek: '" << c << "')" << endl;
  switch (c) {
    case '(':
      {
        match(in, '(');
        pair<unsigned int,unsigned int> nfa1 = lexE(in, nfa);
        match(in, ')');
        cout << "lexG out: " << show(nfa1) << endl;
        return nfa1;
      }
    case '|':
    case EOF:
    case ')':
    case '*':
      cerr << "lexG Error: '" << c << "'" << endl;
      exit(1);
    default:
      {
        match(in, c);
        pair<unsigned int,unsigned int> out;
        
        out.first = nfa.ns.size();
        nfa.ns.emplace_back();

        out.second = nfa.ns.size();
        nfa.ns.emplace_back();

        auto it = nfa.symbolToId.find(c);
        unsigned int id;
        if (it == nfa.symbolToId.end()) {
          id = nfa.symbolToId.size();
          cout << "new symbol: '" << c << "' (id: " << id << ")" << endl;
          nfa.symbolToId[c] = id;
          nfa.idToSymbol.push_back(c);
        } else
          id = it->second;
        nfa.ns[out.first].ns[id].push_back(out.second);
        cout << "lexG out: " << show(out) << endl;
        return out;
      }
  }
}

pair<unsigned int,unsigned int> lexFR(istream& in, nfaBuilder& nfa, pair<unsigned int,unsigned int> nfa1) {
  char c = in.peek();
  cout << "lexFR(" << show(nfa1) << ") (peek: '" << c << "')" << endl;
  switch (c) {
    case '*':
      {
        match(in, '*');
        auto out = nfa.closure(nfa1);
        cout << "lexFR out: " << show(out) << endl;
        return out;
      }
    default:
      cout << "lexFR out: " << show(nfa1) << endl;
      return nfa1;
  }
}

pair<unsigned int,unsigned int> lexF(istream& in, nfaBuilder& nfa) {
  char c = in.peek();
  cout << "lexF (peek: '" << c << "')" << endl;
  switch (c) {
    case '|':
    case EOF:
    case ')':
    case '*':
      cerr << "lexF Error: '" << c << "'" << endl;
      exit(1);
    default:
      {
        pair<unsigned int,unsigned int> nfa1 = lexG(in, nfa);
        pair<unsigned int,unsigned int> nfa2 = lexFR(in, nfa, nfa1);
        cout << "lexF out: " << show(nfa2) << endl;
        return nfa2;
      }
  }
}

pair<unsigned int,unsigned int> lexTR(istream& in, nfaBuilder& nfa, pair<unsigned int,unsigned int> nfa1) {
  char c = in.peek();
  cout << "lexTR(" << show(nfa1) << ") (peek: '" << c << "')" << endl;
  switch (c) {
    case '|':
    case EOF:
    case ')':
      cout << "lexTR out: " << show(nfa1) << endl;
      return nfa1;
    case '*':
      cerr << "lexTR Error: '" << c << "'" << endl;
      exit(1);
    default:
      {
        pair<unsigned int,unsigned int> nfa2 = lexF(in, nfa);
        auto nfa3 = nfa.cat(nfa1, nfa2);
        pair<unsigned int,unsigned int> nfa4 = lexTR(in, nfa, nfa3);
        cout << "lexTR out: " << show(nfa4) << endl;
        return nfa4;
      }
  }
}

pair<unsigned int,unsigned int> lexT(istream& in, nfaBuilder& nfa) {
  char c = in.peek();
  cout << "lexT (peek: '" << c << "')" << endl;
  switch (c) {
    case '|':
    case EOF:
    case ')':
    case '*':
      cerr << "lexT Error: '" << c << "'" << endl;
      exit(1);
    default:
      {
        pair<unsigned int,unsigned int> nfa1 = lexF(in, nfa);
        pair<unsigned int,unsigned int> nfa2 = lexTR(in, nfa, nfa1);
        cout << "lexT out: " << show(nfa2) << endl;
        return nfa2;
      }
  }
}

pair<unsigned int,unsigned int> lexER(istream& in, nfaBuilder& nfa, pair<unsigned int,unsigned int> nfa1) {
  char c = in.peek();
  cout << "lexER(" << show(nfa1) << ") (peek: '" << c << "')" << endl;
  switch (c) {
    case '|':
      {
        match(in, '|');
        pair<unsigned int,unsigned int> nfa2 = lexT(in, nfa);
        
        auto nfa3 = nfa.alt(nfa1, nfa2);
        
        pair<unsigned int,unsigned int> nfa4 = lexER(in, nfa, nfa3);
        cout << "lexER out: " << show(nfa4) << endl;
        return nfa4;
      }
    case EOF:
    case ')':
      cout << "lexER out: " << show(nfa1) << endl;
      return nfa1;
    default:
      cerr << "lexER Error: '" << c << "'" << endl;
      exit(1);
  }
}

pair<unsigned int,unsigned int> lexE(istream& in, nfaBuilder& nfa) {
  char c = in.peek();
  cout << "lexE (peek: '" << c << "')" << endl;
  switch (c) {
    case '|':
    case EOF:
    case ')':
    case '*':
      cerr << "lexE Error: '" << c << "'" << endl;
      exit(1);
    default:
      {
        pair<unsigned int,unsigned int> nfa1 = lexT(in, nfa);
        pair<unsigned int,unsigned int> nfa2 = lexER(in, nfa, nfa1);
        cout << "lexE out: " << show(nfa2) << endl;
        return nfa2;
      }
  }
}

nfaBuilder lexRegex(istream& in) {
  cout << "entered lexRegex" << endl;
  nfaBuilder nfa;
  pair<unsigned int,unsigned int> nfa1 = lexE(in, nfa);
  nfa.ns[nfa.p.first].ns[0].push_back(nfa1.first);
  nfa.ns[nfa1.second].ns[0].push_back(nfa.p.second);
  cout << "done with lexRegex" << endl;
  cout << "out: (" << nfa.p.first << "," << nfa.p.second << ")" << endl;
  return move(nfa);
}
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
