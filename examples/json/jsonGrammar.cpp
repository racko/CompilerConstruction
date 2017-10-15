#include "jsonGrammar.h"

//void json::Token::accept(TokenVisitor& v) {
//    v.visit(*this);
//}
//
//void json::Token::accept(TokenConstVisitor& v) const {
//    v.visit(*this);
//}

using namespace json;
using json::T;
using json::NT;

const NonterminalID<Grammar> json::Grammar::start = NonterminalID<json::Grammar>(NT::START);
const TerminalID<Grammar> json::Grammar::eof = TerminalID<json::Grammar>(T::EOI);
const TerminalID<Grammar> json::Grammar::eps = TerminalID<json::Grammar>(T::EPS);
//std::unordered_map<NonterminalID<json::Grammar>, std::vector<json::Grammar::String>> makeProductions();
std::vector<std::vector<json::Grammar::String>> makeProductions();

//const std::unordered_map<NonterminalID<json::Grammar>, std::vector<json::Grammar::String>> json::Grammar::productions = makeProductions();
const std::vector<std::vector<json::Grammar::String>> json::Grammar::productions = makeProductions();
const std::unordered_map<GrammarElement<json::Grammar>, std::unordered_set<TerminalID<json::Grammar>>> json::Grammar::firsts = allFirsts<json::Grammar>();
const std::unordered_map<NonterminalID<json::Grammar>, std::unordered_set<TerminalID<json::Grammar>>> json::Grammar::follows = allFollows<json::Grammar>();

const char* json::nt_strings[] = {
    "start", "json-text", "value", "object", "member", "members", "array", "values"
};

const char* json::t_strings[] = {
    "begin-array", "begin-object", "end-array", "end-object", "name-separator", "value-separator", "false", "null", "true", "number", "string", "ws", "EOF", "EPS"
};

//std::unordered_map<NonterminalID<json::Grammar>, std::vector<json::Grammar::String>> makeProductions() {
//  std::unordered_map<NonterminalID<json::Grammar>, std::vector<json::Grammar::String>> productions(json::Grammar::numberOfNonterminals);
//
//  productions[NT::START] = {
//      {NT::JSON_TEXT}
//  };
//
//  productions[NT::JSON_TEXT] = {
//      {NT::VALUE}
//  };
//
//  productions[NT::VALUE] = {
//      {T::FALSE}, {T::NULL}, {T::TRUE}, {NT::OBJECT}, {NT::ARRAY}, {T::NUM}, {T::STRING}
//  };
//
//  productions[NT::OBJECT] = {
//      {T::BEGIN_OBJECT, NT::MEMBER, NT::MEMBERS, T::END_OBJECT},
//      {T::BEGIN_OBJECT, T::END_OBJECT}
//  };
//
//  productions[NT::MEMBER] = {
//      {T::STRING, T::NAME_SEPARATOR, NT::VALUE}
//  };
//
//  productions[NT::MEMBERS] = {
//      {T::EPS}, {T::VALUE_SEPARATOR, NT::MEMBER, NT::MEMBERS}
//  };
//
//  productions[NT::ARRAY] = {
//      {T::BEGIN_ARRAY, NT::VALUE, NT::VALUES, T::END_ARRAY},
//      {T::BEGIN_ARRAY, T::END_ARRAY}
//  };
//
//  productions[NT::VALUES] = {
//      {T::EPS}, {T::VALUE_SEPARATOR, NT::VALUE, NT::VALUES}
//  };
//
//  return productions;
//}

std::vector<std::vector<json::Grammar::String>> makeProductions() {
    std::vector<std::vector<json::Grammar::String>> productions(json::Grammar::numberOfNonterminals);

    productions[(unsigned)NT::START] = {
        {NT::JSON_TEXT}
    };

    productions[(unsigned)NT::JSON_TEXT] = {
        {NT::VALUE}
    };

    productions[(unsigned)NT::VALUE] = {
        {T::FALSE}, {T::NULL}, {T::TRUE}, {NT::OBJECT}, {NT::ARRAY}, {T::NUM}, {T::STRING}
    };

    productions[(unsigned)NT::OBJECT] = {
        {T::BEGIN_OBJECT, NT::MEMBER, NT::MEMBERS, T::END_OBJECT},
        {T::BEGIN_OBJECT, T::END_OBJECT}
    };

    productions[(unsigned)NT::MEMBER] = {
        {T::STRING, T::NAME_SEPARATOR, NT::VALUE}
    };

    productions[(unsigned)NT::MEMBERS] = {
        {T::EPS}, {T::VALUE_SEPARATOR, NT::MEMBER, NT::MEMBERS}
    };

    productions[(unsigned)NT::ARRAY] = {
        {T::BEGIN_ARRAY, NT::VALUE, NT::VALUES, T::END_ARRAY},
        {T::BEGIN_ARRAY, T::END_ARRAY}
    };

    productions[(unsigned)NT::VALUES] = {
        {T::EPS}, {T::VALUE_SEPARATOR, NT::VALUE, NT::VALUES}
    };

    return productions;
}

const std::vector<json::Grammar::String>& json::Grammar::getProductions(NonterminalID<json::Grammar> A) {
    //return productions.at(A);
    return productions[A.x];
}

kind json::Grammar::kindOf(GrammarElement<json::Grammar> X) {
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

const std::unordered_set<TerminalID<json::Grammar>>& json::Grammar::getFirsts(const GrammarElement<json::Grammar>& X) {
    return firsts.at(X);
}

const std::unordered_set<TerminalID<json::Grammar>>& json::Grammar::getFollows(const NonterminalID<json::Grammar>& A) {
    return follows.at(A);
}

std::ostream& operator<<(std::ostream& s, json::T const& a) {
    return s << json::t_strings[json::type(a)];
}

std::ostream& operator<<(std::ostream& s, json::NT const& A) {
    return s << json::nt_strings[json::type(A)];
}

std::ostream& operator<<(std::ostream& s, TerminalID<json::Grammar> const& a) {
    return s << json::T(a);
}

std::ostream& operator<<(std::ostream& s, NonterminalID<json::Grammar> const& A) {
    return s << json::NT(A);
}

