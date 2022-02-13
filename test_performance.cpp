#include "common.hpp"
#include "strategy.hpp"
#include "gameplay.hpp"

int main(int argc, char *argv[]) {
    assert(argc == 3);
    std::ifstream candidate{argv[1]};
    StrategyTree strategy{argv[2]};
    BotPlayer bot{&strategy};
    std::string answer;
    long long sum = 0;
    int test_cnt = 0, worst = 0;
    while (candidate >> answer) {
        ++test_cnt;
        FixedWordGameHost host{answer};
        int steps = bot.play(&host);
        if (steps < 0) {
            std::cout << "Error occured when testing " << answer << '\n';
            return EXIT_FAILURE;
        }
        sum += steps;
        worst = std::max(worst, steps);
    }
    std::cout << "average cost: " << static_cast<double>(sum) / test_cnt << '\n';
    std::cout << "worst cost: " << worst << '\n';
}