#include "jsonLRGrammar2.h"

#include "Grammar2.h" // for Grammar::Productions, GrammarElement
#include <cstdint>    // for uint8_t
#include <iterator>   // for begin, end
#include <vector>     // for vector

#undef NULL

namespace jsonLR2 {
using type = uint8_t;

enum class T : type {
    BEGIN_ARRAY,
    BEGIN_OBJECT,
    END_ARRAY,
    END_OBJECT,
    NAME_SEPARATOR,
    VALUE_SEPARATOR,
    FALSE,
    NULL,
    TRUE,
    NUM,
    STRING,
    WS,
    EOI,
    EPS
};

enum class NT : type { START, JSON_TEXT, VALUE, OBJECT, MEMBER, MEMBERS, ARRAY, VALUES };

namespace {
constexpr type numberOfNonterminals = type(NT::VALUES) + 1;
constexpr type numberOfTerminals = type(T::EPS) + 1;

const char* nt_strings[] = {"start", "json-text", "value", "object", "member", "members", "array", "values"};

const char* t_strings[] = {"begin-array",
                           "begin-object",
                           "end-array",
                           "end-object",
                           "name-separator",
                           "value-separator",
                           "false",
                           "null",
                           "true",
                           "number",
                           "string",
                           "ws",
                           "EOF",
                           "EPS"};

grammar2::GrammarElement toGE(const T x) { return grammar2::GrammarElement((uint8_t)x); };
grammar2::GrammarElement toGE(const NT x) { return grammar2::GrammarElement((uint8_t)x + numberOfTerminals); };

grammar2::Grammar::Productions makeProductions() {
    grammar2::Grammar::Productions productions(numberOfNonterminals);

    productions[(type)NT::START] = {{toGE(NT::JSON_TEXT)}};

    productions[(type)NT::JSON_TEXT] = {{toGE(NT::VALUE)}};

    productions[(type)NT::VALUE] = {{toGE(T::FALSE)},
                                    {toGE(T::NULL)},
                                    {toGE(T::TRUE)},
                                    {toGE(NT::OBJECT)},
                                    {toGE(NT::ARRAY)},
                                    {toGE(T::NUM)},
                                    {toGE(T::STRING)}};

    productions[(type)NT::OBJECT] = {{toGE(T::BEGIN_OBJECT), toGE(NT::MEMBER), toGE(NT::MEMBERS), toGE(T::END_OBJECT)},
                                     {toGE(T::BEGIN_OBJECT), toGE(T::END_OBJECT)}};

    productions[(type)NT::MEMBER] = {{toGE(T::STRING), toGE(T::NAME_SEPARATOR), toGE(NT::VALUE)}};

    productions[(type)NT::MEMBERS] = {{toGE(T::EPS)}, {toGE(T::VALUE_SEPARATOR), toGE(NT::MEMBER), toGE(NT::MEMBERS)}};

    productions[(type)NT::ARRAY] = {{toGE(T::BEGIN_ARRAY), toGE(NT::VALUE), toGE(NT::VALUES), toGE(T::END_ARRAY)},
                                    {toGE(T::BEGIN_ARRAY), toGE(T::END_ARRAY)}};

    productions[(type)NT::VALUES] = {{toGE(T::EPS)}, {toGE(T::VALUE_SEPARATOR), toGE(NT::VALUE), toGE(NT::VALUES)}};

    return productions;
}
} // namespace
const grammar2::Grammar grammar(numberOfTerminals,
                                numberOfNonterminals,
                                (grammar2::NonterminalID)(uint8_t)NT::START,
                                (grammar2::TerminalID)(uint8_t)T::EPS,
                                (grammar2::TerminalID)(uint8_t)T::EOI,
                                std::vector<const char*>{std::begin(t_strings), std::end(t_strings)},
                                std::vector<const char*>{std::begin(nt_strings), std::end(nt_strings)},
                                makeProductions());
} // namespace jsonLR2
