#include "TokenAttributeConversion.h"

#include "FunctionalAttribute.h"
#include "FunctionalGrammar.h"

struct TerminalToAttribute : public Functional::TokenConstVisitor {
    Attribute* result = nullptr;
    void visit(const Functional::Num& num) override {
        result = new ExprAttribute(new NumConst(num.n));
    }

    void visit(const Functional::Var& v) override {
        result = new ExprAttribute(new ::Var(v.s));
    }

    void visit(const Functional::Token&) override {}
};

Attribute* toAttribute(const Functional::Token& t) {
    TerminalToAttribute visitor;
    t.accept(visitor);
    return visitor.result;
}

