#pragma once

#include <functional>

namespace Functional {
struct LambdaExpr;

using Strategy_t = const LambdaExpr*(const LambdaExpr&);

using Strategy = std::function<Strategy_t>;

Strategy_t lazy;

template <typename Eval>
const LambdaExpr* eager(const LambdaExpr& e, Eval eval) {
    return eval(e, eager);
}

template <typename Eval>
Strategy make_eager(Eval eval) {
    return [eval](const LambdaExpr& e) { return eval(e, make_eager(eval)); };
}
} // namespace Functional
