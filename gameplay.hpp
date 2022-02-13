#pragma once
#include <iostream>
#include "common.hpp"
#include "strategy.hpp"

class GameHost {
public:
    virtual response_t query(const std::string &query_word) = 0;
};

class BotPlayer {
    StrategyTree *strategy;
public:
    BotPlayer(StrategyTree *s): strategy(s) {}
    void play(GameHost *host) {
        size_t state = 0;
        assert(!strategy->nodes[0].terminal);
        while (true) {
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
                return;
            }
            if (strategy->nodes[state].terminal) {
                std::cout << "Success!\n";
                return;
            }
        }
    }
};