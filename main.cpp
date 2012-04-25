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
#include <array>
using std::array;

struct Token {
  enum struct type : unsigned int {
    LET = 256,
    IN,
    LETREC,
    VAR,
    NUM,
    LE,
    EQ,
    NE,
    GE
  };
  unsigned int tag;
  Token(unsigned int t) : tag(t) {}
  virtual ~Token() {}
  virtual void print(ostream& s) = 0;
};

template<class T>
struct TokenBase : public Token {
  T val;
  
  TokenBase(unsigned int t, const T& v) : Token(t), val(v) {
    cout << "TokenBase: copy constructor" << endl;
  }
  virtual void print(ostream& s) {
    s << val;
  }
};

struct Keyword : public TokenBase<string> {
  static array<string, 3> strs;
  Keyword(unsigned int op) : TokenBase<string>(op, op >= unsigned(Token::type::LET) && op <= unsigned(Token::type::LETREC) ? strs[op - unsigned(Token::type::LET)] : nullptr) {
    if (op < unsigned(Token::type::LET) || op > unsigned(Token::type::LETREC)) {
      cerr << "invalid keyword: " << op << endl;
      throw exception();
    }
  }
};

array<string, 3> Keyword::strs = {{"let", "in", "letrec"}};

struct BinOp : public TokenBase<string> {
  static array<string, 4> strs;
  BinOp(unsigned int op) : TokenBase<string>(op, op >= (unsigned)Token::type::LE && op <= (unsigned)Token::type::GE ? strs[op - (unsigned)Token::type::LE] : op < 256 ? string(1, (char)op) : nullptr) {
    if ((op < (unsigned)Token::type::LE || op > (unsigned)Token::type::GE) && op >= 256) {
      cerr << "invalid binop: " << op << endl;
      throw exception();
    }
  }
};

array<string, 4> BinOp::strs = {{"<=", "==", "!=", ">="}};

struct Num : public TokenBase<unsigned long long> {
  Num(unsigned long long n) : TokenBase<unsigned long long>((unsigned)Token::type::NUM, n) {}
};

struct Other : public TokenBase<char> {
  Other(char c) : TokenBase<char>((unsigned)c, c) {}
};

struct Var : public TokenBase<string> {
  Var(string s) : TokenBase<string>((unsigned)Token::type::VAR, s) {}
};

struct myLexer : public Lexer<Token*> {
  vector<unsigned int> m;
  myLexer(const DFA& dfa)
    : Lexer(dfa),
      m{0, ';', '=', '.', '(', ')', '+', '-', '*', '/', '<', '>', '&', '|', '\\',
        (unsigned)Token::type::LE, (unsigned)Token::type::EQ, (unsigned)Token::type::NE, (unsigned)Token::type::GE,
        (unsigned)Token::type::LET, (unsigned)Token::type::IN, (unsigned)Token::type::LETREC,
        (unsigned)Token::type::VAR, (unsigned)Token::type::NUM} {}
  Token* action(char* s, unsigned int n, unsigned int t) {
    cout << "action(" << n << ", " << t << ")" << endl;
    Token* tok;
    unsigned int c = m[t];
    switch(c) {
      case ';':
      case '=':
      case '.':
      case '(':
      case ')':
      case '\\':
        tok = new Other(c);
        break;
      case '+':
      case '-':
      case '*':
      case '/':
      case '<':
      case '>':
      case '&':
      case '|':
      case (unsigned)Token::type::LE:
      case (unsigned)Token::type::EQ:
      case (unsigned)Token::type::NE:
      case (unsigned)Token::type::GE:
        tok = new BinOp(c);
        break;
      case (unsigned)Token::type::LET:
      case (unsigned)Token::type::IN:
      case (unsigned)Token::type::LETREC:
        tok = new Keyword(c);
        break;
      case (unsigned)Token::type::VAR:
        tok = new Var(string(s, n));
        break;
      case (unsigned)Token::type::NUM:
        {
          unsigned long long i;
          stringstream ss;
          ss.read(s, n);
          ss >> i;
          tok = new Num(i);
          break;
        }
      default:
        cerr << "invalid token type: " << t << endl;
        throw exception();
    }
    return tok;
  }
};

int main(int argc, char** args) {

//  if (argc == 1) {
//    cerr << "too few arguments:" << endl;
//    for (int i = 1; i < argc; i++)
//      cerr << "'" << args[i] << "'" << endl;
//    return 1;
//  }

  stringstream in;
  nfaBuilder builder;

  char alpha[] = "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z)";
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
  while (*lex.c != EOF) {
    Token* t = lex.getToken();
    cout << "\ngot \"";
    t->print(cout);
    cout << "\"" << endl;
  }
  return 0;
}
