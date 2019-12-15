#pragma once
#include "Regex/Token.h"

using TokenId = int; // otherwise I need to cast the second argument to the lexRegex calls
constexpr TokenId eof_terminalid = 11;
constexpr TokenId ws_terminalid = 12;

// struct Token;

/*constexpr*/ Token<TokenId> bla(const char* text);
