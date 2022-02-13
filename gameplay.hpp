#pragma once
#include <iostream>
#include "common.hpp"
#include "strategy.hpp"

class GameHost {
public:
    virtual response_t query(const std::string &query_word) = 0;
};

class FixedWordGameHost : public GameHost {
    std::string answer;
public:
    FixedWordGameHost(std::string a): answer(std::move(a)) {
        assert(answer.length() == LEN);
    }
    response_t query(const std::string &query_word) override {
        return get_query_response(query_word, answer);
    }
};

class BotPlayer {
    StrategyTree *strategy;
public:
    BotPlayer(StrategyTree *s): strategy(s) {}
    int play(GameHost *host) {
        size_t state = 0;
        assert(!strategy->nodes[0].terminal);
        int ret = 0;
        while (true) {
            ++ret;
            response_t r = host->query(strategy->nodes[state].query_word);
            bool found = false;
            for (auto &i : strategy->nodes[state].children) {
                if (i.first == r) {
                    found = true;
                    state = i.second;
                    break;
                }
            }
            if (!found) {
                std::cout << "Invalid response: the answer is out of the vocabulary used to build this strategy.\n";
                return -1;
            }
            if (strategy->nodes[state].terminal) {
                return ret;
            }
        }
    }
};