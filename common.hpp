#pragma once
#include <cassert>
#include <utility>
#include <string>
#include <vector>
#include <memory>

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

response_t encode_query_response(response_t result[]) {
    response_t ret = 0;
    for (int i = 0; i < LEN; ++i) {
        ret = ret * 3 + result[i];
    }
    return ret;
}
response_t get_query_response(const std::string &query, const std::string &answer) {
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
    ResponseNode *parent;
    std::unique_ptr<ResponseNode> child;
    std::string best_query;

    GuessNode(int d, ResponseNode *p): depth(d), parent(p) {}
};
struct ResponseNode {
    int depth;
    GuessNode *parent;
    std::string query_word;
    std::vector<std::pair<response_t, std::unique_ptr<GuessNode>>> children;

    ResponseNode(int d, GuessNode *p, std::string q): depth(d), parent(p), query_word(std::move(q)) {}
};