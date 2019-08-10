#include "json.h"

#include "jsonLexer.h"    // for myLexer
#include "parser.h"       // for Parser
#include <jsonLRParser.h> // for Parser
#include <utility>        // for move

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
