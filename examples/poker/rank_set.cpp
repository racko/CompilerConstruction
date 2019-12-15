#include "rank_set.h"

#include "Regex/Print.h"
#include <ostream>

std::ostream& operator<<(std::ostream& s, const RankSet& x) { return s << bin(x.bits()); }
