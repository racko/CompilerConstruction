#ifndef _REGEX_H_
#define _REGEX_H_

#include "Print.h"
#include "DFA.h"
#include <sstream>
#include <iostream>
#include <iomanip>

template<class T>
struct Lexer {
    const char* c;
    using Symbol = char;
    using State = uint16_t;
    using TokenId = uint8_t;
    using DFA_t = DFA<Symbol,State,TokenId>;
    DFA_t dfa;

    Lexer() = default;

    virtual T action(const char*, size_t, TokenId) = 0;

    template<class T1>
    Lexer(T1&& _dfa) : dfa(std::forward<T1>(_dfa)) { }

    virtual T eofToken() const = 0;
    virtual T whiteSpaceToken() const = 0;
    T getToken();

    virtual ~Lexer() = default;

    struct iterator {
        Lexer& lex;
        T token;

        iterator(Lexer& l) : lex(l), token(lex.getToken()) {}

        const T& operator*() const { return token; }
        // required to move from token. Is there a better solution?
        T& operator*() { return token; }
        iterator& operator++() { token = lex.getToken(); return *this; }
    };

    iterator begin() { return iterator(*this); }
};
template<class T>
T Lexer<T>::getToken() {
    if (*c == EOF) {
        return eofToken();
    }
    //cout << "entered getToken()" << endl;
    auto s = dfa.start;
    auto f = dfa.final[s];
    //cout << "starting in state " << s << ", type " << f << endl;
    // TODO: make sure c points to input?
    const auto start = c;
    auto lastFinal = c;
    auto current = c;
    const auto fptr = dfa.final.data();
    const auto Tptr = dfa.T.data();
    const auto sptr = dfa.symbolToId.data();
    const auto scount = dfa.symbolCount;
    const auto dstate = dfa.deadState;
    while(/**current != EOF && */s != dstate) {
        //cout << "got '" << showCharEscaped(*current) << "'" << endl;
        //auto _c = dfa.symbolToId.at(*current);
        const auto _c = sptr[*current];
        //if (_c == scount) {
        //    throw std::runtime_error("invalid symbol '" + printEscaped(*current) + "'");
        //}
        s = Tptr[s * scount + _c];
        const auto ff = fptr[s];
        if (ff) {
            f = ff;
            lastFinal = current;
        }
        //cout << "new state " << s << ", type " << fptr[s] << ", last final: " << f << endl;
        //cout << "read " << (current - start + 1) << ", current token length: " << (lastFinal - start + 1) << endl;
        ++current;
        //cin.get();
    }

    //if (*current == EOF)
    //  cout << "got EOF" << endl;
    //if (s == dstate)
    //  cout << "ran into dead end" << endl;

    if (f != 0) {
        c = lastFinal + 1;
        auto tok = action(start, lastFinal - start + 1, f);
        if (tok != whiteSpaceToken())
            return tok;
        else
            return getToken();
    //} else if (*current == EOF) {
    //    c = current;
    //    return eofToken();
    } else {
        throw std::runtime_error("Lexical error");
    }
}
#endif
