#pragma once

#include "Regex/Grammar2.h" // for NonterminalID, operator==, Grammar (ptr only)
#include <cassert>    // for assert
#include <cstdint>    // for uint32_t
#include <memory>     // for unique_ptr

namespace lr_parser2 {
enum class type : uint32_t { SHIFT, REDUCE, ACCEPT, FAIL };

struct action {
    uint32_t x;

    action() = default;
    action(type t, uint32_t s) : x(bits(t) | s) { assert(s < (1u << 31) && t == type::SHIFT); }
    action(type t) : x(bits(t)) { assert(t == type::ACCEPT || t == type::FAIL); }
    action(type t, grammar2::NonterminalID A, uint32_t production) : x(bits(t) | A.x << 15 | production) {
        assert(A.x < (1u << 16) && production < (1u << 16) && t == type::REDUCE);
        assert(getHead() == A && getProduction() == production);
    }
    type getType() const { return type(x >> 30); }
    uint32_t getState() const {
        assert(getType() == type::SHIFT);
        return x & ~(3u << 30);
    }
    grammar2::NonterminalID getHead() const {
        assert(getType() == type::REDUCE);
        auto shifted = x >> 15;
        auto m = (1U << 15) - 1U;
        return grammar2::NonterminalID(shifted & m);
    }
    uint32_t getProduction() const {
        assert(getType() == type::REDUCE);
        return x & ((1U << 15) - 1U);
    }

  private:
    uint32_t bits(type t) { return uint32_t(t) << 30; }
};

class LRParser {
  public:
    LRParser(const grammar2::Grammar& grammar);
    ~LRParser();

    action step(grammar2::TerminalID);
    void reset();
    const grammar2::Grammar& grammar() const;

  private:
    struct impl;
    std::unique_ptr<impl> pimpl;
};
} // namespace lr_parser2
