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

#ifndef KNUTH_BENDIX_H
#define KNUTH_BENDIX_H

#include <cstdio>

#include <string>
#include <vector>
#include <concepts>
#include <iostream>

///@mainpage Knuth-Bendix
///
/// This is a header-only c++20 library for running the [Knuth-Bendix
/// procedure](https://en.wikipedia.org/wiki/Knuth%E2%80%93Bendix_completion_algorithm)
/// with different rule lookup strategies. Lookup strategies must expose member
/// functions for inserting and removing rules, and rewriting strings. Three such
/// strategies are implemented by default: 
///     - @ref KnuthBendix::NaiveLookup which simply checks every active rule,
///     - @ref KnuthBendix::OrderedLookup which stores rules in reverse
///       lexicorgraphic order to find suffixes using a binary search,
///     - @ref KnuthBendix::TrieLookup which can find prefixes in time linear to
///       the length of the string.
///
/// @code
///     std::vector<KnuthBendix::Rule> rules = {
///         KnuthBendix::Rule {"aa",     ""},
///         KnuthBendix::Rule {"bbb",    ""},
///         KnuthBendix::Rule {"ababab", ""}
///     };
///
///     std::vector<KnuthBendix::Rule> confluent =  KnuthBendix::knuthbendix<KnuthBendix::TrieLookup>(rules);
/// @endcode

/// This is the root namespace
namespace KnuthBendix {

using RuleIndex = size_t;

inline auto shortlex_compare(const std::string& lhs, const std::string& rhs) {
    auto comp = lhs.length() <=> rhs.length();

    if (comp == std::strong_ordering::equal) {
        return lhs <=> rhs;
    } else {
        return comp;
    }
}

/// @brief A rule contains two strings, and defines a mapping from the left to the right.
struct Rule {
    /// The left hand side of the mapping, is greater than the rhs under shortlex 
    std::string left;
    /// The right hand side of the mapping, is less than the lhs under shortlex 
    std::string right;

    Rule() = delete;
    Rule(std::string _left, std::string _right) : left(std::move(_left)), right(std::move(_right)) {};

    /// @brief Swaps left and right such that left <= right under
    ///        length-lexicographical ordering.
    int order() {
        if (shortlex_compare(this->left, this->right) == std::strong_ordering::less) {
            std::swap(this->left, this->right);
            return 1;
        }

        return 0;
    }
};

/// This defines the interface to a lookup strategy.
/// 
/// @brief Lookup strategies provide methods to rewrite strings, and to update
/// their internal data structures when rules are inserted and removed.
/// 
/// Lookups are intended to take and store references to the rule list, active
/// list and number of active rules.
template<class L>
concept Lookup = requires(
    L l,
    const std::vector<Rule>& rules,
    const std::vector<bool>& active,
    const int& n_active,
    const Rule& rule,
    RuleIndex idx,
    std::string& string
) {
    { L(rules, active, n_active, 100) } -> std::same_as<L>;
    /// Insert a new rule, @p rule, to internal state with index @p idx in @ref
    /// rules.
    l.insert(rule, idx);
    /// Removes the rule @p rule from internal state.
    l.remove(rule);
    /// Exhaustively rewrite the string using the active rules.
    l.rewrite(string);
};

/// @brief Contains the state of the Knuth-Bendix procedure.
template<Lookup L>
struct KnuthBendixState {
    /// The stack of rules which are yet to be added to the list of active rules.
    std::vector<Rule> stack;

    // TODO: This grows continuously, could pause the procedure and prune after
    //       a number of inactive rules?
    /// The list of all rules which have been active. 
    std::vector<Rule> rules;

    /// If an entry is true, then the corresponding entry in @ref rules is active.
    std::vector<bool> active;

    /// The number of active rules.
    int n_active;

    /// The rule lookup struct.
    L lookup;

    /// @brief Returns whether the rule at index @p i in @ref rules is active.
    inline bool is_active(RuleIndex i) {
        return this->active[i];
    }

    /// @brief Adds a new rule to the list of active rules.
    void add_rule(Rule&& rule) {
        this->active.push_back(true);
        this->n_active += 1;
        this->rules.push_back(std::move(rule));

        this->lookup.insert(this->rules[rules.size() - 1], rules.size() - 1);
    }

    /// @brief Removes a rule from the list of active rules and places it on the stack.
    void remove_rule(Rule&& to_check, int i) {
        this->active[i] = false;
        this->n_active -= 1;
        this->stack.push_back(to_check);

        this->lookup.remove(to_check);
    }

    /// @brief Pops each rule from the stack, adds it to the rewriting system,
    /// and removes newly redundent rules.
    void clear_stack() {
        while (!this->stack.empty()) {
            Rule rule = std::move(this->stack.back());
            this->stack.pop_back();
            this->lookup.rewrite(rule.left);
            this->lookup.rewrite(rule.right);

            if (rule.left != rule.right) {
                rule.order();
                this->add_rule(std::move(rule));
                Rule& rule_ = this->rules.back();

                for (RuleIndex i = 0; i < this->rules.size() - 1; i++) {
                    if (this->is_active(i)) {
                        Rule& to_check = this->rules[i];
                        if (to_check.left.find(rule_.left) != std::string::npos) {
                            this->remove_rule(std::move(to_check), i);
                        } else if (to_check.right.find(rule_.left) != std::string::npos) {
                            this->lookup.rewrite(to_check.right);
                        }
                    }
                }
            }
        }
    }

    /// @brief checks for all overlaps between the end of the rule at index @p i
    /// and the rule at index @p j. Creates a new rule if rewriting the
    /// resulting string differs using the two rules.
    void overlap(RuleIndex i, RuleIndex j) {
        for (RuleIndex k = 1; k < std::min(rules[i].left.length(), rules[j].left.length()); k++) {
            Rule& a = this->rules[i];
            Rule& b = this->rules[j];
            std::string_view suffix {a.left.data() + (a.left.length() - k), k};

            if (b.left.starts_with(suffix)) {
                // a.left = XY, b.left = YZ
                // xbr = X|b.right, arz = a.right|Z
                std::string xbr;
                xbr.reserve(a.left.length() - k + b.right.length());
                xbr.append(a.left, 0, a.left.length() - k);
                xbr.append(b.right);

                std::string arz;
                arz.reserve(a.right.length() + b.left.length() - k);
                arz.append(a.right);
                arz.append(b.left, k, b.left.length() - k);

                this->stack.emplace_back(xbr, arz);
                this->clear_stack();

                if (!(this->is_active(i) && this->is_active(j))) {
                    return;
                }
            }
        }
    }

    KnuthBendixState(const std::vector<Rule>& initial_rules)
        : lookup(rules, active, n_active, initial_rules.size())
    {
        this->stack.reserve(initial_rules.size());
        this->rules.reserve(initial_rules.size());
        this->active.reserve(initial_rules.size());
        this->n_active = 0;

        for (const Rule& rule : initial_rules) {
            this->stack.push_back(rule);
            this->clear_stack();
        }
    }
};

/// @brief Runs the Knuth-Bendix procedure on @p initial_rules, hopefully
/// returning a confluent rule rewriting system.
template<Lookup L>
std::vector<Rule> knuthbendix(const std::vector<Rule>& initial_rules) {
    // Setup
    KnuthBendixState<L> state {initial_rules};

    // Check overlaps
    for (RuleIndex i = 0; i < state.rules.size(); i++) {
        #ifndef BENCHMARK
            printf("\rActive/Generated Rules: %d/%zu", state.n_active, state.rules.size());
            fflush(stdout);
        #endif
        for (RuleIndex j = 0; j <= i && state.is_active(i); j++) {
            if (state.is_active(j)) state.overlap(i, j);
            if (j < i && state.is_active(j) && state.is_active(i)) {
                state.overlap(j, i);
            }
        }
    }

    #ifndef BENCHMARK
        printf("\n");
    #endif

    std::vector<Rule> confluent;
    confluent.reserve(state.n_active);

    for (RuleIndex i = 0; i < state.rules.size(); i++) {
        if (state.is_active(i)) {
            confluent.push_back(std::move(state.rules[i]));
        }
    }

    return confluent;
}

}
#endif
