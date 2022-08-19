#include <iostream>
#include <vector>

#include <KnuthBendix/knuth_bendix.hpp>
#include <KnuthBendix/naive_lookup.hpp>
#include <KnuthBendix/ordered_lookup.hpp>
#include <KnuthBendix/trie_lookup.hpp>
#include <KnuthBendix/hattrie_lookup.hpp>

template<KnuthBendix::Lookup L>
void run(std::vector<KnuthBendix::Rule>& rules) {
    auto confluent = KnuthBendix::knuthbendix<L>(rules);

    for (auto& rule : confluent) {
       std::cout << "\"" << rule.left << "\" -> \"" << rule.right << "\"" << std::endl;
    }
}

int main() {
    // "aa"    -> ""
    // "bbb"   -> ""
    // "abba"  -> "bab"
    // "baba"  -> "abb"
    // "abab"  -> "bba"
    // "bbabb" -> "aba"

    // Should be 6 rules
    std::vector<KnuthBendix::Rule> rules = {
        KnuthBendix::Rule {"aa",     ""},
        KnuthBendix::Rule {"bbb",    ""},
        KnuthBendix::Rule {"ababab", ""}
    };

    run<KnuthBendix::HatTrieLookup>(rules);
    run<KnuthBendix::TrieLookup>(rules);
    run<KnuthBendix::NaiveLookup>(rules);
    run<KnuthBendix::OrderedLookup>(rules);
}
