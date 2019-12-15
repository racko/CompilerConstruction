#include "Functional.h"
#include "FunctionalEvaluator1.h"
#include "FunctionalEvaluator2.h"
#include "FunctionalLexer.h"
#include "FunctionalParser.h"
#include "FunctionalStrategy.h"

#include <fstream>

namespace Functional {
namespace evaluator = evaluator2;
// actually wanted to name it eval ... but I already have to many of those ...
// not sure how to name them most consistently ...
const LambdaExpr* reduce(const LambdaExpr& p) {
    /// @TODO: this doesn't work if p contains references to other "named" lambda expressions
    return evaluator::eval(p, make_eager(evaluator::eval));
}

const LambdaExpr* run(const char* text) {
    myLexer lex;
    Functional::Parser parser;
    lex.setText(text);
    auto result = parser.parse(lex);
    // return reduce(result);
    return evaluator::evalProgram(toProgram(result), make_eager(evaluator::eval));
}
} // namespace Functional
