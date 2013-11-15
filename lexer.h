#ifndef _LEXER_H_
#define _LEXER_H_

#include "Regex.h"
#include <iostream>
using std::ostream;
using std::cerr;
using std::cout;
#include <iomanip>
using std::endl;
#include <exception>
using std::exception;
#include <string>
using std::string;
#include <array>
using std::array;
#include <sstream>
using std::stringstream;
#undef NDEBUG
#include <cassert>

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
    GE,
    WS,
  };
  unsigned int tag;
  Token(unsigned int t) : tag(t) {}
  virtual ~Token() {}
  virtual void print(ostream& s) const = 0;
};

template<class T>
struct TokenBase : public Token {
  T val;
  
  TokenBase(unsigned int t, const T& v) : Token(t), val(v) {
    cout << "TokenBase(" << t << ", \"" << v << "\")" << endl;
  }

  virtual void print(ostream& s) const {
    s << val;
  }
};

struct Keyword : public TokenBase<string> {
  static const array<string, 3> strs;
  Keyword(unsigned int op) : TokenBase<string>(op, op >= unsigned(Token::type::LET) && op <= unsigned(Token::type::LETREC) ? strs[op - unsigned(Token::type::LET)] : "") {
    if (op < unsigned(Token::type::LET) || op > unsigned(Token::type::LETREC)) {
      cerr << "invalid keyword: " << op << endl;
      throw exception();
    }
  }
};

const array<string, 3> Keyword::strs{{"let", "in", "letrec"}};

struct BinOp : public TokenBase<string> {
  static const array<string, 4> strs;
  BinOp(unsigned int op) : TokenBase<string>(op, op >= (unsigned)Token::type::LE && op <= (unsigned)Token::type::GE ? strs[op - (unsigned)Token::type::LE] : op < 256 ? string(1, (char)op) : "") {
    if ((op < (unsigned)Token::type::LE || op > (unsigned)Token::type::GE) && op >= 256) {
      cerr << "invalid binop: " << op << endl;
      throw exception();
    }
  }
};

const array<string, 4> BinOp::strs = {{"<=", "==", "!=", ">="}};

struct Num : public TokenBase<unsigned long long> {
  Num(unsigned long long n) : TokenBase<unsigned long long>((unsigned)Token::type::NUM, n) {}
};

struct Other : public TokenBase<char> {
  Other(char c) : TokenBase<char>((unsigned)c, c) {}
};

struct Var : public TokenBase<string> {
  Var(string s) : TokenBase<string>((unsigned)Token::type::VAR, s) {}
};

struct Whitespace : public TokenBase<char> {
  Whitespace() : TokenBase<char>((unsigned)Token::type::WS, ' ') {}
};

struct myLexer : public Lexer<Token*> {
  static const vector<unsigned int> m;

  template<class T>
  myLexer(T&& dfa) : Lexer(std::forward<T>(dfa)) {}

  Token* eofToken() {
    return new Other(EOF);
  }

  Token* action(char* s, unsigned int n, unsigned int t) {
    cout << "action(\"";
    cout.write(s, n);
    cout << "\", " << n << ", " << t << ")" << endl;
    Token* tok;
    assert(t < m.size());
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
          ss.write(s, n);
          ss >> i;
          tok = new Num(i);
          break;
        }
      case (unsigned)Token::type::WS:
        tok = new Whitespace();
        break;
      default:
        cerr << "invalid token type: " << t << endl;
        throw exception();
    }
    return tok;
  }
};

const vector<unsigned int> myLexer::m{0, ';', '=', '.', '(', ')', '+', '-', '*', '/', '<', '>', '&', '|', '\\',
    (unsigned)Token::type::LE, (unsigned)Token::type::EQ, (unsigned)Token::type::NE, (unsigned)Token::type::GE,
    (unsigned)Token::type::LET, (unsigned)Token::type::IN, (unsigned)Token::type::LETREC,
    (unsigned)Token::type::VAR, (unsigned)Token::type::NUM, (unsigned)Token::type::WS};

#endif
