#include "Regex.h"

#include <cstdio>    // for EOF, size_t
#include <stdexcept> // for runtime_error
#include <vector>    // for vector

auto Lexer::getToken() -> Token {
    if (*c == EOF) {
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
    const auto scount = dfa_.symbols_.count();
    const auto dstate = dfa_.deadState;
    while (/**current != EOF && */ s != dstate) {
        // cout << "got '" << showCharEscaped(*current) << "'" << endl;
        // auto _c = dfa_.symbolToId.at(*current);
        const auto _c = dfa_.symbols_.symbolToId(*current);
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
