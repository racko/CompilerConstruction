#include "FunctionalLexer.h"
#include "nfaBuilder.h"
#include <fstream>

auto myLexer::getDFA() -> DFA_t {
    std::stringstream in;
    nfaBuilder<Symbol,State,TokenId> builder;

    char alpha[] = "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)";
    char digit[] = "(0|1|2|3|4|5|6|7|8|9)";
    std::stringstream varch;
    varch << "(" << alpha << "|" << digit << "|_)";
    std::stringstream num;
    num << digit << digit << "*";
    std::stringstream var;
    var << alpha << varch.rdbuf() << "*";
    in << ";";
    builder.lexRegex(in, (unsigned)T::SEMICOLON + 1);
    in.clear();
    in << "=";
    builder.lexRegex(in, (unsigned)T::ASSIGN + 1);
    in.clear();
    in << ".";
    builder.lexRegex(in, (unsigned)T::PERIOD + 1);
    in.clear();
    in << "\\(";
    builder.lexRegex(in, (unsigned)T::LPAREN + 1);
    in.clear();
    in << "\\)";
    builder.lexRegex(in, (unsigned)T::RPAREN + 1);
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
    in << "<";
    builder.lexRegex(in, (unsigned)T::LT + 1);
    in.clear();
    in << ">";
    builder.lexRegex(in, (unsigned)T::GT + 1);
    in.clear();
    in << "&";
    builder.lexRegex(in, (unsigned)T::AND + 1);
    in.clear();
    in << "\\|";
    builder.lexRegex(in, (unsigned)T::OR + 1);
    in.clear();
    in << "\\\\";
    builder.lexRegex(in, (unsigned)T::BSL + 1);
    in.clear();
    in << "<=";
    builder.lexRegex(in, (unsigned)T::LE + 1);
    in.clear();
    in << "==";
    builder.lexRegex(in, (unsigned)T::EQ + 1);
    in.clear();
    in << "!=";
    builder.lexRegex(in, (unsigned)T::NE + 1);
    in.clear();
    in << ">=";
    builder.lexRegex(in, (unsigned)T::GE + 1);
    in.clear();
    in << "let";
    builder.lexRegex(in, (unsigned)T::LET + 1);
    in.clear();
    in << "in";
    builder.lexRegex(in, (unsigned)T::IN + 1);
    in.clear();
    in << "letrec";
    builder.lexRegex(in, (unsigned)T::LETREC + 1);
    builder.lexRegex(var, (unsigned)T::VAR + 1);
    builder.lexRegex(num, (unsigned)T::NUM + 1);
    in.clear();
    in << "( |\t|\n)( |\t|\n)*";
    builder.lexRegex(in, (unsigned)T::WS + 1);

    NFA<Symbol,State,TokenId> nfa1(builder);
    std::ofstream file1("functional_nfa.dot");
    file1 << nfa1;
    DFA_t dfa1(nfa1);
    std::ofstream file2("functional_dfa.dot");
    file2 << dfa1;
    dfa1.minimize();
    std::ofstream file3("functional_min_dfa.dot");
    file3 << dfa1;
    return dfa1;
}

auto myLexer::action(const char* s, size_t n, TokenId t) -> Token* {
    cout << "action(\"";
    cout.write(s, n);
    cout << "\", " << n << ", " << int(t) << ")" << endl;
    Token* tok;
    auto tid = TerminalID<Functional::Grammar>(t - 1);
    switch(tid) {
    case T::SEMICOLON:
    case T::ASSIGN:
    case T::PERIOD:
    case T::LPAREN:
    case T::RPAREN:
    case T::BSL:
    case T::PLUS:
    case T::MINUS:
    case T::TIMES:
    case T::DIV:
    case T::LT:
    case T::GT:
    case T::AND:
    case T::OR:
    case T::LE:
    case T::EQ:
    case T::NE:
    case T::GE:
    case T::LET:
    case T::IN:
    case T::LETREC:
        tok = new Token(tid);
        break;
    case T::VAR:
        tok = new Functional::Var(string(s, n));
        break;
    case T::NUM: {
        unsigned long long i;
        std::stringstream ss;
        ss.exceptions(std::stringstream::failbit | std::stringstream::badbit);
        ss.write(s, n);
        ss >> i;
        tok = new Functional::Num(i);
        break;
    }
    case T::WS:
        tok = nullptr;
        break;
    case T::EOI:
        tok = new Token(T::EOI);
        break;
    default: {
        std::stringstream ss;
        ss << "invalid token type: " << t;
        throw std::runtime_error(ss.str());
    }

    }
    return tok;
}
