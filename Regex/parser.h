#pragma once

#include "Regex/token_stream.h"

template<typename OutputType, typename TokenType>
struct Parser {
    virtual OutputType parse(TokenStream<TokenType>& w) = 0;
    virtual ~Parser() = default;
};
