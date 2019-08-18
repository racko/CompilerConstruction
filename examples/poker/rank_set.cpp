#include "rank_set.h"

#include <ostream>
#include "Print.h"

std::ostream& operator<<(std::ostream& s, const RankSet& x) { return s << bin(x.bits()); }
