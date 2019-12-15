#pragma once

#include "Regex/Print.h"
#include "ranks.h"
#include <cstdint>
#include <iosfwd>

class RankSet {
  public:
    RankSet() = default;
    explicit RankSet(const std::uint64_t x) : x_{x} {}
    bool hasRank(std::uint32_t rank) const { return x_ & (1ULL << rank); }
    RankSet getHighestRank() const { return RankSet{::getHighestRank(x_)}; }
    RankSet removeLowestRank() const { return RankSet{::removeLowestRank(x_)}; }
    RankSet removeNLowestRanks(const int n) const { return RankSet{::removeNLowestRanks(x_, n)}; }
    RankSet removeRanks(const RankSet ranks) const { return RankSet{x_ & ~ranks.bits()}; }
    template <typename R = int>
    R getLowestRank() const {
        return ::getLowestRank<R>(x_);
    }
    template <typename R = int>
    R countRanks() const {
        return ::countCards<R>(x_);
    }
    bool operator==(const RankSet& rhs) const { return x_ == rhs.x_; }
    bool operator!=(const RankSet& rhs) const { return x_ != rhs.x_; }
    std::uint64_t bits() const { return x_; }
    std::uint64_t& bits() { return x_; }
    bool empty() const { return x_ == 0; }

  private:
    std::uint64_t x_{};
};

std::ostream& operator<<(std::ostream& s, const RankSet& x);
