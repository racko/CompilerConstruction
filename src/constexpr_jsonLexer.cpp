#include "constexpr_jsonLexer.h"

#include "BitSet_constexpr.h"           // for BitSet
#include "DFA_minimization_constexpr.h" // for operator|=, minimize, a_and_...
#include "HashSet_constexpr.h"          // for HashSet
#include "NFA_constexpr.h"              // for NFA
#include "NFA_to_DFA_constexpr.h"       // for toDFA, States
#include "Regex_constexpr.h"            // for Lexer
#include "constexpr.h"                  // for strcat, string, vector, vect...
#include "nfaBuilder_constexpr.h"       // for nfaBuilder, CountingGraph
#include <algorithm>                    // for max, min
#include <cstdint>                      // for uint8_t, int64_t, int16_t
#include <iostream>                     // for endl, cout, ostream

static constexpr int64_t MaxSymbols = 128;
using Symbol = char;
using State = int16_t;

template <typename G>
constexpr nfaBuilder<Symbol, State, MaxSymbols, G> make_nfaBuilder() {
    nfaBuilder<Symbol, State, MaxSymbols, G> builder;
    builder.lexRegex("false", 1);
    builder.lexRegex("true", 2);
    builder.lexRegex("null", 3);

    char hexdigit[] = "(0|1|2|3|4|5|6|7|8|9|a|b|c|d|e|f|A|B|C|D|E|F)";
    char nonzerodigit[] = "(1|2|3|4|5|6|7|8|9)";
    char ws[] = "( |\t|\n|\r)*";

    auto digit = strcat("(0|", nonzerodigit, ")");
    auto integer = strcat("(0|", nonzerodigit, digit, "*)");
    auto exp = strcat(R"(((e|E)(-|\+)?)", digit, digit, "*)");
    auto frac = strcat("(.", digit, digit, "*)");
    auto num = strcat("(-?", integer, frac, "?", exp, "?)");

    string<195> unescaped;
    unescaped += "( |!"; // 0x20, 0x21
    // 0x22 is ", so it has to be escaped
    for (char i = 0x23; i <= 0x27; ++i)
        (unescaped += '|') += i;
    unescaped += R"(|\(|\)|\*|\+)";
    for (char i = 0x2C; i <= 0x3E; ++i)
        (unescaped += '|') += i;
    unescaped += R"(|\?)";
    for (char i = 0x40; i <= 0x5B; ++i)
        (unescaped += '|') += i;
    // 0x5C is \ -> escape
    for (char i = 0x5D; i <= 0x7B; ++i)
        (unescaped += '|') += i;
    unescaped += R"(|\|)";
    for (int i = 0x7D; i <= 0x7F;
         ++i) // int + static_cast because i <= 0x7F is always true for "char i" (infinite loop)
        (unescaped += '|') += static_cast<char>(i);
    // only ascii for now ...
    unescaped += ')';

    // note: forward slash can be used escaped as well as unescaped. That's part of the json spec.
    // Stackoverflow explains that this is helpful for embedding json in javascript.
    auto chr = strcat("(", unescaped, R"(|\\("|\\|/|b|f|n|r|t|u)", hexdigit, hexdigit, hexdigit, hexdigit, "))");
    auto str = strcat(R"((")", chr, R"(*"))");

    builder.lexRegex(strcat(ws, "[", ws).data(), 4);
    builder.lexRegex(strcat(ws, "]", ws).data(), 5);
    builder.lexRegex(strcat(ws, "{", ws).data(), 6);
    builder.lexRegex(strcat(ws, "}", ws).data(), 7);
    builder.lexRegex(strcat(ws, ":", ws).data(), 8);
    builder.lexRegex(strcat(ws, ",", ws).data(), 9);
    builder.lexRegex(num.data(), 10);
    builder.lexRegex(str.data(), 11);
    return builder;
}

static constexpr const auto MaxNodes = [] {
    using G = CountingGraph<State, TokenId>;
    const auto builder = make_nfaBuilder<G>();
    return builder.ns.getStateCount();
}();

static constexpr const auto MaxTransitions = [] {
    using G = TransitionCountingGraph<State, TokenId, MaxNodes>;
    auto builder = make_nfaBuilder<G>();
    const auto& transitionCounts = builder.ns.getTransitionCounts();
    return *max_element(transitionCounts.begin(), transitionCounts.end());
    // return *max_element(transitionCounts.begin(), transitionCounts.begin() + MaxNodes);
}();

static constexpr const auto MaxResultStates = [] {
    using G = ResultingStateCountingGraph<State, TokenId, MaxNodes, MaxTransitions>;
    const auto builder = make_nfaBuilder<G>();
    const auto& transitionCounts = builder.ns.getResultingStateCounts();
    const auto maxResultingStates = [](const auto& a) {
        return max_element(a.begin(), a.end(), [](auto c, auto d) { return c.second < d.second; })->second;
    };
    return accumulate(transitionCounts.begin(), transitionCounts.end(), 0LL, [&](auto a, const auto& b) {
        auto c = maxResultingStates(b);
        return std::max<int64_t>(a, c);
    });
}();

// TODO: MaxTransitions is not "Max Labels (symbols) per State".
// It's the total number of transitions per state (just as it's name says). But we would actually like to have the
// potentially lower number of max labels, because of the way the data structures we use structure the transitions. e.g.
// MaxTransitions is high because for the starting state and eps we have a lot of transitions (one per terminalid) but
// we might only have a maximum of two labels per state (eps and one non-eps symbol). [I'm not sure.] We can compute
// this number based on the transitionCounts above.

//#define CONSTEXPR
#ifdef CONSTEXPR
static constexpr const auto nfa = [] {
    using G = Graph<State, TokenId, MaxNodes, MaxTransitions, MaxResultStates>;
    auto builder = make_nfaBuilder<G>();
    return NFA<Symbol, State, TokenId, MaxNodes, MaxSymbols, MaxTransitions, MaxResultStates>(builder);
}();
static constexpr const auto dfa = toDFA<State>(nfa);
static constexpr const auto min_dfa = minimize<uint8_t>(dfa);
#else
auto nfa = [] {
    std::cout << std::endl;
    using G = Graph<State, TokenId, MaxNodes, MaxTransitions, MaxResultStates>;
    auto builder = make_nfaBuilder<G>();
    return NFA<Symbol, State, TokenId, MaxNodes, MaxSymbols, MaxTransitions, MaxResultStates>(builder);
}();

auto dfa = toDFA<State>(nfa);
auto min_dfa = [] {
    auto min_dfa_ = minimize<uint8_t>(dfa);
    // std::cout << "state count: " << nfa.stateCount << std::endl;
    // std::cout << "symbol count: " << nfa.symbolCount << std::endl;
    // std::cout << "start: " << nfa.start << std::endl;
    // std::cout << "T: \n";
    // for (const auto& t : nfa.table) {
    //    std::copy(t.begin(), t.end(), std::ostream_iterator<HashSet<MaxTransitions>>(std::cout, ", "));
    //    std::cout << std::endl;
    //}
    // std::cout << std::endl;
    // std::cout << "finals: ";
    // std::copy(nfa.finals.begin(), nfa.finals.end(), std::ostream_iterator<int>(std::cout, ", "));
    // std::cout << std::endl;
    // std::cout << "symbols: ";
    // std::copy(nfa.symbols.begin(), nfa.symbols.end(), std::ostream_iterator<int>(std::cout, ", "));
    // std::cout << std::endl;
    // std::cout << nfa << std::endl;
    // std::cout << std::endl;

    // std::cout << "state count: " << dfa.stateCount << std::endl;
    // std::cout << "symbol count: " << dfa.symbolCount << std::endl;
    // std::cout << "start: " << dfa.start << std::endl;
    // std::cout << "dead state: " << dfa.deadState << std::endl;
    // std::cout << "T: \n";
    // for (auto i = 0; i < dfa.stateCount; ++i) {
    //    std::copy(dfa.T.begin() + i * dfa.symbolCount, dfa.T.begin() + (i+1)*dfa.symbolCount,
    //    std::ostream_iterator<uint16_t>(std::cout, ", ")); std::cout << std::endl;
    //}
    // std::cout << "finals: ";
    // std::copy(dfa.finals.begin(), dfa.finals.end(), std::ostream_iterator<int>(std::cout, ", "));
    // std::cout << std::endl;
    // std::cout << "symbols: ";
    // std::copy(dfa.symbolToId.begin(), dfa.symbolToId.end(), std::ostream_iterator<int>(std::cout, ", "));
    // std::cout << std::endl;
    // std::cout << dfa << std::endl;

    // std::cout << "state count: " << min_dfa_.stateCount << std::endl;
    // std::cout << "symbol count: " << min_dfa_.symbolCount << std::endl;
    // std::cout << "start: " << int(min_dfa_.start) << std::endl;
    // std::cout << "dead state: " << int(min_dfa_.deadState) << std::endl;
    // std::cout << "T: \n";
    // for (auto i = 0; i < min_dfa_.stateCount; ++i) {
    //    std::copy(min_dfa_.T.begin() + i * min_dfa_.symbolCount, min_dfa_.T.begin() + (i+1)*min_dfa_.symbolCount,
    //    std::ostream_iterator<int>(std::cout, ", ")); std::cout << std::endl;
    //}
    // std::cout << "finals: ";
    // std::copy(min_dfa_.finals.begin(), min_dfa_.finals.end(), std::ostream_iterator<int>(std::cout, ", "));
    // std::cout << std::endl;
    // std::cout << "symbols: ";
    // std::copy(min_dfa_.symbolToId.begin(), min_dfa_.symbolToId.end(), std::ostream_iterator<int>(std::cout, ", "));
    // std::cout << std::endl;
    // std::cout << min_dfa_ << std::endl;
    return min_dfa_;
}();
#endif

/*constexpr*/ Token<TokenId> bla(const char* text) {
    Lexer<MaxNodes, MaxSymbols, Symbol, uint8_t, TokenId> lexer(min_dfa, {nullptr, 0, eof_terminalid}, ws_terminalid);
    lexer.c = text;
    return lexer.getToken();
}
