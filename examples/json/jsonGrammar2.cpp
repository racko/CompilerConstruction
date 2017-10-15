#include "jsonGrammar2.h"
#undef NULL

namespace json2 {
using type = uint8_t;

enum class T : type {
    BEGIN_ARRAY, BEGIN_OBJECT, END_ARRAY, END_OBJECT, NAME_SEPARATOR, VALUE_SEPARATOR, FALSE, NULL, TRUE, NUM, STRING, WS, EOI, EPS
};

enum class NT : type {
    START, JSON_TEXT, VALUE, OBJECT, MEMBER, MEMBERS, ARRAY, VALUES
};

namespace {
constexpr type numberOfNonterminals = type(NT::VALUES) + 1;
constexpr type numberOfTerminals = type(T::EPS) + 1;

const char* nt_strings[] = {
    "start", "json-text", "value", "object", "member", "members", "array", "values"
};

const char* t_strings[] = {
    "begin-array", "begin-object", "end-array", "end-object", "name-separator", "value-separator", "false", "null", "true", "number", "string", "ws", "EOF", "EPS"
};

grammar2::Grammar::Productions makeProductions() {
    grammar2::Grammar::Productions productions(numberOfNonterminals);

    productions[(type)NT::START] = {
        {NT::JSON_TEXT}
    };

    productions[(type)NT::JSON_TEXT] = {
        {NT::VALUE}
    };

    productions[(type)NT::VALUE] = {
        {T::FALSE}, {T::NULL}, {T::TRUE}, {NT::OBJECT}, {NT::ARRAY}, {T::NUM}, {T::STRING}
    };

    productions[(type)NT::OBJECT] = {
        {T::BEGIN_OBJECT, NT::MEMBER, NT::MEMBERS, T::END_OBJECT},
        {T::BEGIN_OBJECT, T::END_OBJECT}
    };

    productions[(type)NT::MEMBER] = {
        {T::STRING, T::NAME_SEPARATOR, NT::VALUE}
    };

    productions[(type)NT::MEMBERS] = {
        {T::EPS}, {T::VALUE_SEPARATOR, NT::MEMBER, NT::MEMBERS}
    };

    productions[(type)NT::ARRAY] = {
        {T::BEGIN_ARRAY, NT::VALUE, NT::VALUES, T::END_ARRAY},
        {T::BEGIN_ARRAY, T::END_ARRAY}
    };

    productions[(type)NT::VALUES] = {
        {T::EPS}, {T::VALUE_SEPARATOR, NT::VALUE, NT::VALUES}
    };

    return productions;
}
} // namespace
const grammar2::Grammar grammar(numberOfTerminals,
            numberOfNonterminals,
            NT::START,
            T::EPS,
            T::EOI,
            std::vector<const char*>{std::begin(t_strings), std::end(t_strings)},
            std::vector<const char*>{std::begin(nt_strings), std::end(nt_strings)},
            makeProductions());
} // namespace json
