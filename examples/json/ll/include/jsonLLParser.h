#pragma once

#include "Regex/llParser.h" // for LLParser
#include "jsonLLGrammar.h"  // for Grammar

namespace json {
struct value;
}

namespace jsonLL {
struct Parser : public ll_parser::LLParser<Grammar, json::value*> {};
} // namespace jsonLL
