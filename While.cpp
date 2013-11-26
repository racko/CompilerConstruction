#include "While.h"

using While::T;
using While::NT;

std::unordered_map<NonterminalID<While::Grammar>, std::vector<While::Grammar::String>> makeProductions();

const NonterminalID<While::Grammar> While::Grammar::start = NonterminalID<While::Grammar>(NT::S); // uint32_t(NT::ASSIGN) - uint32_t(T::EPS) - 1
const TerminalID<While::Grammar> While::Grammar::eof = TerminalID<While::Grammar>(T::EOI);
const TerminalID<While::Grammar> While::Grammar::eps = TerminalID<While::Grammar>(T::EPS);
const size_t While::Grammar::numberOfNonterminals = uint32_t(NT::ARGS) + 1;
const size_t While::Grammar::numberOfTerminals = uint32_t(T::EPS) + 1;
const std::unordered_map<NonterminalID<While::Grammar>, std::vector<While::Grammar::String>> While::Grammar::productions = makeProductions();
const std::unordered_map<GrammarElement<While::Grammar>, std::unordered_set<TerminalID<While::Grammar>>> While::Grammar::firsts = allFirsts<While::Grammar>();


const char* While::nt_strings[] = {
    "S", "PROGRAM", "FUNCS", "FUNC", "OPTPARAMS", "PARAMS", "BLOCK", "DECLS", "DECL", "TYPE", "STMTS", "STMT", "LOC", "ASSIGN", "BOOL", "JOIN", "EQ", "REL", "EXPR", "TERM", "UNARY", "FACTOR", "FUNCALL", "OPTARGS", "ARGS"
};

const char* While::t_strings[] = {
    "DEF", "LEFTCURLY", "RIGHTCURLY", "SEMICOLON", "BASIC", "RECORD", "IF", "ELSE", "WHILE", "DO", "BREAK", "CONTINUE", "RETURN", "[", "]", "ID", ".", "=", "||", "&&", "==", "!=", "<", "<=", ">=", ">", "+", "-", "*", "/", "!", "(", ")", "NUM", "REAL", "TRUE", "FALSE", "STRING", ",", "WS", "EOF",
    "EPS"
};

std::unordered_map<NonterminalID<While::Grammar>, std::vector<While::Grammar::String>> makeProductions() {
  std::unordered_map<NonterminalID<While::Grammar>, std::vector<While::Grammar::String>> productions(While::Grammar::numberOfNonterminals);

  productions[NT::S] = {
      {NT::PROGRAM}
  };
  productions[NT::PROGRAM] = {
      {NT::FUNCS}
  };
  productions[NT::FUNCS] = {
      {NT::FUNC, NT::FUNCS}, {T::EPS}
  };
  productions[NT::FUNC] = {
      {T::DEF, NT::TYPE, T::ID, T::LEFTPAR, NT::OPTPARAMS, T::RIGHTPAR, NT::BLOCK},
      {T::DEF, NT::TYPE, T::ID, T::LEFTPAR, NT::OPTPARAMS, T::RIGHTPAR, T::SEMICOLON}
  };
  productions[NT::OPTPARAMS] = {
      {NT::PARAMS}, {T::EPS}
  };
  productions[NT::PARAMS] = {
      {NT::TYPE, T::ID, T::COMMA, NT::PARAMS}, {NT::TYPE, T::ID}
  };
  productions[NT::BLOCK] = {
      {T::LEFTCURLY, NT::DECLS, NT::STMTS, T::RIGHTCURLY}
  };
  productions[NT::DECLS] = {
      {NT::DECLS, NT::DECL}, {T::EPS}
  };
  productions[NT::DECL] = {
      {NT::TYPE, T::ID, T::SEMICOLON}
  };
  productions[NT::TYPE] = {
      {NT::TYPE, T::LEFTBR, T::NUM, T::RIGHTBR}, {T::BASIC}, {T::RECORD, T::LEFTCURLY, NT::DECLS, T::RIGHTCURLY}
  };
  productions[NT::STMTS] = {
      {NT::STMTS, NT::STMT}, {T::EPS}
  };
  productions[NT::STMT] = {
      {NT::ASSIGN, T::SEMICOLON},
      {T::IF, T::LEFTPAR, NT::ASSIGN, T::RIGHTPAR, NT::STMT},
      {T::IF, T::LEFTPAR, NT::ASSIGN, T::RIGHTPAR, NT::STMT, T::ELSE, NT::STMT},
      {T::WHILE, T::LEFTPAR, NT::ASSIGN, T::RIGHTPAR, NT::STMT},
      {T::DO, NT::STMT, T::WHILE, T::LEFTPAR, NT::ASSIGN, T::SEMICOLON},
      {T::BREAK, T::SEMICOLON},
      {T::CONTINUE, T::SEMICOLON},
      {T::RETURN, T::SEMICOLON},
      {T::RETURN, NT::LOC, T::SEMICOLON},
      {NT::BLOCK}
  };
  productions[NT::LOC] = {
      {NT::LOC, T::LEFTBR, NT::ASSIGN, T::RIGHTBR}, {T::ID}, {NT::LOC, T::PERIOD, T::ID}
  };
  productions[NT::ASSIGN] = {
      {NT::BOOL, T::ASSIGN, NT::ASSIGN}, {NT::BOOL}
  };
  productions[NT::BOOL] = {
      {NT::BOOL, T::OR, NT::JOIN}, {NT::JOIN}
  };
  productions[NT::JOIN] = {
      {NT::JOIN, T::AND, NT::EQ}, {NT::EQ}
  };
  productions[NT::EQ] = {
      {NT::EQ, T::EQ, NT::REL}, {NT::EQ, T::NEQ, NT::REL}, {NT::REL}
  };
  productions[NT::REL] = {
      {NT::EXPR, T::LT, NT::EXPR}, {NT::EXPR, T::LE, NT::EXPR}, {NT::EXPR, T::GE, NT::EXPR}, {NT::EXPR, T::GT, NT::EXPR}, {NT::EXPR}
  };
  productions[NT::EXPR] = {
      {NT::EXPR, T::PLUS, NT::TERM}, {NT::EXPR, T::MINUS, NT::TERM}, {NT::TERM}
  };
  productions[NT::TERM] = {
      {NT::TERM, T::TIMES, NT::UNARY}, {NT::TERM, T::DIV, NT::UNARY}, {NT::UNARY}
  };
  productions[NT::UNARY] = {
      {T::NOT, NT::UNARY}, {T::MINUS, NT::UNARY}, {NT::FACTOR}
  };
  productions[NT::FACTOR] = {
      {T::LEFTPAR, NT::ASSIGN, T::RIGHTPAR}, {NT::LOC}, {T::NUM}, {T::REAL}, {T::TRUE}, {T::FALSE}, {T::STRING}, {NT::FUNCALL}
  };
  productions[NT::FUNCALL] = {
      {T::ID, T::LEFTPAR, NT::OPTARGS, T::RIGHTPAR}
  };
  productions[NT::OPTARGS] = {
      {NT::ARGS}, {T::EPS}
  };
  productions[NT::ARGS] = {
      {NT::ASSIGN, T::COMMA, NT::ARGS}, {NT::ASSIGN}
  };

  return productions;
}

const std::vector<While::Grammar::String>& While::Grammar::getProductions(NonterminalID<While::Grammar> A) {
  auto it = productions.find(A);
  if (it == productions.end())
    throw std::runtime_error("getProductions(NonterminalID<While::Grammar>): invalid nonterminal");
  return it->second;
//  return productions[A];
}

kind While::Grammar::kindOf(GrammarElement<While::Grammar> X) {
  if (X.x == uint32_t(T::EPS))
    return kind::EPS;
  else if (X.x == uint32_t(T::EOI))
    return kind::EOI;
  else if (X.x < numberOfTerminals)
    return kind::TERMINAL;
  else if (X.x < getNumberOfGrammarElements())
    return kind::NONTERMINAL;
  else
    throw std::logic_error("kindOf: invalid GrammarElement");
}

const std::unordered_set<TerminalID<While::Grammar>>& While::Grammar::getFirsts(const GrammarElement<While::Grammar>& X) {
  // workaround for compiler error: X was considered to have type const GrammarElement<While::Grammar> (missing &) when used with operator[].
  // even this should have worked with operator[const GrammarElement<While::Grammar>&], but didn't
  auto it = firsts.find(X);
  if (it == firsts.end())
    throw std::runtime_error("getFirsts: invalid grammar element");
  return it->second;
//  const std::unordered_map<GrammarElement<While::Grammar>, std::unordered_set<TerminalID<While::Grammar>>>::key_type& xx = X;
//  return firsts[xx];
}
