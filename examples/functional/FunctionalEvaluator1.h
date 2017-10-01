#pragma once

#include "FunctionalStrategy.h"

struct LambdaExpr;
struct Program;

namespace evaluator1 {
const LambdaExpr* eval(const LambdaExpr& e, Strategy);
const LambdaExpr* evalProgram(const Program& p, Strategy strategy);
}
