#pragma once

#include "json_token.h"   // for Token
#include <memory>         // for unique_ptr
#include "Regex/token_stream.h" // for TokenStream

namespace json {
struct myLexer : public TokenStream<Token> {

    myLexer();
    ~myLexer();

    void setText(const char*);

    void step() override;
    const Token& peek() const override;
    Token& peek() override;

    struct impl;
    std::unique_ptr<impl> pimpl;
};
} // namespace json
