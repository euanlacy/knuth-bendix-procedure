// Copyright (C) 2022 Euan Lacy
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef TRIE_LOOKUP_HPP
#define TRIE_LOOKUP_HPP

#include <array>
#include <iostream>
#include <memory>
#include <optional>

#include "knuth_bendix.hpp"

namespace KnuthBendix {

struct TrieLookup {
    const static size_t ALPHABET_SIZE = 256;
    struct TrieNode;

    // TODO: custom allocator which allocates TrieNodes contiguously for better
    //       cache performance.
    using TrieNodePtr = std::unique_ptr<TrieNode>;
    using RuleIndex = size_t;

    struct TrieNode {
        std::array<TrieNodePtr, ALPHABET_SIZE> children; 
        std::optional<RuleIndex> index;
    };

    const std::vector<Rule>& rules;
    const std::vector<bool>& active;
    const int& n_active;

    std::unique_ptr<TrieNode> root;

    TrieLookup() = delete;

    TrieLookup(
        const std::vector<Rule>& rules,
        const std::vector<bool>& active,
        const int& n_active,
        size_t capacity
    ) : rules(rules), active(active), n_active(n_active) {
        this->root = std::make_unique<TrieNode>();
        (void)capacity;
    }

    std::optional<RuleIndex> find(const std::string_view string) {
        std::optional<RuleIndex> index;
        TrieNode* trie = this->root.get();

        for (char c : string) {
            if (!trie->children[c]) {
                return index;
            }

            trie = trie->children[c].get();
            if (trie->index.has_value()) {
                index = trie->index;
            }
        }

        return index;
    }

    void rewrite(std::string& string) {
        size_t pos = 0;

        while (pos != string.length()) {
            std::optional<RuleIndex> index = find({string.data() + pos, string.length() - pos});
            if (index.has_value()) {
                const Rule& rule = this->rules[index.value()];
                // Copy the rhs of the rule
                string.replace(pos, rule.right.length(), rule.right);

                // Delete the difference between the lhs and rhs of the rule
                string.erase(pos + rule.right.length(), rule.left.length() - rule.right.length());

                pos = 0;
            } else {
                pos++;
            }
        }
    }

    void insert(const Rule& rule, RuleIndex idx) {
        TrieNode* trie = this->root.get();

        for (char c : rule.left) {
            TrieNodePtr& ptr = trie->children[c];

            if (!ptr) {
                ptr = std::make_unique<TrieNode>();
            }

            trie = ptr.get();
        }

        trie->index = idx;
    }

    // Assumes the rule must exist
    void remove(const Rule& rule) {
        TrieNode* trie = this->root.get();

        for (char c : rule.left) {
            trie = trie->children[c].get();
        }

        trie->index = {};
    }
};

}
#endif
