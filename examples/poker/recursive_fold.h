#pragma once

#include <functional>

template <typename Operation, typename StateRepr>
struct lazy_rec_tree {
    using result_t = decltype(std::declval<Operation>()(StateRepr{}, nullptr, nullptr));
    using labelled_child = std::pair<std::uint32_t, result_t>;

    lazy_rec_tree(const Operation f, const std::size_t initial_memory_size) : f_{f}, memory{initial_memory_size} {}

    result_t fold(const StateRepr k) {
        auto step = [this](const Transition<StateRepr> t) -> labelled_child {
            if (const auto result = memory.find(t.child); result != memory.end()) {
                return {t.r, result->second};
            }
            return {t.r, fold(t.child)};
        };
        return memory[k] = ::apply(std::bind_front(f_, k), transform(step, children(k)));
    }

    Operation f_;
    std::unordered_map<StateRepr, result_t> memory;
};

template <typename Operation, typename StateRepr>
auto foldRecursive(Operation f, const StateRepr k, const std::size_t initial_memory_size) {
    return lazy_rec_tree<Operation, StateRepr>{f, initial_memory_size}.fold(k);
}
