#pragma once
#include <map>
#include <fstream>
#include "common.hpp"

struct StrategyNode {
    bool terminal;
    std::string query_word;
    std::vector<std::pair<response_t, size_t>> children;
};

struct StrategyTree {
    std::vector<StrategyNode> nodes;

    StrategyTree(GuessNode *root) {
        assert(root->depth == 0);
        std::map<GuessNode*, size_t> node_table;
        dfs_register(root, node_table);
        nodes.resize(node_table.size());
        dfs(root, node_table);
    }
    StrategyTree(const std::string &filename) {
        std::ifstream f{filename};
        size_t total;
        f >> total;
        nodes.resize(total);
        for (size_t i = 0; i < total; ++i) {
            f >> nodes[i].query_word;
            if (nodes[i].query_word == "terminal") {
                nodes[i].terminal = true;
                nodes[i].query_word.clear();
            } else {
                assert(nodes[i].query_word.length() == LEN);
                nodes[i].terminal = false;
                size_t children_cnt;
                f >> children_cnt;
                nodes[i].children.reserve(children_cnt);
                for (size_t j = 0; j < children_cnt; ++j) {
                    std::pair<response_t, size_t> cur;
                    f >> cur.first >> cur.second;
                    nodes[i].children.push_back(cur);
                }
            }
        }
    }
    void dump(const std::string &filename) {
        std::ofstream f{filename};
        f << nodes.size() << '\n';
        for (auto &i : nodes) {
            if (i.terminal) {
                f << "terminal\n";
            } else {
                f << i.query_word << ' ' << i.children.size() << ' ';
                for (auto &j : i.children) {
                    f << j.first << ' ' << j.second << ' ';
                }
                f << '\n';
            }
        }
    }
private:
    void dfs_register(GuessNode *node, std::map<GuessNode*, size_t> &node_table) {
        size_t cur = node_table.size();
        node_table[node] = cur;
        if (node->child) {
            for (auto &i : node->child->children) {
                dfs_register(i.second.get(), node_table);
            }
        }
    }
    void dfs(GuessNode *node, std::map<GuessNode*, size_t> &node_table) {
        size_t cur = node_table[node];
        if (node->child) {
            nodes[cur].terminal = false;
            nodes[cur].query_word = node->best_query;
            nodes[cur].children.reserve(node->child->children.size());
            for (auto &i : node->child->children) {
                nodes[cur].children.emplace_back(i.first, node_table[i.second.get()]);
                dfs(i.second.get(), node_table);
            }
        } else {
            nodes[cur].terminal = true;
        }
    }
};

