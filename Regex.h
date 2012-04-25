#ifndef _REGEX_H_
#define _REGEX_H_

#include "DFA.h"
#include <ostream>
using std::ostream;
#include <vector>
using std::vector;
#include <functional>
using std::function;
#include <istream>
using std::istream;
#include <fstream>
using std::filebuf;
#include <tuple>
using std::pair;
#include <iostream>
using std::cerr;
using std::cout;
using std::cin;
#include <iomanip>
using std::endl;
#include <ios>
using std::ios_base;
#include <exception>
using std::exception;

template<class T>
ostream& showVector(const vector<T>& v, ostream& os);

template<class T>
function<ostream&(ostream&)> show(const vector<T>& v);

template<class T>
function<ostream&(ostream&)> bin(T x);

template<class T1, class T2>
function<ostream&(ostream&)> show(const pair<T1,T2>& p);

ostream& operator<<(ostream&, const function<ostream&(ostream&)>&);

template<>
ostream& showVector<bool>(const vector<bool>& v, ostream& os);

template<class T>
struct Lexer {
  char a[4096];
  char b[4096];
  char *p1, *p2, *c;
  const DFA& dfa;
  virtual T action(char*, unsigned int, unsigned int) = 0;

  Lexer(const DFA& _dfa) : p1(a), p2(b), c(p1), dfa(_dfa) {
    a[4095] = '\0';
    b[4095] = '\0';
  };
  
  virtual T eofToken() = 0;
  virtual T getToken() final {
    cout << "entered getToken()" << endl;
    auto s = dfa.start;
    auto f = dfa.final[s];
    cout << "starting in state " << s << ", type " << f << endl;
    // TODO: make sure c points to input?
    auto c0 = c, c1 = c;
    while(*c != EOF && s != dfa.deadState) {
      cout << "got '" << (int)*c << "'" << endl;
      auto _c = dfa.symbolToId.at(*c);
      if (_c == dfa.symbolCount) {
        cout << "invalid symbol '" << (int)*c << "'" << endl;
        throw exception();
      }
      s = dfa.T[s][_c];
      if (dfa.final[s]) {
        f = dfa.final[s];
        c1 = c;
      }
      cout << "new state " << s << ", type " << dfa.final[s] << ", last final: " << f << endl;
      cout << "read " << (c - c0 + 1) << ", current token length: " << (c1 - c0 + 1) << endl;
      c++;
      //cin.get();
    }
    if (*c == EOF)
      cout << "got EOF" << endl;
    if (s == dfa.deadState)
      cout << "ran into dead end" << endl;
    if (f != 0) {
      c = c1 + 1;
      return action(c0, c1 - c0 + 1, f);
    } else if (*c == EOF) {
      return eofToken();
    } else {
      cerr << "Lexical error" << endl;
      throw exception();
    }
  };

};
#endif
