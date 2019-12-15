#pragma once

#include "Regex/Grammar.h" // for first, follow, kind, kind::TERMINAL
#include <assert.h>  // for assert
#include <cstdint>   // for uint32_t
#include <iostream>  // for operator<<, stringstream, ostream, basic_ostream
#include "Regex/parser.h"  // for Parser
#include <sstream>
#include <stdexcept> // for runtime_error, logic_error
#include <vector>    // for vector

template <typename TokenType>
class TokenStream;

namespace ll_parser {
enum class type : uint32_t { PRODUCTION, FAIL };

inline uint32_t bits(type t) { return uint32_t(t) << 30; }

template <class G>
struct action {
    uint32_t x;

    action() = default;

    action(type t) : x(bits(t)) { assert(t == type::FAIL); }

    action(type t, typename G::NonterminalID A, uint32_t production) : x(bits(t) | A.x << 15 | production) {
        assert(A.x < (1u << 16) && production < (1u << 16) && t == type::PRODUCTION);
        auto a = getHead();
        auto p = getProduction();
        assert(a == A && p == production);
    }

    type getType() const { return type(x >> 30); }

    typename G::NonterminalID getHead() const {
        assert(getType() == type::PRODUCTION);
        auto shifted = x >> 15U;
        auto id = shifted & ((1U << 15U) - 1U);
        assert(id < G::getNumberOfNonterminals());
        return typename G::NonterminalID(static_cast<typename G::type>(id));
    }

    uint32_t getProduction() const {
        assert(getType() == type::PRODUCTION);
        return x & ((1 << 15) - 1);
    }
};

template <class G>
std::ostream& operator<<(std::ostream& s, const action<G>& a) {
    switch (a.getType()) {
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

template <class G, class T>
struct LLParser : Parser<T, typename G::Token> {
    LLParser();

    T parse(TokenStream<typename G::Token>& w) override;

  private:
    // TODO. Semantics: Assume that A is on the stack and contains inherited attributes. Push symbols of the given production of A onto the stack and copy inherited attributes into place.
    // Maybe keep A on the stack below the symbols of the new production so that we can use it as a place to store synthetic attributes. Then we need a way to check if we need to expand A or just run another action to copy the synthesized attributes to somewhere below A.
    // virtual T expand(typename G::NonterminalID& A, uint32_t production, T* alpha) = 0;
    // virtual T shift(typename G::Token&& t) = 0;

    using state = uint32_t;
    std::vector<std::vector<action<G>>> action_table;

    void setAction(typename G::NonterminalID A, typename G::TerminalID a, std::size_t p) {
        if (action_table[A.x][a.x].getType() != type::FAIL) {
            std::stringstream message;
            message << "Conflict in action_table[" << A << "][" << a << "]: " << action_table[A.x][a.x] << " or "
                    << action<G>(type::PRODUCTION, A, p);
            throw std::runtime_error(message.str());
        }
        std::cout << "action_table[" << A << "][" << a << "]: " << action<G>(type::PRODUCTION, A, p) << std::endl;
        action_table[A.x][a.x] = action<G>(type::PRODUCTION, A, p);
    }

  protected:
    ~LLParser() = default;
};

/// @TODO: CONFLICT HANDLING
template <class G, class T>
LLParser<G, T>::LLParser()
    : action_table(G::getNumberOfNonterminals(),
                   std::vector<action<G>>(G::getNumberOfTerminals(), action<G>(type::FAIL))) {
    for (auto i = 0; i < G::getNumberOfNonterminals(); ++i) {
        typename G::NonterminalID A(i);
        const auto& productions = G::getProductions(A);
        for (auto p = 0; p < productions.size(); ++p) {
            const auto& alpha = productions[p];
            for (auto a : first<G>(alpha)) {
                if (a != G::eps) {
                    setAction(A, a, p);
                } else {
                    for (auto b : follow<G>(A)) {
                        setAction(A, b, p);
                    }
                }
            }
        }
    }
}

template <class G, class T>
T LLParser<G, T>::parse(TokenStream<typename G::Token>& token_stream) {
    std::vector<typename G::GrammarElement> stack{G::eof, G::start};
    // std::vector<T> attributes{T(), T()}; /// @TODO: this gotta be wrong ...
    while (stack.back() != typename G::GrammarElement(G::eof)) {
        auto tag = G::getTag(token_stream.peek());

        if (stack.back() == typename G::GrammarElement(tag)) {
            std::cout << "match " << tag << std::endl;
            stack.pop_back();
            token_stream.step();
        } else if (G::kindOf(stack.back()) == kind::TERMINAL) {
            std::stringstream message;
            message << "Expected " << stack.back() << ", got " << tag;
            throw std::runtime_error(message.str());
        } else {
            auto A = static_cast<typename G::NonterminalID>(stack.back());
            if (action_table[A.x][tag.x].getType() == type::FAIL) {
                std::stringstream message;
                message << "ACTION[" << A << "][" << tag << "] is FAIL";
                throw std::runtime_error(message.str());
            } else {
                const auto& action_ = action_table[A.x][tag.x];
                const auto& production = G::getProductions(action_.getHead())[action_.getProduction()];
                std::cout << action_.getHead() << " -> " << production << std::endl;
                stack.pop_back();
                if (production[0] != typename G::GrammarElement(G::eps))
                    stack.insert(stack.end(), production.rbegin(), production.rend());
            }
        }
    }
    return T();
}
} // namespace ll_parser
