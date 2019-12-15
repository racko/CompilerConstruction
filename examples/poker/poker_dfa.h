#pragma once

#include "Regex/DFA_fwd.h"
#include "types.h"
#include "terminals.h"
#include <utility>
#include <iosfwd>

std::pair<DFA<Symbol, StateId, TerminalId>, Terminals> make_minimal_DFA(std::ostream& logger);
