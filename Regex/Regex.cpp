#include "Regex/Regex.h"

#include <cstdio>    // for EOF, size_t
#include <stdexcept> // for runtime_error
#include <vector>    // for vector

auto Lexer::getToken() -> Token {
    // cout << "entered getToken()" << endl;
    if (*c_ == EOF) {
        return eof_;
    }

    const auto start = c_;
    const auto dstate = dfa_.GetDeadState();

    // "iterators"
    auto s = dfa_.GetStart();
    auto current = c_;

    // "accumulators"
    auto f = dfa_.GetTokenId(s);
    auto lastFinal = c_;

    // cout << "starting in state " << s << ", type " << f << endl;
    while (s != dstate) {
        // cout << "got '" << showCharEscaped(*current) << "'" << endl;
        // auto symbol = dfa_.symbolToId.at(*current);

        // if (symbol == scount) {
        //    throw std::runtime_error("invalid symbol '" + printEscaped(*current) + "'");
        //}
        s = dfa_.GetTransition(s, dfa_.symbolToId(*current));

        if (const auto ff = dfa_.GetTokenId(s); ff) {
            f = ff;
            lastFinal = current;
        }
        // cout << "new state " << s << ", type " << fptr[s] << ", last final: " << f << endl;
        // cout << "read " << (current - start + 1) << ", current token length: " << (lastFinal - start + 1) << endl;
        ++current;
    }

    if (f == 0) {
        throw std::runtime_error("Lexical error");
    }

    c_ = lastFinal + 1;
    return f != whitespace_ ? Token{start, static_cast<size_t>(lastFinal - start + 1), f} : getToken();
}
