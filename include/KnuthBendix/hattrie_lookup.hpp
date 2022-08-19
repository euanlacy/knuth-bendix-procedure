#ifndef HATTRIE_LOOKUP_HPP
#define HATTRIE_LOOKUP_HPP

#include <array>
#include <iostream>
#include <memory>
#include <optional>

#include <tsl/htrie_map.h>

#include "knuth_bendix.hpp"

namespace KnuthBendix {

// Hashing is too expensive for our small strings
struct HatTrieLookup {
    using RuleIndex = size_t;

    const std::vector<Rule>& rules;
    const std::vector<bool>& active;
    const int& n_active;

    tsl::htrie_map<char, int> map;

    HatTrieLookup() = delete;

    HatTrieLookup(
        const std::vector<Rule>& rules,
        const std::vector<bool>& active,
        const int& n_active,
        size_t capacity
    ) : rules(rules), active(active), n_active(n_active) {
        // Optimise for prefix searches and speed
        this->map.burst_threshold(128);
        this->map.max_load_factor(1.0);
        (void)capacity;
    }

    void rewrite(std::string& string) {
        size_t pos = 0;

        while (pos != string.length()) {
            auto index = this->map.longest_prefix({string.data() + pos, string.length() - pos});
            if (index != this->map.end()) {
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

    void inline insert(const Rule& rule, RuleIndex idx) {
        this->map.insert(rule.left, idx);
    }

    // Assumes the rule must exist
    void inline remove(const Rule& rule) {
        this->map.erase(rule.left);
    }
};

}
#endif
