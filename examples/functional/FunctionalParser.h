#pragma once

#include "FunctionalAttribute.h"
#include "FunctionalGrammar.h"
#include "TokenAttributeConversion.h"
#include "lrParser.h"

namespace Functional {
struct Parser : public lr_parser::LRParser<Functional::Grammar, Attribute*> {
    using NT = Functional::NT;

    using LRParser<Functional::Grammar, Attribute*>::LRParser;

    Attribute* reduce(NonterminalID A, uint32_t production, Attribute** alpha, size_t) override {
        Attribute* val = nullptr;
        switch (A) {
        case NT::START:
            val = alpha[0];
            break;
            // case NT::Q:
            //    val = new StateAttribute(toProgram(alpha[0]), toExpr(alpha[1]));
            //    break;
        case NT::P:
            switch (production) {
            case 0: {
                // auto p = new Program();
                // p->scs.push_back(toSupercombinator(alpha[0]));
                // val = new ProgramAttribute(p);
                // break;
                val = new ProgramAttribute();
                break;
            }
            case 1:
                // val = alpha[2];
                // toProgram(val)->scs.push_front(toSupercombinator(alpha[0]));
                // break;
                val = alpha[0];
                toProgram(val).scs.push_back(std::move(toSupercombinator(alpha[1])));
                break;
            default:
                throw std::runtime_error("wrong production id");
            }
            break;
        case NT::S:
            switch (production) {
            case 0:
                val = new ScAttribute(Supercombinator(std::move(toVar(alpha[0])->name), VarList(), toExpr(alpha[2])));
                break;
            case 1:
                val = new ScAttribute(
                    Supercombinator(std::move(toVar(alpha[0])->name), toVarList(alpha[1]), toExpr(alpha[3])));
                break;
            default:
                throw std::runtime_error("wrong production id");
            }
            break;
        case NT::A:
            switch (production) {
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
            switch (production) {
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

    Attribute* shift(Functional::Grammar::Token&& t) override {
        return toAttribute(t);
    }
};
} // namespace Functional
