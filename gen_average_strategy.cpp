#include "common.hpp"
#include "solver.hpp"
#include "strategy.hpp"

int main(int argc, char *argv[]) {
    assert(argc == 4);
    load_vocabulary(argv[1], argv[2]);
    std::unique_ptr<GuessNode> root = std::make_unique<GuessNode>(0);
    std::vector<word_id> universal_set;
    universal_set.reserve(candidate_words.size());
    for (word_id i = 0; i < candidate_words.size(); ++i) {
        universal_set.push_back(i);
    }
    auto value = AverageOptimal::search(root.get(), universal_set);
    std::cout << "value of the game: " << value << '\n';
    StrategyTree strategy{root.get()};
    strategy.dump(argv[3]);
}