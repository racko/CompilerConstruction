#include "While.h"
#include "lrParser.h"
#include "Regex.h"
#include "nfaBuilder.h"
#include "NFA.h"
#include <iostream>
#include <fstream>

#undef NDEBUG
#include <cassert>

namespace While {
struct LambdaExpr {
    virtual ~LambdaExpr() {}
};

struct App : public LambdaExpr {

};

struct Abs : public LambdaExpr {

};

struct Atom : public LambdaExpr {

};

struct Const : public Atom {

};

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

    template<class T>
        StringConst(T&& _val) : val(std::forward<T>(_val)) {}
};

struct FunConst : public Const {
    std::function<Const*(Const*)> f;

    FunConst(std::function<Const*(Const*)> _f) : f(_f) {}
};

struct Var : public Atom {
    std::string name;
    template<class T>
        Var(T&& _name) : name(std::forward<T>(_name)) {}
};

struct Parser : public LRParser<Grammar, LambdaExpr*> {
    using LRParser<Grammar, LambdaExpr*>::LRParser;

    std::map<std::string, Var*> vars;
    std::unordered_map<Var*, Const*> env;

    LambdaExpr* reduce(NonterminalID<Grammar> A, uint32_t production, LambdaExpr** alpha, size_t n) override {
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
        //        {NT::EXPR, T::LT, NT::EXPR}, {NT::EXPR, T::LE, NT::EXPR}, {NT::EXPR, T::GE, NT::EXPR}, {NT::EXPR, T::GT, NT::EXPR}, {NT::EXPR}
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
        //        {T::LEFTPAR, NT::ASSIGN, T::RIGHTPAR}, {NT::LOC}, {T::NUM}, {T::REAL}, {T::TRUE}, {T::FALSE}, {T::STRING}, {NT::FUNCALL}
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

    LambdaExpr* shift(const Token* t) override {
        assert(t != nullptr);
        LambdaExpr* val = nullptr;

        switch(t->tag) {
            case T::NUM:
                val = new NumConst(static_cast<const Num*>(t)->value);
                break;
            case T::REAL:
                val = new RealConst(static_cast<const Real*>(t)->value);
                break;
            case T::ID:
                val = new Var(std::move(static_cast<const ID*>(t)->value));
                break;
            case T::STRING:
                val = new StringConst(std::move(static_cast<const String*>(t)->value));
                break;
            default:
                break;
        }

        delete t;
        return val;
    }
};

struct myLexer : public Lexer<Token*> {
    DFA_t getDFA() {
        std::stringstream in;
        nfaBuilder<Symbol,State,TokenId> builder;

        char alpha[] = "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)";
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


        NFA<Symbol,State,TokenId> nfa1(builder);
        DFA_t dfa1(nfa1);
        dfa1.minimize();

        return dfa1;
    }


    myLexer() : Lexer(getDFA()) {}

    Token* eofToken() const {
        return new Token(TerminalID<Grammar>(T::EOI));
    }

    Token* whiteSpaceToken() const {
        return nullptr;
    }

    Token* action(const char* s, size_t n, TokenId t) override {
        cout << "action(\"";
        cout.write(s, n);
        cout << "\", " << n << ", " << t << ")" << endl;
        Token* tok;

        switch(TerminalID<Grammar>(t - 1)) {
            case T::DEF:
                tok = new Token(T::DEF);
                break;
            case T::LEFTCURLY:
                tok = new Token(T::LEFTCURLY);
                break;
            case T::RIGHTCURLY:
                tok = new Token(T::RIGHTCURLY);
                break;
            case T::SEMICOLON:
                tok = new Token(T::SEMICOLON);
                break;
            case T::BASIC:
                tok = new Basic(std::string(s, n)); // here
                break;
            case T::RECORD:
                tok = new Token(T::RECORD);
                break;
            case T::IF:
                tok = new Token(T::IF);
                break;
            case T::ELSE:
                tok = new Token(T::ELSE);
                break;
            case T::WHILE:
                tok = new Token(T::WHILE);
                break;
            case T::DO:
                tok = new Token(T::DO);
                break;
            case T::BREAK:
                tok = new Token(T::BREAK);
                break;
            case T::CONTINUE:
                tok = new Token(T::CONTINUE);
                break;
            case T::RETURN:
                tok = new Token(T::RETURN);
                break;
            case T::LEFTBR:
                tok = new Token(T::LEFTBR);
                break;
            case T::RIGHTBR:
                tok = new Token(T::RIGHTBR);
                break;
            case T::ID:
                tok = new ID(std::string(s, n)); // here
                break;
            case T::PERIOD:
                tok = new Token(T::PERIOD);
                break;
            case T::ASSIGN:
                tok = new Token(T::ASSIGN);
                break;
            case T::OR:
                tok = new Token(T::OR);
                break;
            case T::AND:
                tok = new Token(T::AND);
                break;
            case T::EQ:
                tok = new Token(T::EQ);
                break;
            case T::NEQ:
                tok = new Token(T::NEQ);
                break;
            case T::LT:
                tok = new Token(T::LT);
                break;
            case T::LE:
                tok = new Token(T::LE);
                break;
            case T::GE:
                tok = new Token(T::GE);
                break;
            case T::GT:
                tok = new Token(T::GT);
                break;
            case T::PLUS:
                tok = new Token(T::PLUS);
                break;
            case T::MINUS:
                tok = new Token(T::MINUS);
                break;
            case T::TIMES:
                tok = new Token(T::TIMES);
                break;
            case T::DIV:
                tok = new Token(T::DIV);
                break;
            case T::NOT:
                tok = new Token(T::NOT);
                break;
            case T::LEFTPAR:
                tok = new Token(T::LEFTPAR);
                break;
            case T::RIGHTPAR:
                tok = new Token(T::RIGHTPAR);
                break;
            case T::NUM:
                {
                    uint64_t i;
                    std::stringstream ss;
                    ss.exceptions(std::ifstream::failbit | std::ifstream::badbit);
                    ss.write(s, n);
                    ss >> i;
                    tok = new Num(i);
                    break;
                }
            case T::REAL:
                {
                    double d;
                    std::stringstream ss;
                    ss.exceptions(std::ifstream::failbit | std::ifstream::badbit);
                    ss.write(s, n);
                    ss >> d;
                    tok = new Real(d);
                    break;
                }
            case T::TRUE:
                tok = new Token(T::TRUE);
                break;
            case T::FALSE:
                tok = new Token(T::FALSE);
                break;
            case T::STRING:
                tok = new String(std::string(s, n)); // here
                break;
            case T::COMMA:
                tok = new Token(T::COMMA);
                break;
            case T::WS:
                tok = nullptr;
                break;
            default:
                {
                    std::stringstream ss;
                    ss << "invalid token type: " << t;
                    throw std::runtime_error(ss.str());
                }

        }
        return tok;
    }
};

}

int main() {
    While::myLexer lex;

    for (auto i = 0u; i < While::Grammar::getNumberOfTerminals(); ++i)
        assert(TerminalID<While::Grammar>(i) == (TerminalID<While::Grammar>)(GrammarElement<While::Grammar>)TerminalID<While::Grammar>(i));
    for (auto i = 0u; i < While::Grammar::getNumberOfNonterminals(); ++i)
        assert(NonterminalID<While::Grammar>(i) == (NonterminalID<While::Grammar>)(GrammarElement<While::Grammar>)NonterminalID<While::Grammar>(i));
    for (auto i = 0u; i < While::Grammar::getNumberOfGrammarElements(); ++i) {
        switch(While::Grammar::kindOf(GrammarElement<While::Grammar>(i))) {
            case kind::TERMINAL:
                assert(GrammarElement<While::Grammar>(i) == (GrammarElement<While::Grammar>)(TerminalID<While::Grammar>)GrammarElement<While::Grammar>(i));
                break;
            case kind::EPS:
                assert(GrammarElement<While::Grammar>(i) == (GrammarElement<While::Grammar>)(TerminalID<While::Grammar>)GrammarElement<While::Grammar>(i));
                break;
            case kind::EOI:
                assert(GrammarElement<While::Grammar>(i) == (GrammarElement<While::Grammar>)(TerminalID<While::Grammar>)GrammarElement<While::Grammar>(i));
                break;
            case kind::NONTERMINAL:
                assert(GrammarElement<While::Grammar>(i) == (GrammarElement<While::Grammar>)(NonterminalID<While::Grammar>)GrammarElement<While::Grammar>(i));
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
    std::cin.getline(line.data(), line.size()-1);
    auto ll = std::cin.gcount();
    std::cout << "read " << ll << " characters" << std::endl;
    std::cout << "parsing \"" << line << "\"" << std::endl;
    // getline appends a \0, which is included in the gcount()
    line[ll-1] = EOF;
    lex.c = line.data();
    auto result = parser.parse(lex.begin());

    //  std::cout << "parsing DEF BASIC ID() { ID[ID] = NUM + NUM; }" << std::endl;
    //  typedef While::T T;
    //  auto tokens = std::initializer_list<T>{{T::DEF, T::BASIC, T::ID, T::LEFTPAR, T::RIGHTPAR, T::LEFTCURLY, T::ID, T::LEFTBR, T::ID, T::RIGHTBR, T::ASSIGN, T::NUM, T::PLUS, T::NUM, T::SEMICOLON, T::RIGHTCURLY, T::EOI}};
    //  std::vector<While::Token> input(tokens.begin(), tokens.end());
    //  parser.parse(input.begin());



    return 0;
}

