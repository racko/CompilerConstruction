#include "TokenAttributeConversion.h"

#include "FunctionalAttribute.h"
#include "FunctionalGrammar.h"

namespace Functional {
struct TerminalToAttribute : public TokenConstVisitor {
    Attribute* result = nullptr;
    void visit(const Num& num) override { result = new ExprAttribute(new NumConst(num.n)); }

    void visit(const Var& v) override { result = new ExprAttribute(new Variable(v.s)); }

    void visit(const Token&) override {}
};

Attribute* toAttribute(const std::unique_ptr<Token>& t) {
    TerminalToAttribute visitor;
    t->accept(visitor);
    return visitor.result;
}
} // namespace Functional
