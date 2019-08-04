#pragma once

#include <memory>
#include <token_stream.h>

namespace json {
struct Token;
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
