#pragma once

#include "Regex/DFA_fwd.h"
#include "terminals.h"
#include "types.h"
#include <iosfwd>
#include <utility>

std::pair<DFA<Symbol, StateId, TerminalId>, Terminals> make_minimal_DFA(std::ostream& logger);
