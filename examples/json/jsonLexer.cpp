#include "jsonLexer.h"

#include <NFA.h>
#include <Regex.h>
#include <json_token.h>
#include <nfaBuilder.h>

namespace json {
struct myLexer::impl {
    using type = json::type;
    using T = json::T;

    using DFA_t = Lexer::DFA_t;
    using Symbol = Lexer::Symbol;
    using State = Lexer::State;
    using TokenId = Lexer::TokenId;

    // static const vector<unsigned int> m;

    Lexer lexer;
    Token token;

    impl() : lexer(getDFA(), {nullptr, 0, static_cast<type>(T::EOI) + 1}, static_cast<type>(T::WS) + 1) {}

    // Token eofToken() const {
    //    return {T::EOI,boost::blank()};
    //}

    // Token whiteSpaceToken() const {
    //    return {T::WS,boost::blank()};
    //}

    static DFA_t getDFA();
    static Token action(const char* s, size_t n, Lexer::TokenId t);
    static Token t2t(Lexer::Token t) { return action(t.start, t.length, t.tokenId); }

    const Token& operator*() const { return token; }
    // required to move from token. Is there a better solution?
    Token& operator*() { return token; }
    impl& operator++() {
        token = t2t(lexer.getToken());
        return *this;
    }
};

myLexer::myLexer() : pimpl(std::make_unique<impl>()) {}
myLexer::~myLexer() = default;

void myLexer::setText(const char* text) {
    pimpl->lexer.c = text;
    pimpl->token = pimpl->t2t(pimpl->lexer.getToken());
}

void myLexer::step() { ++(*pimpl); }

auto myLexer::peek() -> Token& { return *(*pimpl); }

auto myLexer::peek() const -> const Token& { return *(*pimpl); }

// const jsonParserImpl::Token& myLexer::operator*() const { return *(*pimpl); }
// jsonParserImpl::Token& myLexer::operator*() { return *(*pimpl); }
// myLexer& myLexer::operator++() {
//    ++(*pimpl);
//    return *this;
//}

auto myLexer::impl::getDFA() -> DFA_t {
    std::stringstream in;
    std::cout << "nfaBuilder" << std::endl;
    nfaBuilder<Symbol, State, TokenId> builder;

    char hexdigit[] = "(0|1|2|3|4|5|6|7|8|9|a|b|c|d|e|f|A|B|C|D|E|F)";
    char nonzerodigit[] = "(1|2|3|4|5|6|7|8|9)";
    char ws[] = "( |\t|\n|\r)*";
    std::stringstream digit;
    digit << "(0|" << nonzerodigit << ")";
    std::stringstream integer;
    integer << "(0|" << nonzerodigit << digit.rdbuf() << "*)";
    digit.seekg(0);
    std::stringstream exp;
    exp << "((e|E)(-|+)?" << digit.rdbuf();
    digit.seekg(0);
    exp << digit.rdbuf() << "*)";
    digit.seekg(0);
    std::stringstream frac;
    frac << "(." << digit.rdbuf();
    digit.seekg(0);
    frac << digit.rdbuf() << "*)";
    digit.seekg(0);
    std::stringstream num;
    num << "(-?" << integer.rdbuf() << frac.rdbuf() << "?" << exp.rdbuf() << "?)";

    std::stringstream unescaped;
    unescaped << "( |!"; // 0x20, 0x21
    // 0x22 is ", so it has to be escaped
    for (char i = 0x23; i <= 0x27; ++i)
        unescaped << "|" << i;
    unescaped << "|\\(|\\)|\\*";
    for (char i = 0x2B; i <= 0x3E; ++i)
        unescaped << "|" << i;
    unescaped << "|\\?";
    for (char i = 0x40; i <= 0x5B; ++i)
        unescaped << "|" << i;
    // 0x5C is \ -> escape
    for (char i = 0x5D; i <= 0x7B; ++i)
        unescaped << "|" << i;
    unescaped << "|\\|";
    for (int i = 0x7D; i <= 0x7F;
         ++i) // int + static_cast because i <= 0x7F is always true for "char i" (infinite loop)
        unescaped << "|" << static_cast<char>(i);
    // only ascii for now ...
    unescaped << ")";

    std::stringstream chr;
    chr << "(" << unescaped.rdbuf() << "|\\\\(\"|\\\\|/|b|f|n|r|t|u" << hexdigit << hexdigit << hexdigit << hexdigit
        << "))";
    std::stringstream str;
    str << "(\"" << chr.rdbuf() << "*\")";

    in.clear();
    in << "false";
    builder.lexRegex(in, (unsigned)T::FALSE + 1);
    in.clear();
    in << "null";
    builder.lexRegex(in, (unsigned)T::NULL + 1);
    in.clear();
    in << "true";
    builder.lexRegex(in, (unsigned)T::TRUE + 1);
    in.clear();

    in << ws << "[" << ws;
    builder.lexRegex(in, (unsigned)T::BEGIN_ARRAY + 1);
    in.clear();
    in << ws << "{" << ws;
    builder.lexRegex(in, (unsigned)T::BEGIN_OBJECT + 1);
    in.clear();
    in << ws << "]" << ws;
    builder.lexRegex(in, (unsigned)T::END_ARRAY + 1);
    in.clear();
    in << ws << "}" << ws;
    builder.lexRegex(in, (unsigned)T::END_OBJECT + 1);
    in.clear();
    in << ws << ":" << ws;
    builder.lexRegex(in, (unsigned)T::NAME_SEPARATOR + 1);
    in.clear();
    in << ws << "," << ws;
    builder.lexRegex(in, (unsigned)T::VALUE_SEPARATOR + 1);

    //// slower than including ws in the tokens as we do above
    // in << "[";
    // builder.lexRegex(in, (unsigned)T::BEGIN_ARRAY + 1);
    // in.clear();
    // in << "{";
    // builder.lexRegex(in, (unsigned)T::BEGIN_OBJECT + 1);
    // in.clear();
    // in << "]";
    // builder.lexRegex(in, (unsigned)T::END_ARRAY + 1);
    // in.clear();
    // in << "}";
    // builder.lexRegex(in, (unsigned)T::END_OBJECT + 1);
    // in.clear();
    // in << ":";
    // builder.lexRegex(in, (unsigned)T::NAME_SEPARATOR + 1);
    // in.clear();
    // in << ",";
    // builder.lexRegex(in, (unsigned)T::VALUE_SEPARATOR + 1);

    builder.lexRegex(num, (unsigned)T::NUM + 1);
    builder.lexRegex(str, (unsigned)T::STRING + 1);
    in.clear();
    //// not required if we include ws in the structural tokens and don't use T::WS in the context-free grammar
    // in << "( |\t|\n|\r)" << ws;
    // builder.lexRegex(in, (unsigned)T::WS + 1);

    std::cout << "NFA" << std::endl;
    NFA<Symbol, State, TokenId> nfa1(builder);
    std::cout << "DFA" << std::endl;
    DFA_t dfa1(nfa1);
    std::cout << "minimal DFA" << std::endl;
    dfa1.minimize();
    return dfa1;
}

auto myLexer::impl::action(const char* s, size_t n, TokenId t) -> Token {
    // cout << "action(\"";
    // cout.write(s, n);
    // cout << "\", " << n << ", " << t << ")" << endl;
    auto tid = TerminalID(t - 1);
    switch (tid) {
    case T::TRUE:
        return {tid, true};
    case T::FALSE:
        return {tid, false};
    case T::NULL:
    case T::BEGIN_ARRAY:
    case T::BEGIN_OBJECT:
    case T::END_ARRAY:
    case T::END_OBJECT:
    case T::NAME_SEPARATOR:
    case T::VALUE_SEPARATOR:
        return {tid, null()};
    case T::STRING:
        return {T::STRING, std::string_view(s + 1, n - 2)}; // the arithmetic removes quotation marks
    case T::NUM: {
        return {T::NUM, num_view(s, n)};
    }
    case T::WS:
        return {T::WS, null()};
    case T::EOI:
        return {T::EOI, null()};
    default: {
        std::stringstream ss;
        ss << "invalid token type: " << int(t);
        throw std::runtime_error(ss.str());
    }
    }
}
} // namespace json
