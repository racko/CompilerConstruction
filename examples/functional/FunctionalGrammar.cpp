#include "FunctionalGrammar.h"


void Functional::Token::accept(TokenVisitor& v) {
    v.visit(*this);
}

void Functional::Token::accept(TokenConstVisitor& v) const {
    v.visit(*this);
}

using namespace Functional;
using Functional::T;
using Functional::NT;

std::unordered_map<NonterminalID<Functional::Grammar>, std::vector<Functional::Grammar::String>> makeProductions();

const NonterminalID<Functional::Grammar> Functional::Grammar::start = NonterminalID<Functional::Grammar>(NT::START); // uint32_t(NT::ASSIGN) - uint32_t(T::EPS) - 1
const TerminalID<Functional::Grammar> Functional::Grammar::eof = TerminalID<Functional::Grammar>(T::EOI);
const TerminalID<Functional::Grammar> Functional::Grammar::eps = TerminalID<Functional::Grammar>(T::EPS);
const Functional::type Functional::Grammar::numberOfNonterminals = Functional::type(NT::L) + 1;
const Functional::type Functional::Grammar::numberOfTerminals = Functional::type(T::EPS) + 1;
const std::unordered_map<NonterminalID<Functional::Grammar>, std::vector<Functional::Grammar::String>> Functional::Grammar::productions = makeProductions();
const std::unordered_map<GrammarElement<Functional::Grammar>, std::unordered_set<TerminalID<Functional::Grammar>>> Functional::Grammar::firsts = allFirsts<Functional::Grammar>();


const char* Functional::nt_strings[] = {
    "START", "P", "S", "D", "C", "A", "B", "L"
};

const char* Functional::t_strings[] = {
    "SEMICOLON", "ASSIGN", "PERIOD", "LPAREN", "RPAREN", "PLUS", "MINUS", "TIMES", "DIV", "LT", "GT", "AND", "OR", "BSL", "LE", "EQ", "NE", "GE", "LET", "IN", "LETREC", "VAR", "NUM", "WS", "EOF", "EPS"
};

std::unordered_map<NonterminalID<Functional::Grammar>, std::vector<Functional::Grammar::String>> makeProductions() {
  std::unordered_map<NonterminalID<Functional::Grammar>, std::vector<Functional::Grammar::String>> productions(Functional::Grammar::numberOfNonterminals);

  productions[NT::START] = {
      {NT::P}
  };

  productions[NT::P] = {
      {T::EPS}, {NT::P, NT::S, T::SEMICOLON}
  };

  productions[NT::S] = {
      {T::VAR, T::ASSIGN, NT::A}, {T::VAR, NT::D, T::ASSIGN, NT::A}
  };

  productions[NT::A] = {
      {T::BSL, NT::D, T::PERIOD, NT::A}, {NT::L}
  };

  productions[NT::L] = {
      {T::LET, NT::P, T::IN, NT::L}, {T::LETREC, NT::P, T::IN, NT::L}, {NT::B}
  };

  productions[NT::B] = {
      {NT::B, NT::C}, {NT::C}
  };

  productions[NT::C] = {
      {T::LPAREN, NT::A, T::RPAREN}, {T::VAR}, {T::NUM}
  };

  productions[NT::D] = {
      {T::VAR, NT::D}, {T::VAR}
  };

  return productions;
}

const std::vector<Functional::Grammar::String>& Functional::Grammar::getProductions(NonterminalID<Functional::Grammar> A) {
  return productions.at(A);
}

kind Functional::Grammar::kindOf(GrammarElement<Functional::Grammar> X) {
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

const std::unordered_set<TerminalID<Functional::Grammar>>& Functional::Grammar::getFirsts(const GrammarElement<Functional::Grammar>& X) {
  return firsts.at(X);
}
