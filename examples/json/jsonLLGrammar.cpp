#include "jsonLLGrammar.h"

//void jsonLL::Token::accept(TokenVisitor& v) {
//    v.visit(*this);
//}
//
//void jsonLL::Token::accept(TokenConstVisitor& v) const {
//    v.visit(*this);
//}

namespace jsonLL {

const NonterminalID<Grammar> Grammar::start = NonterminalID<Grammar>(NT::START);
const TerminalID<Grammar> Grammar::eof = TerminalID<Grammar>(T::EOI);
const TerminalID<Grammar> Grammar::eps = TerminalID<Grammar>(T::EPS);
//std::unordered_map<NonterminalID<Grammar>, std::vector<Grammar::String>> makeProductions();
std::vector<std::vector<Grammar::String>> makeProductions();

//const std::unordered_map<NonterminalID<Grammar>, std::vector<Grammar::String>> Grammar::productions = makeProductions();
const std::vector<std::vector<Grammar::String>> Grammar::productions = makeProductions();
const std::unordered_map<GrammarElement<Grammar>, std::unordered_set<TerminalID<Grammar>>> Grammar::firsts = allFirsts<Grammar>();
const std::unordered_map<NonterminalID<Grammar>, std::unordered_set<TerminalID<Grammar>>> Grammar::follows = allFollows<Grammar>();

const char* nt_strings[] = {
    "start", "json-text", "value", "object", "member", "members", "array", "values"
};

const char* t_strings[] = {
    "begin-array", "begin-object", "end-array", "end-object", "name-separator", "value-separator", "false", "null", "true", "number", "string", "ws", "EOF", "EPS"
};

std::vector<std::vector<Grammar::String>> makeProductions() {
    std::vector<std::vector<Grammar::String>> productions(Grammar::numberOfNonterminals);

    productions[(unsigned)NT::START] = {
        {NT::JSON_TEXT}
    };

    productions[(unsigned)NT::JSON_TEXT] = {
        {NT::VALUE}
    };

    productions[(unsigned)NT::VALUE] = {
        {T::FALSE}, {T::NULL}, {T::TRUE}, {T::BEGIN_OBJECT, NT::OBJECT, T::END_OBJECT}, {T::BEGIN_ARRAY, NT::ARRAY, T::END_ARRAY}, {T::NUM}, {T::STRING}
    };

    productions[(unsigned)NT::OBJECT] = {
        {NT::MEMBER, NT::MEMBERS},
        {T::EPS}
    };

    productions[(unsigned)NT::MEMBER] = {
        {T::STRING, T::NAME_SEPARATOR, NT::VALUE}
    };

    productions[(unsigned)NT::MEMBERS] = {
        {T::EPS}, {T::VALUE_SEPARATOR, NT::MEMBER, NT::MEMBERS}
    };

    productions[(unsigned)NT::ARRAY] = {
        {NT::VALUE, NT::VALUES},
        {T::EPS}
    };

    productions[(unsigned)NT::VALUES] = {
        {T::EPS}, {T::VALUE_SEPARATOR, NT::VALUE, NT::VALUES}
    };

    return productions;
}

const std::vector<Grammar::String>& Grammar::getProductions(NonterminalID<Grammar> A) {
    //return productions.at(A);
    return productions[A.x];
}

kind Grammar::kindOf(GrammarElement<Grammar> X) {
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

const std::unordered_set<TerminalID<Grammar>>& Grammar::getFirsts(const GrammarElement<Grammar>& X) {
    return firsts.at(X);
}

const std::unordered_set<TerminalID<Grammar>>& Grammar::getFollows(const NonterminalID<Grammar>& A) {
    return follows.at(A);
}

std::ostream& operator<<(std::ostream& s, T const& a) {
    return s << t_strings[type(a)];
}

std::ostream& operator<<(std::ostream& s, NT const& A) {
    return s << nt_strings[type(A)];
}

std::ostream& operator<<(std::ostream& s, TerminalID<Grammar> const& a) {
    return s << T(a);
}

std::ostream& operator<<(std::ostream& s, NonterminalID<Grammar> const& A) {
    return s << NT(A);
}
} // namespace jsonLL
