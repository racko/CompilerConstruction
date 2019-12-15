#include "sorted_cards.h"

#include "Regex/Print.h"
#include "bits.h"
#include <ostream>

std::ostream& operator<<(std::ostream& s, const SortedCards cards) { return s << bin(cards.bits); }

int countCards(const SortedCards cards) { return countBits(cards.bits); }
