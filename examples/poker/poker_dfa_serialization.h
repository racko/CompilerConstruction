#pragma once

#include "Regex/DFA.h"
#include "terminals.h"
#include "types.h"
#include <iosfwd>
#include <utility>

std::pair<DFA<Symbol, StateId, TerminalId>, Terminals> loadRanker(std::ostream& logger);
