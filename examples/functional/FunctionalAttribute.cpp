#include "FunctionalAttribute.h"

#include <iostream>

struct NumEqual : public LambdaConstVisitor {
    const NumConst& lhs_;
    bool equal;
    NumEqual(const NumConst& lhs) : lhs_(lhs) {}
    void visit(const App&) {
        equal = false;
    }
    void visit(const Abs&) {
        equal = false;
    }
    void visit(const NumConst& rhs) {
        equal = rhs.val == lhs_.val;
    }
    void visit(const ::Var&) {
        equal = false;
    }
};

struct VarEqual : public LambdaConstVisitor {
    const ::Var& lhs_;
    bool equal;
    VarEqual(const ::Var& lhs) : lhs_(lhs) {}
    void visit(const App&) {
        equal = false;
    }
    void visit(const Abs&) {
        equal = false;
    }
    void visit(const NumConst&) {
        equal = false;
    }
    void visit(const ::Var& rhs) {
        equal = rhs.name == lhs_.name;
    }
};

struct AbsEqual : public LambdaConstVisitor {
    const Abs& lhs_;
    bool equal;
    AbsEqual(const Abs& lhs) : lhs_(lhs) {}
    void visit(const App&) {
        equal = false;
    }
    void visit(const Abs& rhs) {
        equal = rhs.x == lhs_.x && *rhs.body == *lhs_.body;
    }
    void visit(const NumConst&) {
        equal = false;
    }
    void visit(const ::Var&) {
        equal = false;
    }
};

struct AppEqual : public LambdaConstVisitor {
    const App& lhs_;
    bool equal;
    AppEqual(const App& lhs) : lhs_(lhs) {}
    void visit(const App& rhs) {
        equal = *rhs.f == *lhs_.f && *rhs.x == *lhs_.x;
    }
    void visit(const Abs&) {
        equal = false;
    }
    void visit(const NumConst&) {
        equal = false;
    }
    void visit(const ::Var&) {
        equal = false;
    }
};

struct LambdaEqual : public LambdaConstVisitor {
    const LambdaExpr& lhs_;
    bool equal = false;
    LambdaEqual(const LambdaExpr& lhs) : lhs_(lhs) {}
    void visit(const App& rhs) {
        AppEqual eq(rhs);
        lhs_.accept(eq);
        equal = eq.equal;
    }
    void visit(const Abs& rhs) {
        AbsEqual eq(rhs);
        lhs_.accept(eq);
        equal = eq.equal;
    }
    void visit(const NumConst& rhs) {
        NumEqual eq(rhs);
        lhs_.accept(eq);
        equal = eq.equal;
    }
    void visit(const ::Var& rhs) {
        VarEqual eq(rhs);
        lhs_.accept(eq);
        equal = eq.equal;
    }
};

bool operator==(const LambdaExpr& lhs, const LambdaExpr& rhs) {
    LambdaEqual eq(lhs);
    rhs.accept(eq);
    return eq.equal;
}

struct LambdaPrinter : public LambdaConstVisitor {
    std::ostream& s;
    LambdaPrinter(std::ostream& s_) : s(s_) {}
    void visit(const App& app) {
        s << "(";
        app.f->accept(*this);
        s << ") (";
        app.x->accept(*this);
        s << ")";
    }
    void visit(const Abs& abs) {
        s << "(\u03BB ";
        s << abs.x << " ";
        s << ". ";
        abs.body->accept(*this);
        s << ")";
    }
    void visit(const NumConst& num) {
        s << num.val;
    }
    void visit(const ::Var& var) {
        s << var.name;
    }
};

std::ostream& operator<<(std::ostream& s, const LambdaExpr& e) {
    LambdaPrinter printer(s);
    e.accept(printer);
    return s;
}

std::ostream& operator<<(std::ostream& s, const Supercombinator& sc) {
    s << sc.f << " ";
    for (const auto& v : sc.params.names)
        s << v << " ";
    s << "= " << *sc.body;
    return s;
}

struct AttributePrinter : public AttributeConstVisitor {
    std::ostream& s;
    AttributePrinter(std::ostream& s_) : s(s_) {}
    void visit(const VarListAttribute&) {
        s << "VarListAttribute";
    }
    void visit(const ExprAttribute&) {
        s << "ExprAttribute";
    }
    void printProgram(const Program& p) {
        for (const auto& sc : p.scs) {
            s << sc;
            s << "\n\n";
        }
    }
    void visit(const ProgramAttribute& program) {
        printProgram(program.p);
    }
    void visit(const ScAttribute&) {
        s << "ScAttribute";
    }
};

std::ostream& operator<<(std::ostream& s, const Attribute& sc) {
    AttributePrinter printer(s);
    sc.accept(printer);
    return s;
}

LambdaExpr* toExpr(Attribute* a) {
    auto expr = dynamic_cast<ExprAttribute*>(a);
    if (!expr) {
        std::cout << "toExpr(";
        if (a) {
            std::cout << *a;
        } else {
            std::cout << "nullptr";
        }
        std::cout << ")" << std::endl;
        throw std::runtime_error("toExpr failed");
    }
    return expr->e;
}

VarList& toVarList(Attribute* a) {
    auto list = dynamic_cast<VarListAttribute*>(a);
    if (!list)
        throw std::runtime_error("toVarList failed");
    return list->l;
}

Supercombinator& toSupercombinator(Attribute* a) {
    auto supercombinator = dynamic_cast<ScAttribute*>(a);
    if (!supercombinator)
        throw std::runtime_error("toSupercombinator failed");
    return supercombinator->sc;
}

Program& toProgram(Attribute* a) {
    auto program = dynamic_cast<ProgramAttribute*>(a);
    if (!program)
        throw std::runtime_error("toProgram failed");
    return program->p;
}

::Var* toVar(Attribute* a) {
    auto v = dynamic_cast<::Var*>(toExpr(a));
    if (!v)
        throw std::runtime_error("toVar failed");
    return v;
}

struct Copy : public LambdaConstVisitor {
    LambdaExpr* result = nullptr;

    void visit(const App& app) {
        result = new App(copy(*app.f), copy(*app.x));
    }

    void visit(const Abs& abs) {
        result = new Abs(abs.x, copy(*abs.body));
    }

    void visit(const NumConst& num) {
        result = new NumConst(num);
    }

    void visit(const ::Var& var) {
        result = new ::Var(var);
    }
};

LambdaExpr* copy(const LambdaExpr& e) {
    Copy c;
    e.accept(c);
    return c.result;
}
