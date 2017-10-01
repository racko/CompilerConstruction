#include "jsonLexer.h"
#include "NFA.h"
#include "nfaBuilder.h"

auto myLexer::getDFA() -> DFA_t {
    std::stringstream in;
    std::cout << "nfaBuilder" << std::endl;
    nfaBuilder<Symbol,State,TokenId> builder;

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
    for (int i = 0x7D; i <= 0x7F; ++i) // int + static_cast because i <= 0x7F is always true for "char i" (infinite loop)
        unescaped << "|" << static_cast<char>(i);
    // only ascii for now ...
    unescaped << ")";

    std::stringstream chr;
    chr << "(" << unescaped.rdbuf() << "|\\\\(\"|\\\\|/|b|f|n|r|t|u" << hexdigit << hexdigit << hexdigit << hexdigit << "))";
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
    //in << "[";
    //builder.lexRegex(in, (unsigned)T::BEGIN_ARRAY + 1);
    //in.clear();
    //in << "{";
    //builder.lexRegex(in, (unsigned)T::BEGIN_OBJECT + 1);
    //in.clear();
    //in << "]";
    //builder.lexRegex(in, (unsigned)T::END_ARRAY + 1);
    //in.clear();
    //in << "}";
    //builder.lexRegex(in, (unsigned)T::END_OBJECT + 1);
    //in.clear();
    //in << ":";
    //builder.lexRegex(in, (unsigned)T::NAME_SEPARATOR + 1);
    //in.clear();
    //in << ",";
    //builder.lexRegex(in, (unsigned)T::VALUE_SEPARATOR + 1);

    builder.lexRegex(num, (unsigned)T::NUM + 1);
    builder.lexRegex(str, (unsigned)T::STRING + 1);
    in.clear();
    //// not required if we include ws in the structural tokens and don't use T::WS in the context-free grammar
    //in << "( |\t|\n|\r)" << ws;
    //builder.lexRegex(in, (unsigned)T::WS + 1);

    std::cout << "NFA" << std::endl;
    NFA<Symbol,State,TokenId> nfa1(builder);
    std::cout << "DFA" << std::endl;
    DFA_t dfa1(nfa1);
    std::cout << "minimal DFA" << std::endl;
    dfa1.minimize();
    return dfa1;
}


