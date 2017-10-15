#pragma once
#include <cstddef>
#include <iterator>

template<typename T>
struct NumIterator {
    using difference_type = std::ptrdiff_t;
    using value_type = const T;
    using pointer = const T*;
    using reference = const T&;
    using iterator_category = std::random_access_iterator_tag;

    T x_;

    constexpr NumIterator() = default;
    constexpr NumIterator(T x) : x_(x) {}

    constexpr T operator*() { return x_; }
    constexpr NumIterator& operator++() { ++x_; return *this; }
    constexpr NumIterator operator++(int) { NumIterator tmp(*this); ++x_; return tmp; }
    constexpr bool operator!=(const NumIterator& other) { return x_ != other.x_; }
    constexpr difference_type operator-(const NumIterator& other) { return x_ - other.x_; }
};

template<typename T>
struct NumRange {
    T a_, b_;
    constexpr NumRange(T a, T b) : a_(a), b_(b) {}

    constexpr NumIterator<T> begin() const { return a_; }
    constexpr NumIterator<T> end() const { return b_; }
};
