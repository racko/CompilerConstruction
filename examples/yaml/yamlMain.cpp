#include "Functional.h"
#include "lrParser.h"
#include "Regex.h"
#include "nfaBuilder.h"
#include "NFA.h"

using namespace Functional;

struct App;
struct Abs;
struct NumConst;
struct Var;

struct LambdaVisitor {
    virtual void visit(App&) = 0;
    virtual void visit(Abs&) = 0;
    virtual void visit(NumConst&) = 0;
    virtual void visit(::Var&) = 0;
    virtual ~LambdaVisitor() = default;
};

struct LambdaConstVisitor {
    virtual void visit(const App&) = 0;
    virtual void visit(const Abs&) = 0;
    virtual void visit(const NumConst&) = 0;
    virtual void visit(const ::Var&) = 0;
    virtual ~LambdaConstVisitor() = default;
};

struct LambdaExpr {
    virtual void accept(LambdaVisitor&) = 0;
    virtual void accept(LambdaConstVisitor&) const = 0;
    virtual ~LambdaExpr() = default;
};

template<typename T>
struct LambdaExprBase : public LambdaExpr {
    void accept(LambdaVisitor& v) {
        v.visit(*static_cast<T*>(this));
    }
    void accept(LambdaConstVisitor& v) const {
        v.visit(*static_cast<const T*>(this));
    }
};


struct NumConst : public LambdaExprBase<NumConst> {
    uint64_t val;

    NumConst(uint64_t _val) : val(_val) {}
};

struct Var : public LambdaExprBase<::Var> {
    std::string name;
    Var(std::string _name) : name(move(_name)) {}
};

struct VarList {
    std::list<std::string> names;
    VarList() = default;
};

struct App : public LambdaExprBase<App> {
    LambdaExpr* f;
    LambdaExpr* x;

    App(LambdaExpr* a, LambdaExpr* b) : f(a), x(b) {}
};

struct Abs : public LambdaExprBase<Abs> {
    std::string x;
    LambdaExpr* body;

    Abs(std::string x_, LambdaExpr* body_) : x(move(x_)), body(body_) {}
};

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


struct Supercombinator {
    std::string f;
    VarList params;
    LambdaExpr* body;
    Supercombinator(std::string f_, VarList xs_, LambdaExpr* body_) : f(move(f_)), params(std::move(xs_)), body(body_) {}
};

std::ostream& operator<<(std::ostream& s, const Supercombinator& sc) {
    s << sc.f << " ";
    for (const auto& v : sc.params.names)
        s << v << " ";
    s << "= " << *sc.body;
    return s;
}

struct Program {
    std::list<Supercombinator> scs;
};

struct Attribute;
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

template<typename T>
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

struct AttributePrinter : public AttributeConstVisitor {
    std::ostream& s;
    AttributePrinter(std::ostream& s_) : s(s_) {}
    void visit(const VarListAttribute& list) {
        s << "VarListAttribute";
    }
    void visit(const ExprAttribute& e) {
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
    void visit(const ScAttribute& sc) {
        s << "ScAttribute";
    }
};

std::ostream& operator<<(std::ostream& s, const Attribute& sc) {
    AttributePrinter printer(s);
    sc.accept(printer);
    return s;
}

struct TerminalToAttribute : public TokenConstVisitor {
    Attribute* result = nullptr;
    void visit(const Num& num) override {
        result = new ExprAttribute(new NumConst(num.n));
    }

    void visit(const Functional::Var& v) override {
        result = new ExprAttribute(new ::Var(v.s));
    }

    void visit(const Token& t) override {}
};

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

struct Parser : public LRParser<Grammar, Attribute*> {
    using LRParser<Grammar, Attribute*>::LRParser;

    Attribute* reduce(NonterminalID<Grammar> A, uint32_t production, std::vector<Attribute*>&& alpha) const override {
        Attribute* val = nullptr;
        switch(A) {
        case NT::START:
            val = alpha[0];
            break;
            //case NT::Q:
            //    val = new StateAttribute(toProgram(alpha[0]), toExpr(alpha[1]));
            //    break;
        case NT::P:
            switch(production) {
            case 0: {
                //auto p = new Program();
                //p->scs.push_back(toSupercombinator(alpha[0]));
                //val = new ProgramAttribute(p);
                //break;
                val = new ProgramAttribute();
                break;
            }
            case 1:
                //val = alpha[2];
                //toProgram(val)->scs.push_front(toSupercombinator(alpha[0]));
                //break;
                val = alpha[0];
                toProgram(val).scs.push_back(std::move(toSupercombinator(alpha[1])));
                break;
            default:
                throw std::runtime_error("wrong production id");
            }
            break;
        case NT::S:
            switch(production) {
            case 0:
                val = new ScAttribute(Supercombinator(std::move(toVar(alpha[0])->name), VarList(), toExpr(alpha[2])));
                break;
            case 1:
                val = new ScAttribute(Supercombinator(std::move(toVar(alpha[0])->name), toVarList(alpha[1]), toExpr(alpha[3])));
                break;
            default:
                throw std::runtime_error("wrong production id");
            }
            break;
        case NT::A:
            switch(production) {
            case 0: {
                LambdaExpr* body = toExpr(alpha[3]);
                const auto& names = toVarList(alpha[1]).names;
                for (auto v = names.rbegin(); v != names.rend(); ++v)
                    body = new Abs(std::move(*v), body);
                val = new ExprAttribute(body);
                break;
            }
            case 1:
                val = alpha[0];
                break;
            default:
                throw std::runtime_error("wrong production id");
            }
            break;
        case NT::L:
            switch (production) {
            case 0:
            case 1:
                throw std::runtime_error("lets are not implemented yet");
                break;
            case 2:
                val = alpha[0];
                break;
            default:
                throw std::runtime_error("wrong production id");
            }
            break;
        case NT::B:
            switch(production) {
            case 0:
                val = new ExprAttribute(new App(toExpr(alpha[0]), toExpr(alpha[1])));
                break;
            case 1:
                val = alpha[0];
                break;
            default:
                throw std::runtime_error("wrong production id");
            }
            break;
        case NT::C:
            switch (production) {
            case 0:
                val = alpha[1];
                break;
            case 1:
            case 2:
                val = alpha[0];
                break;
            default:
                throw std::runtime_error("wrong production id");
            }
            break;
        case NT::D:
            switch (production) {
            case 0:
                val = alpha[1];
                toVarList(val).names.push_front(std::move(toVar(alpha[0])->name));
                break;
            case 1: {
                VarList l;
                l.names.push_back(std::move(toVar(alpha[0])->name));
                val = new VarListAttribute(std::move(l));
                break;
            }
            default:
                throw std::runtime_error("wrong production id");
            }
            break;
        }
        return val;
    }

    Attribute* shift(const Token* t) const override {
        assert(t != nullptr);
        TerminalToAttribute visitor;
        t->accept(visitor);
        auto val = visitor.result;

        delete t;
        return val;
    }
};

using std::stringstream;
struct myLexer : public Lexer<Token*> {
    static const vector<unsigned int> m;

    DFA_t getDFA() {
        stringstream in;
        nfaBuilder<Symbol,State> builder;

        char alpha[] = "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)";
        char digit[] = "(0|1|2|3|4|5|6|7|8|9)";
        stringstream varch;
        varch << "(" << alpha << "|" << digit << "|_)";
        stringstream num;
        num << digit << digit << "*";
        stringstream var;
        var << alpha << varch.rdbuf() << "*";
        in << ";";
        builder.lexRegex(in, (unsigned)T::SEMICOLON + 1);
        in.clear();
        in << "=";
        builder.lexRegex(in, (unsigned)T::ASSIGN + 1);
        in.clear();
        in << ".";
        builder.lexRegex(in, (unsigned)T::PERIOD + 1);
        in.clear();
        in << "\\(";
        builder.lexRegex(in, (unsigned)T::LPAREN + 1);
        in.clear();
        in << "\\)";
        builder.lexRegex(in, (unsigned)T::RPAREN + 1);
        in.clear();
        in << "+";
        builder.lexRegex(in, (unsigned)T::PLUS + 1);
        in.clear();
        in << "-";
        builder.lexRegex(in, (unsigned)T::MINUS + 1);
        in.clear();
        in << "\\*";
        builder.lexRegex(in, (unsigned)T::TIMES + 1);
        in.clear();
        in << "/";
        builder.lexRegex(in, (unsigned)T::DIV + 1);
        in.clear();
        in << "<";
        builder.lexRegex(in, (unsigned)T::LT + 1);
        in.clear();
        in << ">";
        builder.lexRegex(in, (unsigned)T::GT + 1);
        in.clear();
        in << "&";
        builder.lexRegex(in, (unsigned)T::AND + 1);
        in.clear();
        in << "\\|";
        builder.lexRegex(in, (unsigned)T::OR + 1);
        in.clear();
        in << "\\\\";
        builder.lexRegex(in, (unsigned)T::BSL + 1);
        in.clear();
        in << "<=";
        builder.lexRegex(in, (unsigned)T::LE + 1);
        in.clear();
        in << "==";
        builder.lexRegex(in, (unsigned)T::EQ + 1);
        in.clear();
        in << "!=";
        builder.lexRegex(in, (unsigned)T::NE + 1);
        in.clear();
        in << ">=";
        builder.lexRegex(in, (unsigned)T::GE + 1);
        in.clear();
        in << "let";
        builder.lexRegex(in, (unsigned)T::LET + 1);
        in.clear();
        in << "in";
        builder.lexRegex(in, (unsigned)T::IN + 1);
        in.clear();
        in << "letrec";
        builder.lexRegex(in, (unsigned)T::LETREC + 1);
        builder.lexRegex(var, (unsigned)T::VAR + 1);
        builder.lexRegex(num, (unsigned)T::NUM + 1);
        in.clear();
        in << "( |\t|\n)( |\t|\n)*";
        builder.lexRegex(in, (unsigned)T::WS + 1);

        NFA<Symbol,State> nfa1(builder);
        DFA_t dfa1(nfa1);
        dfa1.minimize();
        return dfa1;
    }

    myLexer() : Lexer(getDFA()) {}

    Token* eofToken() const {
        return new Token(T::EOI);
    }

    Token* whiteSpaceToken() const {
        return nullptr;
    }

    Token* action(char* s, unsigned int n, unsigned int t) {
        cout << "action(\"";
        cout.write(s, n);
        cout << "\", " << n << ", " << t << ")" << endl;
        Token* tok;
        auto tid = TerminalID<Grammar>(t - 1);
        switch(tid) {
        case T::SEMICOLON:
        case T::ASSIGN:
        case T::PERIOD:
        case T::LPAREN:
        case T::RPAREN:
        case T::BSL:
        case T::PLUS:
        case T::MINUS:
        case T::TIMES:
        case T::DIV:
        case T::LT:
        case T::GT:
        case T::AND:
        case T::OR:
        case T::LE:
        case T::EQ:
        case T::NE:
        case T::GE:
        case T::LET:
        case T::IN:
        case T::LETREC:
            tok = new Token(tid);
            break;
        case T::VAR:
            tok = new Functional::Var(string(s, n));
            break;
        case T::NUM: {
            unsigned long long i;
            stringstream ss;
            ss.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            ss.write(s, n);
            ss >> i;
            tok = new Num(i);
            break;
        }
        case T::WS:
            tok = nullptr;
            break;
        default: {
            stringstream ss;
            ss << "invalid token type: " << t;
            throw std::runtime_error(ss.str());
        }

        }
        return tok;
    }
};

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

struct Const {
    virtual ~Const() = default;
    virtual void accept(ConstVisitor&) = 0;
    virtual void accept(ConstConstVisitor&) const = 0;
};

template<typename T>
struct ConstBase : public Const {
    void accept(ConstVisitor& v) {
        v.visit(*static_cast<T*>(this));
    }
    void accept(ConstConstVisitor& v) const {
        v.visit(*static_cast<const T*>(this));
    }
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

std::ostream& operator<<(std::ostream& s, const Const& c);

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

std::ostream& operator<<(std::ostream& s, const Value& v) {
    return s << *v.v;
}

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


class Environment {
    using env_t = std::unordered_map<std::string, const Const*>;
    env_t env;

public:
    void set(std::string name, const Const* c) {
        std::cout << "set(" << name << ", " << c << ")" << std::endl;
        if (env.find(name) == env.end())
            env.emplace(move(name), c);
        else
            throw std::runtime_error(name + " redefined");
    }

    const Const* getOrDefine(const std::string& name) {
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

    const Const* get(const std::string& name) const {
        std::cout << "get(" << name << "): " << std::endl;
        auto c = env.find(name);
        if (c != env.end()) {
            std::cout << c->second << std::endl;
            return c->second;
        } else
            throw std::runtime_error("Undefined: " + name);
    }

    const env_t& get() const {
        return env;
    }
};

class Environment2 {
    using env_t = std::unordered_map<std::string, const LambdaExpr*>;
    env_t env;

public:
    void set(std::string name, const LambdaExpr* c) {
        std::cout << "set(" << name << ", " << *c << ")" << std::endl;
        if (env.find(name) == env.end())
            env.emplace(move(name), c);
        else
            throw std::runtime_error(name + " redefined");
    }

    void redefine(std::string name, const LambdaExpr* c) {
        std::cout << "redefine(" << name << ", " << *c << ")" << std::endl;
        env[name] = c;
    }

    const LambdaExpr* getOrDefine(const std::string& name) {
        std::cout << "getOrDefine(" << name << "): " << std::flush;
        auto c = env.find(name);
        if (c != env.end()) {
            std::cout << *c->second << std::endl;
            return c->second;
        } else {
            std::cout << "Defining " << name << std::endl;
            return env[name] = new ::Var(name);
        }
    }

    const LambdaExpr* get(const std::string& name) const {
        std::cout << "get(" << name << "): " << std::flush;
        auto c = env.find(name);
        if (c != env.end()) {
            std::cout << *c->second << std::endl;
            return c->second;
        } else
            throw std::runtime_error("Undefined: " + name);
    }

    const LambdaExpr* getWithDefault(const std::string& name, const LambdaExpr* def) const {
        std::cout << "get(" << name << "): " << std::flush;
        auto c = env.find(name);
        if (c != env.end()) {
            std::cout << *c->second << std::endl;
            return c->second;
        } else {
            std::cout << "default" << std::endl;
            return def;
        }
    }

    const env_t& get() const {
        return env;
    }
};

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

std::ostream& operator<<(std::ostream& s, const Environment2& env_) {
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

struct LambdaEvaluator : public LambdaConstVisitor {
    Environment env;
    const Const* result = nullptr;

    //LambdaEvaluator() { std::cout << "new LambdaEvaluator" << std::endl; }
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
        } while(!func);
        //auto f = toFunction(result);
        app.x->accept(*this);
        auto x = result;
        return func->f(x);
    }

    const Const* eval(const Abs& abs) {
        std::cout << "LambdaEvaluator::eval(" << abs << ")" << std::endl;
        const auto& env = this->env;
        auto r = new Function([abs, env] (const Const* d) {
                //std::cout << "Applying " << abs << " to " << d << " in " << env << std::endl;
                LambdaEvaluator inner;
                inner.env = std::move(env);
                const auto& param = abs.x;
                //std::cout << "Abstraction has only one argument" << std::endl;
                inner.env.set(param, d);
                abs.body->accept(inner);
                std::cout << "result: " << inner.result << std::endl;

                return inner.result;
            });
        std::cout << "r: " << r << std::endl;
        return r;
    }

    const Const* eval(const NumConst& num) {
        return new Value(&num);
    }

    const Const* eval(const ::Var& var) {
        return env.getOrDefine(var.name);
    }

    void visit(const App& app) {
        result = eval(app);
    }
    void visit(const Abs& abs) {
        std::cout << "visit(" << abs << ")" << std::endl;
        result = eval(abs);
        std::cout << "result: " << result << std::endl;
    }
    void visit(const NumConst& num) {
        result = eval(num);
    }
    void visit(const ::Var& var) {
        result = eval(var);
    }
};

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

LambdaExpr* canonym(const LambdaExpr& e, const std::unordered_map<std::string, std::string>& m, const std::string& prefix);

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

LambdaExpr* canonym(const LambdaExpr& e, const std::unordered_map<std::string, std::string>& m, const std::string& prefix) {
    Canonym c(m, prefix);
    e.accept(c);
    return c.result;
}

LambdaExpr* canonym(const LambdaExpr& e, const std::string& prefix) {
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

    void visit(const NumConst& num) {}

    void visit(const ::Var& var) {
        result.insert(var.name);
    }
};

std::unordered_set<std::string> freeVars(const LambdaExpr& e) {
    FreeVars fv;
    e.accept(fv);
    return fv.result;
}

LambdaExpr* copy(const LambdaExpr& e);

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

LambdaExpr* substitute(const LambdaExpr& e, const std::string& v, const LambdaExpr& n);

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

LambdaExpr* substitute(const LambdaExpr& e, const std::string& v, const LambdaExpr& n) {
    Substitution subs(v, n);
    e.accept(subs);
    return subs.result;
}

using Strategy = std::function<LambdaExpr*(const LambdaExpr&)>;

struct LambdaEvaluator2 : public LambdaConstVisitor {
    LambdaExpr* result = nullptr;
    Strategy strategy;

    LambdaEvaluator2(Strategy strategy_) : strategy(strategy_) {}

    LambdaExpr* eval(const App& app);
    LambdaExpr* eval(const Abs& abs);
    LambdaExpr* eval(const NumConst& num);
    LambdaExpr* eval(const ::Var& var);

    void visit(const App& app);
    void visit(const Abs& abs);
    void visit(const NumConst& num);
    void visit(const ::Var& var);
};


LambdaExpr* eval(const LambdaExpr& e, Strategy strategy);

LambdaExpr* apply(LambdaExpr* x, LambdaExpr* y) {
    auto f = dynamic_cast<Abs*>(x);
    if (f) {
        return canonym(*substitute(*f->body, f->x, *y), f->x);
    } else
        return new App(x, y);
}

LambdaExpr* LambdaEvaluator2::eval(const App& app) {
    //auto f = dynamic_cast<const Abs*>(::eval(*app.f));
    //if (f == nullptr)
    //    throw std::runtime_error("f == nullptr");
    //auto x = ::eval(*app.x);

    //std::cout << "Applying " << *f << " to " << *x << std::endl;
    //auto r = ::eval(*substitute(*f->body, f->x, *x));

    //std::cout << "result: " << *r << std::endl;
    //
    //return r;
    return ::eval(*apply(::eval(*app.f, strategy), strategy(*app.x)), strategy); // TODO: Join with apply to avoid recursion when app.f is not Abs?
}

LambdaExpr* LambdaEvaluator2::eval(const Abs& abs) {
    return new Abs(abs);
}

LambdaExpr* LambdaEvaluator2::eval(const NumConst& num) {
    return new NumConst(num);
}

LambdaExpr* LambdaEvaluator2::eval(const ::Var& var) {
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
    //    r = ::eval(*r, env);
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

LambdaExpr* eval(const LambdaExpr& e, Strategy strategy) {
    LambdaEvaluator2 evaluator(strategy);
    e.accept(evaluator);
    return evaluator.result;
}

LambdaExpr* eval(const LambdaExpr& e, const std::string& prefix, Strategy strategy) {
    return eval(*canonym(e, prefix), strategy);
}

//LambdaExpr* eval(const LambdaExpr& e, const std::unordered_map<std::string, LambdaExpr*>& env, const std::string& prefix, Strategy strategy) {
//    // TODO: replace variables in e with their bodies in env until convergence, then call eval(const LambdaExpr&) on the result
//    LambdaExpr* e_ = copy(e);
//    for (const auto& p : env)
//        e_ = substitute(*e_, p.first, *p.second);
//
//    return nullptr;
//}

const Const* evalLambda(LambdaExpr* e, LambdaEvaluator& evaluator) {
    std::cout << "evalLambda(" << *e << ")" << std::endl;
    e->accept(evaluator);
    std::cout << "result(evalLambda): " << *evaluator.result << std::endl;
    return evaluator.result;
}

LambdaExpr* lazy(const LambdaExpr& e) {
    return copy(e);
}

LambdaExpr* eager(const LambdaExpr& e) {
    return eval(e, eager);
}

LambdaExpr* replace_vars(LambdaExpr* e, const std::unordered_map<std::string, LambdaExpr*>& env) {
    for (const auto& p : env)
        e = substitute(*e, p.first, *p.second);
    return e;
}

void evalProgram(const Program& p, Strategy strategy) {
    //std::cout << "evalProgram" << std::endl;
    std::unordered_map<std::string, LambdaExpr*> env;
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
}

int main() {
    myLexer lex;
    Parser parser;
    std::cout << std::boolalpha;
    cout << "> ";
    cin.get(lex.c, 4096);
    auto ll = cin.gcount();
    cout << "read " << ll << " characters" << endl;
    std::cout << "parsing \"" << lex.c << "\"" << std::endl;
    lex.c[ll] = EOF;
    auto result = parser.parse(lex.begin());
    std::cout << *result << std::endl;
    evalProgram(toProgram(result), eager);

    return 0;
}
