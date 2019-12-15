#include "rank_count.h"

#include "Regex/Print.h"
#include <ostream>

std::ostream& operator<<(std::ostream& s, const RankCount& x) { return s << bin(x.bits()); }
