#include "json.h"

#include "jsonLexer.h"
#include <jsonLRParser.h>
#include <jsonValue.h>

namespace json {
JsonParser::JsonParser() : lex_(std::make_unique<json::myLexer>()), parser_(std::make_unique<jsonLR::Parser>()) {}

JsonParser::JsonParser(std::unique_ptr<Parser> parser)
    : lex_(std::make_unique<json::myLexer>()), parser_(std::move(parser)) {}

JsonParser::~JsonParser() = default;

json::value* JsonParser::run(const char* text) {
    lex_->setText(text);
    return parser_->parse(*lex_);
}
} // namespace json
