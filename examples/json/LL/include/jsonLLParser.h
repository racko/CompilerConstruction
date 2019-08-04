#pragma once
#include "llParser.h"
#include "jsonLLGrammar.h"
#include "jsonValue.h"

#include <array>
#include <list>

namespace jsonLL {
struct Parser : public ll_parser::LLParser<Grammar, json::value*> {
};
}
