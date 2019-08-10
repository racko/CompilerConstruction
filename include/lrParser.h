#pragma once

#include <Grammar.h> // for kind, kind::EOI, kind::NONTERMINAL, kind::TERMINAL
#include <cassert>   // for assert
#include <cstddef>   // for ptrdiff_t, size_t
#include <cstdint>   // for uint32_t
#include <iostream>  // for operator<<, endl, ostream, cout, basic_ostream
#include <limits>    // for numeric_limits
#include <list>      // for list
#include <map>       // for map
#include <parser.h>  // for Parser
#include <set>       // for set
#include <stdexcept> // for logic_error, runtime_error
#include <vector>    // for vector

template <typename TokenType>
class TokenStream;

namespace lr_parser {
enum class type : uint32_t { SHIFT, REDUCE, ACCEPT, FAIL };

inline uint32_t bits(type t) { return uint32_t(t) << 30; }

template <class G>
struct action {
    uint32_t x;
    static const uint32_t mask = 3u << 30;

    action() = default;
    action(type t, uint32_t s) : x(bits(t) | s) { assert(s < (1u << 31) && t == type::SHIFT); }
    action(type t) : x(bits(t)) { assert(t == type::ACCEPT || t == type::FAIL); }
    action(type t, typename G::NonterminalID A, uint32_t production) : x(bits(t) | A.x << 15 | production) {
        assert(A.x < (1u << 16) && production < (1u << 16) && t == type::REDUCE);
        auto a = getHead();
        auto p = getProduction();
        assert(a == A && p == production);
    }
    type getType() const { return type(x >> 30); }
    uint32_t getState() const {
        auto t = getType();
        assert(t == type::SHIFT);
        return x & ~mask;
    }
    typename G::NonterminalID getHead() const {
        assert(getType() == type::REDUCE);
        auto shifted = x >> 15;
        auto m = (1 << 15) - 1;
        return typename G::NonterminalID(shifted & m);
    }
    uint32_t getProduction() const {
        assert(getType() == type::REDUCE);
        return x & ((1 << 15) - 1);
    }
};

template <class G>
struct item {
    using String = std::list<typename G::GrammarElement>;
    typename G::NonterminalID A;
    String l;
    String r;
    typename G::TerminalID a;
    uint32_t production;

    item(typename G::NonterminalID _A,
         String const& _l,
         String const& _r,
         typename G::TerminalID _a,
         uint32_t _production)
        : A(_A), l(_l), r(_r), a(_a), production(_production) {}
};

template <class G>
using SetOfItems = std::set<item<G>>;

template <class G, class T>
struct LRParser : Parser<T, typename G::Token> {
    LRParser();

    T parse(TokenStream<typename G::Token>& w) override;

  private:
    virtual T reduce(typename G::NonterminalID A, uint32_t production, T* alpha, size_t n) = 0;
    virtual T shift(typename G::Token&& t) = 0;

    using state = uint32_t;
    std::vector<SetOfItems<G>> items;
    // std::vector<std::unordered_map<typename G::NonterminalID, state>> goto_table;
    // std::vector<std::unordered_map<typename G::TerminalID, action<G>>> action_table;
    std::vector<std::vector<state>> goto_table;
    std::vector<std::vector<action<G>>> action_table;

  protected:
    ~LRParser() = default;
};

template <class G>
bool operator<(const item<G>& lhs, const item<G>& rhs) {
    //  print(std::cout, lhs.G, lhs);
    //  std::cout << " < ";
    //  print(std::cout, rhs.G, rhs);
    //  std::cout.flush();
    bool result;
    if (lhs.A < rhs.A)
        result = true;
    else if (lhs.A > rhs.A)
        result = false;
    else if (lhs.l < rhs.l)
        result = true;
    else if (lhs.l > rhs.l)
        result = false;
    else if (lhs.r < rhs.r)
        result = true;
    else if (lhs.r > rhs.r)
        result = false;
    else if (lhs.a < rhs.a)
        result = true;
    else
        result = false;
    //  std::cout << " = " << std::boolalpha << result << std::endl;
    return result;
}

template <class G>
std::ostream& operator<<(std::ostream& s, const action<G>& a) {
    switch (a.getType()) {
    case type::ACCEPT:
        s << "acc";
        break;
    case type::FAIL:
        s << "fail";
        break;
    case type::REDUCE:
        s << "r(" << a.getHead() << " -> " << G::getProductions(a.getHead())[a.getProduction()] << ")";
        break;
    case type::SHIFT:
        s << "s" << a.getState();
        break;
    default:
        throw std::logic_error("unhandled action type");
    }
    return s;
}

template <class G>
std::ostream& operator<<(std::ostream& s, const item<G>& i) {
    return s << "[" << i.A << " -> " << i.l << "\u00B7  " << i.r << " " << i.a << "]";
}

template <class G>
std::ostream& operator<<(std::ostream& s, const std::set<item<G>>& I) {
    for (const auto& i : I) {
        s << i << std::endl;
    }
    return s;
}

template <class G>
std::ostream& operator<<(std::ostream& s, const std::set<std::set<item<G>>>& C) {
    auto i = 0u;
    for (const auto& I : C) {
        s << "Set" << i++ << ":" << std::endl;
        s << I << std::endl;
    }
    return s;
}

template <class G>
std::ostream& operator<<(std::ostream& s, const std::vector<std::set<item<G>>>& C) {
    auto i = 0u;
    for (const auto& I : C) {
        s << "Set " << i++ << ":" << std::endl;
        s << I << std::endl;
    }
    return s;
}

template <class G>
SetOfItems<G> closure(SetOfItems<G>&& I) {
    bool changed;
    do {
        changed = false;
        for (const auto& i : I) {
            if (i.r.empty() || G::kindOf(i.r.front()) != kind::NONTERMINAL)
                continue;
            //      std::cout << "Considering item";
            //      print(std::cout, G, i);
            const auto& B = typename G::NonterminalID(i.r.front());
            //      std::cout << "B = " << NT(G.idOfNonterminal(B)) << std::endl;
            auto num_productions = G::getProductions(B).size();
            for (auto k = 0u; k < num_productions; ++k) {
                const auto& gamma = G::getProductions(B)[k];
                //        std::cout << "gamma = ";
                //        print(std::cout, G, gamma);
                //        std::cout << std::endl;
                auto betaStart = i.r.begin();
                if (betaStart != i.r.end())
                    std::advance(betaStart, 1);
                typename G::String rest(betaStart, i.r.end());
                rest.push_back(typename G::GrammarElement(i.a));
                // std::cout << "rest = ";
                // std::cout << rest << std::endl;
                auto firstRest = first<G>(rest);
                // std::cout << "first(rest) = ";
                // std::cout << firstRest << std::endl;
                for (auto b : firstRest) {
                    //          typename G::TerminalID b(_b);
                    //          auto it = firstRest.find(b);
                    //          if (![b])
                    //            continue;
                    //          std::cout << "b = " << T(b) << std::endl;
                    auto res =
                        I.emplace(B,
                                  typename item<G>::String{},
                                  typename item<G>::String{
                                      gamma.front() != typename G::GrammarElement(G::eps) ? gamma.begin() : gamma.end(),
                                      gamma.end()},
                                  b,
                                  k);

                    //          if (res.second) {
                    //            std::cout << "Adding ";
                    //            print(std::cout, G, *res.first);
                    //          }

                    changed |= res.second;
                }
            }
        }
    } while (changed);
    //  std::cout << "After closure: ";
    //  print(std::cout, G, I);
    //  std::cout << std::endl;
    return move(I);
}

template <class G>
SetOfItems<G> GOTO(const SetOfItems<G>& I, typename G::GrammarElement X) {
    //  std::cout << "In GOTO" << std::endl;
    //  print(std::cout, G, I);
    //  print(std::cout, G, X);
    //  std::cout << std::endl;
    SetOfItems<G> J;
    for (const auto& i : I) {
        //    std::cout << "considering item ";
        //    print(std::cout, G, i);
        //    std::cout << std::endl;
        if (i.r.empty() || i.r.front() != X) {
            //      std::cout << "not relevant" << std::endl;
            continue;
        }
        auto l = i.l;
        l.push_back(X);
        auto r = i.r;
        r.pop_front();
        //    item j(G, i.A, l, r, i.a);
        //    std::cout << "adding item ";
        //    print(std::cout, G, j);
        //    std::cout << std::endl;
        J.emplace(i.A, move(l), move(r), i.a, i.production);
    }
    //  std::cout << "Before closure:" << std::endl;
    //  print(std::cout, G, J);
    return closure(move(J));
}

template <class G, class T>
LRParser<G, T>::LRParser() {
    SetOfItems<G> init{
        item<G>(G::start,
                typename item<G>::String(),
                typename item<G>::String{G::getProductions(G::start)[0].begin(), G::getProductions(G::start)[0].end()},
                G::eof,
                0)};
    std::cout << "Starting with " << init << std::endl;
    auto C = std::vector<SetOfItems<G>>{closure(std::move(init))};
    std::map<SetOfItems<G>, state> ids{{C[0], 0}};
    std::map<state, std::map<typename G::NonterminalID, state>> _goto_table;
    std::map<state, std::map<typename G::TerminalID, action<G>>> _action_table;
    std::vector<uint32_t> stack;
    stack.push_back(0);

    while (!stack.empty()) {
        auto i = stack.back();
        stack.pop_back();
        auto I = C[i]; // copying here to avoid invalid reference when C changes
        std::cout << "Considering item set " << i << std::endl;
        std::cout << I;
        for (auto x = 0u; x < G::getNumberOfGrammarElements(); ++x) {
            typename G::GrammarElement X(x);
            if (G::kindOf(X) == kind::TERMINAL || G::kindOf(X) == kind::EOI) {
                for (const auto& item : I) {
                    if (item.r.empty() && item.a == typename G::TerminalID(X)) {
                        action<G> a(type::FAIL);
                        if (item.A == G::start)
                            a = action<G>(type::ACCEPT);
                        else
                            a = action<G>(type::REDUCE, item.A, item.production);
                        auto currentAction = _action_table[i].find(typename G::TerminalID(X));
                        auto gotConflict = currentAction != _action_table[i].end();
                        if (!gotConflict) {
                            std::cout << "adding action " << X << " -> " << a << std::endl;
                            _action_table[i].insert({typename G::TerminalID(X), a});
                        } else {
                            std::cout << "conflict: ACTION(" << i << ", " << X
                                      << "): " << _action_table[i].find(typename G::TerminalID(X))->second << " or "
                                      << a << std::endl;
                            std::cout << "item set:" << std::endl;
                            std::cout << I;
                            if (a.getType() == type::REDUCE && currentAction->second.getType() == type::SHIFT) {
                                std::cout << "preferring shift" << std::endl;
                            } else {
                                throw std::runtime_error("conflict");
                            }
                        }
                    }
                }
            }

            std::cout << "GOTO ";
            std::cout << X;
            std::cout << ":" << std::endl;

            auto J = GOTO(I, X);
            if (!J.empty()) {
                std::cout << J;
                auto it = ids.find(J);
                uint32_t j;
                if (it == ids.end()) {

                    auto id = C.size();
                    auto result = ids.insert({J, id});
                    assert(result.second);
                    C.emplace_back(J);
                    j = result.first->second;
                    stack.push_back(j);
                    std::cout << "new item set: " << j << std::endl;
                } else {
                    j = it->second;
                    std::cout << "saw it before: " << j << std::endl;
                }

                //        auto& actions_i = _action_table[i];
                //        auto& goto_i = _goto_table[i];

                if (G::kindOf(X) == kind::NONTERMINAL) {
                    std::cout << "adding goto " << X << " -> " << j << std::endl;
                    _goto_table[i][typename G::NonterminalID(X)] = j;
                } else if (G::kindOf(X) == kind::TERMINAL || G::kindOf(X) == kind::EOI) {
                    auto a = action<G>(type::SHIFT, j);
                    auto currentAction = _action_table[i].find(X);
                    auto gotConflict = currentAction != _action_table[i].end();
                    if (gotConflict) {
                        std::cout << "conflict: ACTION(" << i << ", " << X << "): " << _action_table[i].find(X)->second
                                  << " or " << a << std::endl;
                        std::cout << "item set:" << std::endl;
                        std::cout << I;
                        if (currentAction->second.getType() == type::REDUCE) {
                            std::cout << "preferring shift" << std::endl;
                            std::cout << "adding action " << X << " -> " << a << std::endl;
                            _action_table[i].insert({X, a});
                        } else {
                            throw std::runtime_error("conflict");
                        }
                    } else {
                        std::cout << "adding action " << X << " -> " << a << std::endl;
                        _action_table[i].insert({X, a});
                    }
                } else
                    throw std::logic_error("eps occured");
            } else {
                std::cout << "empty" << std::endl;
            }
        }
    }

    std::cout << C;

    // goto_table.resize(C.size(), std::unordered_map<typename G::NonterminalID, state>(G::getNumberOfNonterminals()));

    // for (const auto& goto_i : _goto_table) {
    //    auto i = goto_i.first;
    //    for (auto it : goto_i.second) {
    //        auto A = it.first;
    //        auto j = it.second;
    //        goto_table[i][A] = j;
    //    }
    //}

    // action_table.resize(C.size(), std::unordered_map<typename G::TerminalID, action<G>>(G::getNumberOfTerminals()));
    // for (const auto& action_i : _action_table) {
    //    auto i = action_i.first;
    //    for (auto it : action_i.second) {
    //        auto a = it.first;
    //        auto j = it.second;
    //        action_table[i].insert({a, j});
    //    }
    //}
    goto_table.resize(C.size(), std::vector<state>(G::getNumberOfNonterminals()));

    for (const auto& goto_i : _goto_table) {
        auto i = goto_i.first;
        for (auto it : goto_i.second) {
            auto A = it.first;
            auto j = it.second;
            goto_table[i][A.x] = j;
        }
    }

    action_table.resize(C.size(), std::vector<action<G>>(G::getNumberOfTerminals()));

    for (const auto& action_i : _action_table) {
        auto i = action_i.first;
        for (auto it : action_i.second) {
            auto a = it.first;
            auto j = it.second;
            action_table[i][a.x] = j;
        }
    }

    items = move(C);
}

template <class G, class T>
T LRParser<G, T>::parse(TokenStream<typename G::Token>& a) {
    std::vector<state> stack{0};
    std::vector<T> attributes{T()}; // Do we need this initialization?
    do {
        auto s = stack.back();
        std::cout << "state " << s << std::endl;
        auto tag = G::getTag(a.peek());
        // auto it = action_table[s].find(tag);
        // if (it == action_table[s].end()) {
        //    std::stringstream ss;
        //    ss << "parse: invalid tag '" << tag << "' in state\n";
        //    ss << items[s];
        //    ss << "action table:\n";
        //    for (const auto& entry : action_table[s])
        //        ss << entry.first << " -> " << entry.second << "\n";
        //    throw std::runtime_error(ss.str());
        //}
        // const action<G>& _action = it->second;
        const auto& _action = action_table[s][tag.x];
        if (_action.getType() == type::SHIFT) {
            stack.push_back(_action.getState());
            attributes.push_back(shift(std::move(a.peek())));
            std::cout << "Shift " << typename G::GrammarElement(tag) << std::endl;
            a.step();
        } else if (_action.getType() == type::REDUCE) {
            auto p = _action.getProduction();
            auto head = _action.getHead();
            const auto& production = G::getProductions(head)[p];
            ptrdiff_t length;
            if (production.at(0) == typename G::GrammarElement(G::eps))
                length = 0;
            else {
                assert(production.size() <= std::numeric_limits<ptrdiff_t>::max());
                length = static_cast<ptrdiff_t>(production.size());
            }
            assert(stack.size() > length);
            auto synthesized_attribute = reduce(head, p, &*(attributes.end() - length), length);
            attributes.erase(attributes.end() - length, attributes.end());
            attributes.push_back(std::move(synthesized_attribute));
            stack.erase(stack.end() - length, stack.end());
            // auto goto_it = goto_table[stack.back()].find(head);
            // if (goto_it == goto_table[stack.back()].end()) {
            //    std::stringstream ss;
            //    ss << "parse: invalid NT '" << head << "' in state " << stack.back() << "\n";
            //    ss << items[stack.back()];
            //    ss << "goto table:\n";
            //    for (const auto& entry : goto_table[stack.back()])
            //        ss << entry.first << " -> " << entry.second << "\n";
            //    throw std::runtime_error(ss.str());
            //}
            // auto t = goto_it->second;
            auto t = goto_table[stack.back()][head.x];
            assert(t < goto_table.size());
            stack.push_back(t);
            std::cout << "Reduce " << production << " to " << typename G::GrammarElement(_action.getHead())
                      << std::endl;
        } else if (_action.getType() == type::ACCEPT) {
            std::cout << "Accept" << std::endl;
            return attributes.back();
        } else {
            std::cout << "Fail." << std::endl;
            return T();
        }
        // Could the stack be empty? 4 cases. SHIFT and REDUCE add elements (after removing a valid number of elements
        // in one case) ACCEPT and FAIL return. So the stack can't be empty.
    } while (true);
}
} // namespace lr_parser
