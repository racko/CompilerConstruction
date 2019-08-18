#pragma once

#include "jsonLLGrammar.h" // for Grammar
#include "llParser.h"      // for LLParser

namespace json {
struct value;
}

namespace jsonLL {
struct Parser : public ll_parser::LLParser<Grammar, json::value*> {};
} // namespace jsonLL
