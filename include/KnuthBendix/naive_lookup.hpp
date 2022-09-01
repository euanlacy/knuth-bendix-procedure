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

#ifndef NAIVE_LOOKUP_HPP
#define NAIVE_LOOKUP_HPP

#include <iostream>

#include "knuth_bendix.hpp"

namespace KnuthBendix {

/// @brief This lookup strategy simply searches every rule in the rule
/// list for a substring match, as such it requires no additional data
/// structures, but has time complexity which scales linearly with the number
/// of rules in the system.
struct NaiveLookup {
    const std::vector<Rule>& rules;
    const std::vector<bool>& active;
    const int& n_active;

    NaiveLookup(
        const std::vector<Rule>& rules,
        const std::vector<bool>& active,
        const int& n_active,
        size_t capacity
    ) : rules(rules), active(active), n_active(n_active) {
        (void)capacity;
    }

    void rewrite(std::string& string) {
        size_t v_len   = 0;
        size_t u_start = 0;

        while (u_start != string.length()) {
            // Move character at end of u to start of v
            string[v_len] = string[u_start];
            v_len++;
            u_start++;

            // for (p, q) in rules
            for (RuleIndex i = 0; i < this->rules.size(); i++) {
                if (!this->active[i]) continue;
                // if v = rp
                const Rule& rule = this->rules[i];
                std::string_view view {string.data(), v_len};
                size_t pos = view.find(rule.left);

                if (pos != string.npos) {
                    // v <- r
                    v_len = pos;

                    // string = rpu
                    // u <- qu
                    u_start -= rule.right.length();
                    string.replace(u_start, rule.right.length(), rule.right);
                    // string = rxqu, where x is garbage

                    break;
                }
            }
        }

        // String might be shorter than old one
        string.erase(v_len, string.length());
    }

    void inline insert(const Rule& rule, RuleIndex idx) {
        (void)rule;
        (void)idx;
    }

    void inline remove(const Rule& rule) {
        (void)rule;
    }
};

}
#endif
