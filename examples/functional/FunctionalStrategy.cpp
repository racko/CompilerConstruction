#include "FunctionalStrategy.h"
#include "FunctionalAttribute.h"

const LambdaExpr* lazy(const LambdaExpr& e) {
    return copy(e);
}

//const LambdaExpr* eager(const LambdaExpr& e) {
//    // @TODO: How to select the right eval? Include the desired here again? That would be bad since we would need to change the header here and in Functional.cpp when we want to change it!
//    return eval(e, eager);
//}
