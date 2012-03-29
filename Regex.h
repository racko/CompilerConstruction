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

template<class T>
ostream& showVector(const vector<T>& v, ostream& os);

template<class T>
function<ostream&(ostream&)> show(const vector<T>& v);

ostream& operator<<(ostream&, const function<ostream&(ostream&)>&);

template<>
ostream& showVector<bool>(const vector<bool>& v, ostream& os);

NFA lexRegex(istream& in);
#endif
