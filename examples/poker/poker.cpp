#include "poker.h"

#include "DFA.h"
#include "NFA.h"
#include "Regex.h"
#include "nfaBuilder.h"
#include <iostream>
#include <iterator>
#include <unordered_map>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/filesystem.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/vector.hpp>

namespace {
// using State = uint16_t;
// using Symbol = uint8_t;
// using TerminalId = uint16_t;
using Symbol = uint8_t;
using State = uint32_t;
using TerminalId = uint16_t;

template <typename T>
T clearLowestBit(const T v) {
    return v & v - 1;
}

template <typename T>
T clearNLowestBits(T v, const int n) {
    for (int j = 0; j < n; j++)
        v = clearLowestBit(v);
    return v;
}

template <typename T>
T getHighestBit(T v) {
    T result;
    while ((v = clearLowestBit(v)) != 0)
        result = v;
    return result;
}

template <typename T>
T getLowestBit(T v) {
    return -v & v;
}

template <typename R = int, typename T>
R countTrailingZeros(T v) {
    R c; // c will be the number of zero bits on the right,
         // so if v is 1101000 (base 2), then c will be 3
    // NOTE: if 0 == v, then c = 31.
    if (v & 0x1) {
        // special case for odd v (assumed to happen half of the time)
        c = 0;
    } else {
        c = 1;
        if ((v & 0xffff) == 0) {
            v >>= 16;
            c += 16;
        }
        if ((v & 0xff) == 0) {
            v >>= 8;
            c += 8;
        }
        if ((v & 0xf) == 0) {
            v >>= 4;
            c += 4;
        }
        if ((v & 0x3) == 0) {
            v >>= 2;
            c += 2;
        }
        c -= v & 0x1;
    }
    return c;
}

template <typename T>
T removeLowestRank(const T v) {
    return clearLowestBit(v);
}

template <typename T>
T removeNLowestRanks(const T v, int n) {
    return clearNLowestBits(v, n);
}

template <typename T>
T getHighestRank(const T v) {
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
template <typename T>
T countBits(T v) {
    T c = 0; // c accumulates the total bits set in v
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
auto addCard(const std::size_t i, const int r) {
    auto bit = 1ULL << r;
    auto k = i | bit;
    int j = 0;
    while (k == i && j < 3) {
        bit <<= 13;
        k |= bit;
        ++j;
    }
    return k;
}

template <typename T>
T getHandType(const T k) {
    // auto kickerMask = 0x1FFFLL;
    // auto pairMask = 0x1FFFLL << 13;
    // auto threeKMask = 0x1FFFLL << 26;
    // auto fourKMask = 0x1FFFLL << 39;

    const auto kickerRanks = k & 0x1FFF;
    const auto pairRanks = (k >> 13) & 0x1FFF;
    const auto threeKRanks = (k >> 26) & 0x1FFF;
    const auto fourKRanks = (k >> 39) & 0x1FFF;

    std::cout << "kickerRanks: " << bin(kickerRanks) << '\n';
    std::cout << "pairRanks: " << bin(pairRanks) << '\n';
    std::cout << "threeKRanks: " << bin(threeKRanks) << '\n';
    std::cout << "fourKRanks: " << bin(fourKRanks) << '\n';

    if (fourKRanks != 0) {
        std::cout << "got 4K" << '\n';

        // The other three cards may be a 3K, a pair + kicker or three kickers.
        // We don't care, we can just select the highest rank from among the
        // kickerRanks.
        const auto kicker = getHighestRank(kickerRanks & ~fourKRanks);
        // there can only be a single 4K because we only have seven cards. So we
        // don't need to check for the highest ranked 4K.
        return (fourKRanks << 39) | kicker;
        // std::cout << "resulting state: " << bin(k) << '\n';

        // To recognize a FH, we need 1 3K and at least TWO pairs because the 3K
        // is also counted as a pair.
    }

    const auto pairCount = countBits(pairRanks);
    // std::cout << "pairCount: " << pairCount << '\n';

    if (threeKRanks != 0 && pairCount > 1) {
        // std::cout << "got FH" << '\n';

        const auto threeKRank = getHighestRank(threeKRanks);
        const auto pairRank = getHighestRank(pairRanks & ~threeKRank);
        std::cout << "resulting state: " << bin((threeKRank << 26) | (pairRank << 13)) << '\n';
        return (threeKRank << 26) | (pairRank << 13);
    } else if (threeKRanks != 0) {
        // std::cout << "got 3K" << '\n';

        // We know that there are neither 4K nor other pairs (other than the 3K
        // which can be counted as a pair). That's why we know that the
        // remaining FOUR cards all have DIFFERENT RANKS. Since we need two
        // kickers, we can just remove the lower two ranks.
        const auto kickers = removeNLowestRanks(kickerRanks & ~threeKRanks, 2);
        // A second 3K would be counted as another pair in which case we would
        // have entered the FH branch instead. That's why we don't need to check
        // for the highest ranked 3K.
        return (threeKRanks << 26) | kickers;
        // std::cout << "resulting state: " << bin(k) << '\n';
    } else if (pairCount > 1) {
        // std::cout << "got 2P" << '\n';

        // Since there are no 4K or 3K, we know that only pairs or kickers
        // remain. Also there are at least two pairs. What about the remaining 3
        // cards? There could be another pair or three kickers. All remaining
        // cards have ranks different from the two highest pairs of highest
        // rank. Otherwise they wouldn't be pairs.
        const auto two_highest_ranked_pairs = pairCount == 2 ? pairRanks : removeLowestRank(pairRanks);

        const auto kicker = getHighestRank(kickerRanks & ~two_highest_ranked_pairs);
        return (two_highest_ranked_pairs << 13) | kicker;
        // std::cout << "resulting state: " << bin(k) << '\n';
    } else if (pairRanks != 0) {
        // std::cout << "got 1P" << '\n';

        // We have exactly one pair and 5 kickers, all having ranks different
        // from the pair's rank (or it wouldn't be a pair) and from each other.
        // That's why we don't need to check for the highest ranked pair. From
        // among the 5 kickers we have to select the three highest ranked.
        const auto kickers = removeNLowestRanks(kickerRanks & ~pairRanks, 2);
        return (pairRanks << 13) | kickers;
        // std::cout << "resulting state: " << bin(k) << '\n';
    } else {
        // std::cout << "got HC" << '\n';

        // There are no duplicate ranks whatsoever, so we have 7 kickers and
        // have to remove the two lowest ranked.
        return removeNLowestRanks(kickerRanks, 2);
        // std::cout << "resulting state: " << bin(k) << '\n';
    }
}

// TODO: Also add a class for the rank count encoding uint64_t we use everywhere

// TODO: implement HandRank with only a single uint64_t. Use the same
// interpretation I use in getHandType, but additionally use the upper 9 bits to
// encode the HandType. Use the HandRank implementation above as a reference
// implementation for unit tests.

uint64_t foo(const HandRank& a) {}

HandRank bar(uint64_t k) {
    const auto kickerRanks = k & 0x1FFF;
    const auto pairRanks = (k >> 13) & 0x1FFF;
    const auto threeKRanks = (k >> 26) & 0x1FFF;
    const auto fourKRanks = (k >> 39) & 0x1FFF;

    std::cout << "kickerRanks: " << bin(kickerRanks) << '\n';
    std::cout << "pairRanks: " << bin(pairRanks) << '\n';
    std::cout << "threeKRanks: " << bin(threeKRanks) << '\n';
    std::cout << "fourKRanks: " << bin(fourKRanks) << '\n';

    if (fourKRanks != 0) {
        return {
            four_k_t{}, countTrailingZeros<std::uint8_t>(fourKRanks), countTrailingZeros<std::uint8_t>(kickerRanks)};
    }

    if (threeKRanks != 0 && pairRanks != 0) {
        return {
            full_house_t{}, countTrailingZeros<std::uint8_t>(threeKRanks), countTrailingZeros<std::uint8_t>(pairRanks)};
    }

    const auto pairCount = countBits(pairRanks);

    if (threeKRanks != 0) {
        const auto lower_kicker = countTrailingZeros<std::uint8_t>(kickerRanks);
        assert(lower_kicker < 14);
        const auto higher_kicker = countTrailingZeros<std::uint8_t>(clearLowestBit(kickerRanks));
        assert(higher_kicker < 14);
        return {three_k_t{}, countTrailingZeros<std::uint8_t>(threeKRanks), higher_kicker, lower_kicker};
    } else if (pairCount > 1) {
        const auto lower_pair = countTrailingZeros<std::uint8_t>(pairRanks);
        assert(lower_pair < 14);
        const auto higher_pair = countTrailingZeros<std::uint8_t>(clearLowestBit(pairRanks));
        assert(higher_pair < 14);
        return {two_pair_t{}, higher_pair, lower_pair, countTrailingZeros<std::uint8_t>(kickerRanks)};
    } else if (pairRanks != 0) {
        const auto kicker3 = countTrailingZeros<std::uint8_t>(kickerRanks);
        assert(kicker3 < 14);
        const auto kicker2 = countTrailingZeros<std::uint8_t>(clearLowestBit(kickerRanks));
        assert(kicker2 < 14);
        const auto kicker1 = countTrailingZeros<std::uint8_t>(clearLowestBit(clearLowestBit(kickerRanks)));
        assert(kicker1 < 14);
        return {pair_t{}, countTrailingZeros<std::uint8_t>(pairRanks), kicker1, kicker2, kicker3};
    } else {
        const std::uint8_t kicker5 = countTrailingZeros<std::uint8_t>(kickerRanks);
        assert(kicker5 < 14);
        const std::uint8_t kicker4 = (kicker5 + 1) + countTrailingZeros<std::uint8_t>(kickerRanks >> (kicker5 + 1));
        assert(kicker4 < 14);
        const std::uint8_t kicker3 = (kicker4 + 1) + countTrailingZeros<std::uint8_t>(kickerRanks >> (kicker4 + 1));
        assert(kicker3 < 14);
        const std::uint8_t kicker2 = (kicker3 + 1) + countTrailingZeros<std::uint8_t>(kickerRanks >> (kicker3 + 1));
        assert(kicker2 < 14);
        const std::uint8_t kicker1 = (kicker2 + 1) + countTrailingZeros<std::uint8_t>(kickerRanks >> (kicker2 + 1));
        assert(kicker1 < 14);
        return {high_card_t{}, kicker1, kicker2, kicker3, kicker4, kicker5};
    }
}

// class States { // TODO: Use class in make_nfaBuilder
//    // We insert the initial state, which has to be 0 to match nfaBuilder's
//    // initialization of the "start" member
//    std::vector<long long> idToState{{0}};
//    std::unordered_map<long long,size_t> stateToId{{0,0}};
//
// public:
//    size_t getStateIndex(long long k) {
//        if (auto it = stateToId.find(k); it != stateToId.end())
//            return it->second;
//        auto stateID = idToState.size();
//        //std::cout << "it's new: id " << ix << '\n';
//        //std::cout << "id in nfaBuilder: " << builder.ns.size() << '\n';
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
    std::vector<long long> idToTerminal{{0}};
    std::unordered_map<long long, size_t> terminalToId{{0, 0}};

  public:
    size_t getHandTypeIndex(long long handType) {
        if (auto it = terminalToId.find(handType); it != terminalToId.end())
            return it->second;
        auto terminalID = idToTerminal.size();
        idToTerminal.push_back(handType);
        terminalToId[handType] = terminalID;
        return terminalID;
    }

    long long getHandType(const std::size_t id) const { return idToTerminal.at(id); }

    size_t size() const { return idToTerminal.size(); }

    friend bool operator==(const Terminals& lhs, const Terminals& rhs) {
        return lhs.idToTerminal == rhs.idToTerminal && lhs.terminalToId == rhs.terminalToId;
    }

    template <typename Archive>
    void serialize(Archive& ar, unsigned int) {
        ar& idToTerminal;
        ar& terminalToId;
    }
};

std::pair<nfaBuilder<Symbol, State, TerminalId>, Terminals> make_nfaBuilder() {
    Terminals terminals;
    std::vector<size_t> stack;
    std::unordered_map<std::size_t, size_t> stateToId;
    std::vector<std::size_t> idToState;
    nfaBuilder<Symbol, State, TerminalId> builder;

    for (Symbol i = 1U; i < 53U; ++i) {
        const auto id = builder.idToSymbol.size();
        builder.idToSymbol.push_back(i);
        builder.symbolToId[i] = id;
    }

    stack.push_back(0);
    idToState.emplace_back();
    stateToId[idToState[0]] = 0;

    auto offset = builder.ns.size();

    // builder.ns.emplace_back();
    // builder.ns[builder.start].ns[builder.symbolToId[builder.eps]].push_back(offset);

    // straight flush
    for (unsigned int c = 0; c < 4; ++c) {
    }
    // flush

    // straight

    offset = builder.ns.size();
    builder.ns.emplace_back();
    builder.ns[builder.start].ns[builder.symbolToId[builder.eps]].push_back(offset);

    while (!stack.empty()) {
        std::cout << "stack: " << show(stack) << '\n';
        const auto ii = stack.back();
        stack.pop_back();
        std::cout << "considering " << ii << '\n';
        const auto i = idToState[ii];
        std::cout << "state: " << bin(i) << '\n';

        const auto rankCount = countBits(i);
        std::cout << "rankCount: " << rankCount << '\n';
        if (rankCount == 7) {
            if (builder.ns[ii + offset].kind == 0) {
                const auto handType = getHandType(i);
                std::cout << "hand type is " << bar(handType) << '\n';
                builder.ns[ii + offset].kind = terminals.getHandTypeIndex(handType);
                std::cout << "setting hand type index to " << builder.ns[ii + offset].kind << '\n';
            }
            continue;
        }
        // less than 7 cards. Add another one.
        for (int r = 0; r < 13; ++r) {
            std::cout << "rank: " << r << '\n';
            const auto k = addCard(i, r);
            if (k == i) {
                std::cout << "already got four of those\n";
                continue;
            }
            std::cout << "resulting state: " << bin(k) << '\n';
            const auto GetStateId = [&stateToId, &builder, &idToState, &stack] (const auto k) {
                if (const auto it = stateToId.find(k); it != stateToId.end()) {
                    std::cout << "already known. id: " << it->second << '\n';
                    return it->second;
                }
                const auto stateId = idToState.size();
                std::cout << "it's new: id " << stateId << '\n';
                std::cout << "id in nfaBuilder: " << builder.ns.size() << '\n';
                builder.ns.emplace_back();
                stack.push_back(stateId);
                idToState.push_back(k);
                stateToId[k] = stateId;
                return stateId;
            };
            int kk = GetStateId(k);

            builder.ns[ii + offset].ns[builder.symbolToId[1 + r]].push_back(kk + offset);
            builder.ns[ii + offset].ns[builder.symbolToId[1 + 13 + r]].push_back(kk + offset);
            builder.ns[ii + offset].ns[builder.symbolToId[1 + 26 + r]].push_back(kk + offset);
            builder.ns[ii + offset].ns[builder.symbolToId[1 + 39 + r]].push_back(kk + offset);
        }
    }
    std::cout << "states: " << idToState.size() << '\n';
    std::cout << "hand types: " << terminals.size() << '\n';
    //std::exit(0);
    return {builder, terminals};
}

std::pair<NFA<Symbol, State, TerminalId>, Terminals> make_NFA() {
    const auto builder = make_nfaBuilder();
    std::cout << "nfa builder done\n";
    return {builder};
}

std::pair<DFA<Symbol, State, TerminalId>, Terminals> make_DFA() {
    const auto nfa = make_NFA();
    std::cout << "nfa done\n";
    // std::cout << nfa.first << '\n';
    return {nfa};
}

std::pair<DFA<Symbol, State, TerminalId>, Terminals> make_minimal_DFA() {
    auto dfa = make_DFA();
    std::cout << "dfa done\n";
    // std::cout << dfa.first << '\n';
    dfa.first.minimize();
    std::cout << "minimization done\n";
    // std::cout << dfa.first << '\n';
    return dfa;
}

std::pair<DFA<Symbol, State, TerminalId>, Terminals> loadRanker() {
    boost::filesystem::path ranker_path{"ranker.dat"};
    if (!boost::filesystem::exists(ranker_path)) {
        const auto ranker = make_minimal_DFA();
        {
            std::ofstream ranker_file{ranker_path};
            boost::archive::binary_oarchive oarch(ranker_file);
            oarch << ranker;
        }
        {
            std::pair<DFA<Symbol, State, TerminalId>, Terminals> deserialized_ranker;
            std::ifstream ranker_file{ranker_path};
            boost::archive::binary_iarchive iarch(ranker_file);
            iarch >> deserialized_ranker;
            if (ranker != deserialized_ranker) {
                throw std::runtime_error("de-/serialization inconsistent");
            }
        }
        return ranker;
    } else {
        std::pair<DFA<Symbol, State, TerminalId>, Terminals> deserialized_ranker;
        std::ifstream ranker_file{ranker_path};
        boost::archive::binary_iarchive iarch(ranker_file);
        iarch >> deserialized_ranker;
        // std::cout << deserialized_ranker.first << '\n';
        return deserialized_ranker;
    }
}

void printValidCards(std::ostream& stream, const DFA<Symbol, State, TerminalId>& dfa, const State s) {
    const auto& transitions = dfa.T.data() + dfa.symbolCount * s;
    for (auto i = 0; i < dfa.symbolCount; ++i) {
        if (transitions[i] != dfa.deadState) {
            stream << int(dfa.idToSymbol[i]) << ' ';
        }
    }
    stream << '\n';
}
} // namespace

HandRank rank(const int* const cards) {
    static auto x = loadRanker();
    const auto& [dfa, terminals] = x;

    auto s = dfa.start;
    const auto fptr = dfa.final.data();
    std::cout << "starting in state " << s << ", type " << fptr[s] << '\n';
    const auto Tptr = dfa.T.data();
    const auto scount = dfa.symbolCount;
    const auto dstate = dfa.deadState;
    printValidCards(std::cout, dfa, s);
    assert(s != dstate);
    for (auto i = 0; i < 6; ++i) {
        s = Tptr[s * scount + dfa.symbolToId[cards[i]]];
        printValidCards(std::cout, dfa, s);
        assert(fptr[s] == 0);
        if (s == dstate) { // Not an assert because this is input validation.
            throw std::runtime_error("not a valid hand");
        }
    }
    s = Tptr[s * scount + dfa.symbolToId[cards[6]]];
    assert(fptr[s] > 0);
    return bar(terminals.getHandType(fptr[s]));
}

std::ostream& operator<<(std::ostream& s, const HandRank& a) {
    s << int(a.type()) << ", ";
    std::copy(a.ranks().begin(), a.ranks().end(), std::ostream_iterator<int>(s, " "));
    return s;
}

