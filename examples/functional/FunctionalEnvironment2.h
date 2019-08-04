#pragma once

#include "FunctionalAttribute.h"

#include <iostream>
#include <string>
#include <unordered_map>

namespace Functional {
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
            return env[name] = new Variable(name);
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

    const env_t& get() const { return env; }
};

std::ostream& operator<<(std::ostream& s, const Environment2& env_);
} // namespace Functional
