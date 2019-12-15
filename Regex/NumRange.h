#pragma once
#include <cstddef>
#include <iterator>

template <typename T>
struct NumIterator {
    using difference_type = std::ptrdiff_t;
    using value_type = const T;
    using pointer = const T*;
    using reference = T;
    using iterator_category = std::random_access_iterator_tag;

    T x_;

    constexpr NumIterator() = default;
    constexpr NumIterator(T x) : x_(x) {}

    constexpr T operator*() { return x_; }
    constexpr T operator*() const { return x_; }

    constexpr NumIterator& operator++() {
        ++x_;
        return *this;
    }
    constexpr NumIterator operator++(int) {
        NumIterator tmp(*this);
        ++x_;
        return tmp;
    }

    constexpr NumIterator& operator--() {
        --x_;
        return *this;
    }
    constexpr NumIterator operator--(int) {
        NumIterator tmp(*this);
        --x_;
        return tmp;
    }

    constexpr bool operator==(const NumIterator& other) const { return x_ == other.x_; }
    constexpr bool operator!=(const NumIterator& other) const { return x_ != other.x_; }

    constexpr difference_type operator-(const NumIterator& other) const { return x_ - other.x_; }
};

template <typename T>
struct NumRange {
    using iterator = NumIterator<T>;
    using const_iterator = NumIterator<T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    T a_, b_;
    constexpr NumRange(T a, T b) : a_(a), b_(b) {}

    constexpr const_iterator begin() const { return a_; }
    constexpr const_iterator cbegin() const { return a_; }

    constexpr const_iterator end() const { return b_; }
    constexpr const_iterator cend() const { return b_; }

    constexpr const_reverse_iterator rbegin() const { return std::make_reverse_iterator(const_iterator{b_}); }
    constexpr const_reverse_iterator crbegin() const { return std::make_reverse_iterator(const_iterator{b_}); }

    constexpr const_reverse_iterator rend() const { return std::make_reverse_iterator(const_iterator{a_}); }
    constexpr const_reverse_iterator crend() const { return std::make_reverse_iterator(const_iterator{a_}); }
};
