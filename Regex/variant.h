#pragma once

// std::variant fails with clang as of 5.0:
// https://stackoverflow.com/questions/46506387/getstring-for-variants-fail-under-clang-but-not-g

// #include <variant>
// template<typename... T>
// using variant = std::variant<T...>;
// using null = std::monostate;
// using std::get;

#include <boost/blank.hpp> // for blank
#include <boost/variant.hpp>
#include <boost/variant/get.hpp> // for get

template <typename... T>
using variant = boost::variant<T...>;
using null = boost::blank;
using boost::get;
