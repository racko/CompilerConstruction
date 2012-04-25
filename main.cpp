//#include "Regex.h"
#include "NFA.h"
//#include "DFA.h"
#include "nfaBuilder.h"
//#include "BitSet.h"
#include "lexer.h"
#include "parser.h"
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
#include <array>
using std::array;

int main(int argc, char** args) {

//  if (argc == 1) {
//    cerr << "too few arguments:" << endl;
//    for (int i = 1; i < argc; i++)
//      cerr << "'" << args[i] << "'" << endl;
//    return 1;
//  }

  stringstream in;
  nfaBuilder builder;

  char alpha[] = "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)";
  char digit[] = "(0|1|2|3|4|5|6|7|8|9)";
  stringstream varch;
  varch << "(" << alpha << "|" << digit << "|_)";
  stringstream num;
  num << digit << digit << "*";
  stringstream var;
  var << alpha << varch.rdbuf() << "*";
  //';', '=', '.', '(', ')', '+', '-', '*', '/', '<', '>', '&', '|', '\\',
  in << ";";
  auto p = builder.lexRegex(in);
  builder.ns[p.second].kind = 1;
  in.clear();
  in << "=";
  p = builder.lexRegex(in);
  builder.ns[p.second].kind = 2;
  in.clear();
  in << ".";
  p = builder.lexRegex(in);
  builder.ns[p.second].kind = 3;
  in.clear();
  in << "\\(";
  p = builder.lexRegex(in);
  builder.ns[p.second].kind = 4;
  in.clear();
  in << "\\)";
  p = builder.lexRegex(in);
  builder.ns[p.second].kind = 5;
  in.clear();
  in << "+";
  p = builder.lexRegex(in);
  builder.ns[p.second].kind = 6;
  in.clear();
  in << "-";
  p = builder.lexRegex(in);
  builder.ns[p.second].kind = 7;
  in.clear();
  in << "\\*";
  p = builder.lexRegex(in);
  builder.ns[p.second].kind = 8;
  in.clear();
  in << "/";
  p = builder.lexRegex(in);
  builder.ns[p.second].kind = 9;
  in.clear();
  in << "<";
  p = builder.lexRegex(in);
  builder.ns[p.second].kind = 10;
  in.clear();
  in << ">";
  p = builder.lexRegex(in);
  builder.ns[p.second].kind = 11;
  in.clear();
  in << "&";
  p = builder.lexRegex(in);
  builder.ns[p.second].kind = 12;
  in.clear();
  in << "\\|";
  p = builder.lexRegex(in);
  builder.ns[p.second].kind = 13;
  in.clear();
  in << "\\\\";
  p = builder.lexRegex(in);
  builder.ns[p.second].kind = 14;
  in.clear();
  in << "<=";
  p = builder.lexRegex(in);
  builder.ns[p.second].kind = 15;
  in.clear();
  in << "==";
  p = builder.lexRegex(in);
  builder.ns[p.second].kind = 16;
  in.clear();
  in << "!=";
  p = builder.lexRegex(in);
  builder.ns[p.second].kind = 17;
  in.clear();
  in << ">=";
  p = builder.lexRegex(in);
  builder.ns[p.second].kind = 18;
  in.clear();
  in << "let";
  p = builder.lexRegex(in);
  builder.ns[p.second].kind = 19;
  in.clear();
  in << "in";
  p = builder.lexRegex(in);
  builder.ns[p.second].kind = 20;
  in.clear();
  in << "letrec";
  p = builder.lexRegex(in);
  builder.ns[p.second].kind = 21;
  p = builder.lexRegex(var);
  builder.ns[p.second].kind = 22;
  p = builder.lexRegex(num);
  builder.ns[p.second].kind = 23;
  in.clear();
  in << "( |\t|\n)( |\t|\n)*";
  p = builder.lexRegex(in);
  builder.ns[p.second].kind = 24;
//  for (int i = 1; i < argc; i++) {
//  for (int i = 1; i < argc; i++) {
//    cout << "main: lexRegex(" << args[i] << ")" << endl;
//    in << args[i];
//    auto p = builder.lexRegex(in);
//    builder.ns[p.second].kind = i;
//    if (in.good()) {
//      cout << "at least one unread character: '" << in.peek() << "'" << endl;
//      in.str("");
//    }
//    in.clear();
//  }
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
  cout << "> ";
  cin.get(lex.c, 4096);
  auto ll = cin.gcount();
  cout << "read " << ll << " characters" << endl;
  lex.c[ll] = EOF;
  Parser pp(lex);
  cout << *pp.parse();
//  while (*lex.c != EOF) {
//    Token* t = lex.getToken();
//    cout << "\ngot \"";
//    t->print(cout);
//    cout << "\"" << endl;
//    delete t;
//  }
  return 0;
}
