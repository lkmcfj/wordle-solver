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
const int LEN = 5;
std::vector<std::string> candidate_words, query_words;
using response_t = unsigned int;
const response_t RESPONSE_RANGE = 243; // 3^5

static void load_vocabulary(const std::string &candidate_vocab_filename, const std::string &query_vocab_filename) {
    std::ifstream f1{candidate_vocab_filename};
    std::string word;
    while (f1 >> word) {
        assert(word.length() == LEN);
        candidate_words.push_back(word);
    }
    std::ifstream f2{query_vocab_filename};
    while (f2 >> word) {
        assert(word.length() == LEN);
        query_words.push_back(word);
    }
}

static response_t encode_query_response(response_t result[]) {
    response_t ret = 0;
    for (int i = 0; i < LEN; ++i) {
        ret = ret * 3 + result[i];
    }
    return ret;
}
static response_t get_query_response(const std::string &query, const std::string &answer) {
    response_t response[LEN];
    for (int i = 0; i < LEN; ++i) {
        if (query[i] == answer[i]) {
            response[i] = 0;
        } else {
            response[i] = 2;
            for (int j = 0; j < LEN; ++j) {
                if (query[i] == answer[j] && query[j] != answer[j]) {
                    response[i] = 1;
                    break;
                }
            }
        }
    }
    return encode_query_response(response);
}

struct ResponseNode;
struct GuessNode {
    int depth;
    std::unique_ptr<ResponseNode> child;
    std::string best_query;

    GuessNode(int d): depth(d){}
};
struct ResponseNode {
    int depth;
    std::string query_word;
    std::vector<std::pair<response_t, std::unique_ptr<GuessNode>>> children;

    ResponseNode(int d, std::string q): depth(d), query_word(std::move(q)) {}
};