#pragma once

#include "FunctionalStrategy.h"

struct LambdaExpr;
struct Program;

namespace evaluator2 {
const LambdaExpr* eval(const LambdaExpr& e, Strategy strategy);

const LambdaExpr* evalProgram(const Program& p, Strategy strategy);
}
