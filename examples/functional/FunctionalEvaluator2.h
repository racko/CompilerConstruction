#pragma once

#include "FunctionalStrategy.h"

namespace Functional {
struct LambdaExpr;
struct Program;

namespace evaluator2 {
const LambdaExpr* eval(const LambdaExpr& e, Strategy strategy);

const LambdaExpr* evalProgram(const Program& p, Strategy strategy);
} // namespace evaluator2
} // namespace Functional
