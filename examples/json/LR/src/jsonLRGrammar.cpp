#include "jsonLRGrammar.h"

#include "Grammar.h"           // for allFirsts, allFollows
#include "json_token.h"        // for T, NT, NT::VALUE, NonterminalID, opera...

namespace jsonLR {
using namespace json;
const NonterminalID Grammar::start = NonterminalID(NT::START);
const TerminalID Grammar::eof = TerminalID(T::EOI);
const TerminalID Grammar::eps = TerminalID(T::EPS);
std::vector<std::vector<Grammar::String>> makeProductions();

const std::vector<std::vector<Grammar::String>> Grammar::productions = makeProductions();
const std::unordered_map<GrammarElement, std::unordered_set<TerminalID>> Grammar::firsts = allFirsts<Grammar>();
const std::unordered_map<NonterminalID, std::unordered_set<TerminalID>> Grammar::follows = allFollows<Grammar>();

std::vector<std::vector<Grammar::String>> makeProductions() {
    std::vector<std::vector<Grammar::String>> productions(Grammar::numberOfNonterminals);

    productions[(unsigned)NT::START] = {{NT::JSON_TEXT}};

    productions[(unsigned)NT::JSON_TEXT] = {{NT::VALUE}};

    productions[(unsigned)NT::VALUE] = {{T::FALSE},  {T::NULL}, {T::TRUE},  {NT::OBJECT},
                                        {NT::ARRAY}, {T::NUM},  {T::STRING}};

    productions[(unsigned)NT::OBJECT] = {{T::BEGIN_OBJECT, NT::MEMBER, NT::MEMBERS, T::END_OBJECT},
                                         {T::BEGIN_OBJECT, T::END_OBJECT}};

    productions[(unsigned)NT::MEMBER] = {{T::STRING, T::NAME_SEPARATOR, NT::VALUE}};

    productions[(unsigned)NT::MEMBERS] = {{T::EPS}, {T::VALUE_SEPARATOR, NT::MEMBER, NT::MEMBERS}};

    productions[(unsigned)NT::ARRAY] = {{T::BEGIN_ARRAY, NT::VALUE, NT::VALUES, T::END_ARRAY},
                                        {T::BEGIN_ARRAY, T::END_ARRAY}};

    productions[(unsigned)NT::VALUES] = {{T::EPS}, {T::VALUE_SEPARATOR, NT::VALUE, NT::VALUES}};

    return productions;
}

const std::vector<Grammar::String>& Grammar::getProductions(NonterminalID A) { return productions[A.x]; }

const std::unordered_set<TerminalID>& Grammar::getFirsts(const GrammarElement& X) { return firsts.at(X); }

const std::unordered_set<TerminalID>& Grammar::getFollows(const NonterminalID& A) { return follows.at(A); }
} // namespace jsonLR
