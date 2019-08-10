#pragma once

#include <cstdint> // for uint64_t
#include <list>    // for list
#include <ostream> // for ostream
#include <string>  // for string
#include <utility> // for move

namespace Functional {
struct App;
struct Abs;
struct NumConst;
struct Variable;

struct LambdaVisitor {
    virtual void visit(App&) = 0;
    virtual void visit(Abs&) = 0;
    virtual void visit(NumConst&) = 0;
    virtual void visit(Variable&) = 0;
    virtual ~LambdaVisitor() = default;
};

struct LambdaConstVisitor {
    virtual void visit(const App&) = 0;
    virtual void visit(const Abs&) = 0;
    virtual void visit(const NumConst&) = 0;
    virtual void visit(const Variable&) = 0;
    virtual ~LambdaConstVisitor() = default;
};

struct LambdaExpr {
    virtual void accept(LambdaVisitor&) = 0;
    virtual void accept(LambdaConstVisitor&) const = 0;
    virtual ~LambdaExpr() = default;
};

template <typename T>
struct LambdaExprBase : public LambdaExpr {
    void accept(LambdaVisitor& v) { v.visit(*static_cast<T*>(this)); }
    void accept(LambdaConstVisitor& v) const { v.visit(*static_cast<const T*>(this)); }
};

struct NumConst : public LambdaExprBase<NumConst> {
    uint64_t val;

    NumConst(uint64_t _val) : val(_val) {}
};

struct Variable : public LambdaExprBase<Variable> {
    std::string name;
    Variable(std::string _name) : name(move(_name)) {}
};

struct App : public LambdaExprBase<App> {
    const LambdaExpr* f;
    const LambdaExpr* x;

    App(const LambdaExpr* a, const LambdaExpr* b) : f(a), x(b) {}
};

struct Abs : public LambdaExprBase<Abs> {
    std::string x;
    const LambdaExpr* body;

    Abs(std::string x_, const LambdaExpr* body_) : x(move(x_)), body(body_) {}
};

bool operator==(const LambdaExpr& lhs, const LambdaExpr& rhs);

std::ostream& operator<<(std::ostream& s, const LambdaExpr& e);

struct VarList {
    std::list<std::string> names;
    VarList() = default;
};

struct Supercombinator {
    std::string f;
    VarList params;
    const LambdaExpr* body;
    Supercombinator(std::string f_, VarList xs_, const LambdaExpr* body_)
        : f(move(f_)), params(std::move(xs_)), body(body_) {}
};

std::ostream& operator<<(std::ostream& s, const Supercombinator& sc);

struct Program {
    std::list<Supercombinator> scs;
};

struct VarListAttribute;
struct ExprAttribute;
struct ProgramAttribute;
struct ScAttribute;

struct AttributeVisitor {
    virtual void visit(VarListAttribute&) = 0;
    virtual void visit(ExprAttribute&) = 0;
    virtual void visit(ProgramAttribute&) = 0;
    virtual void visit(ScAttribute&) = 0;
    virtual ~AttributeVisitor() = default;
};

struct AttributeConstVisitor {
    virtual void visit(const VarListAttribute&) = 0;
    virtual void visit(const ExprAttribute&) = 0;
    virtual void visit(const ProgramAttribute&) = 0;
    virtual void visit(const ScAttribute&) = 0;
    virtual ~AttributeConstVisitor() = default;
};

struct Attribute {
    virtual ~Attribute() = default;
    virtual void accept(AttributeVisitor&) = 0;
    virtual void accept(AttributeConstVisitor&) const = 0;
};

template <typename T>
struct AttributeBase : public Attribute {
    void accept(AttributeVisitor& v) { v.visit(*static_cast<T*>(this)); }
    void accept(AttributeConstVisitor& v) const { v.visit(*static_cast<const T*>(this)); }
};

struct VarListAttribute : public AttributeBase<VarListAttribute> {
    VarList l;
    VarListAttribute(VarList l_) : l(std::move(l_)) {}
};

struct ExprAttribute : public AttributeBase<ExprAttribute> {
    LambdaExpr* e;
    ExprAttribute(LambdaExpr* e_) : e(e_) {}
};

struct ProgramAttribute : public AttributeBase<ProgramAttribute> {
    Program p;
    ProgramAttribute() = default;
    ProgramAttribute(Program p_) : p(std::move(p_)) {}
};

struct ScAttribute : public AttributeBase<ScAttribute> {
    Supercombinator sc;
    ScAttribute(Supercombinator sc_) : sc(std::move(sc_)) {}
};

std::ostream& operator<<(std::ostream& s, const Attribute& sc);

LambdaExpr* toExpr(Attribute* a);

VarList& toVarList(Attribute* a);

Supercombinator& toSupercombinator(Attribute* a);

Program& toProgram(Attribute* a);

Variable* toVar(Attribute* a);

LambdaExpr* copy(const LambdaExpr& e);
} // namespace Functional
