#include "hand_rank.h"

#include <algorithm>
#include <iterator>
#include <ostream>

bool operator==(const HandRank& a, const HandRank& b) { return a.type() == b.type() && a.ranks() == b.ranks(); }

bool operator<(const HandRank& a, const HandRank& b) {
    if (a.type() < b.type())
        return true;
    if (a.type() == b.type())
        return a.ranks() < b.ranks();
    return false;
}

std::ostream& operator<<(std::ostream& s, const HandRank& a) {
    s << int(a.type()) << ", ";
    std::copy(a.ranks().begin(), a.ranks().end(), std::ostream_iterator<int>(s, " "));
    return s;
}
