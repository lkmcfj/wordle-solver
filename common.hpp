#pragma once
#include <cassert>
#include <utility>
#include <string>
#include <vector>
#include <memory>
#include <fstream>

/*
This global variable style assumes that only one translation unit includes this header.
candidate_words contains the words that could be the final answer.
query_words contains the words that could be used for query.
LEN is the length of all words.
*/
const int LEN = 5, K = 26;
const char START = 'a';
std::vector<std::string> candidate_words, query_words;
using word_id = unsigned int;
using response_t = unsigned int;
const response_t RESPONSE_RANGE = 243; // 3^5
std::vector<std::vector<response_t>> response_table; // first index: query word, second index: candidate word
std::vector<word_id> candidate_query_id; // candidate word id -> query word id

static response_t encode_query_response(response_t result[]) {
    response_t ret = 0;
    for (int i = 0; i < LEN; ++i) {
        ret = ret * 3 + result[i];
    }
    return ret;
}
static response_t get_query_response(const std::string &query, const std::string &answer) {
    response_t response[LEN];
    int cnt[K] = {0};
    for (int i = 0; i < LEN; ++i) {
        if (query[i] == answer[i]) {
            response[i] = 0;
        } else {
            ++cnt[answer[i] - START];
        }
    }
    for (int i = 0; i < LEN; ++i) {
        if (query[i] != answer[i]) {
            if (cnt[query[i] - START] > 0) {
                --cnt[query[i] - START];
                response[i] = 1;
            } else {
                response[i] = 2;
            }
        }
    }
    return encode_query_response(response);
}

static void load_vocabulary(const std::string &candidate_vocab_filename, const std::string &query_vocab_filename) {
    std::ifstream f1{candidate_vocab_filename};
    std::string word;
    while (f1 >> word) {
        assert(word.length() == LEN);
        for (char c : word) {
            assert(c >= START && c < START + K);
        }
        candidate_words.push_back(word);
    }
    std::ifstream f2{query_vocab_filename};
    while (f2 >> word) {
        assert(word.length() == LEN);
        for (char c : word) {
            assert(c >= START && c < START + K);
        }
        query_words.push_back(word);
    }
    size_t qn = query_words.size(), cn = candidate_words.size();
    response_table.resize(qn);
    candidate_query_id.resize(cn);
    for (word_id q = 0; q < qn; ++q) {
        response_table[q].reserve(cn);
        for (word_id c = 0; c < cn; ++c) {
            if (query_words[q] == candidate_words[c]) {
                candidate_query_id[c] = q;
            }
            response_table[q].push_back(get_query_response(query_words[q], candidate_words[c]));
        }
    }
}

struct ResponseNode;
struct GuessNode {
    int depth;
    std::unique_ptr<ResponseNode> child;
    word_id best_query;

    GuessNode(int d): depth(d){}
};
struct ResponseNode {
    int depth;
    word_id query_word;
    std::vector<std::pair<response_t, std::unique_ptr<GuessNode>>> children;

    ResponseNode(int d, word_id q): depth(d), query_word(q) {}
};