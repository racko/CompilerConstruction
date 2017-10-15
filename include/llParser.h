#pragma once

#include "Grammar.h"
#include "Print.h"

#include <iostream>
#include <sstream>

enum class type : uint32_t {
    PRODUCTION,
    FAIL
};

inline uint32_t bits(type t) { return uint32_t(t) << 30; }

template<class G>
struct action {
    uint32_t x;

    action() = default;

    action(type t) : x(bits(t)) { assert(t == type::FAIL); }

    action(type t, NonterminalID<G> A, uint32_t production) : x(bits(t) | A.x << 15 | production) {
        assert(A.x < (1u << 16) && production < (1u << 16) && t == type::PRODUCTION);
        auto a = getHead();
        auto p = getProduction();
        assert(a == A && p == production);
    }

    type getType() const { return type(x >> 30); }

    NonterminalID<G> getHead() const {
        assert(getType() == type::PRODUCTION);
        auto shifted = x >> 15U;
        auto id = shifted & ((1U << 15U) - 1U);
        assert(id < G::getNumberOfNonterminals());
        return NonterminalID<G>(static_cast<typename G::type>(id));
    }

    uint32_t getProduction() const { assert(getType() == type::PRODUCTION); return x & ((1 << 15) - 1); }
};

template <class G>
std::ostream& operator<<(std::ostream& s, const action<G>& a) {
    switch(a.getType()) {
    case type::FAIL:
        s << "fail";
        break;
    case type::PRODUCTION:
        s << "(" << a.getHead() << " -> " << G::getProductions(a.getHead())[a.getProduction()] << ")";
        break;
    default:
        throw std::logic_error("unhandled action type");
    }
    return s;
}

template<class G, class T>
struct LLParser {
    LLParser();

    template<class InputIterator>
    T parse(InputIterator w);

private:
    //virtual T action(NonterminalID<G> A, uint32_t production, T* alpha, size_t n) = 0;
    //virtual T shift(typename G::Token_rv_reference t) = 0;

    using state = uint32_t;
    std::vector<std::vector<action<G>>> action_table;

protected:
    ~LLParser() = default;
};


/// @TODO: CONFLICT HANDLING
template<class G, class T>
LLParser<G, T>::LLParser() : action_table(G::getNumberOfNonterminals(), std::vector<action<G>>(G::getNumberOfTerminals(), action<G>(type::FAIL))) {
    for (auto i = 0; i < G::getNumberOfNonterminals(); ++i) {
        NonterminalID<G> A(i);
        const auto& productions = G::getProductions(A);
        for (auto p = 0; p < productions.size(); ++p) {
            const auto& alpha = productions[p];
            for (auto a : first(alpha)) {
                if (a != G::eps) {
                    if (action_table[A.x][a.x].getType() != type::FAIL) {
                        std::stringstream message;
                        message << "Conflict in action_table[" << A << "][" << a << "]: " << action_table[A.x][a.x] << " or " << action<G>(type::PRODUCTION, A, p);
                        throw std::runtime_error(message.str());
                    }
                    std::cout << "action_table[" << A << "][" << a << "]: " << action<G>(type::PRODUCTION, A, p) << std::endl;
                    action_table[A.x][a.x] = action<G>(type::PRODUCTION, A, p);
                } else {
                    for (auto b : follow(A)) {
                        if (action_table[A.x][b.x].getType() != type::FAIL) {
                            std::stringstream message;
                            message << "Conflict in action_table[" << A << "][" << b << "]: " << action_table[A.x][b.x] << " or " << action<G>(type::PRODUCTION, A, p);
                            throw std::runtime_error(message.str());
                        }
                        std::cout << "action_table[" << A << "][" << b << "]: " << action<G>(type::PRODUCTION, A, p) << std::endl;
                        action_table[A.x][b.x] = action<G>(type::PRODUCTION, A, p);
                    }
                }
            }
        }
    }
}

template<class G, class T>
template<typename InputIterator>
T LLParser<G, T>::parse(InputIterator a) {
    std::vector<GrammarElement<G>> stack{G::eof, G::start};
    //std::vector<T> attributes{T(), T()}; /// @TODO: this gotta be wrong ...
    while (stack.back() != GrammarElement<G>(G::eof)) {
        auto token = std::move(*a); /// @FIXME: only move from *a when we shift. Current implementation only works because it just copies
        auto tag = G::getTag(token);

        if (stack.back() == GrammarElement<G>(tag)) {
            std::cout << "match " << tag << std::endl;
            stack.pop_back();
            ++a;
        } else if (G::kindOf(stack.back()) == kind::TERMINAL) {
            std::stringstream message;
            message << "Expected " << stack.back() << ", got " << tag;
            throw std::runtime_error(message.str());
        } else {
            auto A = static_cast<NonterminalID<G>>(stack.back());
            if (action_table[A.x][tag.x].getType() == type::FAIL) {
                std::stringstream message;
                message << "ACTION[" << A << "][" << tag << "] is FAIL";
                throw std::runtime_error(message.str());
            } else {
                const auto& action_ = action_table[A.x][tag.x];
                const auto& production = G::getProductions(action_.getHead())[action_.getProduction()];
                std::cout << action_.getHead() << " -> " << production << std::endl;
                stack.pop_back();
                if (production[0] != GrammarElement<G>(G::eps))
                    stack.insert(stack.end(), production.rbegin(), production.rend());
            }
        }
    }
    return T();
}
