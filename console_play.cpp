#include "common.hpp"
#include "gameplay.hpp"
#include "strategy.hpp"

int main(int argc, char *argv[]) {
    assert(argc == 2);
    StrategyTree strategy{argv[1]};
    BotPlayer bot{&strategy};
    ConsoleRespond host;
    bot.play(&host);
}