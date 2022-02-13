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

class ConsoleRespond : public GameHost {
    static bool legal(const std::string &s) {
        if (s.length() != LEN) return false;
        for (char c : s) {
            if (c != '0' && c != '1' && c != '2') {
                return false;
            }
        }
        return true;
    }
public:
    response_t query(const std::string &query_word) override {
        std::cout << "guess: " << query_word << "\nresponse: ";
        std::string response;
        std::cin >> response;
        while (!legal(response)) {
            std::cout << "Please input 5 digits consisting of 0, 1 and 2.\n0 stands for green, 1 stands for yellow, and 2 stands for grey.\nresponse: ";
            std::cin >> response;
        }
        response_t buf[LEN];
        for (int i = 0; i < LEN; ++i) buf[i] = response[i] - '0';
        return encode_query_response(buf);
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