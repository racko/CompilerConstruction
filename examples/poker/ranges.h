#pragma once

#include <boost/iterator/counting_iterator.hpp>
#include <boost/iterator/filter_iterator.hpp>
#include <boost/iterator/function_output_iterator.hpp>
#include <boost/iterator/reverse_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <cstddef>

// boost::counting_iterator does not work with reverse_iterator: reverse_iterator<boost::counting_iterator>::operator*
// returns a dangling reference to an expired local variable.
template <class Incrementable, class CategoryOrTraversal = boost::use_default, class Difference = boost::use_default>
class counting_iterator
    : public boost::iterator_adaptor<counting_iterator<Incrementable, CategoryOrTraversal, Difference>,
                                     Incrementable,
                                     Incrementable,
                                     boost::random_access_traversal_tag,
                                     Incrementable, // Reference. boost::counting_iterator uses const Incrementable&
                                     std::ptrdiff_t> {
    using super_t = typename counting_iterator::iterator_adaptor_;

    friend class boost::iterator_core_access;

  public:
    typedef typename super_t::difference_type difference_type;

    counting_iterator() {}

    counting_iterator(counting_iterator const& rhs) : super_t(rhs.base()) {}

    counting_iterator(Incrementable x) : super_t(x) {}

  private:
    typename super_t::reference dereference() const { return this->base_reference(); }

    template <class OtherIncrementable>
    difference_type distance_to(counting_iterator<OtherIncrementable, CategoryOrTraversal, Difference> const& y) const {
        return this->base() - y.base();
    }
};

template <typename T>
class CountingRange {
  public:
    using iterator = counting_iterator<T>;

    CountingRange(const T start, const T stop) : start_{start}, stop_{stop} {}

    iterator begin() const { return iterator(start_); }
    iterator end() const { return iterator(stop_); }

  private:
    T start_;
    T stop_;
};

template <typename T>
CountingRange<T> range(const T start, const T stop) {
    return CountingRange<T>{start, stop};
}

template <typename InputRange>
class ReverseRange {
  public:
    using iterator = boost::reverse_iterator<typename InputRange::iterator>;

    ReverseRange(const InputRange in) : in_{in} {}

    iterator begin() const { return iterator(in_.end()); }
    iterator end() const { return iterator(in_.begin()); }

  private:
    InputRange in_;
};

template <typename InputRange>
ReverseRange<InputRange> reverse(const InputRange in) {
    return ReverseRange<InputRange>{in};
}

template <typename UnaryFunction, typename InputRange>
class TransformRange {
  public:
    using iterator = boost::transform_iterator<UnaryFunction, typename InputRange::iterator>;

    TransformRange(UnaryFunction f, InputRange in) : f_{f}, in_{in} {}

    iterator begin() const { return iterator(in_.begin(), f_); }
    iterator end() const { return iterator(in_.end(), f_); }

  private:
    UnaryFunction f_;
    InputRange in_;
};

template <typename UnaryFunction, typename InputRange>
TransformRange<UnaryFunction, InputRange> transform(UnaryFunction f, InputRange in) {
    return TransformRange<UnaryFunction, InputRange>{f, in};
}

template <typename Predicate, typename InputRange>
class FilterRange {
  public:
    using iterator = boost::filter_iterator<Predicate, typename InputRange::iterator>;

    FilterRange(Predicate p, InputRange in) : p_{p}, in_{in} {}

    iterator begin() const { return iterator(p_, in_.begin(), in_.end()); }
    iterator end() const { return iterator(p_, in_.end(), in_.end()); }

  private:
    Predicate p_;
    InputRange in_;
};

template <typename Predicate, typename InputRange>
FilterRange<Predicate, InputRange> filter(Predicate p, InputRange in) {
    return FilterRange<Predicate, InputRange>{p, in};
}

template <typename Function, typename InputRange>
auto apply(Function f, InputRange in) {
    return f(in.begin(), in.end());
}
