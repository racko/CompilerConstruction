#include "json.h"
#include "jsonLexer.h"
#include "jsonParser.h"

JsonParser::JsonParser() : lex(std::make_unique<myLexer>()), parser(std::make_unique<Parser>()) {}
JsonParser::~JsonParser() = default;

value* JsonParser::run(const char* text) {
    lex->setText(text);
    return parser->parse(lex->begin());
}
