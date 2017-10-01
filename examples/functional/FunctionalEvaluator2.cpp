#include "FunctionalEvaluator2.h"

#include "FunctionalAttribute.h"

#include <iostream>
#include <sstream>
#include <unordered_set>

namespace evaluator2 {
namespace {
struct IsWHNF : public LambdaConstVisitor {
    bool result;
    void visit(const App& app) {
        result = false;
    }
    void visit(const Abs& abs) {
        result = true;
    }
    void visit(const NumConst& num) {
        result = true;
    }
    void visit(const ::Var& var) {
        result = false;
    }
};

bool isWHNF(const LambdaExpr* e) {
    std::cout << "isWHNF(" << *e << "): " << std::flush;
    IsWHNF whnf;
    e->accept(whnf);
    std::cout << whnf.result << std::endl;
    return whnf.result;
}

const LambdaExpr* canonym(const LambdaExpr& e, const std::unordered_map<std::string, std::string>& m, const std::string& prefix);

struct Canonym : public LambdaConstVisitor {
    LambdaExpr* result = nullptr;
    const std::unordered_map<std::string, std::string>& m;
    const std::string& prefix;

    Canonym(const std::unordered_map<std::string, std::string>& m_, const std::string& prefix_) : m(m_), prefix(prefix_) {}

    void visit(const App& app) {
        result = new App(canonym(*app.f, m, prefix + "F"), canonym(*app.x, m, prefix + "S"));
    }

    void visit(const Abs& abs) {
        auto m2 = m;
        m2.insert({abs.x, prefix});
        result = new Abs(prefix, canonym(*abs.body, std::move(m2), prefix + "N"));
    }

    void visit(const NumConst& num) {
        result = new NumConst(num);
    }

    void visit(const ::Var& var) {
        auto it = m.find(var.name);
        if (it == m.end())
            result = new ::Var(var);
        else
            result = new ::Var(it->second);
    }
};

const LambdaExpr* canonym(const LambdaExpr& e, const std::unordered_map<std::string, std::string>& m, const std::string& prefix) {
    Canonym c(m, prefix);
    e.accept(c);
    return c.result;
}

const LambdaExpr* canonym(const LambdaExpr& e, const std::string& prefix) {
    return canonym(e, std::unordered_map<std::string, std::string>{}, prefix);
}

std::unordered_set<std::string> freeVars(const LambdaExpr& e);

struct FreeVars : public LambdaConstVisitor {
    std::unordered_set<std::string> result;

    void visit(const App& app) {
        result = freeVars(*app.f);
        auto result2 = freeVars(*app.x);
        result.insert(result2.begin(), result2.end());
    }

    void visit(const Abs& abs) {
        result = freeVars(*abs.body);
        result.erase(abs.x);
    }

    void visit(const NumConst&) {}

    void visit(const ::Var& var) {
        result.insert(var.name);
    }
};

std::unordered_set<std::string> freeVars(const LambdaExpr& e) {
    FreeVars fv;
    e.accept(fv);
    return fv.result;
}

const LambdaExpr* substitute(const LambdaExpr& e, const std::string& v, const LambdaExpr& n);

struct Substitution : public LambdaConstVisitor {
    LambdaExpr* result = nullptr;
    const std::string& v;
    const LambdaExpr& n;

    Substitution(const std::string& v_, const LambdaExpr& n_) : v(v_), n(n_) {}

    void visit(const App& app) {
        result = new App(substitute(*app.f, v, n), substitute(*app.x, v, n));
    }

    void visit(const Abs& abs) {
        if (abs.x == v)
            result = new Abs(abs);
        else {
            if (freeVars(n).count(abs.x) != 0) {
                std::stringstream nString;
                nString << n;
                throw std::runtime_error("abs.x \\in FV(n) [abs.x = " + abs.x + ", n:" + nString.str() + "]"); // temporary ...
            }
            result = new Abs(abs.x, substitute(*abs.body, v, n));
        }
    }

    void visit(const NumConst& num) {
        result = new NumConst(num);
    }

    void visit(const ::Var& var) {
        if (var.name == v)
            result = copy(n);
        else
            result = new ::Var(var);
    }
};

const LambdaExpr* substitute(const LambdaExpr& e, const std::string& v, const LambdaExpr& n) {
    Substitution subs(v, n);
    e.accept(subs);
    return subs.result;
}

struct LambdaEvaluator2 : public LambdaConstVisitor {
    const LambdaExpr* result = nullptr;
    Strategy strategy;

    LambdaEvaluator2(Strategy strategy_) : strategy(strategy_) {}

    const LambdaExpr* eval(const App& app);
    const LambdaExpr* eval(const Abs& abs);
    const LambdaExpr* eval(const NumConst& num);
    const LambdaExpr* eval(const ::Var& var);

    void visit(const App& app);
    void visit(const Abs& abs);
    void visit(const NumConst& num);
    void visit(const ::Var& var);
};

const LambdaExpr* apply(const LambdaExpr* x, const LambdaExpr* y) {
    auto f = dynamic_cast<const Abs*>(x);
    if (f) {
        return canonym(*substitute(*f->body, f->x, *y), f->x);
    } else
        return new App(x, y);
}

const LambdaExpr* LambdaEvaluator2::eval(const App& app) {
    //auto f = dynamic_cast<const Abs*>(evaluator2::eval(*app.f));
    //if (f == nullptr)
    //    throw std::runtime_error("f == nullptr");
    //auto x = evaluator2::eval(*app.x);

    //std::cout << "Applying " << *f << " to " << *x << std::endl;
    //auto r = evaluator2::eval(*substitute(*f->body, f->x, *x));

    //std::cout << "result: " << *r << std::endl;
    //
    //return r;
    return evaluator2::eval(*apply(evaluator2::eval(*app.f, strategy), strategy(*app.x)), strategy); // TODO: Join with apply to avoid recursion when app.f is not Abs?
}

const LambdaExpr* LambdaEvaluator2::eval(const Abs& abs) {
    return new Abs(abs);
}

const LambdaExpr* LambdaEvaluator2::eval(const NumConst& num) {
    return new NumConst(num);
}

const LambdaExpr* LambdaEvaluator2::eval(const ::Var& var) {
    //std::cout << "eval(" << var << ")" << std::endl;
    //const LambdaExpr* r_ = env.getWithDefault(var.name, nullptr);
    //if (!r_) {
    //    std::cout << "variable " << var << " is undefined. Returning symbol." << std::endl;
    //    return new ::Var(var);
    //}
    //LambdaExpr* r = copy(*r_);
    //std::cout << "got " << var << ": " << *r << std::endl;
    //
    //LambdaExpr* old_r = r;
    //while(!isWHNF(r) && r != old_r) {
    //    old_r = r;
    //    //r->accept(*this);
    //    r = evaluator2::eval(*r, env);
    //    std::cout << "at loop end: r = " << r << ", old_r=" << old_r << std::endl;
    //}
    //return r;
    return new ::Var(var);
}

void LambdaEvaluator2::visit(const App& app) {
    result = eval(app);
}
void LambdaEvaluator2::visit(const Abs& abs) {
    result = eval(abs);
}
void LambdaEvaluator2::visit(const NumConst& num) {
    result = eval(num);
}
void LambdaEvaluator2::visit(const ::Var& var) {
    result = eval(var);
}

const LambdaExpr* eval(const LambdaExpr& e, const std::string& prefix, Strategy strategy) {
    return evaluator2::eval(*canonym(e, prefix), strategy);
}

const LambdaExpr* replace_vars(const LambdaExpr* e, const std::unordered_map<std::string, const LambdaExpr*>& env) {
    for (const auto& p : env)
        e = substitute(*e, p.first, *p.second);
    return e;
}
} // namespace

const LambdaExpr* eval(const LambdaExpr& e, Strategy strategy) {
    LambdaEvaluator2 evaluator(strategy);
    e.accept(evaluator);
    return evaluator.result;
}


const LambdaExpr* evalProgram(const Program& p, Strategy strategy) {
    //std::cout << "evalProgram" << std::endl;
    std::unordered_map<std::string, const LambdaExpr*> env;
    //std::cout << "building environment" << std::endl;
    for (auto sc : p.scs) {
        //std::cout << sc << std::endl;
        const auto& n = sc.f;
        const auto& params = sc.params;
        auto body = sc.body;
        for (auto it = params.names.rbegin(); it != params.names.rend(); ++it)
            body = new Abs(*it, body);
        body = replace_vars(body, env);
        for (auto& p : env)
            p.second = substitute(*p.second, n, *body);
        env.insert({n, body});
    }

    std::cout << "env:" << std::endl;
    for (const auto& p : env)
        std::cout << p.first << ": " << *p.second << std::endl;

    std::cout << "Evaluation:" << std::endl;
    auto main = env.at("main");
    auto r = canonym(*eval(*main, "x", strategy), "x");

    std::cout << *r << std::endl;
    return r;
}
}

//LambdaExpr* eval(const LambdaExpr& e, const std::unordered_map<std::string, LambdaExpr*>& env, const std::string& prefix, Strategy strategy) {
//    // TODO: replace variables in e with their bodies in env until convergence, then call eval(const LambdaExpr&) on the result
//    LambdaExpr* e_ = copy(e);
//    for (const auto& p : env)
//        e_ = substitute(*e_, p.first, *p.second);
//
//    return nullptr;
//}

//const Const* evalLambda(LambdaExpr* e, LambdaEvaluator& evaluator) {
//    std::cout << "evalLambda(" << *e << ")" << std::endl;
//    e->accept(evaluator);
//    std::cout << "result(evalLambda): " << *evaluator.result << std::endl;
//    return evaluator.result;
//}

