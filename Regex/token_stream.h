#pragma once

template <typename TokenType>
class TokenStream {
  public:
    virtual void step() = 0;
    virtual TokenType& peek() = 0;
    virtual const TokenType& peek() const = 0;
    virtual ~TokenStream() = default;
};
