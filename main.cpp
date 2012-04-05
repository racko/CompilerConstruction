#include "Regex.h"
#include "NFA.h"
#include "DFA.h"
#include "nfaBuilder.h"
#include "BitSet.h"
#include <iostream>
#include <iomanip>
using std::cout;
using std::cin;
using std::cerr;
using std::endl;
#include <sstream>
using std::stringstream;
using std::move;

enum struct Symbol : unsigned int {
  a = 0,
  b = 1,
  last = b,
  eps = last + 1
};

int main(int argc, char** args) {
  
  BitSet bs(65);
  cout << "n: " << bs.n << endl;
  cout << "m: " << bs.m << endl;
  for (auto i = 0u; i < bs.size(); i++)
    bs[i] = i & 1;
  cout << bs << endl;
  bs.resize(130);
  cout << bs << endl;
  cout << "n: " << bs.n << endl;
  cout << "m: " << bs.m << endl;
  for (auto i = 0u; i < bs.size(); i++)
    bs[i] = i & 1;
  cout << bs << endl;


  return 0;
  if (argc == 1) {
    cerr << "too few arguments:" << endl;
    for (unsigned int i = 1; i < argc; i++)
      cerr << "'" << args[i] << "'" << endl;
    return 1;
  }

  stringstream in;
  nfaBuilder builder;
  for (unsigned int i = 1; i < argc; i++) {
    cout << "main: lexRegex(" << args[i] << ")" << endl;
    in << args[i];
    auto p = builder.lexRegex(in);
    builder.ns[p.second].kind = i;
    if (in.good())
      cout << "at least one unread character: '" << in.peek() << "'" << endl;
    in.str("");
    in.clear();
  }
  NFA nfa1(move(builder));
  DFA dfa1(nfa1);
  cout << "start: " << dfa1.start << endl;
  cout << "final: " << show(dfa1.final) << endl;
  for (unsigned int q = 0; q < dfa1.stateCount; q++) {
    for (unsigned int a = 0; a < dfa1.symbolCount; a++)
      cout << "(" << q << "," << a << ") -> " << dfa1.T[q][a] << endl;
  }
  dfa1.minimize();
  cout << "start: " << dfa1.start << endl;
  cout << "final: " << show(dfa1.final) << endl;
  for (unsigned int q = 0; q < dfa1.stateCount; q++) {
    for (unsigned int a = 0; a < dfa1.symbolCount; a++)
      cout << "(" << q << "," << a << ") -> " << dfa1.T[q][a] << endl;
  }
  return 0;
}
