#include "While.h"
#include "lrParser.h"
#include "Regex.h"
#include "nfaBuilder.h"
#include "NFA.h"
#include <iostream>

#undef NDEBUG
#include <cassert>

namespace While {
struct AST_Node {};

struct Parser : public LRParser<Grammar, AST_Node*> {
  using LRParser<Grammar, AST_Node*>::LRParser;
  AST_Node* reduce(NonterminalID<Grammar> A, uint32_t production, std::vector<AST_Node*>&& alpha) const override { return nullptr; }
  AST_Node* shift(Token const * t) const override {
    return nullptr;
  }
};

struct myLexer : public Lexer<Token*> {
  DFA getDFA() {
      std::stringstream in;
      nfaBuilder builder;

      char alpha[] = "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)";
      char digit[] = "(0|1|2|3|4|5|6|7|8|9)";
      char special[] = "(!|\"|#|$|%|&|'|\\(|\\)|\\*|+|,|-|.|/|:|;|<|=|>|?|@|[|\\\\|]|^|_|`|{|\\||}|~)";
      std::stringstream varch;
      varch << "(" << alpha << "|" << digit << "|_)";
      std::stringstream num;
      num << digit << digit << "*";
      auto numstr = num.str();
      std::stringstream var;
      var << alpha << varch.rdbuf() << "*";
      std::stringstream str;
      str << "\"(" << alpha << "|" << digit << "|" << special << "| )*\"";

      in << "def";
      auto p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::DEF + 1;
      in.clear();
      in << "{";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::LEFTCURLY + 1;
      in.clear();
      in << "}";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::RIGHTCURLY + 1;
      in.clear();
      in << ";";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::SEMICOLON + 1;
      in.clear();
      in << "(int|double|bool|string)";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::BASIC + 1;
      in.clear();
      in << "record";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::RECORD + 1;
      in.clear();
      in << "if";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::IF + 1;
      in.clear();
      in << "else";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::ELSE + 1;
      in.clear();
      in << "while";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::WHILE + 1;
      in.clear();
      in << "do";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::DO + 1;
      in.clear();
      in << "break";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::BREAK + 1;
      in.clear();
      in << "continue";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::CONTINUE + 1;
      in.clear();
      in << "return";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::RETURN + 1;
      in.clear();
      in << "[";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::LEFTBR + 1;
      in.clear();
      in << "]";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::RIGHTBR + 1;
      in.clear();
      in << ".";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::PERIOD + 1;
      in.clear();
      in << "=";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::ASSIGN + 1;
      in.clear();
      in << "\\|\\|";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::OR + 1;
      in.clear();
      in << "&&";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::AND + 1;
      in.clear();
      in << "==";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::EQ + 1;
      in.clear();
      in << "!=";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::NEQ + 1;
      in.clear();
      in << "<";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::LT + 1;
      in.clear();
      in << "<=";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::LE + 1;
      in.clear();
      in << ">=";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::GE + 1;
      in.clear();
      in << ">";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::GT + 1;
      in.clear();
      in << "+";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::PLUS + 1;
      in.clear();
      in << "-";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::MINUS + 1;
      in.clear();
      in << "\\*";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::TIMES + 1;
      in.clear();
      in << "/";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::DIV + 1;
      in.clear();
      in << "!";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::NOT + 1;
      in.clear();
      in << "\\(";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::LEFTPAR + 1;
      in.clear();
      in << "\\)";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::RIGHTPAR + 1;
      in.clear();
      in << "true";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::TRUE + 1;
      in.clear();
      in << "false";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::FALSE + 1;
      in.clear();
      in << ",";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::COMMA + 1;
      in.clear();
      in << "( |\t|\n)( |\t|\n)*";
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::WS + 1;
      in.clear();
      in << numstr << "." << numstr;
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::REAL + 1;
      in.clear();
      in << numstr;
      p = builder.lexRegex(in);
      builder.ns[p.second].kind = (unsigned)T::NUM + 1;
      p = builder.lexRegex(str);
      builder.ns[p.second].kind = (unsigned)T::STRING + 1;
      p = builder.lexRegex(var);
      builder.ns[p.second].kind = (unsigned)T::ID + 1;


      NFA nfa1(builder);
      DFA dfa1(nfa1);
      dfa1.minimize();

      return dfa1;
  }


  myLexer() : Lexer(getDFA()) {}

  Token* eofToken() const {
    return new Token(TerminalID<Grammar>(T::EOI));
  }

  Token* whiteSpaceToken() const {
    return nullptr;
  }

  Token* action(char* s, unsigned int n, unsigned int t) {
    cout << "action(\"";
    cout.write(s, n);
    cout << "\", " << n << ", " << t << ")" << endl;
    Token* tok;

    switch(TerminalID<Grammar>(t - 1)) {
    case T::DEF:
      tok = new Token(T::DEF);
      break;
    case T::LEFTCURLY:
      tok = new Token(T::LEFTCURLY);
      break;
    case T::RIGHTCURLY:
      tok = new Token(T::RIGHTCURLY);
      break;
    case T::SEMICOLON:
      tok = new Token(T::SEMICOLON);
      break;
    case T::BASIC:
      tok = new Token(T::BASIC);
      break;
    case T::RECORD:
      tok = new Token(T::RECORD);
      break;
    case T::IF:
      tok = new Token(T::IF);
      break;
    case T::ELSE:
      tok = new Token(T::ELSE);
      break;
    case T::WHILE:
      tok = new Token(T::WHILE);
      break;
    case T::DO:
      tok = new Token(T::DO);
      break;
    case T::BREAK:
      tok = new Token(T::BREAK);
      break;
    case T::CONTINUE:
      tok = new Token(T::CONTINUE);
      break;
    case T::RETURN:
      tok = new Token(T::RETURN);
      break;
    case T::LEFTBR:
      tok = new Token(T::LEFTBR);
      break;
    case T::RIGHTBR:
      tok = new Token(T::RIGHTBR);
      break;
    case T::ID:
      tok = new Token(T::ID);
      break;
    case T::PERIOD:
      tok = new Token(T::PERIOD);
      break;
    case T::ASSIGN:
      tok = new Token(T::ASSIGN);
      break;
    case T::OR:
      tok = new Token(T::OR);
      break;
    case T::AND:
      tok = new Token(T::AND);
      break;
    case T::EQ:
      tok = new Token(T::EQ);
      break;
    case T::NEQ:
      tok = new Token(T::NEQ);
      break;
    case T::LT:
      tok = new Token(T::LT);
      break;
    case T::LE:
      tok = new Token(T::LE);
      break;
    case T::GE:
      tok = new Token(T::GE);
      break;
    case T::GT:
      tok = new Token(T::GT);
      break;
    case T::PLUS:
      tok = new Token(T::PLUS);
      break;
    case T::MINUS:
      tok = new Token(T::MINUS);
      break;
    case T::TIMES:
      tok = new Token(T::TIMES);
      break;
    case T::DIV:
      tok = new Token(T::DIV);
      break;
    case T::NOT:
      tok = new Token(T::NOT);
      break;
    case T::LEFTPAR:
      tok = new Token(T::LEFTPAR);
      break;
    case T::RIGHTPAR:
      tok = new Token(T::RIGHTPAR);
      break;
    case T::NUM:
      tok = new Token(T::NUM);
      break;
    case T::REAL:
      tok = new Token(T::REAL);
      break;
    case T::TRUE:
      tok = new Token(T::TRUE);
      break;
    case T::FALSE:
      tok = new Token(T::FALSE);
      break;
    case T::STRING:
      tok = new Token(T::STRING);
      break;
    case T::COMMA:
      tok = new Token(T::COMMA);
      break;
    case T::WS:
      tok = nullptr;
      break;
    default:
    {
      std::stringstream ss;
      ss << "invalid token type: " << t;
      throw std::runtime_error(ss.str());
    }

    }
    return tok;
  }
};

}

int main() {
//  std::stringstream ss;
//  auto coutBuf = std::cout.rdbuf();
//  std::cout.rdbuf(ss.rdbuf());
  While::myLexer lex;
//  std::cout.rdbuf(coutBuf);

  for (auto i = 0u; i < While::Grammar::getNumberOfTerminals(); ++i)
    assert(TerminalID<While::Grammar>(i) == (TerminalID<While::Grammar>)(GrammarElement<While::Grammar>)TerminalID<While::Grammar>(i));
  for (auto i = 0u; i < While::Grammar::getNumberOfNonterminals(); ++i)
    assert(NonterminalID<While::Grammar>(i) == (NonterminalID<While::Grammar>)(GrammarElement<While::Grammar>)NonterminalID<While::Grammar>(i));
  for (auto i = 0u; i < While::Grammar::getNumberOfGrammarElements(); ++i) {
    switch(While::Grammar::kindOf(GrammarElement<While::Grammar>(i))) {
    case kind::TERMINAL:
      assert(GrammarElement<While::Grammar>(i) == (GrammarElement<While::Grammar>)(TerminalID<While::Grammar>)GrammarElement<While::Grammar>(i));
      break;
    case kind::EPS:
      assert(GrammarElement<While::Grammar>(i) == (GrammarElement<While::Grammar>)(TerminalID<While::Grammar>)GrammarElement<While::Grammar>(i));
      break;
    case kind::EOI:
      assert(GrammarElement<While::Grammar>(i) == (GrammarElement<While::Grammar>)(TerminalID<While::Grammar>)GrammarElement<While::Grammar>(i));
      break;
    case kind::NONTERMINAL:
      assert(GrammarElement<While::Grammar>(i) == (GrammarElement<While::Grammar>)(NonterminalID<While::Grammar>)GrammarElement<While::Grammar>(i));
      break;
    default:
      throw std::logic_error("Error in GrammarElement test");
    }
  }

  const auto& fs = While::Grammar::getFirsts();
  for (const auto& f : fs) {
    std::cout << f.first << ": ";
    for (const auto& a : f.second)
      std::cout << a << " ";
    std::cout << std::endl;
  }

  While::Parser parser;

  cout << "> ";
  cin.get(lex.c, 4096);
  auto ll = cin.gcount();
  cout << "read " << ll << " characters" << endl;
  std::cout << "parsing \"" << lex.c << "\"" << std::endl;
  lex.c[ll] = EOF;
  parser.parse(lex.begin());

//  std::cout << "parsing DEF BASIC ID() { ID[ID] = NUM + NUM; }" << std::endl;
//  typedef While::T T;
//  auto tokens = std::initializer_list<T>{{T::DEF, T::BASIC, T::ID, T::LEFTPAR, T::RIGHTPAR, T::LEFTCURLY, T::ID, T::LEFTBR, T::ID, T::RIGHTBR, T::ASSIGN, T::NUM, T::PLUS, T::NUM, T::SEMICOLON, T::RIGHTCURLY, T::EOI}};
//  std::vector<While::Token> input(tokens.begin(), tokens.end());
//  parser.parse(input.begin());



  return 0;
}

