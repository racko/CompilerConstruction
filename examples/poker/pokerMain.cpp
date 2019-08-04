#include "NFA.h"
#include "DFA.h"
#include "nfaBuilder.h"
#include <ctime>
#include <iostream>
#include <iomanip>
using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::hex;
#include <sstream>
using std::stringstream;
using std::move;
#include <unordered_map>
using std::unordered_map;

//using State = uint16_t;
//using Symbol = uint8_t;
//using TerminalId = uint16_t;
using State = uint32_t;
using Symbol = uint8_t;
using TerminalId = uint16_t;

auto clearLowestBit(const auto v) {
    return v & v - 1;
}

auto clearNLowestBits(auto v, const auto n) {
    for (int j = 0; j < n; j++)
        v = clearLowestBit(v);
    return v;
}

auto getHighestBit(auto v) {
    decltype(v) result;
    while ((v = clearLowestBit(v)) != 0)
        result = v;
    return result;
}

auto removeLowestRank(const auto v) {
    return clearLowestBit(v);
}

auto removeNLowestRanks(const auto v, auto n) {
    return clearNLowestBits(v, n);
}

auto getHighestRank(const auto v) {
    return getHighestBit(v);
}

// Count the number of bits set in v.
// https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetNaive
//
// Brian Kernighan's method goes through as many iterations as there are set
// bits. So if we have a 32-bit word with only the high bit set, then it will
// only go once through the loop.
//
// Published in 1988, the C Programming Language 2nd Ed. (by Brian W. Kernighan
// and Dennis M. Ritchie) mentions this in exercise 2-9. On April 19, 2006 Don
// Knuth pointed out to me that this method "was first published by Peter Wegner
// in CACM 3 (1960), 322. (Also discovered independently by Derrick Lehmer and
// published in 1964 in a book edited by Beckenbach.)"
unsigned int countBits(unsigned int v) {
    unsigned int c = 0; // c accumulates the total bits set in v
    for (; v; ++c)
      v = clearLowestBit(v); // clear the least significant bit set
    return c;
}

std::array<unsigned char, 256> makeLogTable() {
    std::array<unsigned char, 256> logTable;
    logTable[0] = logTable[1] = 0;
    for (int i = 2; i < 256; ++i)
        logTable[i] = (unsigned char)(1 + logTable[i / 2]);
    return logTable;
}

// Add another card of rank r. Bit (r + j*13) encodes the existence of (j+1) cards of rank r. (0 <= j < 4)
// If we already have 4 cards of rank r in i, return i.
auto addCard(const auto i, const auto r) {
    auto bit = 1LL << r;
    auto k = i | bit;
    int j = 0;
    while (k == i && j < 3) {
        bit <<= 13;
        k |= bit;
        ++j;
    }
    return k;
}

auto getHandType(const auto k) {
    // auto kickerMask = 0x1FFFLL;
    // auto pairMask = 0x1FFFLL << 13;
    // auto threeKMask = 0x1FFFLL << 26;
    // auto fourKMask = 0x1FFFLL << 39;

    const auto kickerRanks = k & 0x1FFF;
    const auto pairRanks = (k >> 13) & 0x1FFF;
    const auto threeKRanks = (k >> 26) & 0x1FFF;
    const auto fourKRanks = (k >> 39) & 0x1FFF;

    //cout << "kickerRanks: " << bin(kickerRanks) << endl;
    //cout << "pairRanks: " << bin(pairRanks) << endl;
    //cout << "threeKRanks: " << bin(threeKRanks) << endl;
    //cout << "fourKRanks: " << bin(fourKRanks) << endl;

    const auto pairCount = countBits(pairRanks);
    //cout << "pairCount: " << pairCount << endl;
    if (fourKRanks != 0) {
        //cout << "got 4K" << endl;

        // The other three cards may be a 3K, a pair + kicker or three kickers.
        // We don't care, we can just select the highest rank from among the
        // kickerRanks.
        const auto kicker = getHighestRank(kickerRanks & ~fourKRanks);
        // there can only be a single 4K because we only have seven cards. So we
        // don't need to check for the highest ranked 4K.
        return (fourKRanks << 39) | kicker;
        //cout << "resulting state: " << bin(k) << endl;

        // To recognize a FH, we need 1 3K and at least TWO pairs because the 3K
        // is also counted as a pair.
    } else if (threeKRanks != 0 && pairCount > 1) {
        //cout << "got FH" << endl;

        const auto threeKRank = getHighestRank(threeKRanks);
        const auto pairRank = getHighestRank(pairRanks & ~threeKRanks);
        return (threeKRank << 26) | (pairRank << 13);
        //cout << "resulting state: " << bin(k) << endl;
    } else if (threeKRanks != 0) {
        //cout << "got 3K" << endl;

        // We know that there are neither 4K nor other pairs (other than the 3K
        // which can be counted as a pair). That's why we know that the
        // remaining FOUR cards all have DIFFERENT RANKS. Since we need two
        // kickers, we can just remove the lower two ranks.
        const auto kickers = removeNLowestRanks(kickerRanks & ~threeKRanks, 2);
        // A second 3K would be counted as another pair in which case we would
        // have entered the FH branch instead. That's why we don't need to check
        // for the highest ranked 3K.
        return (threeKRanks << 26) | kickers;
        //cout << "resulting state: " << bin(k) << endl;
    } else if (pairCount > 1) {
        //cout << "got 2P" << endl;

        // Since there are no 4K or 3K, we know that only pairs or kickers
        // remain. Also there are at least two pairs. What about the remaining 3
        // cards? There could be another pair or three kickers. All remaining
        // cards have ranks different from the two highest pairs of highest
        // rank. Otherwise they wouldn't be pairs.
        const auto two_highest_ranked_pairs = pairCount == 2 ? pairRanks : removeLowestRank(pairRanks);

        const auto kicker = getHighestRank(kickerRanks & ~two_highest_ranked_pairs);
        return (two_highest_ranked_pairs << 13) | kickerRanks;
        //cout << "resulting state: " << bin(k) << endl;
    } else if (pairRanks != 0) {
        //cout << "got 1P" << endl;

        // We have exactly one pair and 5 kickers, all having ranks different
        // from the pair's rank (or it wouldn't be a pair) and from each other.
        // That's why we don't need to check for the highest ranked pair. From
        // among the 5 kickers we have to select the three highest ranked.
        const auto kickers = removeNLowestRanks(kickerRanks & ~pairRanks, 2);
        return (pairRanks << 13) | kickers;
        //cout << "resulting state: " << bin(k) << endl;
    } else {
        //cout << "got HC" << endl;

        // There are no duplicate ranks whatsoever, so we have 7 kickers and
        // have to remove the two lowest ranked.
        return removeNLowestRanks(kickerRanks, 2);
        //cout << "resulting state: " << bin(k) << endl;
    }
}

// TODO: Also add a class for the rank count encoding uint64_t we use everywhere

// TODO: Add class for the hand type. Needs to support comparison.

enum class HandType {
    HIGH_CARD,
    PAIR,
    TWO_PAIR,
    THREE_OF_A_KIND,
    STRAIGHT,
    FLUSH,
    FULL_HOUSE,
    FOUR_OF_A_KIND,
    STRAIGHT_FLUSH,
    /* ROYAL_FLUSHes are just the highest ranked straight flushes. No need to complicate the code further */
};

using high_card_t = std::integral_constant<HandType, HandType::HIGH_CARD>;
using pair_t = std::integral_constant<HandType, HandType::PAIR>;
using two_pair_t = std::integral_constant<HandType, HandType::TWO_PAIR>;
using three_k_t = std::integral_constant<HandType, HandType::THREE_OF_A_KIND>;
using straight_t = std::integral_constant<HandType, HandType::STRAIGHT>;
using flush_t = std::integral_constant<HandType, HandType::FLUSH>;
using full_house_t = std::integral_constant<HandType, HandType::FULL_HOUSE>;
using four_k_t = std::integral_constant<HandType, HandType::FOUR_OF_A_KIND>;
using straight_flush_t = std::integral_constant<HandType, HandType::STRAIGHT_FLUSH>;

class HandRank {
    HandType t_;
    // always exactly 5 cards count. Most of the time we need to compare fewer cards because of some constraints:
    // PAIR has 1 pair (of the same rank) and 3 kickers, so only 4 ranks
    // TWO_PAIRS: 2 pairs, 1 kicker = 3 ranks
    // THREE_OF_A_KIND: 1 triplet, 2 kickers = 3 ranks
    // STRAIGHT: The highest rank is enough
    // FLUSH: 5 ranks
    // FULL_HOUSE: 1 triplet, 1 pair = 2 ranks
    // FOUR_OF_A_KIND: 1 four-tuple, 1 kicker = 2 ranks
    // STRAIGHT_FLUSH: The highest rank is enough
    //
    // THIS RELATION BETWEEN t_ AND ranks_ IS A CLASS INVARIANT!
    // So don't make the members public.
    using rank = uint8_t;
    std::array<rank, 5> ranks_;

    // TODO: fancy, but currently not worth it:
    //       Could just do HandRank(...) : t_(...), ranks_{...} everywhere below
    //       without it being less maintainable.
    template<typename... T, std::enable_if_t<(std::is_same_v<rank, T> && ...),int> = 0>
    HandRank(HandType t, T... ranks) : t_(t), ranks_{ranks...} {}
public:
    // NOTE: if we provide less than 5 ranks, the remaining entries in ranks_ get value-initialized (set to 0).
    // (See http://en.cppreference.com/w/cpp/language/aggregate_initialization)
    // 0s are okay because they don't influence the comparison operators. Any fixed value would do.
    HandRank(high_card_t, rank a, rank b, rank c, rank d, rank e) : HandRank(HandType::HIGH_CARD, a, b, c, d, e) {}
    HandRank(pair_t, rank pair_rank, rank a, rank b, rank c, rank d) : HandRank(HandType::PAIR, pair_rank, a, b, c, d) {}
    HandRank(two_pair_t, rank pair_rank1, rank pair_rank2, rank a) : HandRank(HandType::TWO_PAIR, pair_rank1, pair_rank2, a) {}
    HandRank(three_k_t, rank three_k_rank, rank a, rank b) : HandRank(HandType::THREE_OF_A_KIND, three_k_rank, a, b) {}
    HandRank(straight_t, rank a) : HandRank(HandType::STRAIGHT, a) {}
    HandRank(flush_t, rank a, rank b, rank c, rank d, rank e) : HandRank(HandType::FLUSH, a, b, c, d, e) {}
    HandRank(full_house_t, rank three_k_rank, rank pair_rank) : HandRank(HandType::FULL_HOUSE, three_k_rank, pair_rank) {}
    HandRank(four_k_t, rank four_k_rank, rank a) : HandRank(HandType::FOUR_OF_A_KIND, four_k_rank, a) {}
    HandRank(straight_flush_t, rank a) : HandRank(HandType::STRAIGHT_FLUSH, a) {}

    HandType type() const { return t_; }
    const std::array<rank, 5>& ranks() const { return ranks_; }
};

bool operator<(const HandRank& a, const HandRank& b) {
    if (a.type() < b.type())
        return true;
    if (a.type() == b.type())
        return a.ranks() < b.ranks();
    return false;
}

std::ostream& operator<<(std::ostream& s, const HandRank& a) {
    // TODO
    return s;
}

// TODO: implement HandRank with only a single uint64_t. Use the same
// interpretation I use in getHandType, but additionally use the upper 9 bits to
// encode the HandType. Use the HandRank implementation above as a reference
// implementation for unit tests.

uint64_t foo(const HandRank& a) {
}

HandRank bar(uint64_t a) {
}

//class States { // TODO: Use class in make_nfaBuilder
//    // We insert the initial state, which has to be 0 to match nfaBuilder's
//    // initialization of the "start" member
//    vector<long long> idToState{{0}};
//    unordered_map<long long,size_t> stateToId{{0,0}};
//
//public:
//    size_t getStateIndex(long long k) {
//        if (auto it = stateToId.find(k); it != stateToId.end())
//            return it->second;
//        auto stateID = idToState.size();
//        //cout << "it's new: id " << ix << endl;
//        //cout << "id in nfaBuilder: " << builder.ns.size() << endl;
//        stack.push_back(stateID); // TODO: add stack!?
//        builder.ns.emplace_back(); // TODO: add builder!?
//        idToState.push_back(k);
//        stateToId[k] = stateID;
//        return stateID;
//    }
//};

class Terminals {
    // We insert dummy entries because terminalID 0 is reserved for
    // non-terminals :)
    // We know that we will never call getHandTypeIndex with a handType of 0
    // because we only call it with integers with exactly 7 set bits, so
    // inserting 0 as the dummy handType will not cause it to be found and
    // returned in getHandTypeIndex.
    vector<long long> idToTerminal{{0}};
    unordered_map<long long,size_t> terminalToId{{0,0}};

public:
    size_t getHandTypeIndex(long long handType) {
        if (auto it = terminalToId.find(handType); it != terminalToId.end())
            return it->second;
        auto terminalID = idToTerminal.size();
        idToTerminal.push_back(handType);
        terminalToId[handType] = terminalID;
        return terminalID;
    }

    size_t size() const { return idToTerminal.size(); }
};

nfaBuilder<Symbol,State,TerminalId> make_nfaBuilder() {
    nfaBuilder<Symbol,State,TerminalId> builder(52); //eps = 52 ... greater than [0..51]
    for (Symbol i = 0U; i < 52U; ++i) {
        builder.idToSymbol.push_back(i);
        builder.symbolToId[i] = i + 1;
    }
    unordered_map<long long,size_t> stateToId;
    vector<long long> idToState;
    idToState.emplace_back();
    size_t ix = 0U;
    stateToId[idToState[ix]] = 0;

    Terminals terminals;

    vector<size_t> stack;
    stack.push_back(ix);
    ix++;
    auto offset = builder.ns.size();
    builder.ns.emplace_back();
    builder.ns[builder.start].ns[builder.eps].push_back(offset);
    //straight flush
    for (unsigned int c = 0; c < 4; ++c) {

    }
    //flush

    //straight

    offset = builder.ns.size();
    builder.ns.emplace_back();
    builder.ns[builder.start].ns[builder.eps].push_back(offset);

    while(!stack.empty()) {
        //cout << "stack: " << show(stack) << endl;
        auto ii = stack.back();
        stack.pop_back();
        //cout << "considering " << ii << endl;
        auto i = idToState[ii];
        //cout << "state: " << bin(i) << endl;

        auto rankCount = countBits(i);
        //cout << "rankCount: " << rankCount << endl;
        if (rankCount < 7) {
            // less than 7 cards. Add another one.
            for (int r = 0; r < 13; ++r) {
                //cout << "rank: " << r << endl;
                const auto k = addCard(i, r);
                if (k != i) {
                    //cout << "resulting state: " << bin(k) << endl;
                    auto it = stateToId.find(k);
                    int kk;
                    if (it == stateToId.end()) {
                        //cout << "it's new: id " << ix << endl;
                        //cout << "id in nfaBuilder: " << builder.ns.size() << endl;
                        builder.ns.emplace_back();
                        stack.push_back(kk);
                        idToState.push_back(k);
                        stateToId[k] = ix;

                        // this operation is not done below because below we
                        // deal with terminal states that don't have to be
                        // revisited

                        kk = ix++;
                    } else {
                        kk = it->second;
                        //cout << "already known. id: " << kk << endl;
                    }
                    builder.ns[ii + offset].ns[r].push_back(kk + offset);
                    builder.ns[ii + offset].ns[13 + r].push_back(kk + offset);
                    builder.ns[ii + offset].ns[26 + r].push_back(kk + offset);
                    builder.ns[ii + offset].ns[39 + r].push_back(kk + offset);
                } else {
                    //cout << "already got four of those" << endl;
                }
            }
        } else {
            // 7 cards.
            if (builder.ns[ii + offset].kind == 0) {
                const auto handType = getHandType(i);
                builder.ns[ii + offset].kind = terminals.getHandTypeIndex(handType);
            }
        }
    }
    cout << "states: " << ix << endl;
    cout << "hand types: " << terminals.size() << endl;
    return builder;
}

NFA<Symbol,State,TerminalId> make_NFA() {
    auto builder = make_nfaBuilder();
    return {builder};
}

DFA<Symbol,State,TerminalId> make_DFA() {
    auto nfa = make_NFA();
    cout << "nfa done" << endl;
    return {nfa};
}

// TODO: Use sort5 before giving the cards to the dfa.
//       Check assembly to make sure that sort5 works on registers only. Then it should be extremely efficient and using it should be better than having a lot more states in the dfa to deal with unsorted cards.
namespace merge {
inline constexpr void swap(int& a, int& b) {
    int tmp = a;
    a = b;
    b = tmp;
}
inline constexpr void merge11(int& a0, int& a1) {
    if (a0 >= a1)
        swap(a0, a1);
}

inline constexpr void merge12(int& x0, int& x1, int& x2) {
    if (x0 < x1) {
        return;
    } else {
        swap(x0, x1);
        merge11(x1, x2);
    }
}

inline constexpr void rotate_right3(int& x0, int& x1, int& x2) {
    swap(x0, x1); // b, a, c
    swap(x0, x2); // c, a, b
}

inline constexpr void merge21(int& x0, int& x1, int& x2) {
    if (x0 < x2) {
        merge11(x1, x2);
    } else {
        rotate_right3(x0, x1, x2);
        merge11(x1, x2);
    }
}

inline constexpr void merge13(int& x0, int& x1, int& x2, int& x3) {
    if (x0 < x1) {
        return;
    } else {
        swap(x0, x1);
        merge12(x1, x2, x3);
    }
}

inline constexpr void merge22(int& x0, int& x1, int& x2, int& x3) {
    if (x0 < x2) {
        merge12(x1, x2, x3);
    } else {
        rotate_right3(x0, x1, x2);
        merge21(x1, x2, x3);
    }
}

inline constexpr void merge23(int& x0, int& x1, int& x2, int& x3, int& x4) {
    if (x0 < x2) {
        merge13(x1, x2, x3, x4);
    } else {
        rotate_right3(x0, x1, x2);
        merge22(x1, x2, x3, x4);
    }
}

constexpr std::array<int,5> sort5(std::array<int,5> x) {
    auto a = x[0];
    auto b = x[1];
    auto c = x[2];
    auto d = x[3];
    auto e = x[4];
    merge11(a, b);
    merge11(d, e);
    merge12(c, d, e);
    merge23(a, b, c, d, e);
    return {a, b, c, d, e};
}
}

int main() {
    {
        auto dfa = make_DFA();
        dfa.minimize();
        //  cout << "start: " << dfa.start << endl;
        //  cout << "final: " << show(dfa.final) << endl;
        //  for (unsigned int q = 0; q < dfa.stateCount; q++) {
        //    for (unsigned int a = 0; a < dfa.symbolCount; a++)
        //      cout << "(" << q << "," << a << ") -> " << dfa.T[q][a] << endl;
        //  }
        cout << "State count: " << dfa.stateCount << endl;
    }
    cout << "The End" << endl;
    return 0;
}
