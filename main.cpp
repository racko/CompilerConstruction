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

struct MyParser : public LRParser<AST_Node*, Token*> {
  using LRParser<AST_Node*, Token*>::LRParser;
  AST_Node* reduce(Grammar::NonterminalID A, uint32_t production, std::vector<AST_Node*>&& alpha) const override {
    return nullptr;
  }

  AST_Node* shift(Token* const & t) const override {
    switch(t->tag) {
    case (unsigned)Token::type::VAR:
    {
      auto var = dynamic_cast<Var*>(t);
      return new VarNode(move(var->val));
    }
    case (unsigned)Token::type::NUM:
    {
      auto num = dynamic_cast<Num*>(t);
      return new NumNode(move(num->val));
    }
    default:
      return nullptr;
    }
  }
};

template<>
struct Token_traits<Token*> {
  static uint32_t getTag(Token* const & t) { return t->tag; }
};

template<>
struct Token_traits<While::Token> {
  static uint32_t getTag(While::Token const & t) { return t; }
};

int main(int argc, char** args) {
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

    While::Parser parser(G);
    //print(std::cout, G, C);
    auto state_count = parser.action_table.size();

    for (auto i = 0u; i < state_count; ++i) {
      for (auto a = 0u; a < G.getNumberOfTerminals(); ++a) {
        print(std::cout, G, parser.action_table[i][a]);
        std::cout << " ";
      }
      std::cout << std::endl;
    }

    for (auto i = 0u; i < state_count; ++i) {
      for (auto A = 0u; A < G.getNumberOfNonterminals(); ++A) {
        std::cout << parser.goto_table[i][A] << " ";
      }
      std::cout << std::endl;
    }

//    std::cout << "parsing NUM" << std::endl;
//    parser.parse({uint32_t(T::NUM), uint32_t(T::EOI)});
//    std::cout << "parsing NUM + NUM" << std::endl;
//    parser.parse({uint32_t(T::NUM), uint32_t(T::PLUS), uint32_t(T::NUM), uint32_t(T::EOI)});
//    std::cout << "parsing NUM * (NUM + NUM)" << std::endl;
//    parser.parse({uint32_t(T::NUM), uint32_t(T::TIMES), uint32_t(T::LEFTPAR), uint32_t(T::NUM), uint32_t(T::PLUS), uint32_t(T::NUM), uint32_t(T::RIGHTPAR), uint32_t(T::EOI)});
    std::cout << "parsing ID[ID] = NUM + NUM" << std::endl;
    Grammar::String input({uint32_t(T::ID), uint32_t(T::LEFTBR), uint32_t(T::ID), uint32_t(T::RIGHTBR), uint32_t(T::ASSIGN), uint32_t(T::NUM), uint32_t(T::PLUS), uint32_t(T::NUM), uint32_t(T::EOI)});
    parser.parse(input.begin());

//    std::cout << "parsing cd: Fail expected" << std::endl;
//    parser.parse({uint32_t(T::C), uint32_t(T::D), uint32_t(T::EOI)});
//    std::cout << "parsing ccd: Fail expected" << std::endl;
//    parser.parse({uint32_t(T::C), uint32_t(T::C), uint32_t(T::D), uint32_t(T::EOI)});
//    std::cout << "parsing cdcd: Accept expected" << std::endl;
//    parser.parse({uint32_t(T::C), uint32_t(T::D), uint32_t(T::C), uint32_t(T::D), uint32_t(T::EOI)});
  }

//  cout << "start: " << dfa1.start << endl;
//  cout << "final: " << show(dfa1.final) << endl;
//  for (unsigned int q = 0; q < dfa1.stateCount; q++) {
//    for (unsigned int a = 0; a < dfa1.symbolCount; a++)
//      cout << "(" << q << "," << a << ") -> " << dfa1.T[q][a] << endl;
//  }

//  myLexer lex;
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



  Grammar G(productions, start, eof, terminalCount, nonterminalCount);

  myLexer lex;
  MyParser parser;

  parser.parse(lex.begin());
  return 0;
}
