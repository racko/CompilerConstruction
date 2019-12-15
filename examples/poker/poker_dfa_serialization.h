#pragma once

#include "Regex/DFA.h"
#include "types.h"
#include "terminals.h"
#include <utility>
#include <iosfwd>

std::pair<DFA<Symbol, StateId, TerminalId>, Terminals> loadRanker(std::ostream& logger);
