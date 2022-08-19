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

    std::cout << "size: " << confluent.size() << std::endl;
}

int main() {
    std::vector<KnuthBendix::Rule> rules {
        KnuthBendix::Rule { "aa",  ""},
        KnuthBendix::Rule { "bc",  ""},
        KnuthBendix::Rule { "cb",  ""},
        KnuthBendix::Rule { "cc",  "b"},
        KnuthBendix::Rule { "cacacac",  "abababa"},
        KnuthBendix::Rule { "acabacabacabacab",  "cabacabacabacaba"}
    };

    run<KnuthBendix::OrderedLookup>(rules);
    /* run<KnuthBendix::TrieLookup>(rules); */
    /* run<KnuthBendix::HatTrieLookup>(rules); */
    /* run<KnuthBendix::NaiveLookup>(rules); */
}
