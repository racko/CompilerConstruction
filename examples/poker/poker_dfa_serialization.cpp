#include "poker_dfa_serialization.h"

#include "Regex/DFA.h"
#include "poker_dfa.h"
#include "terminals.h"
#include "types.h"
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/filesystem.hpp>
#include <boost/serialization/serialization.hpp>
#include <fstream>
#include <ostream>
#include <stdexcept>
#include <utility>

std::pair<DFA<Symbol, StateId, TerminalId>, Terminals> loadRanker(std::ostream& logger) {
    boost::filesystem::path ranker_path{"ranker.dat"};
    if (!boost::filesystem::exists(ranker_path)) {
        const auto ranker = make_minimal_DFA(logger);
        {
            std::ofstream ranker_file{ranker_path};
            ranker_file.exceptions(std::ofstream::failbit | std::ofstream::badbit);
            boost::archive::binary_oarchive oarch(ranker_file);
            oarch << ranker;
        }
        {
            std::pair<DFA<Symbol, StateId, TerminalId>, Terminals> deserialized_ranker;
            std::ifstream ranker_file{ranker_path};
            ranker_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            boost::archive::binary_iarchive iarch(ranker_file);
            iarch >> deserialized_ranker;
            if (ranker != deserialized_ranker) {
                throw std::runtime_error("de-/serialization inconsistent");
            }
        }
        return ranker;
    } else {
        std::pair<DFA<Symbol, StateId, TerminalId>, Terminals> deserialized_ranker;
        std::ifstream ranker_file{ranker_path};
        ranker_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        boost::archive::binary_iarchive iarch(ranker_file);
        iarch >> deserialized_ranker;
        // // logger << deserialized_ranker.first << '\n';
        return deserialized_ranker;
    }
}
