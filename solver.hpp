#pragma once
#include <cmath>
#include <iostream>
#include <algorithm>
#include "common.hpp"

static double calc_entropy(const std::vector<std::string> &words, const std::string &query, double hit_bonus) {
    int cnt[RESPONSE_RANGE] = {0};
    for (auto &s : words) ++cnt[get_query_response(query, s)];
    int non_zero = 0;
    double ret = 0;
    for (response_t i = 0; i < RESPONSE_RANGE; ++i) {
        if (cnt[i]) {
            ++non_zero;
            double per = static_cast<double>(cnt[i]) / words.size();
            ret -= per * log2(per);
        }
    }
    if (non_zero == 1) ret = 0;
    if (cnt[0] > 0) ret += hit_bonus;
    return ret;
}

static std::vector<std::string> select_query_words(const std::vector<std::string> &words) {
    if (words.size() <= 2) {
        return words;
    }
    std::vector<std::pair<std::string, double>> select;
    select.reserve(query_words.size());
    for (auto &s : query_words) {
        double entropy = calc_entropy(words, s, 1.0 / words.size());
        if (entropy > 0) {
            select.emplace_back(s, entropy);
        }
    }
    std::sort(select.begin(), select.end(), [](const std::pair<std::string, double> &l, const std::pair<std::string, double> &r) {
        return l.second > r.second;
    });
    size_t useful = std::min(select.size(), words.size() * 2);
    std::vector<std::string> ret;
    ret.reserve(useful);
    for (size_t i = 0; i < useful; ++i) ret.push_back(std::move(select[i].first));
    return ret;
}

namespace Minimax {
    using value_t = int;
    static value_t search(ResponseNode *node, value_t parent_best, const std::vector<std::string> &words);
    static value_t search(GuessNode *node, const std::vector<std::string> &words) {
        const value_t INF = 1000000000; // 1e9
        assert(!node->child);
        value_t min_child_value = INF;
        int process = 0;
        for (auto &s : select_query_words(words)) {
            std::unique_ptr<ResponseNode> new_child = std::make_unique<ResponseNode>(node->depth + 1, s);
            value_t cur = search(new_child.get(), min_child_value, words);
            if (cur < min_child_value) {
                node->child = std::move(new_child);
                node->best_query = s;
                min_child_value = cur;
            }
            if (node->depth == 0) {
                ++process;
                std::cerr << "root children: " << process << ", word: " << s << ", value: " << cur << '\n';
            }
        }
        return min_child_value;
    }
    static value_t search(ResponseNode *node, value_t parent_best, const std::vector<std::string> &words) {
        std::vector<std::string> possible_responses[RESPONSE_RANGE];
        for (const auto &s : words) {
            possible_responses[get_query_response(node->query_word, s)].push_back(s);
        }
        value_t max_child_value = 0;
        if (possible_responses[0].size()) {
            std::unique_ptr<GuessNode> cur_child = std::make_unique<GuessNode>(node->depth);
            node->children.emplace_back(0, std::move(cur_child));
            max_child_value = std::max(max_child_value, static_cast<value_t>(node->depth));
        }
        if (max_child_value >= parent_best) return max_child_value;
        for (response_t i = 1; i < RESPONSE_RANGE; ++i) {
            if (possible_responses[i].size()) {
                std::unique_ptr<GuessNode> cur_child = std::make_unique<GuessNode>(node->depth);
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
    static value_t search(ResponseNode *node, value_t parent_best, const std::vector<std::string> &words);
    static value_t search(GuessNode *node, const std::vector<std::string> &words) {
        const value_t INF = 1000000000000000000LL; // 1e18
        assert(!node->child);
        value_t min_child_value = INF;
        int process = 0;
        for (auto &s : select_query_words(words)) {
            std::unique_ptr<ResponseNode> new_child = std::make_unique<ResponseNode>(node->depth + 1, s);
            value_t cur = search(new_child.get(), min_child_value, words);
            if (cur < min_child_value) {
                node->child = std::move(new_child);
                node->best_query = s;
                min_child_value = cur;
            }
            if (node->depth == 0) {
                ++process;
                std::cerr << "root children: " << process << ", word: " << s << ", value: " << cur << '\n';
            }
        }
        return min_child_value;
    }
    static value_t search(ResponseNode *node, value_t parent_best, const std::vector<std::string> &words) {
        std::vector<std::string> possible_responses[RESPONSE_RANGE];
        for (const auto &s : words) {
            possible_responses[get_query_response(node->query_word, s)].push_back(s);
        }
        value_t sum_child_value = 0;
        if (possible_responses[0].size()) {
            std::unique_ptr<GuessNode> cur_child = std::make_unique<GuessNode>(node->depth);
            node->children.emplace_back(0, std::move(cur_child));
            assert(possible_responses[0].size() == 1);
            sum_child_value += static_cast<value_t>(node->depth);
        }
        if (sum_child_value >= parent_best) return sum_child_value;
        for (response_t i = 1; i < RESPONSE_RANGE; ++i) {
            if (possible_responses[i].size()) {
                std::unique_ptr<GuessNode> cur_child = std::make_unique<GuessNode>(node->depth);
                value_t cur = search(cur_child.get(), possible_responses[i]);
                sum_child_value += cur;
                node->children.emplace_back(i, std::move(cur_child));
                if (sum_child_value >= parent_best) return sum_child_value;
            }
        }
        return sum_child_value;
    }
}