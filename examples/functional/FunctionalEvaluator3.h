#pragma once

#include "FunctionalStrategy.h"

struct LambdaExpr;
struct Program;

namespace evaluator3 {
const LambdaExpr* eval(const LambdaExpr& e, Strategy);
const LambdaExpr* evalProgram(const Program& p, Strategy strategy);
} // namespace evaluator3
