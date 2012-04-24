#include "Regex.h"
#include "NFA.h"
#include "DFA.h"
#include "nfaBuilder.h"
#include "BitSet.h"
#include <ctime>
#include <iostream>
#include <iomanip>
using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::hex;
#include <sstream>
using std::stringstream;
using std::move;
#include <string>
using std::string;

struct myLexer : public Lexer<string> {
  myLexer(const DFA& dfa) : Lexer(dfa) {}
  string action(char* s, unsigned int n, unsigned int t) {
    cout << "action(" << n << ", " << t << ")" << endl;
    return string(s, n);
  }
};

int main(int argc, char** args) {

  if (argc == 1) {
    cerr << "too few arguments:" << endl;
    for (int i = 1; i < argc; i++)
      cerr << "'" << args[i] << "'" << endl;
    return 1;
  }

  stringstream in;
  nfaBuilder builder;
  for (int i = 1; i < argc; i++) {
    cout << "main: lexRegex(" << args[i] << ")" << endl;
    in << args[i];
    auto p = builder.lexRegex(in);
    builder.ns[p.second].kind = i;
    if (in.good()) {
      cout << "at least one unread character: '" << in.peek() << "'" << endl;
      in.str("");
    }
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
  myLexer lex(dfa1);
  cin.get(lex.c, 4096);
  auto ll = cin.gcount();
  cout << "read " << ll << " characters" << endl;
  lex.c[ll] = EOF;
  while (*lex.c != EOF) {
    string s = lex.getToken();
    cout << "\ngot \"" << s << "\"" << endl;
  }
  return 0;
}
