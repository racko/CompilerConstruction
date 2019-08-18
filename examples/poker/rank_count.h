#pragma once

#include "Print.h"
#include "ranges.h"
#include "rank_set.h"
#include "ranks.h"
#include "transition.h"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iosfwd>

class RankCount {
  public:
    RankCount() = default;
    explicit RankCount(const std::uint64_t x) : x_{x} {}
    RankCount(const RankSet fourKRanks, const RankSet threeKRanks, const RankSet pairRanks, const RankSet kickerRanks)
        : x_{(fourKRanks.bits() << 39) | (threeKRanks.bits() << 26) | (pairRanks.bits() << 13) | kickerRanks.bits()} {}
    RankSet kickerRanks() const { return RankSet{x_ & 0x1FFF}; }
    RankSet pairRanks() const { return RankSet{(x_ >> 13) & 0x1FFF}; }
    RankSet threeKRanks() const { return RankSet{(x_ >> 26) & 0x1FFF}; }
    RankSet fourKRanks() const { return RankSet{(x_ >> 39) & 0x1FFF}; }
    RankCount addRank(const std::uint32_t r) const { return RankCount{::addRank(x_, r)}; }
    template <typename R = int>
    R countRanks() const {
        return ::countCards<R>(x_);
    }
    bool operator==(const RankCount& rhs) const { return x_ == rhs.x_; }
    bool operator!=(const RankCount& rhs) const { return x_ != rhs.x_; }
    std::uint64_t bits() const { return x_; }
    std::uint64_t& bits() { return x_; }
    bool empty() const { return x_ == 0; }

  private:
    std::uint64_t x_{};
};

std::ostream& operator<<(std::ostream& s, const RankCount& x);

inline int countCards(const RankCount k) { return countCards(k.bits()); }

namespace std {
template <>
class hash<RankCount> {
  public:
    std::size_t operator()(const RankCount s) const noexcept { return std::hash<std::uint64_t>()(s.bits()); }
};
} // namespace std

namespace boost {
namespace serialization {

template <class Archive>
void serialize(Archive& ar, RankCount& k, unsigned int) {
    ar& k.bits();
}

} // namespace serialization
} // namespace boost

inline auto children(const RankCount k) {
    auto make_transition = [k](const std::uint32_t rank) { return Transition<RankCount>{k, k.addRank(rank), rank}; };
    auto is_valid = [](const Transition<RankCount> t) { return t.child != t.parent; };
    return filter(is_valid, transform(make_transition, range(k.countRanks() != 7 ? 0U : 13U, 13U)));
}
