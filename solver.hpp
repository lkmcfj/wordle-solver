#pragma once
#include "common.hpp"

std::vector<std::string> select_query_words(const std::vector<std::string> &words);

namespace Minimax {
    using value_t = int;
    value_t search(GuessNode *node, const std::vector<std::string> &words) {
        const value_t INF = 1000000000; // 1e9
        assert(!node->child);
        value_t min_child_value = INF;
        for (auto &s : select_query_words(words)) {
            std::unique_ptr<ResponseNode> new_child = std::make_unique<ResponseNode>(node->depth + 1, node, s);
            value_t cur = search(new_child.get(), min_child_value, words);
            if (cur < min_child_value) {
                node->child = std::move(new_child);
                node->best_query = s;
                min_child_value = cur;
            }
        }
        return min_child_value;
    }
    value_t search(ResponseNode *node, value_t parent_best, const std::vector<std::string> &words) {
        std::vector<std::string> possible_responses[RESPONSE_RANGE];
        for (const auto &s : words) {
            possible_responses[get_query_response(node->query_word, s)].push_back(s);
        }
        value_t max_child_value = 0;
        if (possible_responses[0].size()) {
            std::unique_ptr<GuessNode> cur_child = std::make_unique<GuessNode>(node->depth, node);
            node->children.emplace_back(0, std::move(cur_child));
            max_child_value = std::max(max_child_value, static_cast<value_t>(node->depth));
        }
        if (max_child_value >= parent_best) return max_child_value;
        for (response_t i = 1; i < RESPONSE_RANGE; ++i) {
            if (possible_responses[i].size()) {
                std::unique_ptr<GuessNode> cur_child = std::make_unique<GuessNode>(node->depth, node);
                value_t cur = search(cur_child.get(), possible_responses[i]);
                max_child_value = std::max(max_child_value, cur);
                node->children.emplace_back(i, std::move(cur_child));
                if (max_child_value >= parent_best) return max_child_value;
            }
        }
        return max_child_value;
    }
}

namespace AverageOptimal {
    using value_t = long long;
    value_t search(GuessNode *node, const std::vector<std::string> &words) {
        const value_t INF = 1000000000000000000LL; // 1e18
        assert(!node->child);
        value_t min_child_value = INF;
        for (auto &s : select_query_words(words)) {
            std::unique_ptr<ResponseNode> new_child = std::make_unique<ResponseNode>(node->depth + 1, node, s);
            value_t cur = search(new_child.get(), min_child_value, words);
            if (cur < min_child_value) {
                node->child = std::move(new_child);
                node->best_query = s;
                min_child_value = cur;
            }
        }
        return min_child_value;
    }
    value_t search(ResponseNode *node, value_t parent_best, const std::vector<std::string> &words) {
        std::vector<std::string> possible_responses[RESPONSE_RANGE];
        for (const auto &s : words) {
            possible_responses[get_query_response(node->query_word, s)].push_back(s);
        }
        value_t sum_child_value = 0;
        if (possible_responses[0].size()) {
            std::unique_ptr<GuessNode> cur_child = std::make_unique<GuessNode>(node->depth, node);
            node->children.emplace_back(0, std::move(cur_child));
            assert(possible_responses[0].size() == 1);
            sum_child_value += static_cast<value_t>(node->depth);
        }
        if (sum_child_value >= parent_best) return sum_child_value;
        for (response_t i = 1; i < RESPONSE_RANGE; ++i) {
            if (possible_responses[i].size()) {
                std::unique_ptr<GuessNode> cur_child = std::make_unique<GuessNode>(node->depth, node);
                value_t cur = search(cur_child.get(), possible_responses[i]);
                sum_child_value += cur;
                node->children.emplace_back(i, std::move(cur_child));
                if (sum_child_value >= parent_best) return sum_child_value;
            }
        }
        return sum_child_value;
    }
}