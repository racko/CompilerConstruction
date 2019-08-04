#include "FunctionalGrammar.h"

namespace Functional {
void Token::accept(TokenVisitor& v) { v.visit(*this); }

void Token::accept(TokenConstVisitor& v) const { v.visit(*this); }

std::unordered_map<NonterminalID, std::vector<Grammar::String>> makeProductions();

const NonterminalID Grammar::start = NonterminalID(NT::START); // uint32_t(NT::ASSIGN) - uint32_t(T::EPS) - 1
const TerminalID Grammar::eof = TerminalID(T::EOI);
const TerminalID Grammar::eps = TerminalID(T::EPS);
const type Grammar::numberOfNonterminals = type(NT::L) + 1;
const type Grammar::numberOfTerminals = type(T::EPS) + 1;
const std::unordered_map<NonterminalID, std::vector<Grammar::String>> Grammar::productions = makeProductions();
const std::unordered_map<GrammarElement, std::unordered_set<TerminalID>> Grammar::firsts = allFirsts<Grammar>();

const char* nt_strings[] = {"START", "P", "S", "D", "C", "A", "B", "L"};

const char* t_strings[] = {"SEMICOLON", "ASSIGN", "PERIOD", "LPAREN", "RPAREN", "PLUS", "MINUS", "TIMES", "DIV",
                           "LT",        "GT",     "AND",    "OR",     "BSL",    "LE",   "EQ",    "NE",    "GE",
                           "LET",       "IN",     "LETREC", "VAR",    "NUM",    "WS",   "EOF",   "EPS"};

std::unordered_map<NonterminalID, std::vector<Grammar::String>> makeProductions() {
    std::unordered_map<NonterminalID, std::vector<Grammar::String>> productions(Grammar::numberOfNonterminals);

    productions[NT::START] = {{NT::P}};

    productions[NT::P] = {{T::EPS}, {NT::P, NT::S, T::SEMICOLON}};

    productions[NT::S] = {{T::VAR, T::ASSIGN, NT::A}, {T::VAR, NT::D, T::ASSIGN, NT::A}};

    productions[NT::A] = {{T::BSL, NT::D, T::PERIOD, NT::A}, {NT::L}};

    productions[NT::L] = {{T::LET, NT::P, T::IN, NT::L}, {T::LETREC, NT::P, T::IN, NT::L}, {NT::B}};

    productions[NT::B] = {{NT::B, NT::C}, {NT::C}};

    productions[NT::C] = {{T::LPAREN, NT::A, T::RPAREN}, {T::VAR}, {T::NUM}};

    productions[NT::D] = {{T::VAR, NT::D}, {T::VAR}};

    return productions;
}

const std::vector<Grammar::String>& Grammar::getProductions(NonterminalID A) { return productions.at(A); }

kind kindOf(GrammarElement X) {
    if (X.x == uint32_t(T::EPS))
        return kind::EPS;
    else if (X.x == uint32_t(T::EOI))
        return kind::EOI;
    else if (X.x < getNumberOfTerminals())
        return kind::TERMINAL;
    else if (X.x < getNumberOfGrammarElements())
        return kind::NONTERMINAL;
    else
        throw std::logic_error("kindOf: invalid GrammarElement");
}

const std::unordered_set<TerminalID>& Grammar::getFirsts(const GrammarElement& X) { return firsts.at(X); }

std::ostream& operator<<(std::ostream& s, TerminalID const& a) { return s << T(a); }

std::ostream& operator<<(std::ostream& s, NonterminalID const& A) { return s << NT(A); }

std::ostream& operator<<(std::ostream& s, GrammarElement const X) {
    switch (kindOf(X)) {
    case kind::TERMINAL:
        return s << TerminalID(X);
    case kind::EPS:
        return s << "EPS";
    case kind::EOI:
        return s << "EOF";
    case kind::NONTERMINAL:
        return s << NonterminalID(X);
    default:
        throw std::logic_error("unhandled case in operator<<(std::ostream& s, GrammarElement const X)");
    }
}

std::ostream& operator<<(std::ostream& s, const std::vector<GrammarElement>& alpha) {
    std::copy(alpha.begin(), alpha.end(), std::ostream_iterator<GrammarElement>(s, " "));
    return s;
}

std::ostream& operator<<(std::ostream& s, const std::list<GrammarElement>& alpha) {
    std::copy(alpha.begin(), alpha.end(), std::ostream_iterator<GrammarElement>(s, " "));
    return s;
}

std::ostream& operator<<(std::ostream& s, const std::unordered_set<TerminalID>& alpha) {
    std::copy(alpha.begin(), alpha.end(), std::ostream_iterator<TerminalID>(s, " "));
    return s;
}
} // namespace Functional
