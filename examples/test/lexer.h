#ifndef _LEXER_H_
#define _LEXER_H_

#include "Regex.h"
#include "nfaBuilder.h"
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

  DFA_t getDFA() {
      stringstream in;
      nfaBuilder<Symbol,State> builder;

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
      NFA<Symbol,State> nfa1(builder);
//      {
//        std::ofstream nfa_dot{"nfa.dot"};
//        nfa_dot << nfa1;
//        nfa_dot.close();
//      }

      DFA_t dfa1(nfa1);
//      {
//        std::ofstream dfa_dot{"dfa.dot"};
//        dfa_dot << dfa1;
//        dfa_dot.close();
//      }
    //  cout << "start: " << dfa1.start << endl;
    //  cout << "final: " << show(dfa1.final) << endl;
    //  for (unsigned int q = 0; q < dfa1.stateCount; q++) {
    //    for (unsigned int a = 0; a < dfa1.symbolCount; a++)
    //      cout << "(" << q << "," << a << ") -> " << dfa1.T[q][a] << endl;
    //  }
      dfa1.minimize();
//      {
//        std::ofstream dfa_dot{"dfa_minimized.dot"};
//        dfa_dot << dfa1;
//        dfa_dot.close();
//      }
      return dfa1;
  }


  myLexer() : Lexer(getDFA()) {}

  Token* eofToken() const {
    return new Other(EOF);
  }

  Token* whiteSpaceToken() const {
    return nullptr;
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
          ss.exceptions(std::ifstream::failbit | std::ifstream::badbit);
          ss.write(s, n);
          ss >> i;
          tok = new Num(i);
          break;
        }
      case (unsigned)Token::type::WS:
        tok = nullptr;
        break;
      default:
      {
        stringstream ss;
        ss << "invalid token type: " << t;
        throw std::runtime_error(ss.str());
      }

    }
    return tok;
  }
};

const vector<unsigned int> myLexer::m{0, ';', '=', '.', '(', ')', '+', '-', '*', '/', '<', '>', '&', '|', '\\',
    (unsigned)Token::type::LE, (unsigned)Token::type::EQ, (unsigned)Token::type::NE, (unsigned)Token::type::GE,
    (unsigned)Token::type::LET, (unsigned)Token::type::IN, (unsigned)Token::type::LETREC,
    (unsigned)Token::type::VAR, (unsigned)Token::type::NUM, (unsigned)Token::type::WS};

#endif
