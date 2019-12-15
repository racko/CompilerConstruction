#pragma once

#include "Regex/NumRange.h" // for NumIterator
#include "Regex/Print.h"
#include <cassert>     // for assert
#include <cstddef>     // for size_t
#include <cstdint>     // for int64_t, uint64_t, int32_t, int8_t, uint32_t
#include <functional>  // for hash, function
#include <iterator>    // for begin, end, iterator_traits, distance
#include <limits>      // for numeric_limits, numeric_limits<>::digits
#include <new>         // for bad_alloc
#include <ostream>     // for operator<<, ostream
#include <stdexcept>   // for range_error
#include <tuple>       // for tuple, forward_as_tuple
#include <type_traits> // for is_nothrow_move_assignable, is_nothrow_move_c...
#include <utility>     // for move, forward, index_sequence, make_index_seq...

// this only benefits Debug mode runtime, not constexpr evaluation
#define INLINE //__attribute__((always_inline))

namespace const_expr {
template <class InputIt1, class InputIt2>
constexpr bool equal(InputIt1 first1, const InputIt1 last1, InputIt2 first2) {
    for (; first1 != last1; ++first1, ++first2) {
        if (!(*first1 == *first2)) {
            return false;
        }
    }
    return true;
}

template <class ForwardIt>
constexpr ForwardIt max_element(ForwardIt first, const ForwardIt last) {
    if (first == last) {
        return last;
    }
    ForwardIt largest = first;
    ++first;
    for (; first != last; ++first) {
        if (*largest < *first) {
            largest = first;
        }
    }
    return largest;
}

template <class ForwardIt, class Compare>
constexpr ForwardIt max_element(ForwardIt first, const ForwardIt last, Compare cmp) {
    if (first == last) {
        return last;
    }
    ForwardIt largest = first;
    ++first;
    for (; first != last; ++first) {
        if (cmp(*largest, *first)) {
            largest = first;
        }
    }
    return largest;
}

template <class InputIt, class T, class BinaryOperation>
constexpr T accumulate(InputIt first, const InputIt last, T init, BinaryOperation op) {
    for (; first != last; ++first) {
        init = op(init, *first);
    }
    return init;
}

template <class InputIt, class UnaryPredicate>
constexpr InputIt find_if_not(InputIt first, const InputIt last, UnaryPredicate q) {
    for (; first != last; ++first) {
        if (!q(*first)) {
            return first;
        }
    }
    return last;
}

template <class InputIt, class UnaryPredicate>
constexpr bool all_of(const InputIt first, const InputIt last, UnaryPredicate p) {
    return find_if_not(first, last, p) == last;
}

template <class ForwardIt, class T>
constexpr void fill(ForwardIt first, const ForwardIt last, const T& value) {
    for (; first != last; ++first) {
        *first = value;
    }
}

template <class OutputIt, class Size, class T>
constexpr OutputIt fill_n(OutputIt first, const Size count, const T& value) {
    for (Size i = 0; i < count; i++) {
        *first++ = value;
    }
    return first;
}

template <class ForwardIt, class T>
constexpr void uninitialized_fill(const ForwardIt first, const ForwardIt last, const T& value) {
    // typedef typename std::iterator_traits<ForwardIt>::value_type Value;
    for (auto current = first; current != last; ++current) {
        // ::new (static_cast<void*>(std::addressof(*current))) Value(value); // illegal in constexpr
        *current = value;
    }
    // Removed try/catch: illegal in constexpr
}

template <class ForwardIt, class Size, class T>
constexpr ForwardIt uninitialized_fill_n(const ForwardIt first, Size count, const T& value) {
    // typedef typename std::iterator_traits<ForwardIt>::value_type Value;
    ForwardIt current = first;
    for (; count > 0; ++current, --count) {
        // ::new (static_cast<void*>(std::addressof(*current))) Value(value); // illegal in constexpr
        *current = value;
    }
    return current;
    // Removed try/catch: illegal in constexpr
}

template <class ForwardIt>
constexpr void uninitialized_value_construct(const ForwardIt first, const ForwardIt last) {
    typedef typename std::iterator_traits<ForwardIt>::value_type Value;
    for (ForwardIt current = first; current != last; ++current) {
        // ::new (static_cast<void*>(std::addressof(*current))) Value(); // illegal in constexpr
        *current = Value();
    }
    // Removed try/catch: illegal in constexpr
}

template <class ForwardIt, class Size>
constexpr ForwardIt uninitialized_value_construct_n(const ForwardIt first, Size n) {
    typedef typename std::iterator_traits<ForwardIt>::value_type Value;
    ForwardIt current = first;
    for (; n > 0; ++current, --n) {
        // ::new (static_cast<void*>(std::addressof(*current))) Value(); // illegal in constexpr
        *current = Value();
    }
    return current;
    // Removed try/catch: illegal in constexpr
}

template <class InputIt, class UnaryPredicate>
constexpr InputIt find_if(InputIt first, const InputIt last, UnaryPredicate p) {
    for (; first != last; ++first) {
        if (p(*first)) {
            return first;
        }
    }
    return last;
}

template <class InputIt, class T>
constexpr InputIt find(InputIt first, const InputIt last, const T& value) {
    for (; first != last; ++first) {
        if (*first == value) {
            return first;
        }
    }
    return last;
}

template <class ForwardIt, class T>
constexpr ForwardIt remove(ForwardIt first, const ForwardIt last, const T& value) {
    first = find(first, last, value);
    if (first != last)
        for (ForwardIt i = first; ++i != last;)
            if (!(*i == value))
                *first++ = std::move(*i);
    return first;
}

template <class T>
constexpr void
swap(T& a, T& b) noexcept(std::is_nothrow_move_constructible<T>::value&& std::is_nothrow_move_assignable<T>::value) {
    T c(std::move(a));
    a = std::move(b);
    b = std::move(c);
}

template <class ForwardIt1, class ForwardIt2>
constexpr void iter_swap(const ForwardIt1 a, const ForwardIt2 b) {
    swap(*a, *b);
}

template <class ForwardIt>
constexpr ForwardIt rotate(ForwardIt first, ForwardIt n_first, const ForwardIt last) {
    if (first == n_first)
        return last;
    if (n_first == last)
        return first;

    ForwardIt next = n_first;

    do {
        iter_swap(first++, next++);
        if (first == n_first)
            n_first = next;
    } while (next != last);

    ForwardIt ret = first;

    for (next = n_first; next != last;) {
        iter_swap(first++, next++);
        if (first == n_first)
            n_first = next;
        else if (next == last)
            next = n_first;
    }

    return ret;
}

template <class InputIt, class OutputIt>
constexpr OutputIt copy(InputIt first, const InputIt last, OutputIt d_first) {
    while (first != last) {
        *d_first++ = *first++;
    }
    return d_first;
}

template <class InputIt, class Size, class OutputIt>
constexpr OutputIt copy_n(InputIt first, const Size count, OutputIt result) {
    if (count > 0) {
        *result++ = *first;
        for (Size i = 1; i < count; ++i) {
            *result++ = *++first;
        }
    }
    return result;
}

template <class InputIt, class T>
constexpr typename std::iterator_traits<InputIt>::difference_type
count(InputIt first, const InputIt last, const T& value) {
    typename std::iterator_traits<InputIt>::difference_type ret = 0;
    for (; first != last; ++first) {
        if (*first == value) {
            ret++;
        }
    }
    return ret;
}

template <class OutputIt, class Size, class Generator>
constexpr OutputIt generate_n(OutputIt first, const Size count, Generator g) {
    for (Size i = 0; i < count; i++) {
        *first++ = g();
    }
    return first;
}

template <class ForwardIt, class T>
constexpr ForwardIt lower_bound(ForwardIt first, const ForwardIt last, const T& value) {
    ForwardIt it;
    typename std::iterator_traits<ForwardIt>::difference_type count, step;
    count = std::distance(first, last);

    while (count > 0) {
        it = first;
        step = count / 2;
        std::advance(it, step);
        if (*it < value) {
            first = ++it;
            count -= step + 1;
        } else
            count = step;
    }
    return first;
}

template <class InputIt1, class InputIt2, class OutputIt>
constexpr OutputIt
set_difference(InputIt1 first1, const InputIt1 last1, InputIt2 first2, const InputIt2 last2, OutputIt d_first) {
    while (first1 != last1) {
        if (first2 == last2)
            return copy(first1, last1, d_first);

        if (*first1 < *first2) {
            *d_first++ = *first1++;
        } else {
            if (!(*first2 < *first1)) {
                ++first1;
            }
            ++first2;
        }
    }
    return d_first;
}

template <class InputIt1, class InputIt2, class OutputIt>
constexpr OutputIt
set_intersection(InputIt1 first1, const InputIt1 last1, InputIt2 first2, const InputIt2 last2, OutputIt d_first) {
    while (first1 != last1 && first2 != last2) {
        if (*first1 < *first2) {
            ++first1;
        } else {
            if (!(*first2 < *first1)) {
                *d_first++ = *first1++;
            }
            ++first2;
        }
    }
    return d_first;
}

template <class InputIt1, class InputIt2, class OutputIt>
constexpr OutputIt
set_union(InputIt1 first1, const InputIt1 last1, InputIt2 first2, const InputIt2 last2, OutputIt d_first) {
    for (; first1 != last1; ++d_first) {
        if (first2 == last2)
            return copy(first1, last1, d_first);
        if (*first2 < *first1) {
            *d_first = *first2++;
        } else {
            *d_first = *first1;
            if (!(*first1 < *first2))
                ++first2;
            ++first1;
        }
    }
    return copy(first2, last2, d_first);
}

template <typename First, typename Second>
struct pair {
    First first{};
    Second second{};

    constexpr INLINE pair() = default;

    constexpr INLINE pair(First f, Second s) : first(std::move(f)), second(std::move(s)) {}

    template <class... Args1, class... Args2>
    constexpr INLINE pair(std::piecewise_construct_t, std::tuple<Args1...> first_args, std::tuple<Args2...> second_args)
        : pair(first_args,
               second_args,
               std::make_index_sequence<sizeof...(Args1)>{},
               std::make_index_sequence<sizeof...(Args2)>{}) {}

    template <class... Args1, class... Args2, size_t... Indexes1, size_t... Indexes2>
    constexpr INLINE pair(std::tuple<Args1...>& first_args,
                          std::tuple<Args2...>& second_args,
                          std::index_sequence<Indexes1...>,
                          std::index_sequence<Indexes2...>)
        : first(std::forward<Args1>(std::get<Indexes1>(first_args))...),
          second(std::forward<Args2>(std::get<Indexes2>(second_args))...) {}
};

template <typename First, typename Second>
constexpr auto INLINE make_pair(First f, Second s) {
    return pair<First, Second>{f, s};
}

// I wanted this to hide the stack_allocator's Size to reduce code bloat in the containers
// but without constexpr virtuals this seems hopeless.
template <typename T, typename Impl>
struct allocator {
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;

    constexpr allocator(Impl* pimpl) : pimpl_(pimpl) {}

    constexpr void swap(allocator& rhs) noexcept {
        using std::swap;
        swap(pimpl_, rhs.pimpl_);
    }

    constexpr T* allocate(int64_t n) { return pimpl_->allocate(n); }

    constexpr void deallocate(T* p, int64_t n) noexcept { return pimpl_->deallocate(p, n); }
    Impl* pimpl_;
};

template <typename T, typename Impl>
constexpr void swap(allocator<T, Impl>& lhs, allocator<T, Impl>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename T, int64_t Size>
struct stack_allocator {
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;

    constexpr stack_allocator() = default;

    constexpr stack_allocator(const stack_allocator&) = delete;
    constexpr stack_allocator& operator=(const stack_allocator&) = delete;
    constexpr stack_allocator(stack_allocator&&) = delete;
    constexpr stack_allocator& operator=(stack_allocator&&) = delete;

    constexpr T* allocate(int64_t n) {
        auto new_pos = pos_ + n;
        if (new_pos > data_ + Size)
            throw std::bad_alloc();
        auto p = pos_;
        pos_ = new_pos;
        return p;
    }

    constexpr void deallocate(T* p, int64_t n) noexcept {
        if (pos_ - n == p)
            pos_ = p;
    }

    pointer pos_ = data_;
    T data_[Size]{};
};

// - array::operator= is faster than copy(...) because the constexpr evaluator does it in one step (I guess)
//   instead of evaluating the loop body etc.
// - std::array only offers data() and operator[], but these are too expensive
//   (compared to accessing array::data directly)
template <typename Type, int64_t Size>
struct array {
    Type data[Size];
    constexpr Type& operator[](int64_t i) { return data[i]; }
    constexpr const Type& operator[](int64_t i) const { return data[i]; }
};

template <typename Type, int64_t Size>
struct vector {
    // using storage = Type[Size];
    using storage = array<Type, Size>;
    using value_type = Type;
    using iterator = Type*;
    using const_iterator = const Type*;

    // iterator stop_ = &data_[0];
    iterator stop_{};
    storage data_{};

    // constexpr vector() = default;
    constexpr vector() { stop_ = data_.data; }

    // constexpr vector(int64_t n) : stop_(&data_[n]), data_()
    constexpr vector(int64_t n) {
        stop_ = &data_.data[n];
        // by having to initialize data_ for constexpr-ness, we already value-construct.
        // uninitialized_value_construct_n(data_, n);
    }

    // template<std::size_t... I>
    // constexpr vector(const Type& x, std::index_sequence<I...>) : data_{((void)I, x)...} {}

    // template<typename Iter, std::size_t... I>
    // constexpr vector(Iter it, std::index_sequence<I...>) : data_{((void)I, *it++)...} {}

    // constexpr vector(int64_t n, const Type& x) : stop_(&data_[n]), data_()
    // constexpr vector(int64_t n, const Type& x) : vector(x, std::make_index_sequence<Size>{}) {
    //    stop_ = &data_[n];
    //}
    constexpr vector(int64_t n, const Type& x) {
        stop_ = &data_.data[n];
        uninitialized_fill_n(data_.data, n, x); // TODO: can I somehow do this with data_(...)?
    }

    // constexpr vector(const vector& rhs) : stop_(&data_[rhs.stop_ - &rhs.data_[0]]), data_()
    // constexpr vector(const vector& rhs) : vector(rhs.data_, std::make_index_sequence<Size>{}) {
    constexpr vector(const vector& rhs) : data_(rhs.data_) {
        stop_ = &data_.data[rhs.stop_ - rhs.data_.data];
        // stop_ = copy(rhs.data_, const_cast<const_iterator>(rhs.stop_), data_);
    }

    constexpr vector& operator=(const vector& rhs) {
        stop_ = &data_.data[rhs.stop_ - rhs.data_.data];
        // stop_ = copy(rhs.data_, const_cast<const_iterator>(rhs.stop_), data_);
        copy(rhs.data_.data, const_cast<const_iterator>(rhs.stop_), data_.data);
        // data_ = rhs.data_;
        return *this;
    }

    // we don't gain anything from move semantics because vector's a pod.
    // ... And there's something strange happening in the one place where I explicitly move
    // Delete doesn't help because then I just get a 'use of deleted function error' when I try to explicitly move
    // instead I *want* it to silently fall back to copy semantics

    // constexpr vector(vector&& rhs) {
    //    stop_ = copy(rhs.data_, rhs.stop_, data_);
    //    rhs.stop_ = rhs.data_;
    //}

    // constexpr vector& operator=(vector&& rhs) {
    //    stop_ = copy(rhs.data_, rhs.stop_, data_);
    //    rhs.stop_ = rhs.data_;
    //    return *this;
    //}

    ~vector() = default;

    constexpr Type* INLINE data() noexcept { return data_.data; }
    constexpr const Type* INLINE data() const noexcept { return data_.data; }

    constexpr void INLINE reserve(int64_t) {}
    constexpr void INLINE clear() noexcept { stop_ = data_.data; }

    constexpr void resize(int64_t count) {
        auto new_stop = data_.data + count;
        if (new_stop <= stop_)
            stop_ = new_stop;
        else {
            uninitialized_value_construct(stop_, new_stop);
            stop_ = new_stop;
        }
    }

    constexpr void resize(int64_t count, const Type& value) {
        auto new_stop = data_.data + count;
        if (new_stop <= stop_)
            stop_ = new_stop;
        else {
            uninitialized_fill(stop_, new_stop, value);
            stop_ = new_stop;
        }
    }

    template <typename... Args>
    constexpr Type& INLINE emplace_back(Args... args) {
        // return *new (&data_[size_++]) Type(std::forward<Args>(args)...); /// Not legal in constexpr
        return *stop_++ = Type(std::forward<Args>(args)...);
    }

    constexpr void INLINE push_back(const Type& x) {
        // new (&data_[size_++]) Type(x); /// Not legal in constexpr
        *stop_++ = x;
    }

    constexpr void INLINE push_back(Type&& x) {
        // new (&data_[size_++]) Type(std::move(x)); /// Not legal in constexpr
        *stop_++ = std::move(x);
    }

    constexpr void INLINE pop_back() { --stop_; }

    constexpr const Type& INLINE operator[](const std::int64_t t_pos) const { return data_.data[t_pos]; }

    constexpr Type& INLINE operator[](const std::int64_t t_pos) { return data_.data[t_pos]; }

    constexpr iterator insert(const_iterator pos, Type value) {

        // TODO: actually need to uninitialized_copy and uninitialized_move.
        // The requirements on this vector::insert overload depend on InputIt
        // being a ForwardIterator or not. Not sure why. But being
        // standard-compliant would seemingly be a lot more complicated.

        *stop_ = std::move(value);
        auto non_const_pos = const_cast<iterator>(pos);
        rotate(non_const_pos, stop_, stop_ + 1);
        ++stop_;
        return non_const_pos; // we do not reallocate, so pos remains valid
    }

    template <class InputIt>
    constexpr iterator insert(const const_iterator pos, const InputIt first, const InputIt last) {
        // TODO: actually need to uninitialized_copy and uninitialized_move.
        // The requirements on this vector::insert overload depend on InputIt
        // being a ForwardIterator or not. Not sure why. But being
        // standard-compliant would seemingly be a lot more complicated.

        const auto new_end = copy(first, last, stop_);
        const auto non_const_pos = const_cast<iterator>(pos);
        rotate(non_const_pos, stop_, new_end);
        stop_ = new_end;
        return non_const_pos; // we do not reallocate, so pos remains valid
    }

    constexpr auto erase(const_iterator first, const_iterator last) {
        // trick taken from libstdc++ to deal with the const_iterators not being iter_swappable
        const auto non_const_first = const_cast<iterator>(first);
        const auto new_end = rotate(non_const_first, const_cast<iterator>(last), stop_);
        stop_ = new_end;
        return non_const_first;
    }

    constexpr auto erase(const_iterator pos) {
        const iterator non_const_pos = const_cast<iterator>(pos);
        rotate(non_const_pos, non_const_pos + 1, stop_);
        --stop_;
        return non_const_pos;
    }

    constexpr const_iterator INLINE begin() const { return data_.data; }
    constexpr iterator INLINE begin() { return data_.data; }
    constexpr const_iterator INLINE end() const { return stop_; }
    constexpr iterator INLINE end() { return stop_; }

    constexpr const_iterator INLINE cbegin() const { return data_.data; }
    constexpr const_iterator INLINE cend() const { return stop_; }

    constexpr int64_t INLINE size() const { return stop_ - data_.data; }

    constexpr bool INLINE empty() const { return data_.data == stop_; }

    constexpr const Type& INLINE back() const { return *(stop_ - 1); }

    constexpr Type& INLINE back() { return *(stop_ - 1); }
};

// template<typename Type, typename Allocator = allocator<Type,stack_allocator<Type,Size>>>
// struct vector {
//    using value_type = Type;
//    using iterator = Type*;
//    using const_iterator = const Type*;
//
//    Allocator alloc_;
//    iterator first_{};
//    iterator last_{};
//    iterator stop_{};
//
//    constexpr vector() noexcept(noexcept(Allocator())): vector(Allocator()) {}
//    constexpr explicit vector(const Allocator& alloc) noexcept : alloc_(alloc) {}
//
//    constexpr explicit vector(int64_t n, const Allocator& alloc = Allocator())
//        : alloc_(alloc),
//          first_(alloc_.allocate(n)),
//          last_(first_ + n),
//          stop_(last_)
//    {
//        uninitialized_value_construct(first_, last_);
//    }
//
//    constexpr vector(int64_t n, const Type& x, const Allocator& alloc = Allocator())
//        : alloc_(alloc),
//          first_(alloc_.allocate(n)),
//          last_(first_ + n),
//          stop_(last_)
//    {
//        uninitialized_fill(first_, last_, x);
//    }
//
//    constexpr vector(const vector& rhs)
//        : alloc_(Allocator()), // FIXME
//          first_(alloc_.allocate(rhs.stop_ - rhs.first_)),
//          last_(first_ + (rhs.stop_ - rhs.first_)),
//          stop_(last_)
//    {
//        copy(rhs.first_, const_cast<const_iterator>(rhs.stop_), first_);
//    }
//
//    constexpr vector& operator=(const vector& rhs) {
//        // FIXME: allocator propagation
//        vector tmp(rhs);
//        swap(rhs);
//        return *this;
//    }
//
//    constexpr vector(vector&& rhs) noexcept
//        : alloc_(Allocator()) // FIXME
//    {
//        swap(rhs);
//    }
//
//    constexpr vector& operator=(vector&& rhs) noexcept {
//        swap(rhs);
//        return *this;
//    }
//
//    ~vector() {
//        if (first_) { // delete may accept nullptr, but deallocate doesn't have to
//            // calling array elements' destructors doesn't make sense in a constexpr context :/
//            // ... ... ... but I cannot mark the destructor constexpr anyhow ... so now what?
//            alloc_.deallocate(first_, last_ - first_);
//        }
//    }
//
//    void swap(vector& rhs) noexcept {
//        using std::swap;
//        swap(alloc_, rhs.alloc_);
//        swap(first_, rhs.first_);
//        swap(last_, rhs.last_);
//        swap(stop_, rhs.stop_);
//    }
//
//    constexpr Type* data() noexcept { return first_; }
//    constexpr const Type* data() const noexcept { return first_; }
//
//    constexpr void reserve(int64_t) {}
//    constexpr void clear() noexcept { stop_ = first_; }
//
//    constexpr void resize(int64_t count) {
//        auto new_stop = first_ + count;
//        if (new_stop <= stop_)
//            stop_ = new_stop;
//        else {
//            uninitialized_value_construct(stop_, new_stop);
//            stop_ = new_stop;
//        }
//    }
//
//    constexpr void resize(int64_t count, const Type& value) {
//        auto new_stop = first_ + count;
//        if (new_stop <= stop_)
//            stop_ = new_stop;
//        else {
//            uninitialized_fill(stop_, new_stop, value);
//            stop_ = new_stop;
//        }
//    }
//
//    template<typename ...Args>
//    constexpr Type& emplace_back(Args... args) {
//        //return *new (&first_[size_++]) Type(std::forward<Args>(args)...); /// Not legal in constexpr
//        return *stop_++ = Type(std::forward<Args>(args)...);
//    }
//
//    constexpr void push_back(const Type& x) {
//        //new (&first_[size_++]) Type(x); /// Not legal in constexpr
//        *stop_++ = x;
//    }
//
//    constexpr void push_back(Type&& x) {
//        //new (&first_[size_++]) Type(std::move(x)); /// Not legal in constexpr
//        *stop_++ = std::move(x);
//    }
//
//    constexpr void pop_back() {
//        --stop_;
//    }
//
//    constexpr const Type& operator[](const std::int64_t t_pos) const {
//        return first_[t_pos];
//    }
//
//    constexpr Type& operator[](const std::int64_t t_pos) {
//        return first_[t_pos];
//    }
//
//    constexpr iterator insert(const_iterator pos, Type value) {
//
//        // TODO: actually need to uninitialized_copy and uninitialized_move.
//        // The requirements on this vector::insert overload depend on InputIt
//        // being a ForwardIterator or not. Not sure why. But being
//        // standard-compliant would seemingly be a lot more complicated.
//
//        *stop_ = std::move(value);
//        auto non_const_pos = const_cast<iterator>(pos);
//        rotate(non_const_pos, stop_, stop_ + 1);
//        ++stop_;
//        return non_const_pos; // we do not reallocate, so pos remains valid
//    }
//
//    template<class InputIt>
//    constexpr iterator insert(const const_iterator pos, const InputIt first, const InputIt last) {
//        // TODO: actually need to uninitialized_copy and uninitialized_move.
//        // The requirements on this vector::insert overload depend on InputIt
//        // being a ForwardIterator or not. Not sure why. But being
//        // standard-compliant would seemingly be a lot more complicated.
//
//        const auto new_end = copy(first, last, stop_);
//        const auto non_const_pos = const_cast<iterator>(pos);
//        rotate(non_const_pos, stop_, new_end);
//        stop_ = new_end;
//        return non_const_pos; // we do not reallocate, so pos remains valid
//    }
//
//    constexpr auto erase(const_iterator first, const_iterator last) {
//        // trick taken from libstdc++ to deal with the const_iterators not being iter_swappable
//        const auto non_const_first = const_cast<iterator>(first);
//        const auto new_end = rotate(non_const_first, const_cast<iterator>(last), stop_);
//        stop_ = new_end;
//        return non_const_first;
//    }
//
//    constexpr auto erase(const_iterator pos) {
//        const iterator non_const_pos = const_cast<iterator>(pos);
//        rotate(non_const_pos, non_const_pos + 1, stop_);
//        --stop_;
//        return non_const_pos;
//    }
//
//    constexpr const_iterator begin() const { return first_; }
//    constexpr iterator begin() { return first_; }
//    constexpr const_iterator end() const { return stop_; }
//    constexpr iterator end() { return stop_; }
//
//    constexpr const_iterator cbegin() const { return first_; }
//    constexpr const_iterator cend() const { return stop_; }
//
//    constexpr int64_t size() const { return stop_ - first_; }
//
//    constexpr bool empty() const { return first_ == stop_; }
//
//    constexpr const Type& back() const {
//        return *(stop_ - 1);
//    }
//
//    constexpr Type& back() {
//        return *(stop_ - 1);
//    }
//};

// template<typename Type, typename Allocator>
// constexpr void swap(vector<Type,Allocator>& lhs, vector<Type,Allocator>& rhs) noexcept {
//    lhs.swap(rhs);
//}
//
// template<typename Type, typename Allocator>
// constexpr bool operator==(const vector<Type,Allocator>& lhs, const vector<Type,Allocator>& rhs) {
//    return lhs.size() == rhs.size() && equal(lhs.begin(), lhs.end(), rhs.begin());
//}

template <typename Type, int64_t Size1, int64_t Size2>
constexpr bool operator==(const vector<Type, Size1>& lhs, const vector<Type, Size2>& rhs) {
    return lhs.size() == rhs.size() && equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Key>
struct ByKey {
    const Key& k_;
    constexpr INLINE ByKey(const Key& key) : k_(key) {}
    template <typename Value>
    constexpr bool INLINE operator()(const pair<Key, Value>& p) const {
        return p.first == k_;
    }
};

template <typename Key, typename Value, int64_t Size>
struct unordered_map {
    // using storage = vector<pair<const Key,Value>,Size>; // not possible with constexpr due to missing placement-new
    using storage = vector<pair<Key, Value>, Size>;
    storage data_;

    using iterator = typename storage::iterator;
    using const_iterator = typename storage::const_iterator;

    constexpr auto INLINE size() const { return data_.size(); }
    constexpr auto INLINE empty() const { return data_.empty(); }

    constexpr auto INLINE begin() const { return data_.begin(); }
    constexpr auto INLINE begin() { return data_.begin(); }
    constexpr auto INLINE end() const { return data_.end(); }
    constexpr auto INLINE end() { return data_.end(); }

    constexpr auto INLINE find(const Key& k) { return find_impl(*this, k); }

    constexpr auto INLINE find(const Key& k) const { return find_impl(*this, k); }

    constexpr const Value& INLINE at(const Key& k) const {
        const auto itr = find(k);
        if (itr != end()) {
            return itr->second;
        } else {
            throw std::range_error("key not found");
        }
    }

    constexpr Value& INLINE operator[](const Key& k) {
        const auto itr = find(k);
        if (itr == end()) {
            // data_.emplace_back(k, Value());
            data_.emplace_back(std::piecewise_construct, std::forward_as_tuple(std::move(k)), std::tuple<>());
            return data_.back().second;
        } else {
            return itr->second;
        }
    }

  private:
    template <typename This>
    // requires std::is_same_v<std::decay_t<This>, unordered_map<Key,Value,Size>>
    static constexpr auto INLINE find_impl(This&& t, const Key& k) {
        return const_expr::find_if(t.begin(), t.end(), ByKey<Key>(k));
    }
};

// template<typename Key, typename Value, typename Allocator = allocator<Type,stack_allocator<Type,Size>>>
// struct unordered_map {
//    using storage = vector<pair<Key,Value>,Allocator>; // 'const Key' is not possible with constexpr due to missing
//    placement-new storage data_;
//
//    using iterator = typename storage::iterator;
//    using const_iterator = typename storage::const_iterator;
//
//    constexpr unordered_map() : data_(Allocator()) {}
//    constexpr explicit unordered_map(const Allocator& alloc) : data_(alloc) {}
//
//    constexpr auto size() const { return data_.size(); }
//    constexpr auto empty() const { return data_.empty(); }
//
//    constexpr auto begin() const { return data_.begin(); }
//    constexpr auto begin() { return data_.begin(); }
//    constexpr auto end() const { return data_.end(); }
//    constexpr auto end() { return data_.end(); }
//
//    constexpr auto find(const Key& k) {
//      return find_impl(*this, k);
//    }
//
//    constexpr auto find(const Key& k) const {
//      return find_impl(*this, k);
//    }
//
//    constexpr const Value& at(const Key& k) const {
//        const auto itr = find(k);
//        if (itr != end()) { return itr->second; }
//        else { throw std::range_error("Key not found"); }
//    }
//
//    constexpr Value& operator[](const Key& k) {
//        const auto itr = find(k);
//        if (itr == end()) {
//            //data_.emplace_back(k, Value());
//            data_.emplace_back(std::piecewise_construct, std::forward_as_tuple(std::move(k)), std::tuple<>());
//            return data_.back().second;
//        } else {
//            return itr->second;
//        }
//    }
// private:
//    template <typename This>
//    //requires std::is_same_v<std::decay_t<This>, unordered_map<Key,Value,Size>>
//    static constexpr auto find_impl(This&& t, const Key& k) {
//        return ::find_if(t.begin(), t.end(), [&k] (const auto& p) { return p.first == k; });
//    }
//};

template <typename Type, int64_t Size>
struct set {
    using storage = vector<Type, Size>;
    storage data_;

    using const_iterator = typename storage::const_iterator;
    using iterator = const_iterator;

    constexpr set() = default;

    constexpr auto INLINE size() const { return data_.size(); }
    constexpr auto INLINE empty() const { return data_.empty(); }

    constexpr auto INLINE begin() const { return data_.cbegin(); }
    constexpr auto INLINE begin() { return data_.cbegin(); }
    constexpr auto INLINE end() const { return data_.cend(); }
    constexpr auto INLINE end() { return data_.cend(); }

    constexpr void INLINE clear() noexcept { data_.clear(); }

    constexpr auto INLINE find(const Type& k) { return find_impl(*this, k); }

    constexpr auto INLINE find(const Type& k) const { return find_impl(*this, k); }

    constexpr int64_t INLINE count(const Type& key) const {
        return find(key) != end();
        // return ::count(begin(), end(), key);
    }

    constexpr pair<iterator, bool> insert(Type value) {
        auto it = lower_bound(begin(), end(), value);
        // it != end() => !(*it < value)
        if (it != end() && !(value < *it)) {
            return make_pair(it, false);
        }
        // value < *it
        it = data_.insert(it, std::move(value));
        return make_pair(it, true);
    }

    template <class InputIt>
    constexpr void insert(InputIt first, InputIt last) {
        for (; first != last; ++first)
            insert(*first);
    }

    constexpr int64_t erase(const Type& key) {
        auto it = lower_bound(begin(), end(), key);
        if (it == end() || key < *it) {
            return 0;
        }
        it = data_.erase(it);
        return 1;
        // auto new_end = remove(data_.begin(), data_.end(), key);
        // auto erase_count = end() - new_end;
        // data_.erase(new_end, data_.end());
        // return erase_count;
    }

  private:
    template <typename This>
    // requires std::is_same_v<std::decay_t<This>, set<Type,Size>>
    static constexpr auto find_impl(This&& t, const Type& k) {
        auto first = t.begin();
        const auto last = t.end();
        first = lower_bound(first, last, k);
        return first != last && !(k < *first) ? first : last;
    }
};

template <typename Type, int64_t Size1, int64_t Size2>
constexpr bool operator==(const set<Type, Size1>& lhs, const set<Type, Size2>& rhs) {
    if (lhs.size() != rhs.size())
        return false;
    return equal(lhs.begin(), lhs.end(), rhs.begin());
}

// template<typename Type, int64_t Size>
// struct unordered_set {
//    using storage = vector<Type,Size>;
//    storage data_;
//
//    using const_iterator = typename storage::const_iterator;
//    using iterator = const_iterator;
//
//    constexpr unordered_set() = default;
//
//    constexpr auto INLINE size() const { return data_.size(); }
//    constexpr auto INLINE empty() const { return data_.data_ == data_.stop_; }
//
//    constexpr auto INLINE begin() { return data_.cbegin(); }
//    constexpr auto INLINE end() { return data_.cend(); }
//    constexpr auto INLINE begin() const { return data_.cbegin(); }
//    constexpr auto INLINE end() const { return data_.cend(); }
//    constexpr auto INLINE cbegin() const { return data_.cbegin(); }
//    constexpr auto INLINE cend() const { return data_.cend(); }
//
//    constexpr void INLINE clear() noexcept { data_.clear(); }
//
//    constexpr auto INLINE find(const Type& k) {
//      return find_impl(*this, k);
//    }
//
//    constexpr auto INLINE find(const Type& k) const {
//      return find_impl(*this, k);
//    }
//
//    constexpr int64_t INLINE count(const Type& key) const {
//        return find(key) != end();
//        //return ::count(begin(), end(), key);
//    }
//
//    constexpr pair<iterator,bool> insert(const Type value) {
//        const auto e = cend();
//        if (const auto it = find(value); it != e)
//            return make_pair(it, false);
//        data_.push_back(std::move(value));
//        return make_pair(e, true);
//    }
//
//    template<class InputIt>
//    constexpr void insert(InputIt first, InputIt last) {
//        for (; first != last; ++first)
//            insert(*first);
//    }
//
//    constexpr int64_t erase(const Type& key) {
//        auto new_end = remove(data_.begin(), data_.end(), key);
//        auto erase_count = end() - new_end;
//        data_.erase(new_end, data_.end());
//        return erase_count;
//    }
// private:
//    template <typename This>
//    //requires std::is_same_v<std::decay_t<This>, unordered_set<Type,Size>>
//    static constexpr auto INLINE find_impl(This&& t, const Type& k) {
//        return ::find(t.begin(), t.end(), k);
//    }
//};
//
// template<typename Type, int64_t Size1, int64_t Size2>
// constexpr bool operator==(const unordered_set<Type,Size1>& lhs, const unordered_set<Type,Size2>& rhs) {
//    if (lhs.size() != rhs.size())
//        return false;
//    for (const auto& x : lhs) {
//        if (rhs.find(x) == rhs.end())
//            return false;
//    }
//    return true;
//}

template <typename Container>
struct back_insert_iterator {
    constexpr explicit INLINE back_insert_iterator(Container& c) : m_c(c) {}

    constexpr back_insert_iterator& INLINE operator=(const typename Container::value_type& value) {
        m_c.push_back(value);
        return *this;
    }

    constexpr back_insert_iterator& INLINE operator=(typename Container::value_type&& value) {
        m_c.push_back(std::move(value));
        return *this;
    }

    constexpr back_insert_iterator& INLINE operator*() { return *this; }
    constexpr back_insert_iterator& INLINE operator++() { return *this; }
    constexpr back_insert_iterator& INLINE operator++(int) { return *this; }

    Container& m_c;
};

template <class Container>
constexpr back_insert_iterator<Container> INLINE back_inserter(Container& c) {
    return back_insert_iterator<Container>(c);
}

constexpr const char* find_0(const char* str) {
    while (*str != '\0') {
        ++str;
    }
    return str;
}

constexpr int64_t strlen(const char* const str) { return find_0(str) - str; }

template <int64_t Size>
struct string {
    using Type = char;
    using storage = Type[Size + 1]; // using vector makes insert(end(), ...) inefficient
    using value_type = Type;

    storage data_{};
    int64_t size_ = 0;

    using iterator = char*;
    using const_iterator = const char*;

    constexpr string() { data_[0] = '\0'; }

    constexpr string(int64_t n, Type x) : size_(n) {
        assert(size_ <= Size);
        fill_n(data_, n, x);
        data_[n] = '\0';
    }

    constexpr string(const Type* str) : size_(const_expr::strlen(str)) {
        assert(size_ <= Size);
        copy_n(str, size_ + 1, data_);
    }

    constexpr Type* data() noexcept { return data_; }
    constexpr const Type* data() const noexcept { return data_; }

    constexpr void reserve(int64_t) {}
    constexpr void clear() noexcept {
        size_ = 0;
        data_[0] = '\0';
    }

    constexpr void resize(int64_t count, Type value = Type()) {
        if (count > size_) {
            fill(data_ + size_, data_ + count, value);
        }
        size_ = count;
        data_[size_] = '\0';
    }

    constexpr void push_back(Type x) {
        data_[size_] = x;
        ++size_;
        data_[size_] = '\0';
    }

    constexpr void pop_back() {
        --size_;
        data_[size_] = '\0';
    }

    constexpr Type operator[](const std::int64_t t_pos) const { return data_[t_pos]; }

    constexpr Type& operator[](const std::int64_t t_pos) { return data_[t_pos]; }

    constexpr iterator insert(const_iterator pos, Type ch) {
        data_[size_] = ch;
        auto new_end = end() + 1;
        auto non_const_pos = begin() + (pos - cbegin());
        rotate(non_const_pos, end(), new_end);
        size_ = new_end - begin();
        data_[size_] = '\0';
        return non_const_pos; // we do not reallocate, so pos remains valid
    }

    template <class InputIt>
    constexpr iterator insert(const_iterator pos, InputIt first, InputIt last) {
        auto new_end = copy(first, last, end());
        auto non_const_pos = begin() + (pos - cbegin());
        rotate(non_const_pos, end(), new_end);
        size_ = new_end - begin();
        data_[size_] = '\0';
        return non_const_pos; // we do not reallocate, so pos remains valid
    }

    constexpr string<Size>& insert(int64_t index, const Type* s, int64_t count) {
        insert(begin() + index, s, s + count);
        return *this;
    }

    constexpr string<Size>& insert(int64_t index, const Type* s) {
        insert(begin() + index, s, find_0(s));
        return *this;
    }

    constexpr string<Size>& append(const Type* s) { return insert(size_, s); }

    constexpr string<Size>& operator+=(Type ch) {
        push_back(ch);
        return *this;
    }

    constexpr string<Size>& operator+=(const Type* s) { return append(s); }

    constexpr auto erase(const_iterator first, const_iterator last) {
        auto new_end = rotate(begin() + (first - cbegin()), begin() + (last - cbegin()), end());
        size_ = new_end - begin();
        data_[size_] = '\0';
        return new_end;
    }

    constexpr auto begin() const { return data_; }
    constexpr auto begin() { return data_; }
    constexpr auto end() const { return data_ + size_; }
    constexpr auto end() { return data_ + size_; }

    constexpr auto cbegin() const { return data_; }
    constexpr auto cend() const { return data_ + size_; }

    constexpr auto size() const { return size_; }

    constexpr auto empty() const { return size_ == 0; }

    constexpr Type back() const { return data_[size_ - 1]; }

    constexpr Type& back() { return data_[size_ - 1]; }
};

template <int64_t Size1, int64_t Size2>
constexpr bool operator==(const string<Size1>& lhs, const string<Size2>& rhs) {
    return lhs.size() == rhs.size() && equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <int64_t N>
using charr = const char[N];

struct string_view {
    constexpr string_view() = default;
    template <int64_t N>
    constexpr string_view(const charr<N>& s) : data(s), stop(s + N - 1) {}
    template <int64_t N>
    constexpr string_view(const string<N>& s) : data(s.begin()), stop(s.end()) {}

    constexpr const char* begin() const { return data; }
    constexpr const char* end() const { return stop; }
    constexpr int64_t size() const { return stop - data; }

    const char* data = nullptr;
    const char* stop = nullptr;
};

template <int64_t N>
constexpr string_view to_string_view(const charr<N>& s) {
    return string_view(s);
}

template <int64_t N>
constexpr string_view to_string_view(const string<N>& s) {
    return string_view(s);
}

template <typename T>
struct static_strlen;

template <int64_t N>
struct static_strlen<charr<N>> {
    static const int64_t value{N - 1};
};

// at least with clang 8 (and the very early v10 branch), string literals are "char[N]" and not "const char[N]" during
// constexpr template type matching (or whatever I should call it ...)
template <int64_t N>
struct static_strlen<char[N]> {
    static const int64_t value{N - 1};
};

template <int64_t N>
struct static_strlen<string<N>> {
    static const int64_t value{N};
};

template <typename... Ts>
constexpr int64_t strcatsize() {
    return (0 + ... + static_strlen<Ts>::value);
}

template <typename... Ts>
struct strcat_impl;

template <typename T>
struct strcat_impl<T> {
    // If you get an error about "no member named cat in strcat_impl<T>", you need to specialize strcat_impl for T.
};

template <int64_t N>
struct strcat_impl<char[N]> {
    template <typename OutputIterator>
    static constexpr OutputIterator cat(OutputIterator out, const charr<N>& s) {
        return copy(std::begin(s), std::end(s) - 1, out);
    }
};

template <int64_t N>
struct strcat_impl<charr<N>> {
    template <typename OutputIterator>
    static constexpr OutputIterator cat(OutputIterator out, const charr<N>& s) {
        return copy(std::begin(s), std::end(s) - 1, out);
    }
};

template <int64_t N>
struct strcat_impl<string<N>> {
    template <typename OutputIterator>
    static constexpr OutputIterator cat(OutputIterator out, const string<N>& s) {
        return copy(std::begin(s), std::end(s), out);
    }
};

// TODO
// template <>
// struct strcat_impl<std::int64_t> {
//    template <typename OutputIterator>
//    static constexpr OutputIterator cat(OutputIterator out, std::int64_t arg) {
//
//    }
//};

template <typename T, typename... Ts>
struct strcat_impl<T, Ts...> {
    template <typename OutputIterator>
    static constexpr OutputIterator cat(OutputIterator out, const T& arg, const Ts&... args) {
        const auto new_out = strcat_impl<T>::cat(out, arg);
        return strcat_impl<Ts...>::cat(new_out, args...);
    }
};

constexpr auto strcat() { return string<0>{}; }

template <typename... Ts>
constexpr auto strcat(const Ts&... args) {
    constexpr auto size = strcatsize<Ts...>();
    string<size> out;
    strcat_impl<Ts...>::cat(back_inserter(out), args...);
    return out;
}

auto constexpr next_highest_power_of_two(int64_t v) {
    // https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
    constexpr auto trip_count = std::numeric_limits<decltype(v)>::digits;
    v--;
    for (int64_t i = 1; i < trip_count; i <<= 1)
        v |= v >> i;
    v++;
    return v;
}

constexpr int64_t count_trailing_zeros1(uint64_t i) {
    assert(i != 0); // precondition ... we could check and return 64, but below we check and do something else, i.e. we
                    // wouldn't use the value 64
    int64_t count = 0;
    if (!(i & 0x1)) {
        if ((i & 0xffffffff) == 0) {
            i >>= 32;
            count += 32;
        }
        if ((i & 0xffff) == 0) {
            i >>= 16;
            count += 16;
        }
        if ((i & 0xff) == 0) {
            i >>= 8;
            count += 8;
        }
        if ((i & 0xf) == 0) {
            i >>= 4;
            count += 4;
        }
        if ((i & 0x3) == 0) {
            i >>= 2;
            count += 2;
        }
        if ((i & 0x1) == 0) {
            i >>= 1;
            count += 1;
        }
    }
    return count;
}
inline int64_t count_trailing_zeros2(uint64_t i) {
    float f = static_cast<float>(i & -i); // cast the least significant bit in v to a float
    uint32_t bitwise_copy = 0;
    auto fptr = reinterpret_cast<unsigned char*>(&f);
    auto copyptr = reinterpret_cast<unsigned char*>(&bitwise_copy); // not allowed in a constexpr context :(
    *copyptr = *fptr;
    *(copyptr + 1) = *(fptr + 1);
    *(copyptr + 2) = *(fptr + 2);
    *(copyptr + 3) = *(fptr + 3);
    // shift out the 23 bit mantissa which leaves the exponent (sign is 0 because we converted a positive number)
    // then subtract the exponent "bias"
    return (bitwise_copy >> 23) - 127;
}

// Based on https://graphics.stanford.edu/~seander/bithacks.html#ZerosOnRightModLookup, adapted for 64 bit numbers
// Why 67?
//
// In [40]: i = 1
//
// In [41]: while np.unique(2**np.arange(64, dtype=np.uint) % i).size != 64:
//     ...:     i = i + 1
//     ...:
//
// In [42]: i
// Out[42]: 67
//
// How did I come up with the table?
// In [74]: t = np.zeros(67, dtype=int)
//
// In [75]: t[2**np.arange(64) % 67, dtype=np.uint] = np.arange(64)
//
// In [76]: t
// Out[76]:
// array([ 0,  0,  1, 39,  2, 15, 40, 23,  3, 12, 16, 59, 41, 19, 24, 54,  4,  0, 13, 10, 17, 62, 60, 28, 42, 30, 20,
// 51, 25, 44, 55, 47,  5, 32,  0, 38, 14, 22, 11, 58, 18, 53, 63,  9, 61, 27, 29, 50, 43, 46, 31, 37, 21, 57, 52,  8,
// 26, 49, 45, 36, 56,
//         7, 48, 35,  6, 34, 33])
//
// In [77]: t[2**np.arange(64, dtype=np.uint) % 67]
// Out[77]:
// array([ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
// 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55,
// 56, 57, 58, 59, 60,
//        61, 62, 63])

constexpr const int8_t Mod67BitPosition[] = {64, 0,  1,  39, 2,  15, 40, 23, 3,  12, 16, 59, 41, 19, 24, 54, 4,
                                             0,  13, 10, 17, 62, 60, 28, 42, 30, 20, 51, 25, 44, 55, 47, 5,  32,
                                             0,  38, 14, 22, 11, 58, 18, 53, 63, 9,  61, 27, 29, 50, 43, 46, 31,
                                             37, 21, 57, 52, 8,  26, 49, 45, 36, 56, 7,  48, 35, 6,  34, 33};

constexpr int64_t INLINE count_trailing_zeros(uint64_t i) { return Mod67BitPosition[(-i & i) % 67]; }

// constexpr const int8_t MultiplyDeBruijnBitPosition[32] =
//{
//  0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
//  31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
//};
//
// constexpr int8_t count_trailing_zeros3(uint32_t i) {
//    return MultiplyDeBruijnBitPosition[((uint32_t)((i & -i) * 0x077CB531U)) >> 27];
//}

// Numerical Recipes 7.6
template <typename Key, int64_t MaxElementCount, typename Hash = std::hash<Key>, typename Index = int32_t>
struct HashTable {
    static constexpr auto HashSize = next_highest_power_of_two(MaxElementCount); // / 0.75?
    static constexpr auto LogHashSize = count_trailing_zeros(HashSize);
    static constexpr auto HashSizeMask = HashSize - 1;
    array<Index, HashSize> htable{};
    array<Index, HashSize> next{};
    // vector<Index,MaxElementCount> garbage;
    vector<uint64_t, HashSize> thehash;
    Hash hash;

    constexpr HashTable() { clear(); }

    // try to remove this later ... there were unspecific "operation not allowed in constexpr context" errors
    constexpr HashTable& operator=(const HashTable& rhs) {
        htable = rhs.htable;
        next = rhs.next;
        thehash = rhs.thehash;
        hash = rhs.hash;
        return *this;
    }

    constexpr Index INLINE get(const Key& key) const { return getByHash(hash(key)); }

    constexpr pair<Index, bool> set(const Key& key) {
        auto pp = hash(key);
        auto& slot = getByHash(pp);

        if (slot != -1)
            return make_pair(slot, false);

        const auto k = new_slot();
        slot = k;
        thehash[k] = pp;
        next.data[k] = -1;
        return make_pair(k, true);
    }

    // here we rely on the fact that we can't delete keys.
    // This allows us to just iterate from 0 to thehash.size()
    using const_iterator = NumIterator<Index>;
    using iterator = const_iterator;

    constexpr auto begin() const { return const_iterator(0); }
    constexpr auto begin() { return const_iterator(0); }
    constexpr auto end() const { return const_iterator(thehash.size()); }
    constexpr auto end() { return const_iterator(thehash.size()); }

    constexpr auto cbegin() const { return const_iterator(0); }
    constexpr auto cend() const { return const_iterator(thehash.size()); }

    constexpr auto size() const { return const_iterator(thehash.size()); }

    constexpr auto empty() const { return thehash.empty(); }

    static constexpr array<Index, HashSize> htable_init = [] {
        array<Index, HashSize> arr{};
        fill(std::begin(arr.data), std::end(arr.data), -1);
        return arr;
    }();
    constexpr void clear() {
        // fill(std::begin(htable.data), std::end(htable.data), -1);
        htable = htable_init;
        thehash.clear();
    }

  private:
    template <typename Iterator>
    static constexpr Iterator iter_list(Index k, const uint64_t pp, Iterator next, const uint64_t* thehash) {
        for (; next[k] != -1; k = next[k]) {
            if (thehash[next[k]] == pp)
                break;
        }
        return next + k;
    }

    template <typename Iterator>
    static constexpr Iterator getByHash(const uint64_t pp, Iterator htable, Iterator next, const uint64_t* thehash) {
        auto slot = &htable[pp & HashSizeMask];

        if (*slot != -1) {
            if (thehash[*slot] == pp)
                return slot;
            slot = iter_list(*slot, pp, next, thehash);
        }
        return slot;
    }

    constexpr const Index& getByHash(const uint64_t pp) const {
        return *getByHash(pp, htable.data, next.data, thehash.data());
    }

    constexpr Index& getByHash(const uint64_t pp) { return *getByHash(pp, htable.data, next.data, thehash.data()); }

    constexpr Index new_slot() {
        // if (!garbage.empty()) {
        //    auto k = garbage.back();
        //    garbage.pop_back();
        //    return k;
        //} else {
        thehash.emplace_back();
        return static_cast<Index>(thehash.size() - 1);
        //}
    }
};

template <typename Key,
          typename Value,
          int64_t MaxElementCount,
          typename Hash = std::hash<Key>,
          typename Index = int32_t>
struct HashMap : private HashTable<Key, MaxElementCount, Hash, Index> {
    // using storage = vector<pair<const key,value>,size>; // not possible with constexpr due to missing placement-new
    using value_type = pair<Key, Value>;
    array<value_type, MaxElementCount> values;

    using const_iterator = const value_type*;
    using iterator = const_iterator; // only iterator->fst should be const, but see above ... and pair<key,value>* does
                                     // not convert to pair<const key,value>*

    constexpr HashMap() = default;

    constexpr int64_t INLINE size() const { return this->thehash.size(); }
    constexpr bool INLINE empty() const { return this->thehash.empty(); }

    constexpr iterator INLINE begin() { return values.data; }
    constexpr iterator INLINE end() { return values.data + this->thehash.size(); }
    constexpr const_iterator INLINE begin() const { return values.data; }
    constexpr const_iterator INLINE end() const { return values.data + this->thehash.size(); }
    constexpr const_iterator INLINE cbegin() const { return values.data; }
    constexpr const_iterator INLINE cend() const { return values.data + this->thehash.size(); }

    constexpr auto INLINE find(const Key& k) { return find_impl(*this, k); }

    constexpr auto INLINE find(const Key& k) const { return find_impl(*this, k); }

    constexpr const Value& INLINE at(const Key& k) const {
        const auto itr = find(k);
        if (itr != end()) {
            return itr->second;
        } else {
            throw std::range_error("key not found");
        }
    }

    constexpr Value& INLINE operator[](const Key& k) {
        const auto i = this->set(k);
        auto it = values.data + i.first;
        if (i.second) {
            *it = pair<Key, Value>(std::piecewise_construct, std::forward_as_tuple(std::move(k)), std::tuple<>());
        }
        return it->second;
    }

  private:
    template <typename This>
    // requires std::is_same_v<std::decay_t<This>, unordered_map<Key,Value,Size>>
    static constexpr auto INLINE find_impl(This&& t, const Key& k) {
        const auto i = t.get(k);
        return i == -1 ? t.end() : t.values.data + i;
    }
};

template <typename Key, int64_t MaxElementCount, typename Hash = std::hash<Key>, typename Index = int32_t>
struct unordered_set : private HashTable<Key, MaxElementCount, Hash, Index> {
    using Base = HashTable<Key, MaxElementCount, Hash, Index>;
    using value_type = Key;
    value_type* stop_; // only for constexpr optimization ... otherwise end and begin are too expensive -.-*
    array<value_type, MaxElementCount> values{};

    using const_iterator = const value_type*;
    using iterator = const_iterator;

    constexpr unordered_set() : stop_{values.data} {}

    // try to remove these later ... there were unspecific "operation not allowed in constexpr context" errors
    constexpr unordered_set(const unordered_set& rhs) : Base(rhs), values(rhs.values) {
        stop_ = values.data + (rhs.stop_ - rhs.values.data);
    }

    constexpr unordered_set& operator=(const unordered_set& rhs) {
        Base::operator=(rhs);
        values = rhs.values;
        stop_ = values.data + (rhs.stop_ - rhs.values.data);
        return *this;
    }

    constexpr int64_t INLINE size() const { return stop_ - values.data; }
    constexpr bool INLINE empty() const { return stop_ == values.data; }

    constexpr iterator INLINE begin() { return values.data; }
    constexpr iterator INLINE end() { return stop_; }
    constexpr const_iterator INLINE begin() const { return values.data; }
    constexpr const_iterator INLINE end() const { return stop_; }
    constexpr const_iterator INLINE cbegin() const { return values.data; }
    constexpr const_iterator INLINE cend() const { return stop_; }

    constexpr void INLINE clear() noexcept {
        Base::clear();
        stop_ = values.data;
    }

    constexpr auto INLINE find(const Key& k) { return find_impl(*this, k); }

    constexpr auto INLINE find(const Key& k) const { return find_impl(*this, k); }

    constexpr int64_t INLINE count(const Key& key) const { return find(key) != stop_; }

    constexpr pair<const_iterator, bool> insert(const Key& value) {
        const auto i = this->set(value);
        auto it = values.data + i.first;
        if (i.second) {
            *it = value;
            ++stop_;
        }
        return make_pair(const_iterator(it), i.second);
    }

    template <class InputIt>
    constexpr void insert(InputIt first, InputIt last) {
        for (; first != last; ++first)
            insert(*first);
    }

  private:
    template <typename This>
    // requires std::is_same_v<std::decay_t<This>, unordered_map<Key,Value,Size>>
    static constexpr auto INLINE find_impl(This&& t, const Key& k) {
        const auto i = t.get(k);
        return i == -1 ? t.stop_ : t.values.data + i;
    }
};
} // namespace const_expr

template <class T, int64_t Size>
struct showImpl<const_expr::vector<T, Size>> {
    static std::ostream& Show(std::ostream& s, const const_expr::vector<T, Size>& v) {
        if (v.empty()) {
            s << "[]";
            return s;
        }
        s << "[ " << v[0];
        for (unsigned int i = 1; i < v.size(); i++)
            s << ", " << v[i];

        s << " ]";
        return s;
    }
};

template <class T1, class T2>
struct showImpl<const_expr::pair<T1, T2>> {
    static std::ostream& Show(std::ostream& s, const const_expr::pair<T1, T2>& p) {
        s << "(" << p.first << "," << p.second << ")";
        return s;
    }
};
