#pragma once

#include <cassert>
#include <vector>
#include <list>
#include <iterator>
#include <ostream>
#include <unordered_set>
#include <unordered_map>

enum class kind { TERMINAL, NONTERMINAL, EPS, EOI };

template<class G>
struct GrammarElement;

template<class G>
struct TerminalID;

template<class G>
struct NonterminalID;

template<class G>
struct GrammarElement {
    uint32_t x;
    explicit constexpr GrammarElement(uint32_t _x) : x(_x) {}
    explicit GrammarElement(TerminalID<G> _x) : x(_x.x) {}
    explicit GrammarElement(NonterminalID<G> _x) : x(_x.x + G::numberOfTerminals + 1) {}

    explicit operator TerminalID<G>() const {
        return TerminalID<G>(x);
    }

    explicit operator NonterminalID<G>() const {
        return NonterminalID<G>(x - G::numberOfTerminals - 1);
    }
};

template<class G>
struct TerminalID {
    uint32_t x;

    explicit constexpr TerminalID(uint32_t _x) : x(_x) {}

    explicit operator GrammarElement<G>() const {
        return GrammarElement<G>(x);
    }
};

template<class G>
struct NonterminalID {
    uint32_t x;

    explicit constexpr NonterminalID(uint32_t _x) : x(_x) {}

    explicit operator GrammarElement<G>() const {
        return GrammarElement<G>(x + G::numberOfTerminals + 1);
    }
};

template<class G>
bool operator==(const TerminalID<G> lhs, const TerminalID<G> rhs) {
    return lhs.x == rhs.x;
}

template<class G>
bool operator!=(const TerminalID<G> lhs, const TerminalID<G> rhs) {
    return !(lhs == rhs);
}

template<class G>
bool operator<(const TerminalID<G> lhs, const TerminalID<G> rhs) {
    return lhs.x < rhs.x;
}

template<class G>
bool operator==(const GrammarElement<G> lhs, const GrammarElement<G> rhs) {
    return lhs.x == rhs.x;
}

template<class G>
bool operator!=(const GrammarElement<G> lhs, const GrammarElement<G> rhs) {
    return !(lhs == rhs);
}

template<class G>
bool operator<(const GrammarElement<G> lhs, const GrammarElement<G> rhs) {
    return lhs.x < rhs.x;
}

template<class G>
bool operator==(const NonterminalID<G> lhs, const NonterminalID<G> rhs) {
    return lhs.x == rhs.x;
}

template<class G>
bool operator!=(const NonterminalID<G> lhs, const NonterminalID<G> rhs) {
    return !(lhs == rhs);
}

template<class G>
bool operator<(const NonterminalID<G> lhs, const NonterminalID<G> rhs) {
    return lhs.x < rhs.x;
}

template<class G>
std::ostream& operator<<(std::ostream& s, GrammarElement<G> const X) {
    switch(G::kindOf(X)) {
        case kind::TERMINAL:
            return s << TerminalID<G>(X);
        case kind::EPS:
            return s << "EPS";
        case kind::EOI:
            return s << "EOF";
        case kind::NONTERMINAL:
            return s << NonterminalID<G>(X);
        default:
            throw std::logic_error("unhandled case in operator<<(std::ostream& s, GrammarElement<G> const X)");
    }
}

template<class G>
std::ostream& operator<<(std::ostream& s, const std::vector<GrammarElement<G>>& alpha) {
    std::copy(alpha.begin(), alpha.end(), std::ostream_iterator<GrammarElement<G>>(s, " "));
    return s;
}

template <class G>
std::ostream& operator<<(std::ostream& s, const std::list<GrammarElement<G>>& alpha) {
    std::copy(alpha.begin(), alpha.end(), std::ostream_iterator<GrammarElement<G>>(s, " "));
    return s;
}

template <class G>
std::ostream& operator<<(std::ostream& s, const std::unordered_set<TerminalID<G>>& alpha) {
    std::copy(alpha.begin(), alpha.end(), std::ostream_iterator<TerminalID<G>>(s, " "));
    return s;
}

namespace std {
template<class G>
struct hash<TerminalID<G>> {
public:
    size_t operator()(const TerminalID<G> a) const {
        return std::hash<decltype(a.x)>()(a.x);
    }
};

template<class G>
struct hash<NonterminalID<G>> {
public:
    size_t operator()(const NonterminalID<G> A) const {
        return std::hash<decltype(A.x)>()(A.x);
    }
};

template<class G>
struct hash<GrammarElement<G>> {
public:
    size_t operator()(const GrammarElement<G> X) const {
        return std::hash<decltype(X.x)>()(X.x);
    }
};
}

template<class G>
void first(const std::vector<GrammarElement<G>>& alpha, const std::unordered_map<GrammarElement<G>, std::unordered_set<TerminalID<G>>>& fs, std::unordered_set<TerminalID<G>>& out) {
    //std::cout << "alpha: " << alpha << std::endl;
    for (auto Y : alpha) {
        //std::cout << "Y: " << Y << std::endl;
        const auto& firstY = fs.at(Y);
        //std::cout << "firstY: " << firstY << std::endl;
        out.insert(firstY.begin(), firstY.end());
        //std::cout << "out: " << out << std::endl;

        auto it = out.find(G::eps);
        if (it == out.end()) {
            //std::cout << "out does not contain eps. Done" << std::endl;
            return;
        }

        //std::cout << "out does contain eps. Removing it before continuing" << std::endl;
        out.erase(it);
    }
    //std::cout << "production can produce eps. Adding it to out again before returning." << std::endl;

    out.insert(G::eps);
}

template<class G>
bool updateFirsts(std::unordered_map<GrammarElement<G>, std::unordered_set<TerminalID<G>>>& fs) {
    bool change = false;
    std::unordered_set<TerminalID<G>> buffer;
    for (auto i = typename G::type(); i < G::getNumberOfNonterminals(); ++i) {
        NonterminalID<G> A(i);
        auto& fsA = fs[A];
        //std::cout << A << ": " << fsA << std::endl;
        auto oldSize = fsA.size();
        const auto& productions = G::getProductions(A);
        for (const auto& alpha : productions) {
            // first needs to be called with empty buffer because that's how we figure out if eps is in the result
            buffer.clear();
            first(alpha, fs, buffer);
            fsA.insert(buffer.begin(), buffer.end());
        }
        //std::cout << A << ": " << fsA << std::endl;

        if (fsA.size() > oldSize)
            change = true;
    }
    //std::cout << std::endl;
    return change;
}

// TODO: topological order
// 1. create initial sets as below
// 2. add eps to first[A] if A -> eps is a production
// 3. create dependency graph using usual rules
// 4. find topological order
// 5. fill first sets in topological order

template<class G>
std::unordered_map<GrammarElement<G>, std::unordered_set<TerminalID<G>>> allFirsts() {
    std::unordered_map<GrammarElement<G>, std::unordered_set<TerminalID<G>>> fs(G::getNumberOfGrammarElements());
    fs[G::eps].insert(G::eps);

    for (auto i = typename G::type(); i < G::getNumberOfTerminals(); ++i) {
        auto a = TerminalID<G>(i);
        fs[a].insert(a);
    }

    for (auto i = typename G::type(); i < G::getNumberOfNonterminals(); ++i) {
        auto a = NonterminalID<G>(i);
        fs[a]; // this intializes an empty set
    }

    bool change;
    do change = updateFirsts<G>(fs);
    while(change);

    return fs;
}

template<class G>
bool updateFollows(std::unordered_map<NonterminalID<G>, std::unordered_set<TerminalID<G>>>& fs) {
    bool change = false;

    std::unordered_set<TerminalID<G>> buffer;
    for (auto i = typename G::type(); i < G::getNumberOfNonterminals(); ++i) {
        NonterminalID<G> A(i);
        for (const auto& production : G::getProductions(A)) {
            assert(production.begin() != production.end());
            auto it = production.begin();
            // "A -> alpha B beta" case
            for (; it != std::prev(production.end()); ++it) {
                if (G::kindOf(*it) == kind::NONTERMINAL) {
                    // first needs to be called with empty buffer because that's how we figure out if eps is in the result
                    buffer.clear();
                    first(typename G::String(std::next(it), production.end()), G::getFirsts(), buffer);
                    if (buffer.find(G::eps) != buffer.end()) {
                        const auto& fsA = fs[A];
                        auto& fsB = fs[*it];
                        auto oldSize = fsB.size();
                        fsB.insert(fsA.begin(), fsA.end());
                        change = fsA.size() != oldSize;
                    }
                }
            }
            /// "A -> alpha B" case
            if (G::kindOf(*it) == kind::NONTERMINAL) {
                const auto& fsA = fs[A];
                auto& fsB = fs[*it];
                auto oldSize = fsB.size();
                fsB.insert(fsA.begin(), fsA.end());
                change = fsA.size() != oldSize;
            }
        }
    }
    return change;
}

template<class G>
std::unordered_map<NonterminalID<G>, std::unordered_set<TerminalID<G>>> allFollows() {
    std::unordered_map<NonterminalID<G>, std::unordered_set<TerminalID<G>>> out(G::getNumberOfNonterminals());

    // $ follows start symbol S
    out[G::start].insert(G::eof);

    // if there is a production A -> alpha B beta, FOLLOW(B) contains FIRST(beta) minus {eps}
    std::unordered_set<TerminalID<G>> buffer;
    for (auto i = typename G::type(); i < G::getNumberOfNonterminals(); ++i) {
        NonterminalID<G> A(i);
        for (const auto& production : G::getProductions(A)) {
            assert(production.begin() != production.end());
            for (auto it = production.begin(); it != std::prev(production.end()); ++it) {
                if (G::kindOf(*it) == kind::NONTERMINAL) {
                    // first needs to be called with empty buffer because that's how we figure out if eps is in the result
                    buffer.clear();
                    first(typename G::String(std::next(it), production.end()), G::getFirsts(), buffer);
                    buffer.erase(G::eps);
                    out[*it].insert(buffer.begin(), buffer.end());
                }
            }
        }
    }


    bool change;
    do change = updateFollows<G>(out);
    while(change);

    return out;
}


template<class G>
std::unordered_set<TerminalID<G>> first(const std::vector<GrammarElement<G>>& alpha) {
    std::unordered_set<TerminalID<G>> out(G::getNumberOfTerminals() + 1);
    first(alpha, G::getFirsts(), out);
    return out;
}

template<class G>
std::unordered_set<TerminalID<G>> follow(NonterminalID<G> A) {
    return G::getFollows(A);
}
