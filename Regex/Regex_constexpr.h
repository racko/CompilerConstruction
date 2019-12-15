#pragma once

#include "Regex/DFA_constexpr.h"
#include "Regex/Token.h"

template <size_t MaxNodes,
          size_t MaxSymbols,
          typename Symbol = char,
          typename State = uint16_t,
          typename TokenId = uint8_t>
struct Lexer {
    const char* c{};
    using DFA_t = DFA<Symbol, State, TokenId, MaxNodes, MaxSymbols>;

    // constexpr Lexer(DFA_t dfa, Token<TokenId> eof, TokenId whitespace, const char* text) : dfa_(std::move(dfa)),
    // eof_(eof), whitespace_(whitespace), c(text) {} constexpr Lexer(DFA_t dfa, Token<TokenId> eof, TokenId whitespace)
    // : dfa_(std::move(dfa)), eof_(eof), whitespace_(whitespace) {}
    constexpr Lexer(const DFA_t& dfa, Token<TokenId> eof, TokenId whitespace)
        : dfa_(dfa), eof_(eof), whitespace_(whitespace) {}

    constexpr Token<TokenId> getToken();

  private:
    const DFA_t& dfa_;
    Token<TokenId> eof_;
    TokenId whitespace_;
};

template <size_t MaxNodes, size_t MaxSymbols, typename Symbol, typename State, typename TokenId>
constexpr Token<TokenId> Lexer<MaxNodes, MaxSymbols, Symbol, State, TokenId>::getToken() {
    if (*c == '\0') {
        return eof_;
    }
    // cout << "entered getToken()" << endl;
    auto s = dfa_.start;
    auto f = dfa_.finals[s];
    // cout << "starting in state " << s << ", type " << f << endl;
    const auto start = c;
    auto lastFinal = c;
    auto current = c;
    const auto fptr = dfa_.finals.data();
    const auto Tptr = dfa_.T.data();
    const auto sptr = dfa_.symbolToId.data();
    const auto scount = dfa_.symbolCount;
    const auto dstate = dfa_.deadState;
    /// we need to check current for eof because eof is not in symbolToId ... or rather symbolToId[eof] ==
    /// symbolToId.size()
    /// @TODO: extend T with this one transition for symbolToId.size() that points to the dead state
    while (*current != '\0' && s != dstate) {
        // cout << "got '" << showCharEscaped(*current) << "'" << endl;
        // auto _c = dfa_.symbolToId.at(*current);
        const auto _c = sptr[*current];
        // if (_c == scount) {
        //    throw std::runtime_error("invalid symbol '" + printEscaped(*current) + "'");
        //}
        s = Tptr[s * scount + _c];
        const auto ff = fptr[s];
        if (ff) {
            f = ff;
            lastFinal = current;
        }
        // cout << "new state " << s << ", type " << fptr[s] << ", last final: " << f << endl;
        // cout << "read " << (current - start + 1) << ", current token length: " << (lastFinal - start + 1) << endl;
        ++current;
        // cin.get();
    }

    // if (*current == EOF)
    //  cout << "got EOF" << endl;
    // if (s == dstate)
    //  cout << "ran into dead end" << endl;

    if (f != 0) {
        c = lastFinal + 1;
        if (f != whitespace_)
            return {start, static_cast<size_t>(lastFinal - start + 1), f};
        else
            return getToken();
        //} else if (*current == EOF) {
        //    c = current;
        //    return eofToken();
    } else {
        throw std::runtime_error("Lexical error");
    }
}
