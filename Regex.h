#ifndef _REGEX_H_
#define _REGEX_H_

#include "NFA.h"
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

template<class T>
ostream& showVector(const vector<T>& v, ostream& os);

template<class T>
function<ostream&(ostream&)> show(const vector<T>& v);

template<class T1, class T2>
function<ostream&(ostream&)> show(const pair<T1,T2>& p);

ostream& operator<<(ostream&, const function<ostream&(ostream&)>&);

template<>
ostream& showVector<bool>(const vector<bool>& v, ostream& os);

nfaBuilder lexRegex(istream& in);

template<class T>
struct Lexer {
  istream& in;
  virtual T action(const char*, unsigned int, unsigned int) = 0;

  Lexer(istream& _in) : in(_in) {};
  T getToken() final { return T(); };

};
#endif
