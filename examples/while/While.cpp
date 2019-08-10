#include "While.h"

#include <algorithm> // for copy
#include <iterator>  // for ostream_iterator
#include <stdexcept> // for logic_error

namespace While {
std::unordered_map<NonterminalID, std::vector<Grammar::String>> makeProductions();

const NonterminalID Grammar::start = NonterminalID(NT::S); // uint32_t(NT::ASSIGN) - uint32_t(T::EPS) - 1
const TerminalID Grammar::eof = TerminalID(T::EOI);
const TerminalID Grammar::eps = TerminalID(T::EPS);
const size_t Grammar::numberOfNonterminals = uint32_t(NT::ARGS) + 1;
const size_t Grammar::numberOfTerminals = uint32_t(T::EPS) + 1;
const std::unordered_map<NonterminalID, std::vector<Grammar::String>> Grammar::productions = makeProductions();
const std::unordered_map<GrammarElement, std::unordered_set<TerminalID>> Grammar::firsts = allFirsts<Grammar>();

const char* nt_strings[] = {"S",    "PROGRAM", "FUNCS", "FUNC",   "OPTPARAMS", "PARAMS",  "BLOCK", "DECLS", "DECL",
                            "TYPE", "STMTS",   "STMT",  "LOC",    "ASSIGN",    "BOOL",    "JOIN",  "EQ",    "REL",
                            "EXPR", "TERM",    "UNARY", "FACTOR", "FUNCALL",   "OPTARGS", "ARGS"};

const char* t_strings[] = {"DEF",   "LEFTCURLY", "RIGHTCURLY", "SEMICOLON", "BASIC", "RECORD", "IF",  "ELSE", "WHILE",
                           "DO",    "BREAK",     "CONTINUE",   "RETURN",    "[",     "]",      "ID",  ".",    "=",
                           "||",    "&&",        "==",         "!=",        "<",     "<=",     ">=",  ">",    "+",
                           "-",     "*",         "/",          "!",         "(",     ")",      "NUM", "REAL", "TRUE",
                           "FALSE", "STRING",    ",",          "WS",        "EOF",   "EPS"};

std::unordered_map<NonterminalID, std::vector<Grammar::String>> makeProductions() {
    std::unordered_map<NonterminalID, std::vector<Grammar::String>> productions(Grammar::numberOfNonterminals);

    productions[NT::S] = {{NT::PROGRAM}};
    productions[NT::PROGRAM] = {{NT::FUNCS}};
    productions[NT::FUNCS] = {{NT::FUNC, NT::FUNCS}, {T::EPS}};
    productions[NT::FUNC] = {{T::DEF, NT::TYPE, T::ID, T::LEFTPAR, NT::OPTPARAMS, T::RIGHTPAR, NT::BLOCK},
                             {T::DEF, NT::TYPE, T::ID, T::LEFTPAR, NT::OPTPARAMS, T::RIGHTPAR, T::SEMICOLON}};
    productions[NT::OPTPARAMS] = {{NT::PARAMS}, {T::EPS}};
    productions[NT::PARAMS] = {{NT::TYPE, T::ID, T::COMMA, NT::PARAMS}, {NT::TYPE, T::ID}};
    productions[NT::BLOCK] = {{T::LEFTCURLY, NT::DECLS, NT::STMTS, T::RIGHTCURLY}};
    productions[NT::DECLS] = {{NT::DECLS, NT::DECL}, {T::EPS}};
    productions[NT::DECL] = {{NT::TYPE, T::ID, T::SEMICOLON}};
    productions[NT::TYPE] = {
        {NT::TYPE, T::LEFTBR, T::NUM, T::RIGHTBR}, {T::BASIC}, {T::RECORD, T::LEFTCURLY, NT::DECLS, T::RIGHTCURLY}};
    productions[NT::STMTS] = {{NT::STMTS, NT::STMT}, {T::EPS}};
    productions[NT::STMT] = {{NT::ASSIGN, T::SEMICOLON},
                             {T::IF, T::LEFTPAR, NT::ASSIGN, T::RIGHTPAR, NT::STMT},
                             {T::IF, T::LEFTPAR, NT::ASSIGN, T::RIGHTPAR, NT::STMT, T::ELSE, NT::STMT},
                             {T::WHILE, T::LEFTPAR, NT::ASSIGN, T::RIGHTPAR, NT::STMT},
                             {T::DO, NT::STMT, T::WHILE, T::LEFTPAR, NT::ASSIGN, T::SEMICOLON},
                             {T::BREAK, T::SEMICOLON},
                             {T::CONTINUE, T::SEMICOLON},
                             {T::RETURN, T::SEMICOLON},
                             {T::RETURN, NT::ASSIGN, T::SEMICOLON},
                             {NT::BLOCK}};
    productions[NT::LOC] = {{NT::LOC, T::LEFTBR, NT::ASSIGN, T::RIGHTBR}, {T::ID}, {NT::LOC, T::PERIOD, T::ID}};
    productions[NT::ASSIGN] = {{NT::BOOL, T::ASSIGN, NT::ASSIGN}, {NT::BOOL}};
    productions[NT::BOOL] = {{NT::BOOL, T::OR, NT::JOIN}, {NT::JOIN}};
    productions[NT::JOIN] = {{NT::JOIN, T::AND, NT::EQ}, {NT::EQ}};
    productions[NT::EQ] = {{NT::EQ, T::EQ, NT::REL}, {NT::EQ, T::NEQ, NT::REL}, {NT::REL}};
    productions[NT::REL] = {{NT::EXPR, T::LT, NT::EXPR},
                            {NT::EXPR, T::LE, NT::EXPR},
                            {NT::EXPR, T::GE, NT::EXPR},
                            {NT::EXPR, T::GT, NT::EXPR},
                            {NT::EXPR}};
    productions[NT::EXPR] = {{NT::EXPR, T::PLUS, NT::TERM}, {NT::EXPR, T::MINUS, NT::TERM}, {NT::TERM}};
    productions[NT::TERM] = {{NT::TERM, T::TIMES, NT::UNARY}, {NT::TERM, T::DIV, NT::UNARY}, {NT::UNARY}};
    productions[NT::UNARY] = {{T::NOT, NT::UNARY}, {T::MINUS, NT::UNARY}, {NT::FACTOR}};
    productions[NT::FACTOR] = {{T::LEFTPAR, NT::ASSIGN, T::RIGHTPAR},
                               {NT::LOC},
                               {T::NUM},
                               {T::REAL},
                               {T::TRUE},
                               {T::FALSE},
                               {T::STRING},
                               {NT::FUNCALL}};
    productions[NT::FUNCALL] = {{T::ID, T::LEFTPAR, NT::OPTARGS, T::RIGHTPAR}};
    productions[NT::OPTARGS] = {{NT::ARGS}, {T::EPS}};
    productions[NT::ARGS] = {{NT::ASSIGN, T::COMMA, NT::ARGS}, {NT::ASSIGN}};

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
} // namespace While
