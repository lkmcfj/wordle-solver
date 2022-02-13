#include "common.hpp"
#include "solver.hpp"
#include "strategy.hpp"

int main(int argc, char *argv[]) {
    assert(argc == 4);
    load_vocabulary(argv[1], argv[2]);
    std::unique_ptr<GuessNode> root = std::make_unique<GuessNode>(0);
    auto value = Minimax::search(root.get(), candidate_words);
    std::cout << "value of the game: " << value << '\n';
    StrategyTree strategy{root.get()};
    strategy.dump(argv[3]);
}