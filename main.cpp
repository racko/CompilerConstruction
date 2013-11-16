//#include "Regex.h"
#include "NFA.h"
//#include "DFA.h"
#include "nfaBuilder.h"
//#include "BitSet.h"
#include "lexer.h"
#include "parser.h"
#include "lrParser.h"
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

#include <boost/dynamic_bitset.hpp>

int main(int argc, char** args) {

//  if (argc == 1) {
//    cerr << "too few arguments:" << endl;
//    for (int i = 1; i < argc; i++)
//      cerr << "'" << args[i] << "'" << endl;
//    return 1;
//  }

//  stringstream in;
//  nfaBuilder builder;
//
//  char alpha[] = "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)";
//  char digit[] = "(0|1|2|3|4|5|6|7|8|9)";
//  stringstream varch;
//  varch << "(" << alpha << "|" << digit << "|_)";
//  stringstream num;
//  num << digit << digit << "*";
//  stringstream var;
//  var << alpha << varch.rdbuf() << "*";
//  //';', '=', '.', '(', ')', '+', '-', '*', '/', '<', '>', '&', '|', '\\',
//  in << ";";
//  auto p = builder.lexRegex(in);
//  builder.ns[p.second].kind = 1;
//  in.clear();
//  in << "=";
//  p = builder.lexRegex(in);
//  builder.ns[p.second].kind = 2;
//  in.clear();
//  in << ".";
//  p = builder.lexRegex(in);
//  builder.ns[p.second].kind = 3;
//  in.clear();
//  in << "\\(";
//  p = builder.lexRegex(in);
//  builder.ns[p.second].kind = 4;
//  in.clear();
//  in << "\\)";
//  p = builder.lexRegex(in);
//  builder.ns[p.second].kind = 5;
//  in.clear();
//  in << "+";
//  p = builder.lexRegex(in);
//  builder.ns[p.second].kind = 6;
//  in.clear();
//  in << "-";
//  p = builder.lexRegex(in);
//  builder.ns[p.second].kind = 7;
//  in.clear();
//  in << "\\*";
//  p = builder.lexRegex(in);
//  builder.ns[p.second].kind = 8;
//  in.clear();
//  in << "/";
//  p = builder.lexRegex(in);
//  builder.ns[p.second].kind = 9;
//  in.clear();
//  in << "<";
//  p = builder.lexRegex(in);
//  builder.ns[p.second].kind = 10;
//  in.clear();
//  in << ">";
//  p = builder.lexRegex(in);
//  builder.ns[p.second].kind = 11;
//  in.clear();
//  in << "&";
//  p = builder.lexRegex(in);
//  builder.ns[p.second].kind = 12;
//  in.clear();
//  in << "\\|";
//  p = builder.lexRegex(in);
//  builder.ns[p.second].kind = 13;
//  in.clear();
//  in << "\\\\";
//  p = builder.lexRegex(in);
//  builder.ns[p.second].kind = 14;
//  in.clear();
//  in << "<=";
//  p = builder.lexRegex(in);
//  builder.ns[p.second].kind = 15;
//  in.clear();
//  in << "==";
//  p = builder.lexRegex(in);
//  builder.ns[p.second].kind = 16;
//  in.clear();
//  in << "!=";
//  p = builder.lexRegex(in);
//  builder.ns[p.second].kind = 17;
//  in.clear();
//  in << ">=";
//  p = builder.lexRegex(in);
//  builder.ns[p.second].kind = 18;
//  in.clear();
//  in << "let";
//  p = builder.lexRegex(in);
//  builder.ns[p.second].kind = 19;
//  in.clear();
//  in << "in";
//  p = builder.lexRegex(in);
//  builder.ns[p.second].kind = 20;
//  in.clear();
//  in << "letrec";
//  p = builder.lexRegex(in);
//  builder.ns[p.second].kind = 21;
//  p = builder.lexRegex(var);
//  builder.ns[p.second].kind = 22;
//  p = builder.lexRegex(num);
//  builder.ns[p.second].kind = 23;
//  in.clear();
//  in << "( |\t|\n)( |\t|\n)*";
//  p = builder.lexRegex(in);
//  builder.ns[p.second].kind = 24;
////  for (int i = 1; i < argc; i++) {
////  for (int i = 1; i < argc; i++) {
////    cout << "main: lexRegex(" << args[i] << ")" << endl;
////    in << args[i];
////    auto p = builder.lexRegex(in);
////    builder.ns[p.second].kind = i;
////    if (in.good()) {
////      cout << "at least one unread character: '" << in.peek() << "'" << endl;
////      in.str("");
////    }
////    in.clear();
////  }
//  NFA nfa1(builder);
//  {
//    std::ofstream nfa_dot{"nfa.dot"};
//    nfa_dot << nfa1;
//    nfa_dot.close();
//  }
//
//  DFA dfa1(nfa1);
//  {
//    std::ofstream dfa_dot{"dfa.dot"};
//    dfa_dot << dfa1;
//    dfa_dot.close();
//  }
////  cout << "start: " << dfa1.start << endl;
////  cout << "final: " << show(dfa1.final) << endl;
////  for (unsigned int q = 0; q < dfa1.stateCount; q++) {
////    for (unsigned int a = 0; a < dfa1.symbolCount; a++)
////      cout << "(" << q << "," << a << ") -> " << dfa1.T[q][a] << endl;
////  }
//  dfa1.minimize();
//  {
//    std::ofstream dfa_dot{"dfa_minimized.dot"};
//    dfa_dot << dfa1;
//    dfa_dot.close();
//  }

  {
    Grammar::NonterminalID start = uint32_t(NT::ASSIGN) - uint32_t(T::EPS) - 1;
    Grammar::TerminalID eof = uint32_t(T::EOI);
    size_t terminalCount = uint32_t(T::EPS);
    size_t nonterminalCount = uint32_t(NT::max) - uint32_t(T::EPS) - 1;
    std::vector<std::vector<Grammar::String>> productions(nonterminalCount);
    productions[int32_t(NT::LOC) - uint32_t(T::EPS) - 1] = {
        {uint32_t(NT::LOC), uint32_t(T::LEFTBR), uint32_t(NT::ASSIGN), uint32_t(T::RIGHTBR)}, {uint32_t(T::ID)}, {uint32_t(NT::LOC), uint32_t(T::PERIOD), uint32_t(T::ID)}
    };
    productions[uint32_t(NT::ASSIGN) - uint32_t(T::EPS) - 1] = {
        {uint32_t(NT::BOOL), uint32_t(T::ASSIGN), uint32_t(NT::ASSIGN)}, {uint32_t(NT::BOOL)}
    };
    productions[uint32_t(NT::BOOL) - uint32_t(T::EPS) - 1] = {
        {uint32_t(NT::BOOL), uint32_t(T::OR), uint32_t(NT::JOIN)}, {uint32_t(NT::JOIN)}
    };
    productions[uint32_t(NT::JOIN) - uint32_t(T::EPS) - 1] = {
        {uint32_t(NT::JOIN), uint32_t(T::AND), uint32_t(NT::EQ)}, {uint32_t(NT::EQ)}
    };
    productions[uint32_t(NT::EQ) - uint32_t(T::EPS) - 1] = {
        {uint32_t(NT::EQ), uint32_t(T::EQ), uint32_t(NT::REL)}, {uint32_t(NT::EQ), uint32_t(T::NEQ), uint32_t(NT::REL)}, {uint32_t(NT::REL)}
    };
    productions[uint32_t(NT::REL) - uint32_t(T::EPS) - 1] = {
        {uint32_t(NT::EXPR), uint32_t(T::LT), uint32_t(NT::EXPR)}, {uint32_t(NT::EXPR), uint32_t(T::LE), uint32_t(NT::EXPR)}, {uint32_t(NT::EXPR), uint32_t(T::GE), uint32_t(NT::EXPR)}, {uint32_t(NT::EXPR), uint32_t(T::GT), uint32_t(NT::EXPR)}, {uint32_t(NT::EXPR)}
    };
    productions[uint32_t(NT::EXPR) - uint32_t(T::EPS) - 1] = {
        {uint32_t(NT::EXPR), uint32_t(T::PLUS), uint32_t(NT::TERM)}, {uint32_t(NT::EXPR), uint32_t(T::MINUS), uint32_t(NT::TERM)}, {uint32_t(NT::TERM)}
    };
    productions[uint32_t(NT::TERM) - uint32_t(T::EPS) - 1] = {
        {uint32_t(NT::TERM), uint32_t(T::TIMES), uint32_t(NT::UNARY)}, {uint32_t(NT::TERM), uint32_t(T::DIV), uint32_t(NT::UNARY)}, {uint32_t(NT::UNARY)}
    };
    productions[uint32_t(NT::UNARY) - uint32_t(T::EPS) - 1] = {
        {uint32_t(T::NOT), uint32_t(NT::UNARY)}, {uint32_t(T::MINUS), uint32_t(NT::UNARY)}, {uint32_t(NT::FACTOR)}
    };
    productions[uint32_t(NT::FACTOR) - uint32_t(T::EPS) - 1] = {
        {uint32_t(T::LEFTPAR), uint32_t(NT::ASSIGN), uint32_t(T::RIGHTPAR)}, {uint32_t(NT::LOC)}, {uint32_t(T::NUM)}, {uint32_t(T::REAL)}, {uint32_t(T::TRUE)}, {uint32_t(T::FALSE)}, {uint32_t(T::STRING)}, {uint32_t(NT::FUNCALL)}
    };
    productions[uint32_t(NT::FUNCALL) - uint32_t(T::EPS) - 1] = {
        {uint32_t(T::ID), uint32_t(T::LEFTPAR), uint32_t(NT::OPTARGS), uint32_t(T::RIGHTPAR)}
    };
    productions[uint32_t(NT::OPTARGS) - uint32_t(T::EPS) - 1] = {
        {uint32_t(NT::ARGS)}, {uint32_t(T::EPS)}
    };
    productions[uint32_t(NT::ARGS) - uint32_t(T::EPS) - 1] = {
        {uint32_t(NT::ASSIGN), uint32_t(T::COMMA), uint32_t(NT::ARGS)}, {uint32_t(NT::ASSIGN)}
    };

//    Grammar::NonterminalID start = uint32_t(NT::S) - uint32_t(T::EPS) - 1;
//    Grammar::TerminalID eof = uint32_t(T::EOI);
//    size_t terminalCount = uint32_t(T::EPS);
//    size_t nonterminalCount = uint32_t(NT::max) - uint32_t(T::EPS) - 1;
//    std::vector<std::vector<Grammar::String>> productions(nonterminalCount);
//
//    productions[int32_t(NT::S) - uint32_t(T::EPS) - 1] = {
//        {uint32_t(NT::C), uint32_t(NT::C)}
//    };
//    productions[uint32_t(NT::C) - uint32_t(T::EPS) - 1] = {
//        {uint32_t(T::C), uint32_t(NT::C)}, {uint32_t(T::D)}
//    };

    Grammar G(productions, start, eof, terminalCount, nonterminalCount);
//
//    // [C ->   c C     c]< [S ->       C C     EOF] = true
//    auto item1 = item(G, uint32_t(NT::C) - uint32_t(T::EPS) - 1, item::String{}, item::String{uint32_t(T::C), uint32_t(NT::C)}, uint32_t(T::C));
//    auto item2 = item(G, uint32_t(NT::S) - uint32_t(T::EPS) - 1, item::String{}, item::String{uint32_t(NT::C), uint32_t(NT::C)}, uint32_t(T::EOI));
//    std::cout <<  "testing... " << (item1 < item2) << std::endl;

    for (Grammar::NonterminalID A = 0u; A < G.getNumberOfNonterminals(); ++A) {
      std::cout << NT(G.idOfNonterminal(A)) << ": ";
      const auto& firsts = G.getFirsts(G.idOfNonterminal(A));
      for (Grammar::TerminalID a = 0u; a < G.getNumberOfTerminals(); ++a)
        if (firsts[a])
          std::cout << T(G.idOfTerminal(a)) << ", ";
      if (firsts[G.idOfEps()])
        std::cout << "EPS, ";
      std::cout << std::endl;
    }

    LRParser parser(G);
    //print(std::cout, G, C);
    auto state_count = parser.action_table.size();

    for (auto i = 0u; i < state_count; ++i) {
      for (auto a = 0u; a < G.getNumberOfTerminals(); ++a) {
        std::cout << parser.action_table[i][a] << " ";
      }
      std::cout << std::endl;
    }

    for (auto i = 0u; i < state_count; ++i) {
      for (auto A = 0u; A < G.getNumberOfNonterminals(); ++A) {
        std::cout << parser.goto_table[i][A] << " ";
      }
      std::cout << std::endl;
    }

    std::cout << "parsing NUM" << std::endl;
    parser.parse({uint32_t(T::NUM), uint32_t(T::EOI)});
    std::cout << "parsing NUM + NUM" << std::endl;
    parser.parse({uint32_t(T::NUM), uint32_t(T::PLUS), uint32_t(T::NUM), uint32_t(T::EOI)});
    std::cout << "parsing NUM * (NUM + NUM)" << std::endl;
    parser.parse({uint32_t(T::NUM), uint32_t(T::TIMES), uint32_t(T::LEFTPAR), uint32_t(T::NUM), uint32_t(T::PLUS), uint32_t(T::NUM), uint32_t(T::RIGHTPAR), uint32_t(T::EOI)});
    std::cout << "parsing ID[ID] = NUM + NUM" << std::endl;
    parser.parse({uint32_t(T::ID), uint32_t(T::LEFTBR), uint32_t(T::ID), uint32_t(T::RIGHTBR), uint32_t(T::ASSIGN), uint32_t(T::NUM), uint32_t(T::PLUS), uint32_t(T::NUM), uint32_t(T::EOI)});

//    std::cout << "parsing cd: Fail expected" << std::endl;
//    parser.parse({uint32_t(T::C), uint32_t(T::D), uint32_t(T::EOI)});
//    std::cout << "parsing ccd: Fail expected" << std::endl;
//    parser.parse({uint32_t(T::C), uint32_t(T::C), uint32_t(T::D), uint32_t(T::EOI)});
//    std::cout << "parsing cdcd: Accept expected" << std::endl;
//    parser.parse({uint32_t(T::C), uint32_t(T::D), uint32_t(T::C), uint32_t(T::D), uint32_t(T::EOI)});
  }

  return 0;
//  cout << "start: " << dfa1.start << endl;
//  cout << "final: " << show(dfa1.final) << endl;
//  for (unsigned int q = 0; q < dfa1.stateCount; q++) {
//    for (unsigned int a = 0; a < dfa1.symbolCount; a++)
//      cout << "(" << q << "," << a << ") -> " << dfa1.T[q][a] << endl;
//  }

//  myLexer lex(dfa1);
//  cout << "> ";
//  cin.get(lex.c, 4096);
//  auto ll = cin.gcount();
//  cout << "read " << ll << " characters" << endl;
//  lex.c[ll] = EOF;
//  Parser pp(lex);
//  auto ast_node = pp.parse();
//  cout << *ast_node << endl;
//  delete ast_node;

//  while (*lex.c != EOF) {
//    Token* t = lex.getToken();
//    cout << "\ngot \"";
//    t->print(cout);
//    cout << "\"" << endl;
//    delete t;
//  }
  return 0;
}
