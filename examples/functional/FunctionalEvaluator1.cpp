#include "FunctionalEvaluator1.h"

#include "FunctionalAttribute.h" // for operator<<, Abs, LambdaExpr, NumConst
#include "FunctionalValue1.h"    // for Value, toValue, Environment, Function
#include <iostream>              // for operator<<, endl, ostream, basic_os...
#include <string>                // for string, allocator, operator<<, char...
#include <unordered_map>         // for unordered_map
#include <utility>               // for move

namespace Functional {
namespace evaluator1 {
namespace {
struct LambdaEvaluator : public LambdaConstVisitor {
    Environment env;
    const Const* result = nullptr;

    // LambdaEvaluator() { std::cout << "new LambdaEvaluator" << std::endl; }
    //~LambdaEvaluator() { std::cout << "delete LambdaEvaluator" << std::endl; }

    const Const* eval(const App& app) {
        const Function* func = nullptr;
        Value tmpValue(app.f);
        const Const* pValue = &tmpValue;
        do {
            auto f = dynamic_cast<const Value*>(pValue)->v;
            f->accept(*this);
            func = dynamic_cast<const Function*>(result);
            pValue = result;
        } while (!func);
        // auto f = toFunction(result);
        app.x->accept(*this);
        auto x = result;
        return func->f(x);
    }

    const Const* eval(const Abs& abs) {
        std::cout << "LambdaEvaluator::eval(" << abs << ")" << std::endl;
        const auto& env = this->env;
        auto r = new Function([abs, env](const Const* d) {
            // std::cout << "Applying " << abs << " to " << d << " in " << env << std::endl;
            LambdaEvaluator inner;
            inner.env = std::move(env);
            const auto& param = abs.x;
            // std::cout << "Abstraction has only one argument" << std::endl;
            inner.env.set(param, d);
            abs.body->accept(inner);
            std::cout << "result: " << inner.result << std::endl;

            return inner.result;
        });
        std::cout << "r: " << r << std::endl;
        return r;
    }

    const Const* eval(const NumConst& num) { return new Value(&num); }

    const Const* eval(const Variable& var) { return env.getOrDefine(var.name); }

    void visit(const App& app) { result = eval(app); }
    void visit(const Abs& abs) {
        std::cout << "visit(" << abs << ")" << std::endl;
        result = eval(abs);
        std::cout << "result: " << result << std::endl;
    }
    void visit(const NumConst& num) { result = eval(num); }
    void visit(const Variable& var) { result = eval(var); }
};
} // namespace

const LambdaExpr* eval(const LambdaExpr& e, Strategy) {
    LambdaEvaluator evaluator;
    e.accept(evaluator);
    return toValue(evaluator.result);
}

const LambdaExpr* evalProgram(const Program& p, Strategy strategy) {
    // std::cout << "evalProgram" << std::endl;
    std::unordered_map<std::string, Const*> env;
    // std::cout << "building environment" << std::endl;
    for (auto sc : p.scs) {
        // std::cout << sc << std::endl;
        const auto& n = sc.f;
        const auto& params = sc.params;
        auto body = sc.body;
        for (auto it = params.names.rbegin(); it != params.names.rend(); ++it)
            body = new Abs(*it, body);
        // body = replace_vars(body, env);
        // for (auto& p : env)
        //    p.second = substitute(*p.second, n, *body);
        env.insert({n, new Value(body)});
    }

    std::cout << "env:" << std::endl;
    for (const auto& p : env)
        std::cout << p.first << ": " << *p.second << std::endl;

    std::cout << "Evaluation:" << std::endl;
    auto main = env.at("main");
    // auto r = canonym(*eval(*main, "x", strategy), "x");
    auto r = eval(*toValue(main), strategy);

    std::cout << *r << std::endl;
    return r;
}
} // namespace evaluator1
} // namespace Functional
