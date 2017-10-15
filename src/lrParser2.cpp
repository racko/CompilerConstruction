#include "lrParser2.h"
#include <list>
#include <set>
#include <iostream>
#include <map>

namespace {

struct item {
    using String = std::list<grammar2::GrammarElement>;
    grammar2::NonterminalID A;
    String l;
    String r;
    grammar2::TerminalID a;
    uint32_t production;

    item(grammar2::NonterminalID _A, String const& _l, String const& _r, grammar2::TerminalID _a, uint32_t _production);
};

bool operator<(const item& lhs, const item& rhs) {
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

using SetOfItems = std::set<item>;


void print(std::ostream& s, const grammar2::Grammar& grammar, const action& a) {
    switch(a.getType()) {
    case type::ACCEPT:
        s << "acc";
        break;
    case type::FAIL:
        s << "fail";
        break;
    case type::REDUCE:
        s << "r(" << grammar.toString(a.getHead()) << " -> ";
        print(s, grammar, grammar.getProductions(a.getHead())[a.getProduction()]);
        s << ")";
        break;
    case type::SHIFT:
        s << "s" << a.getState();
        break;
    default:
        throw std::logic_error("unhandled action type");
    }
}

void print(std::ostream& s, const grammar2::Grammar& grammar, const item& i) {
    s << "[" << grammar.toString(i.A) << " -> ";
    print(s, grammar, i.l);
    s << "\u00B7  ";
    print(s, grammar, i.r);
    s << " " << grammar.toString(i.a) << "]";
}

void print(std::ostream& s, const grammar2::Grammar& grammar, const SetOfItems& I) {
    for (const auto& i : I) {
        print(s, grammar, i);
        s << std::endl;
    }
}

void print(std::ostream& s, const grammar2::Grammar& grammar, const std::vector<SetOfItems>& C) {
    auto i = 0u;
    for (const auto& I : C) {
        s << "Set " << i++ << ":" << std::endl;
        print(s, grammar, I);
        s << std::endl;
    }
}

SetOfItems closure(const grammar2::Grammar& grammar, SetOfItems&& I) {
    bool changed;
    do {
        changed = false;
        for (const auto& i : I) {
            if (i.r.empty() || grammar.kindOf(i.r.front()) != grammar2::kind::NONTERMINAL)
                continue;
//      std::cout << "Considering item";
//      print(std::cout, G, i);
            const auto& B = grammar.toNonterminalID(i.r.front());
//      std::cout << "B = " << NT(G.idOfNonterminal(B)) << std::endl;
            auto num_productions = grammar.getProductions(B).size();
            for (auto k = 0u; k < num_productions; ++k) {
                const auto& gamma = grammar.getProductions(B)[k];
//        std::cout << "gamma = ";
//        print(std::cout, G, gamma);
//        std::cout << std::endl;
                auto betaStart = i.r.begin();
                if (betaStart != i.r.end())
                    std::advance(betaStart, 1);
                typename grammar2::Grammar::String rest(betaStart, i.r.end());
                rest.push_back(grammar.toGrammarElement((i.a)));
        //std::cout << "rest = ";
        //std::cout << rest << std::endl;
                auto firstRest = first(grammar, rest);
        //std::cout << "first(rest) = ";
        //std::cout << firstRest << std::endl;
                for (auto b : firstRest) {
//          grammar2::TerminalID<G> b(_b);
//          auto it = firstRest.find(b);
//          if (![b])
//            continue;
//          std::cout << "b = " << T(b) << std::endl;
                    auto res = I.emplace(B, item::String{}, item::String{gamma.front() != grammar.toGrammarElement(grammar.getEps()) ? gamma.begin() : gamma.end(), gamma.end()}, b, k);

//          if (res.second) {
//            std::cout << "Adding ";
//            print(std::cout, G, *res.first);
//          }

                    changed |= res.second;
                }
            }
        }
    } while(changed);
//  std::cout << "After closure: ";
//  print(std::cout, G, I);
//  std::cout << std::endl;
    return std::move(I);
}

SetOfItems GOTO(const grammar2::Grammar& grammar, const SetOfItems& I, grammar2::GrammarElement X) {
//  std::cout << "In GOTO" << std::endl;
//  print(std::cout, G, I);
//  print(std::cout, G, X);
//  std::cout << std::endl;
    SetOfItems J;
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
        J.emplace(i.A, std::move(l), std::move(r), i.a, i.production);

    }
//  std::cout << "Before closure:" << std::endl;
//  print(std::cout, G, J);
    return closure(grammar, std::move(J));
}
}

item::item(grammar2::NonterminalID _A, String const& _l, String const& _r, grammar2::TerminalID _a, uint32_t _production) : A(_A), l(_l), r(_r), a(_a), production(_production) {}

struct LRParser::impl {
    impl(const grammar2::Grammar& grammar);

    action step(grammar2::TerminalID);

    using state = uint32_t;
    grammar2::Grammar grammar_;
    std::vector<SetOfItems> items; //only stored for debugging purposes
    std::vector<std::vector<state>> goto_table;
    std::vector<std::vector<action>> action_table;

    std::vector<state> stack_{{0}};
};

LRParser::impl::impl(const grammar2::Grammar& grammar) : grammar_(grammar) {
    SetOfItems init{item(grammar.getStart(), item::String(), item::String{grammar.getProductions(grammar.getStart())[0].begin(), grammar.getProductions(grammar.getStart())[0].end()}, grammar.getEoi(), 0)};
    std::cout << "Starting with ";
    print(std::cout, grammar, init);
    std::cout << std::endl;
    auto C = std::vector<SetOfItems>{closure(grammar, std::move(init))};
    std::map<SetOfItems, state> ids{{C[0], 0}};
    std::map<state,std::map<grammar2::NonterminalID,state>> _goto_table;
    std::map<state,std::map<grammar2::TerminalID,action>> _action_table;
    std::vector<uint32_t> stack;
    stack.push_back(0);

    while(!stack.empty()) {
        auto i = stack.back();
        stack.pop_back();
        auto I = C[i]; // copying here to avoid invalid reference when C changes
        std::cout << "Considering item set " << i << std::endl;
        print(std::cout, grammar, I);
        for (auto x = 0u; x < grammar.getNumberOfGrammarElements(); ++x) {
            grammar2::GrammarElement X(x);
            if (grammar.kindOf(X) == grammar2::kind::TERMINAL || grammar.kindOf(X) == grammar2::kind::EOI) {
                for (const auto& item : I) {
                    if (item.r.empty() && item.a == grammar.toTerminalID(X)) {
                        action a(type::FAIL);
                        if (item.A == grammar.getStart())
                            a = action(type::ACCEPT);
                        else
                            a = action(type::REDUCE, item.A, item.production);
                        auto currentAction = _action_table[i].find(grammar.toTerminalID(X));
                        auto gotConflict = currentAction != _action_table[i].end();
                        if (!gotConflict) {
                            std::cout << "adding action " << grammar.toString(X) << " -> ";
                            print(std::cout, grammar, a);
                            std::cout << std::endl;
                            _action_table[i].insert({grammar.toTerminalID(X), a});
                        }else {
                            std::cout << "conflict: ACTION(" << i << ", " << grammar.toString(X) << "): ";
                            print(std::cout, grammar, _action_table[i].find(grammar.toTerminalID(X))->second);
                            std::cout << " or ";
                            print(std::cout, grammar, a);
                            std::cout << std::endl;
                            std::cout << "item set:" << std::endl;
                            print(std::cout, grammar, I);
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
            std::cout << grammar.toString(X);
            std::cout << ":" << std::endl;

            auto J = GOTO(grammar, I, X);
            if (!J.empty()) {
                print(std::cout, grammar, J);
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

                if (grammar.kindOf(X) == grammar2::kind::NONTERMINAL) {
                    std::cout << "adding goto " << grammar.toString(X) << " -> " << j << std::endl;
                    _goto_table[i][grammar.toNonterminalID(X)] = j;
                } else if (grammar.kindOf(X) == grammar2::kind::TERMINAL || grammar.kindOf(X) == grammar2::kind::EOI) {
                    auto a = action(type::SHIFT, j);
                    auto term = grammar.toTerminalID(X);
                    auto currentAction = _action_table[i].find(term);
                    auto gotConflict = currentAction != _action_table[i].end();
                    if (gotConflict) {
                        std::cout << "conflict: ACTION(" << i << ", " << grammar.toString(X) << "): ";
                        print(std::cout, grammar, currentAction->second);
                        std::cout << " or ";
                        print(std::cout, grammar, a);
                        std::cout << std::endl;
                        std::cout << "item set:" << std::endl;
                        print(std::cout, grammar, I);
                        if (currentAction->second.getType() == type::REDUCE) {
                            std::cout << "preferring shift" << std::endl;
                            std::cout << "adding action " << grammar.toString(X) << " -> ";
                            print(std::cout, grammar, a);
                            std::cout << std::endl;
                            _action_table[i].insert({term, a});
                        } else {
                            throw std::runtime_error("conflict");
                        }
                    } else {
                        std::cout << "adding action " << grammar.toString(X) << " -> ";
                        print(std::cout, grammar, a);
                        std::cout << std::endl;
                        _action_table[i].insert({term, a});
                    }
                } else
                    throw std::logic_error("eps occured");
            } else {
                std::cout << "empty" << std::endl;
            }
        }
    }

    print(std::cout, grammar, C);

    //goto_table.resize(C.size(), std::unordered_map<grammar2::NonterminalID<G>, state>(G::getNumberOfNonterminals()));

    //for (const auto& goto_i : _goto_table) {
    //    auto i = goto_i.first;
    //    for (auto it : goto_i.second) {
    //        auto A = it.first;
    //        auto j = it.second;
    //        goto_table[i][A] = j;
    //    }
    //}

    //action_table.resize(C.size(), std::unordered_map<grammar2::TerminalID<G>, action<G>>(G::getNumberOfTerminals()));
    //for (const auto& action_i : _action_table) {
    //    auto i = action_i.first;
    //    for (auto it : action_i.second) {
    //        auto a = it.first;
    //        auto j = it.second;
    //        action_table[i].insert({a, j});
    //    }
    //}
    goto_table.resize(C.size(), std::vector<state>(grammar.getNumberOfNonterminals()));

    for (const auto& goto_i : _goto_table) {
        auto i = goto_i.first;
        for (auto it : goto_i.second) {
            auto A = it.first;
            auto j = it.second;
            goto_table[i][A.x] = j;
        }
    }

    action_table.resize(C.size(), std::vector<action>(grammar.getNumberOfTerminals()));

    for (const auto& action_i : _action_table) {
        auto i = action_i.first;
        for (auto it : action_i.second) {
            auto a = it.first;
            auto j = it.second;
            action_table[i][a.x] = j;
        }
    }

    items = std::move(C);
}

//action LRParser_::step(grammar2::TerminalID tag) {
action LRParser::impl::step(grammar2::TerminalID tag) {
    assert(!stack_.empty());
    auto s = stack_.back();
    std::cout << "state " << s << std::endl;
    //auto it = lrparser_.action_table[s].find(tag);
    //if (it == lrparser_.action_table[s].end()) {
    //    std::stringstream ss;
    //    ss << "parse: invalid tag '" << tag << "' in state\n";
    //    ss << items[s];
    //    ss << "action table:\n";
    //    for (const auto& entry : lrparser_.action_table[s])
    //        ss << entry.first << " -> " << entry.second << "\n";
    //    throw std::runtime_error(ss.str());
    //}
    //const action<G>& _action = it->second;
    const auto& _action = action_table[s][tag.x];
    if (_action.getType() == type::SHIFT) {
        stack_.push_back(_action.getState());
        std::cout << "Shift " << grammar_.toString(grammar_.toGrammarElement(tag)) << std::endl;
    } else if (_action.getType() == type::REDUCE) {
        auto p = _action.getProduction();
        auto head = _action.getHead();
        const auto& production = grammar_.getProductions(head)[p];
        ptrdiff_t length;
        if (production.at(0) == grammar_.toGrammarElement(grammar_.getEps()))
            length = 0;
        else {
            assert(production.size() <= std::numeric_limits<ptrdiff_t>::max());
            length = static_cast<ptrdiff_t>(production.size());
        }
        assert(stack_.size() > length);
        stack_.erase(stack_.end() - length, stack_.end());
        //auto goto_it = lrparser_.goto_table[stack_.back()].find(head);
        //if (goto_it == lrparser_.goto_table[stack_.back()].end()) {
        //    std::stringstream ss;
        //    ss << "parse: invalid NT '" << head << "' in state " << stack_.back() << "\n";
        //    ss << items[stack_.back()];
        //    ss << "goto table:\n";
        //    for (const auto& entry : lrparser_.goto_table[stack_.back()])
        //        ss << entry.first << " -> " << entry.second << "\n";
        //    throw std::runtime_error(ss.str());
        //}
        //auto t = goto_it->second;
        auto t = goto_table[stack_.back()][head.x];
        assert(t < goto_table.size());
        stack_.push_back(t);
        std::cout << "Reduce ";
        print(std::cout, grammar_, production);
        std::cout << " to " << grammar_.toString(grammar_.toGrammarElement(_action.getHead())) << std::endl;
    } else if (_action.getType() == type::ACCEPT) {
        std::cout << "Accept" << std::endl;
    } else {
        std::cout << "Fail." << std::endl;
    }
    return _action;
}

LRParser::LRParser(const grammar2::Grammar& grammar) : pimpl(std::make_unique<impl>(grammar)) {}

LRParser::~LRParser() = default;

const grammar2::Grammar& LRParser::grammar() const {
    return pimpl->grammar_;
}

action LRParser::step(grammar2::TerminalID tag) {
    return pimpl->step(tag);
}
