#pragma once

#include "rank_count.h"
#include <cstdint>
#include <vector>
#include <tuple>
#include <cassert>

template <typename Operation>
auto foldIterative(Operation f, const RankCount root, const std::size_t initial_memory_size) {
    using result_t = decltype(f(RankCount{}, nullptr, nullptr));
    using labelled_child = std::pair<std::uint32_t, result_t>;
    using labelled_children_t = std::vector<labelled_child>;
    using stack_record = std::tuple<RankCount, labelled_children_t, std::uint32_t>;
    std::vector<stack_record> stack;
    stack.reserve(90);
    // dummy record that will receive the result in the end
    stack.emplace_back(RankCount{}, labelled_children_t{{0, result_t{}}}, 0);
    stack.emplace_back(root, labelled_children_t{}, 0);
    std::unordered_map<RankCount, result_t> memory(initial_memory_size);
    auto i = 0;
    while (stack.size() != 1) {
        ++i;
        // logger << "stack size: " << stack.size() << '\n';
        assert(stack.size() > 1);
        auto& [node, labelled_children, rr] = stack.back();
        // logger << "top: " << node << ' ' << show(labelled_children) << ' ' << rr << '\n';
        if (countCards(node) == 7 || !labelled_children.empty()) {
            // logger << "countRanks(node) == 7 || !labelled_children.empty()\n";
            auto& result = memory[node] = f(node, labelled_children.begin(), labelled_children.end());
            std::get<labelled_children_t>(stack[stack.size() - rr - 2])[rr].second = result;
            // pop stack after using the record because we only take a reference
            stack.pop_back();
            continue;
        }
        // logger << "countRanks(node) != 7 && labelled_children.empty()\n";
        labelled_children.reserve(13);
        for (std::uint32_t r = 0; r < 13; ++r) {
            const auto child = node.addRank(r);
            if (child == node) {
                continue;
            }

            if (const auto result = memory.find(child); result != memory.end()) {
                labelled_children.emplace_back(r, result->second);
                continue;
            }

            stack.emplace_back(child, labelled_children_t{}, labelled_children.size());
            labelled_children.emplace_back(r, result_t{});
        }
        assert(stack.size() >= 1);
    }
    // logger << "iteration count: " << i << '\n';
    return std::get<labelled_children_t>(stack[0])[0].second;
}
