#include <json_token.h>

namespace json {
const char* nt_strings[] = {"start", "json-text", "value", "object", "member", "members", "array", "values"};

const char* t_strings[] = {"begin-array",     "begin-object", "end-array", "end-object", "name-separator",
                           "value-separator", "false",        "null",      "true",       "number",
                           "string",          "ws",           "EOF",       "EPS"};

std::ostream& operator<<(std::ostream& s, T const& a) { return s << t_strings[type(a)]; }

std::ostream& operator<<(std::ostream& s, NT const& A) { return s << nt_strings[type(A)]; }

kind kindOf(GrammarElement X) {
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
}
