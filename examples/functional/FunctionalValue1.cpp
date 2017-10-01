#include "FunctionalValue1.h"

#include "FunctionalAttribute.h"

const LambdaExpr* toValue(const Const* c) {
    auto value = dynamic_cast<const Value*>(c);
    if (!value) {
        if (!c)
            throw std::runtime_error("toFunction(nullptr)");
        else
            throw std::runtime_error("toValue failed");
    }
    return value->v;
}

const Function::type toFunction(const Const* c) {
    auto func = dynamic_cast<const Function*>(c);
    if (!func) {
        if (!c)
            throw std::runtime_error("toFunction(nullptr)");
        else {
            std::stringstream sstr;
            sstr << *c;
            throw std::runtime_error("toFunction failed. Have " + sstr.str());
        }
    }
    return func->f;
}

std::ostream& operator<<(std::ostream& s, const Value& v) {
    return s << *v.v;
}

class IdGenerator {
    uint32_t id = 0;
public:
    uint32_t getNext() {
        auto v = id;
        ++id;
        //std::cout << "IdGenerator: " << v << std::endl;
        return v;
    }
};

struct VariableNameGenerator {
    IdGenerator gen;
    std::string getNext() {
        auto name = "x" + std::to_string(gen.getNext());
        //std::cout << "VariableNameGenerator: " << name << std::endl;
        return name;
    }
};

struct ConstPrinter : public ConstConstVisitor {
    std::ostream& s;
    VariableNameGenerator g;
    ConstPrinter(std::ostream& s_) : s(s_) {
        //std::cout << "new ConstPrinter" << std::endl;
    }
    void visit(const Function& f) {
        auto x = g.getNext();
        auto v = new Value(new ::Var(x));
        s << "(\\" << x << " . ";
        f.f(v)->accept(*this);
        s << ")";
    }
    void visit(const Value& v) {
        s << v;
    }
};

std::ostream& operator<<(std::ostream& s, const Function& f) {
    ConstPrinter printer(s);
    f.accept(printer);
    return s;
}

std::ostream& operator<<(std::ostream& s, const Const& c) {
    ConstPrinter printer(s);
    c.accept(printer);
    return s;
}

std::ostream& operator<<(std::ostream& s, const Environment& env_) {
    const auto& env = env_.get();
    s << "[";
    if (!env.empty()) {
        auto e = env.begin();
        s << e->first << ": " << *e->second;
        auto end = env.end();
        for (++e; e != end; ++e)
            s << ", " << e->first << ": " << *e->second;
    }
    return s << "]";
}

void Environment::set(std::string name, const Const* c) {
    std::cout << "set(" << name << ", " << c << ")" << std::endl;
    if (env.find(name) == env.end())
        env.emplace(move(name), c);
    else
        throw std::runtime_error(name + " redefined");
}

const Const* Environment::getOrDefine(const std::string& name) {
    std::cout << "getOrDefine(" << name << "): " << std::flush;
    auto c = env.find(name);
    if (c != env.end()) {
        std::cout << c->second << std::endl;
        return c->second;
    } else {
        std::cout << "Defining " << name << std::endl;
        return env[name] = new Value(new ::Var(name));
    }
}

const Const* Environment::get(const std::string& name) const {
    std::cout << "get(" << name << "): " << std::endl;
    auto c = env.find(name);
    if (c != env.end()) {
        std::cout << c->second << std::endl;
        return c->second;
    } else
        throw std::runtime_error("Undefined: " + name);
}
