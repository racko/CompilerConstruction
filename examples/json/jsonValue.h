#pragma once
#include "jsonGrammar.h"

#include <boost/variant.hpp>
#include <boost/utility/string_view.hpp>
#include <iosfwd>
#include <utility>

struct value;

// linked list. first pointer: payload, second pointer: next list node (array)
using array_ = std::pair<value*,value*>;
struct array : public array_ {
    using array_::array_;
};

// linked list. first pointer: array of size two - first entry is key, second is value. second pointer: next list node (object)
using object_ = std::pair<array*,value*>;
struct object : public object_ {
    using object_::object_;
};

using null = boost::blank;

using value_ = boost::variant<null, bool, json::num_view, boost::string_view, array, object>;
struct value : public value_ {
    using value_::value_;
};

bool operator==(const value& lhs, const value& rhs);

std::ostream& operator<<(std::ostream& s, const value& v);

