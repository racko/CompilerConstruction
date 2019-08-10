#pragma once

#include <variant.h>                  // for null
#include <boost/variant/variant.hpp>  // for variant
#include <iosfwd>                     // for ostream
#include <string_view>                // for string_view
#include <utility>                    // for pair, pair<>::pair
#include <num_view.h>

namespace json {
struct value;

// linked list. first pointer: payload, second pointer: next list node (array)
using array_ = std::pair<value*, value*>;
struct array : public array_ {
    using array_::array_;
};

// linked list. first pointer: array of size two - first entry is key, second is value. second pointer: next list node
// (object)
using object_ = std::pair<array*, value*>;
struct object : public object_ {
    using object_::object_;
};

using value_ = variant<null, bool, num_view, std::string_view, array, object>;
struct value : public value_ {
    using value_::value_;
};

bool operator==(const value& lhs, const value& rhs);

std::ostream& operator<<(std::ostream& s, const value& v);
} // namespace json
