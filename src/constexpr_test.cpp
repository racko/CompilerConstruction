#include "constexpr.h"
#include "constexpr_jsonLexer.h"

#include <iostream>
#include <iterator>

#include <catch.hpp>

TEST_CASE("HashTable get fails if empty", "[HashTable]") {
    HashTable<int, 1> t;
    REQUIRE(t.get(0) == -1);
    REQUIRE(t.get(1) == -1);
    REQUIRE(t.get(2) == -1);
    REQUIRE(t.get(3) == -1);
}

TEST_CASE("HashTable set + get", "[HashTable]") {
    HashTable<int, 1> t;
    t.set(0);
    REQUIRE(t.get(0) != -1);
    REQUIRE(t.get(1) == -1);
    REQUIRE(t.get(2) == -1);
    REQUIRE(t.get(3) == -1);
    t.set(2);
    REQUIRE(t.get(0) != -1);
    REQUIRE(t.get(1) == -1);
    REQUIRE(t.get(2) != -1);
    REQUIRE(t.get(3) == -1);
}


//static constexpr const int x = [] {
//    using Alloc = stack_allocator<int,1024>;
//    Alloc stack_alloc;
//    allocator<int,Alloc> alloc(&stack_alloc);
//    int* arr = alloc.allocate(10);
//    arr[0] = 1;
//    arr[1] = 2;
//    arr[2] = 3;
//    alloc.deallocate(arr, 10);
//    return 0;
//}();

TEST_CASE("strcatsize no argument", "[strcatsize]") {
    REQUIRE(strcatsize() == 0);
}

TEST_CASE("strcatsize empty cstr", "[strcatsize]") {
    REQUIRE(strcatsize<charr<1>>() == 0);
    REQUIRE(strcatsize<char[1]>() == 0);
    REQUIRE(strcatsize<const char[1]>() == 0);
}

TEST_CASE("strcatsize singleton cstr", "[strcatsize]") {
    REQUIRE(strcatsize<charr<2>>() == 1);
    REQUIRE(strcatsize<char[2]>() == 1);
    REQUIRE(strcatsize<const char[2]>() == 1);
}

TEST_CASE("strcatsize empty string", "[strcatsize]") {
    REQUIRE(strcatsize<string<0>>() == 0);
}

TEST_CASE("strcatsize singleton string", "[strcatsize]") {
    REQUIRE(strcatsize<string<1>>() == 1);
}

TEST_CASE("strcat no argument", "[strcat]") {
    REQUIRE(strcat() == string<0>{});
}

TEST_CASE("strcat empty cstr", "[strcat]") {
    REQUIRE(strcat("") == string<0>{});
}

TEST_CASE("strcat empty cstr + empty cstr", "[strcat]") {
    REQUIRE(strcat("", "") == string<0>{});
}

TEST_CASE("strcat singleton cstr", "[strcat]") {
    REQUIRE(strcat("a") == string<1>{"a"});
}

TEST_CASE("strcat single cstr", "[strcat]") {
    REQUIRE(strcat("abc") == string<3>{"abc"});
}

TEST_CASE("strcat empty cstr + singleton cstr", "[strcat]") {
    REQUIRE(strcat("", "a") == string<1>{"a"});
}

TEST_CASE("strcat singleton cstr + singleton cstr", "[strcat]") {
    REQUIRE(strcat("a", "b") == string<2>{"ab"});
}

TEST_CASE("strcat empty cstr + cstr", "[strcat]") {
    REQUIRE(strcat("", "abc") == string<3>{"abc"});
}

TEST_CASE("strcat cstr + cstr", "[strcat]") {
    REQUIRE(strcat("ab", "abc") == string<5>{"ababc"});
}

TEST_CASE("strcat empty string", "[strcat]") {
    REQUIRE(strcat(string<0>{}) == string<0>{});
}

TEST_CASE("strcat empty string + empty string", "[strcat]") {
    REQUIRE(strcat(string<0>{}, string<0>{}) == string<0>{});
}

TEST_CASE("strcat singleton string", "[strcat]") {
    REQUIRE(strcat(string<1>{"a"}) == string<1>{"a"});
}

TEST_CASE("strcat single string", "[strcat]") {
    REQUIRE(strcat(string<3>{"abc"}) == string<3>{"abc"});
}

TEST_CASE("strcat empty string + singleton string", "[strcat]") {
    REQUIRE(strcat(string<0>{}, string<1>{"a"}) == string<1>{"a"});
}

TEST_CASE("strcat singleton string + singleton string", "[strcat]") {
    REQUIRE(strcat(string<1>{"a"}, string<1>{"b"}) == string<2>{"ab"});
}

TEST_CASE("strcat empty string + string", "[strcat]") {
    REQUIRE(strcat(string<0>{}, string<3>{"abc"}) == string<3>{"abc"});
}

TEST_CASE("strcat string + string", "[strcat]") {
    REQUIRE(strcat(string<2>{"ab"}, string<3>{"abc"}) == string<5>{"ababc"});
}

TEST_CASE("strcat cstr + string", "[strcat]") {
    REQUIRE(strcat("ab", string<3>{"abc"}) == string<5>{"ababc"});
}

TEST_CASE("strcat string + cstr", "[strcat]") {
    REQUIRE(strcat(string<2>{"ab"}, "abc") == string<5>{"ababc"});
}

TEST_CASE("vector emplace", "[vector]") {
    vector<int,10> v;
    v.emplace_back();
    v.emplace_back();
    v.emplace_back();
    REQUIRE(v.size() == 3);
    REQUIRE(!v.empty());
    REQUIRE(v.begin() < v.end());
}

//static constexpr size_t MaxSymbols = 128;
//static constexpr auto eof_terminalid = 11;
//static constexpr auto ws_terminalid = 12;
//using Symbol = char;
//using State = uint16_t;
//using TokenId = uint8_t;
//
//template<typename G>
//constexpr nfaBuilder<Symbol,State,MaxSymbols,G> make_nfaBuilder(int) {
//    nfaBuilder<Symbol,State,MaxSymbols,G> builder;
//    builder.lexRegex("false", 1);
//    builder.lexRegex("true", 2);
//    builder.lexRegex("null", 3);
//
//    char hexdigit[] = "(0|1|2|3|4|5|6|7|8|9|a|b|c|d|e|f|A|B|C|D|E|F)";
//    char nonzerodigit[] = "(1|2|3|4|5|6|7|8|9)";
//    char ws[] = "( |\t|\n|\r)*";
//
//    auto digit = strcat("(0|", nonzerodigit, ")");
//    auto integer = strcat("(0|", nonzerodigit, digit, "*)");
//    auto exp = strcat(R",(((e|E)(-|\+)?),", digit, digit, "*)");
//    auto frac = strcat("(.", digit, digit, "*)");
//    auto num = strcat("(-?", integer, frac, "?", exp, "?)");
//
//    string<195> unescaped;
//    unescaped += "( |!"; // 0x20, 0x21
//    // 0x22 is ", so it has to be escaped
//    for (char i = 0x23; i <= 0x27; ++i)
//        (unescaped += '|') += i;
//    unescaped += "|\\(|\\)|\\*|\\+";
//    for (char i = 0x2C; i <= 0x3E; ++i)
//        (unescaped += '|') += i;
//    unescaped += "|\\?";
//    for (char i = 0x40; i <= 0x5B; ++i)
//        (unescaped += '|') += i;
//    // 0x5C is \ -> escape
//    for (char i = 0x5D; i <= 0x7B; ++i)
//        (unescaped += '|') += i;
//    unescaped += "|\\|";
//    for (int i = 0x7D; i <= 0x7F; ++i) // int + static_cast because i <= 0x7F is always true for "char i" (infinite loop)
//        (unescaped += '|') += static_cast<char>(i);
//    // only ascii for now ...
//    unescaped += ')';
//
//    auto chr = strcat("(", unescaped, "|\\\\(\"|\\\\|/|b|f|n|r|t|u", hexdigit, hexdigit, hexdigit, hexdigit, "))");
//    auto str = strcat("(\"", chr, "*\")");
//
//    builder.lexRegex(strcat(ws, "[", ws).data(), 4);
//    builder.lexRegex(strcat(ws, "]", ws).data(), 5);
//    builder.lexRegex(strcat(ws, "{", ws).data(), 6);
//    builder.lexRegex(strcat(ws, "}", ws).data(), 7);
//    builder.lexRegex(strcat(ws, ":", ws).data(), 8);
//    builder.lexRegex(strcat(ws, ",", ws).data(), 9);
//    builder.lexRegex(num.data(), 10);
//    builder.lexRegex(str.data(), 11);
//    return builder;
//}
//
//static constexpr const auto MaxNodes = [] {
//    using G = CountingGraph<State,Symbol,TokenId>;
//    auto builder = make_nfaBuilder<G>(0);
//    return builder.ns.getStateCount();
//}();
//
//static constexpr const auto MaxTransitions = [] {
//    using G = TransitionCountingGraph<State,Symbol,TokenId,MaxNodes>;
//    auto builder = make_nfaBuilder<G>(0);
//    const auto& transitionCounts = builder.ns.getTransitionCounts();
//    return *max_element(transitionCounts.begin(), transitionCounts.end());
//}();
//
//static constexpr const auto MaxResultStates = [] {
//    using G = ResultingStateCountingGraph<State,Symbol,TokenId,MaxNodes,MaxTransitions>;
//    auto builder = make_nfaBuilder<G>(0);
//    const auto& transitionCounts = builder.ns.getResultingStateCounts();
//    auto maxResultingStates = [] (const auto& a) {return max_element(a.begin(), a.end(), [] (auto c, auto d) { return c.second < d.second; })->second; };
//    return accumulate(transitionCounts.begin(), transitionCounts.end(), 0, [&] (auto a, const auto& b) { auto c = maxResultingStates(b); return std::max<size_t>(a, c); });
//}();

//static constexpr const auto nfa = [] {
//    using G = Graph<State,Symbol,TokenId,MaxNodes,MaxTransitions,MaxResultStates>;
//    auto builder = make_nfaBuilder<G>();
//    return NFA<Symbol,State,TokenId,MaxNodes,MaxSymbols,MaxTransitions,MaxResultStates>(builder);
//}();
//
//static constexpr const auto dfa = toDFA(nfa);
//static constexpr const auto min_dfa = minimize<State>(dfa);
//
//constexpr Token bla(const char* text) {
//    Lexer<MaxNodes,MaxSymbols> lexer(min_dfa, {nullptr, 0, eof_terminalid}, ws_terminalid);
//    lexer.c = text;
//    return lexer.getToken();
//}

TEST_CASE("constexpr", "constexpr") {
    //const auto nfa = [] {
    //    using G = Graph<State,Symbol,TokenId,MaxNodes,MaxTransitions,MaxResultStates>;
    //    int i;
    //    std::cin >> i;
    //    auto builder = make_nfaBuilder<G>(i);
    //    return NFA<Symbol,State,TokenId,MaxNodes,MaxSymbols,MaxTransitions,MaxResultStates>(builder);
    //}();

    //const auto dfa = toDFA(nfa);
    //const auto min_dfa = minimize<State>(dfa);

    //auto bla = [&min_dfa] (const char* text) {
    //    Lexer<MaxNodes,MaxSymbols> lexer(min_dfa, {nullptr, 0, eof_terminalid}, ws_terminalid);
    //    lexer.c = text;
    //    return lexer.getToken();
    //};

    //std::cout << "state count: " << nfa.stateCount << std::endl;
    //std::cout << "symbol count: " << nfa.symbolCount << std::endl;
    //std::cout << "start: " << nfa.start << std::endl;
    //std::cout << "T: \n";
    //for (const auto& t : nfa.table) {
    //    std::copy(t.begin(), t.end(), std::ostream_iterator<HashSet<MaxNodes>>(std::cout, ", "));
    //    std::cout << std::endl;
    //}
    //std::cout << std::endl;
    //std::cout << "finals: ";
    //std::copy(nfa.finals.begin(), nfa.finals.end(), std::ostream_iterator<int>(std::cout, ", "));
    //std::cout << std::endl;
    //std::cout << "symbols: ";
    //std::copy(nfa.symbols.begin(), nfa.symbols.end(), std::ostream_iterator<int>(std::cout, ", "));
    //std::cout << std::endl;
    //std::cout << nfa << std::endl;
    //std::cout << std::endl;

    //std::cout << "state count: " << dfa.stateCount << std::endl;
    //std::cout << "symbol count: " << dfa.symbolCount << std::endl;
    //std::cout << "start: " << dfa.start << std::endl;
    //std::cout << "dead state: " << dfa.deadState << std::endl;
    //std::cout << "T: \n";
    //for (auto i = 0; i < dfa.stateCount; ++i) {
    //    std::copy(dfa.T.begin() + i * dfa.symbolCount, dfa.T.begin() + (i+1)*dfa.symbolCount, std::ostream_iterator<uint16_t>(std::cout, ", "));
    //    std::cout << std::endl;
    //}
    //std::cout << "finals: ";
    //std::copy(dfa.finals.begin(), dfa.finals.end(), std::ostream_iterator<int>(std::cout, ", "));
    //std::cout << std::endl;
    //std::cout << "symbols: ";
    //std::copy(dfa.symbolToId.begin(), dfa.symbolToId.end(), std::ostream_iterator<int>(std::cout, ", "));
    //std::cout << std::endl;
    //std::cout << dfa << std::endl;

    //std::cout << "state count: " << min_dfa.stateCount << std::endl;
    //std::cout << "symbol count: " << min_dfa.symbolCount << std::endl;
    //std::cout << "start: " << min_dfa.start << std::endl;
    //std::cout << "dead state: " << min_dfa.deadState << std::endl;
    //std::cout << "T: \n";
    //for (auto i = 0; i < min_dfa.stateCount; ++i) {
    //    std::copy(min_dfa.T.begin() + i * min_dfa.symbolCount, min_dfa.T.begin() + (i+1)*min_dfa.symbolCount, std::ostream_iterator<uint16_t>(std::cout, ", "));
    //    std::cout << std::endl;
    //}
    //std::cout << "finals: ";
    //std::copy(min_dfa.finals.begin(), min_dfa.finals.end(), std::ostream_iterator<int>(std::cout, ", "));
    //std::cout << std::endl;
    //std::cout << "symbols: ";
    //std::copy(min_dfa.symbolToId.begin(), min_dfa.symbolToId.end(), std::ostream_iterator<int>(std::cout, ", "));
    //std::cout << std::endl;
    //std::cout << min_dfa << std::endl;
//
//    static_assert(bla("false").tokenId == 1);
//    static_assert(bla("true").tokenId == 2);
//    static_assert(bla("null").tokenId == 4);
//

    REQUIRE(bla("false").tokenId == 1);
    REQUIRE(bla("true").tokenId == 2);
    REQUIRE(bla("null").tokenId == 3);
    REQUIRE(bla("[").tokenId == 4);
    REQUIRE(bla("]").tokenId == 5);
    REQUIRE(bla("{").tokenId == 6);
    REQUIRE(bla("}").tokenId == 7);
    REQUIRE(bla(":").tokenId == 8);
    REQUIRE(bla(",").tokenId == 9);

    REQUIRE(bla("0").tokenId == 10);
    REQUIRE(bla("1").tokenId == 10);
    REQUIRE(bla("1.0").tokenId == 10);
    REQUIRE(bla("0.0").tokenId == 10);
    REQUIRE(bla("-1").tokenId == 10);
    REQUIRE(bla("-1.0").tokenId == 10);
    REQUIRE(bla("1e0").tokenId == 10);
    REQUIRE(bla("1E0").tokenId == 10);
    REQUIRE(bla("1e-0").tokenId == 10);
    REQUIRE(bla("1e01").tokenId == 10);
    REQUIRE(bla("1e+0").tokenId == 10);

    {
    // this one's tricky: "01" is not a valid number, but that doesn't mean that the lexer will throw.
    // Instead, the lexer returns the longest valid token: "0"
    auto token = bla("01");
    REQUIRE(token.tokenId == 10);
    REQUIRE(token.start != nullptr);
    REQUIRE(*token.start == '0');
    REQUIRE(token.length == 1);
    }

    {
    // same here
    auto token = bla("1.");
    REQUIRE(token.tokenId == 10);
    REQUIRE(token.start != nullptr);
    REQUIRE(*token.start == '1');
    REQUIRE(token.length == 1);
    }

    REQUIRE_THROWS(bla("-"));
    REQUIRE_THROWS(bla("."));

    REQUIRE(bla(R"("")").tokenId == 11);
    REQUIRE(bla(R"("abc")").tokenId == 11);
    REQUIRE(bla(R"("Hello World!")").tokenId == 11);
    REQUIRE(bla(R"("\ttwo\n\tlines\r\n\twith tabs etc.")").tokenId == 11);
    REQUIRE(bla(R"("two\nlines")").tokenId == 11);
    REQUIRE(bla(R"("Back slash: \\")").tokenId == 11);
    REQUIRE(bla(R"("\"Hello World!\"")").tokenId == 11);
    REQUIRE(bla(R"("\/usr\/bin\/bash")").tokenId == 11);
    REQUIRE(bla(R"("\u00b0 is uncode for the degree sign")").tokenId == 11);
    REQUIRE(bla(R"("\u00B0 is uncode for the degree sign with a capital B")").tokenId == 11);
    REQUIRE_THROWS(bla(R"("\u0b0 is not valid because we require 4 hex-digits")"));
    REQUIRE_THROWS(bla(R"(")"));
    REQUIRE_THROWS(bla(R"("abc)"));
    REQUIRE_THROWS(bla(R"(abc")"));
    REQUIRE_THROWS(bla(R"(a)"));
    REQUIRE_THROWS(bla(R"(abc)"));

    REQUIRE(bla("").tokenId == eof_terminalid);
    REQUIRE_THROWS(bla(" "));
    REQUIRE_THROWS(bla("garbage"));
}
