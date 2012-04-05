#include "Regex.h"
#include "NFA.h"
#include "DFA.h"
#include "nfaBuilder.h"
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
//  const unsigned int stateCount = 11, symbolCount = (unsigned int)(Symbol::last) + 1, a = (unsigned int)(Symbol::a), b = (unsigned int)(Symbol::b), eps = (unsigned int)(Symbol::eps); // less-than-ideal solution
//  vector<bool> final(stateCount, false);
//  final[10] = true;
//  NFA nfa(symbolCount + 1, stateCount, eps, 0, std::move(final));
//  nfa.table.resize(stateCount, vector<vector<bool>>(symbolCount + 1, vector<bool>(stateCount)));
//  nfa.table[0][eps][1] = true;
//  nfa.table[0][eps][7] = true;
//  nfa.table[1][eps][2] = true;
//  nfa.table[1][eps][4] = true;
//  nfa.table[2][a][3] = true;
//  nfa.table[4][b][5] = true;
//  nfa.table[3][eps][6] = true;
//  nfa.table[5][eps][6] = true;
//  nfa.table[6][eps][1] = true;
//  nfa.table[6][eps][7] = true;
//  nfa.table[7][a][8] = true;
//  nfa.table[8][b][9] = true;
//  nfa.table[9][b][10] = true;
//  DFA dfa(nfa);
//  cout << "start: " << dfa.start << endl;
//  cout << "final: " << show(dfa.final) << endl;
//  for (unsigned int q = 0; q < dfa.stateCount; q++) {
//    for (unsigned int a = 0; a < dfa.symbolCount; a++)
//      cout << "(" << q << "," << a << ") -> " << dfa.T[q][a] << endl;
//  }
//  dfa.minimize();
//  cout << "start: " << dfa.start << endl;
//  cout << "final: " << show(dfa.final) << endl;
//  for (unsigned int q = 0; q < dfa.stateCount; q++) {
//    for (unsigned int a = 0; a < dfa.symbolCount; a++)
//      cout << "(" << q << "," << a << ") -> " << dfa.T[q][a] << endl;
//  }
  
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
