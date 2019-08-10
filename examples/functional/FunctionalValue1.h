#pragma once

#include <functional>    // for function
#include <ostream>       // for ostream
#include <string>        // for string
#include <unordered_map> // for unordered_map

namespace Functional {
struct LambdaExpr;

struct Function;
struct Value;

struct ConstVisitor {
    virtual ~ConstVisitor() = default;
    virtual void visit(Function&) = 0;
    virtual void visit(Value&) = 0;
};

struct ConstConstVisitor {
    virtual ~ConstConstVisitor() = default;
    virtual void visit(const Function&) = 0;
    virtual void visit(const Value&) = 0;
};

// template<typename Visitor>
// struct Foo {
//    using return_type =  typename Visitor::return_type;
//
//    template<typename T>
//    struct Model : Concept {
//        return_type accept(Visitor& v) override { return v.visit(data); }
//        T* data;
//    };
//    struct Concept {
//        virtual ~Concept() = default;
//        return_type accept(Visitor&) = 0;
//    };
//
//    Concept* p;
//};

struct Const {
    virtual ~Const() = default;
    virtual void accept(ConstVisitor&) = 0;
    virtual void accept(ConstConstVisitor&) const = 0;
};

template <typename T>
struct ConstBase : public Const {
    void accept(ConstVisitor& v) { v.visit(*static_cast<T*>(this)); }
    void accept(ConstConstVisitor& v) const { v.visit(*static_cast<const T*>(this)); }
};

struct Function : public ConstBase<Function> {
    using type = std::function<const Const*(const Const*)>;
    type f;
    Function(type f_) : f(f_) {}
};

struct Value : public ConstBase<Value> {
    const LambdaExpr* v;
    Value(const LambdaExpr* v_) : v(v_) {}
};

const LambdaExpr* toValue(const Const* c);

std::ostream& operator<<(std::ostream& s, const Const& c);

const Function::type toFunction(const Const* c);

std::ostream& operator<<(std::ostream& s, const Value& v);

std::ostream& operator<<(std::ostream& s, const Function& f);

std::ostream& operator<<(std::ostream& s, const Const& c);

class Environment {
    using env_t = std::unordered_map<std::string, const Const*>;
    env_t env;

  public:
    void set(std::string name, const Const* c);

    const Const* getOrDefine(const std::string& name);

    const Const* get(const std::string& name) const;

    const env_t& get() const { return env; }
};

std::ostream& operator<<(std::ostream& s, const Environment& env_);
} // namespace Functional
