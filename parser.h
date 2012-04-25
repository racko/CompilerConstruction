#ifndef _PARSER_H_
#define _PARSER_H_

#include "lexer.h"
using std::forward;

struct AST_Node {
  virtual ~AST_Node() {}
  virtual void writeTo(ostream&) = 0;
};

ostream& operator<<(ostream& s, AST_Node& n) {
  n.writeTo(s);
  return s << endl;
}

struct AbsNode : public AST_Node {
  string id;
  AST_Node* body;
  AbsNode(string&& _id, AST_Node* _body) : id(forward<string>(_id)), body(_body) {}
  ~AbsNode() {
    if (body)
      delete body;
  }

  void writeTo(ostream& s) {
    s << "\\ " << id << " . (";
    body->writeTo(s);
    s << ")";
  }
};

struct AppNode : public AST_Node {
  AST_Node* a;
  AST_Node* b;
  AppNode(AST_Node* _a, AST_Node* _b) : a(_a), b(_b) {}
  ~AppNode() {
    if (a)
      delete a;
    if (b)
      delete b;
  }

  void writeTo(ostream&s) {
    s << "App (";
    a->writeTo(s);
    s << ") (";
    b->writeTo(s);
    s << ")";
  }
};

struct VarNode : public AST_Node {
  string id;
  VarNode(string&& _id) : id(forward<string>(_id)) {}
  void writeTo(ostream&s) {
    s << "Var " << id;
  }
};

struct NumNode : public AST_Node {
  unsigned long long n;
  NumNode(unsigned long long _n) : n(_n) {}
  void writeTo(ostream&s) {
    s << "Num " << n;
  }
};

struct Parser {
  myLexer& lex;
  Token* t;

  Parser(myLexer& l) : lex(l) {
    move();
  }

  void ignoreMatch(unsigned int type) {
    delete match(type);
  }

  Token* move() {
    cout << "move()" << endl;
    t = lex.getToken();
    while (t->tag == (unsigned)Token::type::WS)
      t = lex.getToken();
    return t;
  }

  Token* match(unsigned int type) {
    cout << "match(" << t->tag << ", " << type << ")" << endl;
    if (t->tag != type) {
      cerr << "expected " << type << ", got " << t->tag << endl;
      throw exception();
    }
    Token* tmp = t;
    move();
    return tmp;
  }

  AST_Node* parseC() {
    cout << "parseC()" << endl;
    switch (t->tag) {
      case '(':
        {
          ignoreMatch('(');
          AST_Node* a = parseA();
          ignoreMatch(')');
          return a;
        }
      case (unsigned)Token::type::VAR:
        {
          Var* id = dynamic_cast<Var*>(match((unsigned)Token::type::VAR)); //TODO: static_cast, if not debugging
          VarNode* b = new VarNode(std::move(id->val));
          delete id;
          return b;
        }
      case (unsigned)Token::type::NUM:
        {
          Num* c = dynamic_cast<Num*>(match((unsigned)Token::type::NUM)); //TODO: static_cast, if not debugging
          NumNode* b = new NumNode(c->val);
          delete c;
          return b;
        }
      default:
        cerr << "Error: " << t->tag << endl;
        throw exception();
    }
  }

  AST_Node* parseBR(AST_Node* a) {
    cout << "parseBR()" << endl;
    switch (t->tag) {
      case '(':
      case (unsigned)Token::type::VAR:
      case (unsigned)Token::type::NUM:
        return parseBR(new AppNode(a, parseC()));
        break;
      case ')':
      case EOF:
        return a;
      default:
        cerr << "Error: " << t->tag << endl;
        throw exception();
    }
  }

  AST_Node* parseB() {
    cout << "parseB()" << endl;
    switch (t->tag) {
      case '(':
      case (unsigned)Token::type::VAR:
      case (unsigned)Token::type::NUM:
        return parseBR(parseC());
      default:
        cerr << "Error: " << t->tag << endl;
        throw exception();
    }
  }

  AST_Node* parseA() {
    cout << "parseA()" << endl;
    switch (t->tag) {
      case '\\':
        {
          ignoreMatch('\\');
          Var* id = dynamic_cast<Var*>(match((unsigned)Token::type::VAR)); //TODO: static_cast, if not debugging
          ignoreMatch('.');
          AST_Node* n = new AbsNode(std::move(id->val), parseA());
          delete id;
          return n;
        }
      case '(':
      case (unsigned)Token::type::VAR:
      case (unsigned)Token::type::NUM:
        return parseB();
      default:
        cerr << "Error: " << t->tag << endl;
        throw exception();
    }
  }

  AST_Node* parse() {
    return parseA();
  }
};

#endif
