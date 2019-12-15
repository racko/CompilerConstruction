#include "Regex/Grammar2.h"

#include <utility> // for move

namespace grammar2 {
Grammar::Grammar(uint32_t numberOfTerminals,
                 uint32_t numberOfNonterminals,
                 NonterminalID start,
                 TerminalID eps,
                 TerminalID eoi,
                 std::vector<const char*> terminalStrings,
                 std::vector<const char*> nonTerminalStrings,
                 Productions productions)
    : numberOfTerminals_(numberOfTerminals),
      numberOfNonterminals_(numberOfNonterminals),
      start_(start),
      eps_(eps),
      eoi_(eoi),
      terminalStrings_(std::move(terminalStrings)),
      nonTerminalStrings_(std::move(nonTerminalStrings)),
      productions_(std::move(productions)),
      firsts_(allFirsts(*this)) {}

void first(TerminalID eps,
           const std::vector<GrammarElement>& alpha,
           const std::unordered_map<GrammarElement, std::unordered_set<TerminalID>>& fs,
           std::unordered_set<TerminalID>& out) {
    // std::cout << "alpha: " << alpha << std::endl;
    for (auto Y : alpha) {
        // std::cout << "Y: " << Y << std::endl;
        const auto& firstY = fs.at(Y);
        // std::cout << "firstY: " << firstY << std::endl;
        out.insert(firstY.begin(), firstY.end());
        // std::cout << "out: " << out << std::endl;

        auto it = out.find(eps);
        if (it == out.end()) {
            // std::cout << "out does not contain eps. Done" << std::endl;
            return;
        }

        // std::cout << "out does contain eps. Removing it before continuing" << std::endl;
        out.erase(it);
    }
    // std::cout << "production can produce eps. Adding it to out again before returning." << std::endl;

    out.insert(eps);
}

bool updateFirsts(const Grammar& grammar, std::unordered_map<GrammarElement, std::unordered_set<TerminalID>>& fs) {
    bool change = false;
    std::unordered_set<TerminalID> buffer;
    for (auto i = 0U; i < grammar.getNumberOfNonterminals(); ++i) {
        NonterminalID A(i);
        auto& fsA = fs[grammar.toGrammarElement(A)];
        // std::cout << A << ": " << fsA << std::endl;
        auto oldSize = fsA.size();
        const auto& productions = grammar.getProductions(A);
        for (const auto& alpha : productions) {
            buffer.clear();
            first(grammar.getEps(), alpha, fs, buffer);
            fsA.insert(buffer.begin(), buffer.end());
        }
        // std::cout << A << ": " << fsA << std::endl;

        if (fsA.size() > oldSize)
            change = true;
    }
    // std::cout << std::endl;
    return change;
}

// TODO: topological order
// 1. create initial sets as below
// 2. add eps to first[A] if A -> eps is a production
// 3. create dependency graph using usual rules
// 4. find topological order
// 5. fill first sets in topological order

std::unordered_map<GrammarElement, std::unordered_set<TerminalID>> allFirsts(const Grammar& grammar) {

    std::unordered_map<GrammarElement, std::unordered_set<TerminalID>> fs(grammar.getNumberOfGrammarElements());

    fs[grammar.toGrammarElement(grammar.getEps())].insert(grammar.getEps());

    for (auto i = 0U; i < grammar.getNumberOfTerminals(); ++i) {
        auto a = TerminalID(i);
        fs[grammar.toGrammarElement(a)].insert(a);
    }

    for (auto i = 0U; i < grammar.getNumberOfNonterminals(); ++i) {
        auto a = NonterminalID(i);
        fs[grammar.toGrammarElement(a)]; // this intializes an empty set
    }

    bool change;
    do
        change = updateFirsts(grammar, fs);
    while (change);

    return fs;
}

std::unordered_set<TerminalID> first(const Grammar& grammar, const std::vector<GrammarElement>& alpha) {
    std::unordered_set<TerminalID> out(grammar.getNumberOfTerminals() + 1U);
    first(grammar.getEps(), alpha, grammar.getFirsts(), out);
    return out;
}
} // namespace grammar2
