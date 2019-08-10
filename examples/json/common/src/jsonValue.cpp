#include "jsonValue.h"

#include "num_view.h"                                   // for num_view
#include "variant.h"                                    // for null
#include <boost/variant/detail/apply_visitor_unary.hpp> // for apply_visitor
#include <boost/variant/get.hpp>                        // for get
#include <boost/variant/static_visitor.hpp>             // for static_visitor
#include <ostream>                                      // for operator<<

namespace json {
namespace {
struct BlankEqual : public boost::static_visitor<bool> {
    bool operator()(null) const { return true; }
    bool operator()(bool) const { return false; }
    bool operator()(const num_view&) const { return false; }
    bool operator()(const std::string_view&) const { return false; }
    bool operator()(const array&) const { return false; }
    bool operator()(const object&) const { return false; }
};

struct BoolEqual : public boost::static_visitor<bool> {
    bool lhs_;
    BoolEqual(bool lhs) : lhs_(lhs) {}
    bool operator()(null) const { return false; }
    bool operator()(bool b) const { return b == lhs_; }
    bool operator()(const num_view&) const { return false; }
    bool operator()(const std::string_view&) const { return false; }
    bool operator()(const array&) const { return false; }
    bool operator()(const object&) const { return false; }
};

struct NumEqual : public boost::static_visitor<bool> {
    const num_view& lhs_;
    NumEqual(const num_view& lhs) : lhs_(lhs) {}
    bool operator()(null) const { return false; }
    bool operator()(bool) const { return false; }
    bool operator()(const num_view& n) const { return n == lhs_; }
    bool operator()(const std::string_view&) const { return false; }
    bool operator()(const array&) const { return false; }
    bool operator()(const object&) const { return false; }
};

struct StringEqual : public boost::static_visitor<bool> {
    const std::string_view& lhs_;
    StringEqual(const std::string_view& lhs) : lhs_(lhs) {}
    bool operator()(null) const { return false; }
    bool operator()(bool) const { return false; }
    bool operator()(const num_view&) const { return false; }
    bool operator()(const std::string_view& s) const { return s == lhs_; }
    bool operator()(const array&) const { return false; }
    bool operator()(const object&) const { return false; }
};

struct ArrayEqual : public boost::static_visitor<bool> {
    const array& lhs_;
    ArrayEqual(const array& lhs) : lhs_(lhs) {}
    bool operator()(null) const { return false; }
    bool operator()(bool) const { return false; }
    bool operator()(const num_view&) const { return false; }
    bool operator()(const std::string_view&) const { return false; }
    bool operator()(const array& rhs) const;
    bool operator()(const object&) const { return false; }
};

struct ObjectEqual : public boost::static_visitor<bool> {
    const object& lhs_;
    ObjectEqual(const object& lhs) : lhs_(lhs) {}
    bool operator()(null) const { return false; }
    bool operator()(bool) const { return false; }
    bool operator()(const num_view&) const { return false; }
    bool operator()(const std::string_view&) const { return false; }
    bool operator()(const array&) const { return false; }
    bool operator()(const object& o) const;
};

struct JsonEqual : public boost::static_visitor<bool> {
    const value& lhs_;
    JsonEqual(const value& lhs) : lhs_(lhs) {}
    bool equals(const value& rhs) const { return boost::apply_visitor(*this, rhs); }
    bool operator()(null) const { return boost::apply_visitor(BlankEqual(), lhs_); }
    bool operator()(bool b) const { return boost::apply_visitor(BoolEqual(b), lhs_); }
    bool operator()(const num_view& n) const { return boost::apply_visitor(NumEqual(n), lhs_); }
    bool operator()(const std::string_view& s) const { return boost::apply_visitor(StringEqual(s), lhs_); }
    bool operator()(const array& a) const { return boost::apply_visitor(ArrayEqual(a), lhs_); }
    bool operator()(const object& o) const { return boost::apply_visitor(ObjectEqual(o), lhs_); }
};

bool ArrayEqual::operator()(const array& rhs) const {
    // both empty
    if (rhs.first == nullptr && lhs_.first == nullptr)
        return true;

    // one empty, one isn't
    if (rhs.first == nullptr || lhs_.first == nullptr)
        return false;

    // both not empty

    bool first_equal = boost::apply_visitor(JsonEqual(*lhs_.first), *rhs.first);
    if (!first_equal)
        return false;

    auto lhs_it = lhs_.second;
    auto rhs_it = rhs.second;

    while (lhs_it != nullptr && rhs_it != nullptr) {
        auto& lhs_a = boost::get<array>(*lhs_it);
        auto& rhs_a = boost::get<array>(*rhs_it);
        bool next_equal = boost::apply_visitor(JsonEqual(*lhs_a.first), *rhs_a.first);
        if (!next_equal)
            return false;
        lhs_it = lhs_a.second;
        rhs_it = rhs_a.second;
    }

    // reached end of list
    if (lhs_it == nullptr && rhs_it == nullptr)
        return true;

    // one at end, one isn't
    return false;
}

/// @TODO: consider objects to be "sets of members".
///        I.e. {"a": 0, "b": 1} == {"b": 1, "a": 0}
///        Otherwise we have to "canonicalize" the member order during construction ... slowing down parsing ...
bool ObjectEqual::operator()(const object&) const {
    return false;
    // s_ << "{";
    // if (o.first != nullptr) {
    //    boost::apply_visitor(*this, *o.first->first);
    //    s_ << ":";
    //    boost::apply_visitor(*this, *o.first->second);
    //    auto it = o.second;
    //    while (it != nullptr) {
    //        auto& o_ = boost::get<object>(*it);
    //        s_ << ",";
    //        boost::apply_visitor(*this, *o_.first->first);
    //        s_ << ":";
    //        boost::apply_visitor(*this, *o_.first->second);
    //        it = o_.second;
    //    }
    //}
    // s_ << "}";
}

struct Printer : public boost::static_visitor<> {
    std::ostream& s_;
    Printer(std::ostream& str) : s_(str) {}
    void print(const value& x) const { boost::apply_visitor(*this, x); }
    void operator()(null) const { s_ << "null"; }
    void operator()(bool b) const { s_ << std::boolalpha << b << std::noboolalpha; }
    void operator()(const num_view& n) const { s_ << n.x; }
    void operator()(const std::string_view& ss) const { s_ << '"' << ss << '"'; }
    void operator()(const array& a) const {
        s_ << '[';
        if (a.first != nullptr) {
            print(*a.first);
            auto it = a.second;
            while (it != nullptr) {
                auto& a_ = boost::get<array>(*it);
                s_ << ',';
                print(*a_.first);
                it = a_.second;
            }
        }
        s_ << ']';
    }
    void operator()(const object& o) const {
        s_ << '{';
        if (o.first != nullptr) {
            print(*o.first->first);
            s_ << ':';
            print(*o.first->second);
            auto it = o.second;
            while (it != nullptr) {
                auto& o_ = boost::get<object>(*it);
                s_ << ',';
                print(*o_.first->first);
                s_ << ':';
                print(*o_.first->second);
                it = o_.second;
            }
        }
        s_ << '}';
    }
};
} // namespace

bool operator==(const value& lhs, const value& rhs) { return JsonEqual(lhs).equals(rhs); }

std::ostream& operator<<(std::ostream& s, const value& v) {
    Printer(s).print(v);
    return s;
}
} // namespace json
