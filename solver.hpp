#pragma once
#include <cmath>
#include <iostream>
#include <algorithm>
#include "common.hpp"

static double calc_entropy(const std::vector<word_id> &words, word_id query, double hit_bonus) {
    int cnt[RESPONSE_RANGE] = {0};
    for (auto &i : words) ++cnt[response_table[query][i]];
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

static std::vector<word_id> select_query_words(const std::vector<word_id> &words) {
    if (words.size() <= 2) {
        std::vector<word_id> ret;
        ret.push_back(candidate_query_id[words[0]]);
        return ret;
    }
    std::vector<std::pair<word_id, double>> select;
    select.reserve(query_words.size());
    for (word_id i = 0; i < query_words.size(); ++i) {
        double entropy = calc_entropy(words, i, 1.0 / words.size());
        if (entropy > 0) {
            select.emplace_back(i, entropy);
        }
    }
    std::sort(select.begin(), select.end(), [](const std::pair<word_id, double> &l, const std::pair<word_id, double> &r) {
        return l.second > r.second;
    });
    size_t useful = std::min(select.size(), static_cast<size_t>(log2(words.size()) * 2));
    std::vector<word_id> ret;
    ret.reserve(useful);
    for (size_t i = 0; i < useful; ++i) ret.push_back(select[i].first);
    return ret;
}

namespace AverageOptimal {
    using value_t = long long;
    static value_t search(ResponseNode *node, value_t parent_best, const std::vector<word_id> &words);
    static value_t search(GuessNode *node, const std::vector<word_id> &words) {
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
                std::cerr << "root children: " << process << ", word: " << query_words[s] << ", value: " << cur << '\n';
            }
        }
        return min_child_value;
    }
    static value_t search(ResponseNode *node, value_t parent_best, const std::vector<word_id> &words) {
        std::vector<word_id> possible_responses[RESPONSE_RANGE];
        for (word_id i : words) {
            possible_responses[response_table[node->query_word][i]].push_back(i);
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