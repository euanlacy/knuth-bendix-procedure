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

#ifndef ORDERED_LOOKUP_HPP
#define ORDERED_LOOKUP_HPP

#include <iostream>
#include <optional>
#include <string>

#include "knuth_bendix.hpp"

namespace KnuthBendix {

std::strong_ordering reverse_lex(const std::string_view lhs, const std::string_view rhs) {
    auto lbegin = lhs.cbegin();
    auto left = lhs.cend();

    auto rbegin = rhs.cbegin();
    auto right = rhs.cend();

    while (left > lbegin && right > rbegin 
           && *left == *right) {
        --left;
        --right;
    }

    if (*left < *right) {
        return std::strong_ordering::less;
    } else if (*left > *right) {
        return std::strong_ordering::greater;
    } else {
        return lhs.length() <=> rhs.length();
    }
}

std::strong_ordering compare_prefix(const std::string_view lhs, const std::string_view rhs) {
    auto lbegin = lhs.cbegin();
    auto left = lhs.cend() - 1;

    auto rbegin = rhs.cbegin();
    auto right = rhs.cend() - 1;

    while (left > lbegin && right > rbegin 
           && *left == *right) {
        --left;
        --right;
    }

    if (*left < *right) {
        return std::strong_ordering::less;
    } else if (*left > *right) {
        return std::strong_ordering::greater;
    } else if (lhs.length() >= rhs.length()) {
        return std::strong_ordering::equal;
    } else {
        return std::strong_ordering::less;
    }
}

using RuleIndex = size_t;

struct OrderedLookup {
    // References to members of the KnuthBendixState
    const std::vector<Rule>& rules;
    const std::vector<bool>& active;
    const int& n_active;

    // For logrithmic lookup time with respect to the number of rules, indices must be sorted
    // in the reverse lexicorgraphic order of the LHS of the rules to which they refer.
    std::vector<RuleIndex> sorted_indices;

    void print() {
        for (auto index : sorted_indices) {
            std::cout << index << ", ";
        }
        std::cout << std::endl;
    }

    OrderedLookup(
        const std::vector<Rule>& rules,
        const std::vector<bool>& active,
        const int& n_active,
        size_t capacity
    ) : rules(rules), active(active), n_active(n_active) {
        this->sorted_indices.reserve(capacity);
    }

    RuleIndex binary_search(const Rule& rule) {
        int m;
        int l = 0;
        int r = this->sorted_indices.size() - 1;

        while (l <= r) {
            m = (l + r) / 2;
            const Rule& found = this->rules[this->sorted_indices[m]];

            auto cmp = reverse_lex(rule.left, found.left);

            if (cmp == std::strong_ordering::equal) {
                return m;
            } else if (cmp == std::strong_ordering::less) {
                r = m - 1;
            } else {
                l = m + 1;
            }
        }

        return l;
    }

    std::optional<RuleIndex> find_suffix(const std::string_view string) {
        std::optional<RuleIndex> retval;

        int m;
        int l = 0;
        int r = this->sorted_indices.size() - 1;

        while (l <= r) {
            m = (l + r) / 2;
            const Rule& found = this->rules[this->sorted_indices[m]];

            auto cmp = compare_prefix(string, found.left);

            if (cmp == std::strong_ordering::equal) {
                retval = this->sorted_indices[m];
                return retval;
            } else if (cmp == std::strong_ordering::less) {
                r = m - 1;
            } else {
                l = m + 1;
            }
        }

        return retval;
    }

    void rewrite(std::string& string) {
        size_t len = 0;

        while (len != string.length()) {
            len++;

            std::optional<RuleIndex> index = find_suffix({string.data(), len});
            if (index.has_value()) {
                const Rule& rule = this->rules[index.value()];
                len -= rule.left.length();

                // Copy the rhs of the rule
                string.replace(len, rule.right.length(), rule.right);

                // Delete the difference between the lhs and rhs of the rule
                string.erase(len + rule.right.length(), rule.left.length() - rule.right.length());
            }
        }
    }

    void insert(const Rule& rule, size_t idx) {
        RuleIndex index = this->binary_search(rule);

        auto pos = this->sorted_indices.begin() + index;
        this->sorted_indices.insert(pos, idx);
    }

    void remove(const Rule& rule) {
        RuleIndex index = this->binary_search(rule);

        auto pos = this->sorted_indices.begin() + index;
        this->sorted_indices.erase(pos);
    }
};

}
#endif
