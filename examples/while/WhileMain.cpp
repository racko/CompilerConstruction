#include "Grammar.h"      // for kind, kind::EOI, kind::EPS, kind::NONTERMINAL
#include "NFA.h"          // for NFA
#include "NFA_to_DFA.h"   // for toDFA
#include "Regex.h"        // for Lexer, Lexer::Token, Lexer::DFA_t, Lexer::...
#include "While.h"        // for Token, T, Grammar, operator<<, GrammarElement
#include "lrParser.h"     // for LRParser
#include "nfaBuilder.h"   // for nfaBuilder, istream
#include "token_stream.h" // for TokenStream
#include <cassert>        // for assert
#include <cstdint>        // for uint32_t, uint64_t
#include <cstdio>         // for size_t, EOF
#include <functional>     // for function
#include <iostream>       // for operator<<, stringstream, basic_ostream
#include <map>            // for map
#include <memory>         // for make_unique, unique_ptr, allocator, operat...
#include <sstream>        // for basic_stringstream<>::__stringbuf_type
#include <stdexcept>      // for logic_error, runtime_error
#include <string>         // for string, operator<<
#include <unordered_map>  // for unordered_map
#include <utility>        // for move, forward, pair

namespace While {
struct LambdaExpr {
    virtual ~LambdaExpr() {}
};

struct App : public LambdaExpr {};

struct Abs : public LambdaExpr {};

struct Atom : public LambdaExpr {};

struct Const : public Atom {};

struct NumConst : public Const {
    uint64_t val;

    NumConst(uint64_t _val) : val(_val) {}
};

struct RealConst : public Const {
    double val;

    RealConst(double _val) : val(_val) {}
};

struct StringConst : public Const {
    std::string val;

    template <class T>
    StringConst(T&& _val) : val(std::forward<T>(_val)) {}
};

struct FunConst : public Const {
    std::function<Const*(Const*)> f;

    FunConst(std::function<Const*(Const*)> _f) : f(_f) {}
};

struct Var : public Atom {
    std::string name;
    template <class T>
    Var(T&& _name) : name(std::forward<T>(_name)) {}
};

struct Parser : public lr_parser::LRParser<Grammar, LambdaExpr*> {
    using LRParser<Grammar, LambdaExpr*>::LRParser;

    std::map<std::string, Var*> vars;
    std::unordered_map<Var*, Const*> env;

    LambdaExpr* reduce(NonterminalID A, uint32_t production, LambdaExpr** alpha, size_t n) override {
        LambdaExpr* val = nullptr;
        switch (A) {
        case NT::S:
            // TODO: Fill
            break;
        case NT::PROGRAM:
            // TODO: Fill
            break;
        case NT::FUNCS:
            // TODO: Fill
            break;
        case NT::FUNC:
            if (production == 0)
                val = alpha[6];
            else {
                // TODO: Fill
            }
            break;
        case NT::BLOCK:
            val = alpha[2];
            break;
        case NT::STMTS:
            // TODO: Now it gets interesting ...
            break;
        default:
            break;
            //    {
            //      std::stringstream ss;
            //      ss << "invalid nonterminal: " << A;
            //      throw std::runtime_error(ss.str());
            //    }
        }

        //    productions[NT::STMTS] = {
        //        {NT::STMTS, NT::STMT}, {T::EPS}
        //    };
        //    productions[NT::STMT] = {
        //        {NT::ASSIGN, T::SEMICOLON},
        //        {T::IF, T::LEFTPAR, NT::ASSIGN, T::RIGHTPAR, NT::STMT},
        //        {T::IF, T::LEFTPAR, NT::ASSIGN, T::RIGHTPAR, NT::STMT, T::ELSE, NT::STMT},
        //        {T::WHILE, T::LEFTPAR, NT::ASSIGN, T::RIGHTPAR, NT::STMT},
        //        {T::DO, NT::STMT, T::WHILE, T::LEFTPAR, NT::ASSIGN, T::SEMICOLON},
        //        {T::BREAK, T::SEMICOLON},
        //        {T::CONTINUE, T::SEMICOLON},
        //        {T::RETURN, T::SEMICOLON},
        //        {T::RETURN, NT::LOC, T::SEMICOLON},
        //        {NT::BLOCK}
        //    };
        //    productions[NT::LOC] = {
        //        {NT::LOC, T::LEFTBR, NT::ASSIGN, T::RIGHTBR}, {T::ID}, {NT::LOC, T::PERIOD, T::ID}
        //    };
        //    productions[NT::ASSIGN] = {
        //        {NT::BOOL, T::ASSIGN, NT::ASSIGN}, {NT::BOOL}
        //    };
        //    productions[NT::BOOL] = {
        //        {NT::BOOL, T::OR, NT::JOIN}, {NT::JOIN}
        //    };
        //    productions[NT::JOIN] = {
        //        {NT::JOIN, T::AND, NT::EQ}, {NT::EQ}
        //    };
        //    productions[NT::EQ] = {
        //        {NT::EQ, T::EQ, NT::REL}, {NT::EQ, T::NEQ, NT::REL}, {NT::REL}
        //    };
        //    productions[NT::REL] = {
        //        {NT::EXPR, T::LT, NT::EXPR}, {NT::EXPR, T::LE, NT::EXPR}, {NT::EXPR, T::GE, NT::EXPR}, {NT::EXPR,
        //        T::GT, NT::EXPR}, {NT::EXPR}
        //    };
        //    productions[NT::EXPR] = {
        //        {NT::EXPR, T::PLUS, NT::TERM}, {NT::EXPR, T::MINUS, NT::TERM}, {NT::TERM}
        //    };
        //    productions[NT::TERM] = {
        //        {NT::TERM, T::TIMES, NT::UNARY}, {NT::TERM, T::DIV, NT::UNARY}, {NT::UNARY}
        //    };
        //    productions[NT::UNARY] = {
        //        {T::NOT, NT::UNARY}, {T::MINUS, NT::UNARY}, {NT::FACTOR}
        //    };
        //    productions[NT::FACTOR] = {
        //        {T::LEFTPAR, NT::ASSIGN, T::RIGHTPAR}, {NT::LOC}, {T::NUM}, {T::REAL}, {T::TRUE}, {T::FALSE},
        //        {T::STRING}, {NT::FUNCALL}
        //    };
        //    productions[NT::FUNCALL] = {
        //        {T::ID, T::LEFTPAR, NT::OPTARGS, T::RIGHTPAR}
        //    };
        //    productions[NT::OPTARGS] = {
        //        {NT::ARGS}, {T::EPS}
        //    };
        //    productions[NT::ARGS] = {
        //        {NT::ASSIGN, T::COMMA, NT::ARGS}, {NT::ASSIGN}
        //    };

        return val;
    }

    LambdaExpr* shift(Grammar::Token&& t) override {
        assert(t != nullptr);
        LambdaExpr* val = nullptr;

        switch (t->tag) {
        case T::NUM:
            val = new NumConst(static_cast<const Num&>(*t).value);
            break;
        case T::REAL:
            val = new RealConst(static_cast<const Real&>(*t).value);
            break;
        case T::ID:
            val = new Var(std::move(static_cast<const ID&>(*t).value));
            break;
        case T::STRING:
            val = new StringConst(std::move(static_cast<const String&>(*t).value));
            break;
        default:
            break;
        }

        return val;
    }
};

struct myLexer : TokenStream<std::unique_ptr<Token>> {
    using DFA_t = Lexer::DFA_t;
    using Symbol = Lexer::Symbol;
    using State = Lexer::State;
    using TokenId = Lexer::TokenId;

    Lexer lexer;
    std::unique_ptr<Token> token = nullptr;

    static DFA_t getDFA();

    myLexer() : lexer(getDFA(), {nullptr, 0, static_cast<uint32_t>(T::EOI) + 1}, static_cast<uint32_t>(T::WS) + 1) {}

    void setText(const char* text) {
        lexer.c = text;
        token = t2t(lexer.getToken());
    }

    // std::unique_ptr<Token> eofToken() const {
    //    return new Token(TerminalID(T::EOI));
    //}

    // std::unique_ptr<Token> whiteSpaceToken() const {
    //    return nullptr;
    //}

    static std::unique_ptr<Token> action(const char* s, size_t n, TokenId t);
    static std::unique_ptr<Token> t2t(Lexer::Token t) { return action(t.start, t.length, t.tokenId); }

    void step() override { token = t2t(lexer.getToken()); }
    const std::unique_ptr<Token>& peek() const { return token; }
    std::unique_ptr<Token>& peek() { return token; }

    // const std::unique_ptr<Token>& operator*() const { return token; }
    //// required to move from token. Is there a better solution?
    // std::unique_ptr<Token>& operator*() { return token; }
    // myLexer& operator++() {
    //    token = t2t(lexer.getToken());
    //    return *this;
    //}

    // struct iterator {
    //    myLexer* lex_;

    //    iterator(myLexer* lex) : lex_(lex) {}

    //    const std::unique_ptr<Token>& operator*() const { return *(*lex_); }
    //    std::unique_ptr<Token>& operator*() { return *(*lex_); }
    //    iterator& operator++() {
    //        ++(*lex_);
    //        return *this;
    //    }
    //};

    // iterator begin() { return iterator(*this); }
    // iterator begin() { return iterator(this); }
};

auto myLexer::getDFA() -> DFA_t {
    std::stringstream in;
    nfaBuilder<Symbol, State, TokenId> builder;

    char alpha[] =
        "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)";
    char digit[] = "(0|1|2|3|4|5|6|7|8|9)";
    char special[] = "(!|\"|#|$|%|&|'|\\(|\\)|\\*|+|,|-|.|/|:|;|<|=|>|\\?|@|[|\\\\|]|^|_|`|{|\\||}|~)";
    std::stringstream varch;
    varch << "(" << alpha << "|" << digit << "|_)";
    std::stringstream num;
    num << digit << digit << "*";
    auto numstr = num.str();
    std::stringstream var;
    var << alpha << varch.rdbuf() << "*";
    std::stringstream str;
    str << "\"(" << alpha << "|" << digit << "|" << special << "| )*\"";

    in << "def";
    builder.lexRegex(in, (unsigned)T::DEF + 1);
    in.clear();
    in << "{";
    builder.lexRegex(in, (unsigned)T::LEFTCURLY + 1);
    in.clear();
    in << "}";
    builder.lexRegex(in, (unsigned)T::RIGHTCURLY + 1);
    in.clear();
    in << ";";
    builder.lexRegex(in, (unsigned)T::SEMICOLON + 1);
    in.clear();
    in << "(int|double|bool|string)";
    builder.lexRegex(in, (unsigned)T::BASIC + 1);
    in.clear();
    in << "record";
    builder.lexRegex(in, (unsigned)T::RECORD + 1);
    in.clear();
    in << "if";
    builder.lexRegex(in, (unsigned)T::IF + 1);
    in.clear();
    in << "else";
    builder.lexRegex(in, (unsigned)T::ELSE + 1);
    in.clear();
    in << "while";
    builder.lexRegex(in, (unsigned)T::WHILE + 1);
    in.clear();
    in << "do";
    builder.lexRegex(in, (unsigned)T::DO + 1);
    in.clear();
    in << "break";
    builder.lexRegex(in, (unsigned)T::BREAK + 1);
    in.clear();
    in << "continue";
    builder.lexRegex(in, (unsigned)T::CONTINUE + 1);
    in.clear();
    in << "return";
    builder.lexRegex(in, (unsigned)T::RETURN + 1);
    in.clear();
    in << "[";
    builder.lexRegex(in, (unsigned)T::LEFTBR + 1);
    in.clear();
    in << "]";
    builder.lexRegex(in, (unsigned)T::RIGHTBR + 1);
    in.clear();
    in << ".";
    builder.lexRegex(in, (unsigned)T::PERIOD + 1);
    in.clear();
    in << "=";
    builder.lexRegex(in, (unsigned)T::ASSIGN + 1);
    in.clear();
    in << "\\|\\|";
    builder.lexRegex(in, (unsigned)T::OR + 1);
    in.clear();
    in << "&&";
    builder.lexRegex(in, (unsigned)T::AND + 1);
    in.clear();
    in << "==";
    builder.lexRegex(in, (unsigned)T::EQ + 1);
    in.clear();
    in << "!=";
    builder.lexRegex(in, (unsigned)T::NEQ + 1);
    in.clear();
    in << "<";
    builder.lexRegex(in, (unsigned)T::LT + 1);
    in.clear();
    in << "<=";
    builder.lexRegex(in, (unsigned)T::LE + 1);
    in.clear();
    in << ">=";
    builder.lexRegex(in, (unsigned)T::GE + 1);
    in.clear();
    in << ">";
    builder.lexRegex(in, (unsigned)T::GT + 1);
    in.clear();
    in << "+";
    builder.lexRegex(in, (unsigned)T::PLUS + 1);
    in.clear();
    in << "-";
    builder.lexRegex(in, (unsigned)T::MINUS + 1);
    in.clear();
    in << "\\*";
    builder.lexRegex(in, (unsigned)T::TIMES + 1);
    in.clear();
    in << "/";
    builder.lexRegex(in, (unsigned)T::DIV + 1);
    in.clear();
    in << "!";
    builder.lexRegex(in, (unsigned)T::NOT + 1);
    in.clear();
    in << "\\(";
    builder.lexRegex(in, (unsigned)T::LEFTPAR + 1);
    in.clear();
    in << "\\)";
    builder.lexRegex(in, (unsigned)T::RIGHTPAR + 1);
    in.clear();
    in << "true";
    builder.lexRegex(in, (unsigned)T::TRUE + 1);
    in.clear();
    in << "false";
    builder.lexRegex(in, (unsigned)T::FALSE + 1);
    in.clear();
    in << ",";
    builder.lexRegex(in, (unsigned)T::COMMA + 1);
    in.clear();
    in << "( |\t|\n)( |\t|\n)*";
    builder.lexRegex(in, (unsigned)T::WS + 1);
    in.clear();
    in << numstr << "." << numstr;
    builder.lexRegex(in, (unsigned)T::REAL + 1);
    in.clear();
    in << numstr;
    builder.lexRegex(in, (unsigned)T::NUM + 1);
    builder.lexRegex(str, (unsigned)T::STRING + 1);
    builder.lexRegex(var, (unsigned)T::ID + 1);

    NFA<Symbol, State, TokenId> nfa1(builder);
    DFA_t dfa1{toDFA(nfa1)};
    dfa1 = minimize(dfa1);

    return dfa1;
}

std::unique_ptr<Token> myLexer::action(const char* s, size_t n, TokenId t) {
    std::cout << "action(\"";
    std::cout.write(s, n);
    std::cout << "\", " << n << ", " << t << ")" << std::endl;
    std::unique_ptr<Token> tok;

    switch (TerminalID(t - 1)) {
    case T::DEF:
        tok = std::make_unique<Token>(T::DEF);
        break;
    case T::LEFTCURLY:
        tok = std::make_unique<Token>(T::LEFTCURLY);
        break;
    case T::RIGHTCURLY:
        tok = std::make_unique<Token>(T::RIGHTCURLY);
        break;
    case T::SEMICOLON:
        tok = std::make_unique<Token>(T::SEMICOLON);
        break;
    case T::BASIC:
        tok = std::make_unique<Basic>(std::string(s, n)); // here
        break;
    case T::RECORD:
        tok = std::make_unique<Token>(T::RECORD);
        break;
    case T::IF:
        tok = std::make_unique<Token>(T::IF);
        break;
    case T::ELSE:
        tok = std::make_unique<Token>(T::ELSE);
        break;
    case T::WHILE:
        tok = std::make_unique<Token>(T::WHILE);
        break;
    case T::DO:
        tok = std::make_unique<Token>(T::DO);
        break;
    case T::BREAK:
        tok = std::make_unique<Token>(T::BREAK);
        break;
    case T::CONTINUE:
        tok = std::make_unique<Token>(T::CONTINUE);
        break;
    case T::RETURN:
        tok = std::make_unique<Token>(T::RETURN);
        break;
    case T::LEFTBR:
        tok = std::make_unique<Token>(T::LEFTBR);
        break;
    case T::RIGHTBR:
        tok = std::make_unique<Token>(T::RIGHTBR);
        break;
    case T::ID:
        tok = std::make_unique<ID>(std::string(s, n)); // here
        break;
    case T::PERIOD:
        tok = std::make_unique<Token>(T::PERIOD);
        break;
    case T::ASSIGN:
        tok = std::make_unique<Token>(T::ASSIGN);
        break;
    case T::OR:
        tok = std::make_unique<Token>(T::OR);
        break;
    case T::AND:
        tok = std::make_unique<Token>(T::AND);
        break;
    case T::EQ:
        tok = std::make_unique<Token>(T::EQ);
        break;
    case T::NEQ:
        tok = std::make_unique<Token>(T::NEQ);
        break;
    case T::LT:
        tok = std::make_unique<Token>(T::LT);
        break;
    case T::LE:
        tok = std::make_unique<Token>(T::LE);
        break;
    case T::GE:
        tok = std::make_unique<Token>(T::GE);
        break;
    case T::GT:
        tok = std::make_unique<Token>(T::GT);
        break;
    case T::PLUS:
        tok = std::make_unique<Token>(T::PLUS);
        break;
    case T::MINUS:
        tok = std::make_unique<Token>(T::MINUS);
        break;
    case T::TIMES:
        tok = std::make_unique<Token>(T::TIMES);
        break;
    case T::DIV:
        tok = std::make_unique<Token>(T::DIV);
        break;
    case T::NOT:
        tok = std::make_unique<Token>(T::NOT);
        break;
    case T::LEFTPAR:
        tok = std::make_unique<Token>(T::LEFTPAR);
        break;
    case T::RIGHTPAR:
        tok = std::make_unique<Token>(T::RIGHTPAR);
        break;
    case T::NUM: {
        uint64_t i;
        std::stringstream ss;
        ss.exceptions(std::stringstream::failbit | std::stringstream::badbit);
        ss.write(s, n);
        ss >> i;
        tok = std::make_unique<Num>(i);
        break;
    }
    case T::REAL: {
        double d;
        std::stringstream ss;
        ss.exceptions(std::stringstream::failbit | std::stringstream::badbit);
        ss.write(s, n);
        ss >> d;
        tok = std::make_unique<Real>(d);
        break;
    }
    case T::TRUE:
        tok = std::make_unique<Token>(T::TRUE);
        break;
    case T::FALSE:
        tok = std::make_unique<Token>(T::FALSE);
        break;
    case T::STRING:
        tok = std::make_unique<String>(std::string(s, n)); // here
        break;
    case T::COMMA:
        tok = std::make_unique<Token>(T::COMMA);
        break;
    case T::WS:
        tok = nullptr;
        break;
    case T::EOI:
        tok = std::make_unique<Token>(T::EOI);
        break;
    default: {
        std::stringstream ss;
        ss << "invalid token type: " << t;
        throw std::runtime_error(ss.str());
    }
    }
    return tok;
}
} // namespace While

int main() {
    While::myLexer lex;
    using While::GrammarElement;
    using While::NonterminalID;
    using While::TerminalID;

    for (auto i = 0u; i < While::Grammar::getNumberOfTerminals(); ++i)
        assert(TerminalID(i) == (TerminalID)(GrammarElement)TerminalID(i));
    for (auto i = 0u; i < While::Grammar::getNumberOfNonterminals(); ++i)
        assert(NonterminalID(i) == (NonterminalID)(GrammarElement)NonterminalID(i));
    for (auto i = 0u; i < While::Grammar::getNumberOfGrammarElements(); ++i) {
        switch (While::Grammar::kindOf(GrammarElement(i))) {
        case kind::TERMINAL:
            assert(GrammarElement(i) == (GrammarElement)(TerminalID)GrammarElement(i));
            break;
        case kind::EPS:
            assert(GrammarElement(i) == (GrammarElement)(TerminalID)GrammarElement(i));
            break;
        case kind::EOI:
            assert(GrammarElement(i) == (GrammarElement)(TerminalID)GrammarElement(i));
            break;
        case kind::NONTERMINAL:
            assert(GrammarElement(i) == (GrammarElement)(NonterminalID)GrammarElement(i));
            break;
        default:
            throw std::logic_error("Error in GrammarElement test");
        }
    }

    const auto& fs = While::Grammar::getFirsts();
    for (const auto& f : fs) {
        std::cout << f.first << ": ";
        for (const auto& a : f.second)
            std::cout << a << " ";
        std::cout << std::endl;
    }

    While::Parser parser;

    std::cout << "> " << std::flush;
    std::string line(4096, EOF);
    std::cin.getline(line.data(), line.size() - 1);
    auto ll = std::cin.gcount();
    std::cout << "read " << ll << " characters" << std::endl;
    std::cout << "parsing \"" << line << "\"" << std::endl;
    // getline appends a \0, which is included in the gcount()
    line[ll - 1] = EOF;
    lex.setText(line.data());
    auto result = parser.parse(lex);

    //  std::cout << "parsing DEF BASIC ID() { ID[ID] = NUM + NUM; }" << std::endl;
    //  typedef While::T T;
    //  auto tokens = std::initializer_list<T>{{T::DEF, T::BASIC, T::ID, T::LEFTPAR, T::RIGHTPAR, T::LEFTCURLY, T::ID,
    //  T::LEFTBR, T::ID, T::RIGHTBR, T::ASSIGN, T::NUM, T::PLUS, T::NUM, T::SEMICOLON, T::RIGHTCURLY, T::EOI}};
    //  std::vector<While::Token> input(tokens.begin(), tokens.end());
    //  parser.parse(input.begin());

    return 0;
}

