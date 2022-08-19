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

#define CATCH_CONFIG_FAST_COMPILE

#include <string>

#include <catch2/catch.hpp>

#include <KnuthBendix/knuth_bendix.hpp>
#include <KnuthBendix/naive_lookup.hpp>
#include <KnuthBendix/ordered_lookup.hpp>
#include <KnuthBendix/trie_lookup.hpp>
#include <KnuthBendix/hattrie_lookup.hpp>

using namespace KnuthBendix;

//TEST_CASE("Sub Search", "[sub]") {
//    State state;
//    state.ordered_rules = OrderedRules_new(10);
//
//    OrderedRules_insert(state.ordered_rules, new_rule("a", ""},
//    OrderedRules_insert(state.ordered_rules, new_rule("aa", ""},
//    OrderedRules_insert(state.ordered_rules, new_rule("ac", ""},
//    OrderedRules_insert(state.ordered_rules, new_rule("b", ""},
//    OrderedRules_insert(state.ordered_rules, new_rule("z", ""},
//
//    SECTION("a") {
//        Rule found;
//        int ret = OrderedRules_find_prefix(state.ordered_rules, "a", 1, &found);
//        CHECK(ret == 1);
//        CHECK(strcmp(found.left, "a") == 0);
//    }
//
//    SECTION("aa") {
//        Rule found;
//        int ret = OrderedRules_find_prefix(state.ordered_rules, "aa", 2, &found);
//        CHECK(ret == 1);
//        CHECK(strcmp(found.left, "aa") == 0);
//    }
//
//    SECTION("z") {
//        Rule found;
//        int ret = OrderedRules_find_prefix(state.ordered_rules, "z", 1, &found);
//        CHECK(ret == 1);
//        CHECK(strcmp(found.left, "z") == 0);
//    }
//
//    SECTION("c") {
//        Rule found;
//        int ret = OrderedRules_find_prefix(state.ordered_rules, "c", 1, &found);
//        CHECK(ret == 0);
//    }
//
//    SECTION("ad") {
//        Rule found;
//        int ret = OrderedRules_find_prefix(state.ordered_rules, "ad", 2, &found);
//        CHECK(ret == 1);
//        CHECK(strcmp(found.left, "a") == 0);
//    }
//
//    SECTION("bcde") {
//        Rule found;
//        int ret = OrderedRules_find_prefix(state.ordered_rules, "bcde", 4, &found);
//        CHECK(ret == 1);
//        CHECK(strcmp(found.left, "b") == 0);
//    }
//
//    SECTION("acde") {
//        Rule found;
//        int ret = OrderedRules_find_prefix(state.ordered_rules, "acde", 4, &found);
//        CHECK(ret == 1);
//        CHECK(strcmp(found.left, "ac") == 0);
//    }
//}

TEMPLATE_TEST_CASE("Rewrite", "[rewrite]", NaiveLookup, OrderedLookup, TrieLookup, HatTrieLookup) {
    std::vector<bool> active;
    int n_active = 0;

    // This is a confluent rewriting system
    std::vector<Rule> rules {
        Rule {"aa",    ""},
        Rule {"bbb",   ""},
        Rule {"abba",  "bab"},
        Rule {"baba",  "abb"},
        Rule {"abab",  "bba"},
        Rule {"bbabb", "aba"}
    };

    TestType lookup {rules, active, n_active, rules.size()};

    size_t idx = 0;
    for (auto& rule : rules) {
        n_active += 1;
        active.push_back(true);
        lookup.insert(rule, idx);
        idx++;
    }

    // Rule that maps to ""
    SECTION("aa") {
        std::string aa = "aa";
        lookup.rewrite(aa);
        CHECK(aa == "");
    }

    // Rule that maps to a non-empty string
    SECTION("bbabb") {
        std::string bbabb = "bbabb";
        lookup.rewrite(bbabb);
        CHECK(bbabb == "aba");
    }

    // String that maps to "" after application of two rules
    SECTION("aabbb") {
        std::string aabbb = "aabbb";
        lookup.rewrite(aabbb);
        CHECK(aabbb == "");
    }

    // String that is not changed
    SECTION("zz") {
        std::string zz = "zz";
        lookup.rewrite(zz);
        CHECK(zz == "zz");
    }

    // String that contains a nested rule
    SECTION("cbbaab") {
        std::string cbbaab = "cbbaab";
        lookup.rewrite(cbbaab);
        CHECK(cbbaab == "c");
    }

    // String that contains a nested rule
    SECTION("abbaaba") {
        std::string abbaaba = "abbaaba";
        lookup.rewrite(abbaaba);
        CHECK(abbaaba == "");
    }
}

/* TEST_CASE("Binary Search") { */
/*     State state; */
/*     state.ordered_rules = OrderedRules_new(10); */

/*     Rule rules[] = { */
/*         new_rule("a", ""), */
/*         new_rule("aa", ""), */
/*         new_rule("ac", ""), */
/*         new_rule("b", ""), */
/*         new_rule("z", ""), */
/*         new_rule("dddd", "") */
/*     }; */

/*     for (auto rule : rules) { */
/*         OrderedRules_insert(state.ordered_rules, rule); */
/*     } */

/*     CHECK(binary_search(state.ordered_rules, rules[0]) == 0); */
/*     CHECK(binary_search(state.ordered_rules, rules[1]) == 0); */
/*     CHECK(binary_search(state.ordered_rules, rules[2]) == 1); */
/*     CHECK(binary_search(state.ordered_rules, rules[3]) == 1); */
/*     CHECK(binary_search(state.ordered_rules, rules[4]) == 2); */
/*     CHECK(binary_search(state.ordered_rules, new_rule("c", "")) == 2); */
/*     CHECK(binary_search(state.ordered_rules, new_rule("ab", "")) == 1); */
/*     CHECK(binary_search(state.ordered_rules, new_rule("zz", "")) == 2); */
/*     CHECK(binary_search(state.ordered_rules, new_rule("abc", "")) == 0); */
/*     CHECK(binary_search(state.ordered_rules, new_rule("aaaa", "")) == 0); */
/* } */

TEMPLATE_TEST_CASE("Knuth-Bendix Implementation", "[knuth]", NaiveLookup, OrderedLookup, TrieLookup, HatTrieLookup) {
    SECTION("Example 5.1") {
        std::vector<Rule> rules {
            Rule {"ab", ""},
            Rule {"ba", ""},
            Rule {"cd", ""},
            Rule {"dc", ""},
            Rule {"ca", "ac"}
        };

        std::vector<Rule> confluent = knuthbendix<TestType>(rules);

        /* REQUIRE(check_confluent(confluent)); */
        REQUIRE(confluent.size() == 8);
    }

    SECTION("Example 5.3") {
        std::vector<Rule> rules {
            Rule {"aa", ""},
            Rule {"bbb", ""},
            Rule {"ababab", ""}
        };

        std::vector<Rule> confluent = knuthbendix<TestType>(rules);

        /* REQUIRE(check_confluent(confluent)); */
        REQUIRE(confluent.size() == 6);
    }

    SECTION("Example 5.4") {
        std::vector<Rule> rules {
            Rule {"aa", ""},
            Rule {"bc", ""},
            Rule {"bbb", ""},
            Rule {"ababab", ""}
        };

        std::vector<Rule> confluent = knuthbendix<TestType>(rules);

        /* REQUIRE(check_confluent(confluent)); */
        REQUIRE(confluent.size() == 11);
    }

    SECTION("Example 6.4") {
        std::vector<Rule> rules {
            Rule {"aa", ""},
            Rule {"bc", ""},
            Rule {"bbb", ""},
            Rule {"ababababababab", ""},
            Rule {"abacabacabacabac", ""},
        };

        std::vector<Rule> confluent = knuthbendix<TestType>(rules);

        /* CHECK(check_confluent(confluent)); */
        CHECK(confluent.size() == 40);
    }

    SECTION("Example 6.5") {
        std::vector<Rule> rules {
            Rule {"aa", ""},
            Rule {"bc", ""},
            Rule {"bb", "c"},
            Rule {"cacacac", "abababa"},
            Rule {"bacabaca", "abacabac"},
        };

        std::vector<Rule> confluent = knuthbendix<TestType>(rules);

        /* CHECK(check_confluent(confluent)); */
        CHECK(confluent.size() == 40);
    }

    SECTION("Reinis example 1") {
        std::vector<Rule> rules {
            Rule {"a", "abb"},
            Rule {"b", "baa"}
        };

        std::vector<Rule> confluent = knuthbendix<TestType>(rules);

        CHECK(confluent.size() == 4);
        /* CHECK(check_confluent(confluent)); */
    }

    SECTION("Libsemi-group test 000") {
        std::vector<Rule> rules {
            Rule {"cb", "bc"},
            Rule {"ca", "ac"},
            Rule {"cc", "c"},
            Rule {"ca", "c"},
            Rule {"ac", "c"},
            Rule {"bb", "bb"},
            Rule {"ba", "ab"},
            Rule {"bbb", "b"},
            Rule {"ba", "b"},
            Rule {"ab", "b"},
            Rule {"c", "b"}
        };

        std::vector<Rule> confluent = knuthbendix<TestType>(rules);

        /* REQUIRE(check_confluent(confluent)); */
        REQUIRE(confluent.size() == 4);
    }

    SECTION("Libsemi-group test 001") {
        std::vector<Rule> rules {
            Rule {"ca", "ac"},
            Rule {"cc", "c"},
            Rule {"ca", "c"},
            Rule {"ac", "c"},
            Rule {"bb", "bb"},
            Rule {"ba", "ab"},
            Rule {"bbb", "b"},
            Rule {"ba", "b"},
            Rule {"ab", "b"},
            Rule {"c", "b"}
        };

        std::vector<Rule> confluent = knuthbendix<TestType>(rules);

        /* REQUIRE(check_confluent(confluent)); */
        REQUIRE(confluent.size() == 4);
    }

    SECTION("Libsemi-group test 003") {
        std::vector<Rule> rules {
            Rule {"aaa", ""},
            Rule {"bbb", ""},
            Rule {"ababab", ""}
        };

        std::vector<Rule> confluent = knuthbendix<TestType>(rules);

        /* REQUIRE(check_confluent(confluent)); */
        REQUIRE(confluent.size() == 4);
    }

    SECTION("Libsemi-group test 009") {
        std::vector<Rule> rules {
            Rule {"02", "0"},
            Rule {"20", "0"},
            Rule {"12", "1"},
            Rule {"21", "1"},
            Rule {"22", "2"},

            Rule {"000", "2"},
            Rule {"111", "2"},
            Rule {"010101", "2"}
        };

        std::vector<Rule> confluent = knuthbendix<TestType>(rules);

        /* REQUIRE(check_confluent(confluent)); */
        REQUIRE(confluent.size() == 9);
    }

    SECTION("Libsemi-group test 010") {
        std::vector<Rule> rules {
            Rule {"aaaaaaa", ""},
            Rule {"ababab", "bb"},
            Rule {"aaaabaaaabaaaabaaaab", "bb"},
            Rule {"aA", ""},
            Rule {"Aa", ""},
            Rule {"bB", ""},
            Rule {"Bb", ""}
        };

        std::vector<Rule> confluent = knuthbendix<TestType>(rules);

        /* REQUIRE(check_confluent(confluent)); */
        REQUIRE(confluent.size() == 152);
    }

    SECTION("Libsemi-group test 011") {
        std::vector<Rule> rules {
            Rule {"ab", "c"},
            Rule {"bc", "d"},
            Rule {"cd", "e"},
            Rule {"de", "a"},
            Rule {"ea", "b"}
        };

        std::vector<Rule> confluent = knuthbendix<TestType>(rules);

        /* CHECK(check_confluent(confluent)); */
        CHECK(confluent.size() == 24);
    }

    SECTION("Libsemi-group test 022") {
        std::vector<Rule> rules {
            Rule {"Aba", "bb"},
            Rule {"Bcb", "cc"},
            Rule {"Cac", "aa"},
            Rule {"aA", ""},
            Rule {"Aa", ""},
            Rule {"bB", ""},
            Rule {"Bb", ""},
            Rule {"Cc", ""},
            Rule {"cC", ""}
        };

        std::vector<Rule> confluent = knuthbendix<TestType>(rules);

        /* REQUIRE(check_confluent(confluent)); */
        REQUIRE(confluent.size() == 6);
    }

    SECTION("Libsemi-group test 023") {
        std::vector<Rule> rules {
            Rule {"ab", ""},
            Rule {"ba", ""},
            Rule {"cd", ""},
            Rule {"dc", ""},
            Rule {"ef", ""},
            Rule {"fe", ""},
            Rule {"gh", ""},
            Rule {"hg", ""},

            Rule {"bfae", "hdgc"}
        };

        std::vector<Rule> confluent = knuthbendix<TestType>(rules);

        /* CHECK(check_confluent(confluent)); */
        CHECK(confluent.size() == 16);
    }

    SECTION("Libsemi-group test 024") {
        std::vector<Rule> rules {
            Rule {"aA", ""},
            Rule {"Aa", ""},
            Rule {"bB", ""},
            Rule {"Bb", ""},

            Rule {"aaa", ""},
            Rule {"bbb", ""},
            Rule {"abababab", ""},
            Rule {"aBaBaBaBaB", ""}
        };

        std::vector<Rule> confluent = knuthbendix<TestType>(rules);

        /* CHECK(check_confluent(confluent)); */
        CHECK(confluent.size() == 183);
    }


    SECTION("Libsemi-group test 025") {
        std::vector<Rule> rules {
            Rule {"ab", ""},
            Rule {"ba", ""},
            Rule {"cd", ""},
            Rule {"dc", ""}
        };

        std::vector<Rule> confluent = knuthbendix<TestType>(rules);

        /* CHECK(check_confluent(confluent)); */
        CHECK(confluent.size() == 4);
    }

    SECTION("Libsemi-group test 026") {
        std::vector<Rule> rules {
            Rule {"aa", ""},
            Rule {"bb", ""},
            Rule {"cc", ""},
            Rule {"dd", ""},
            Rule {"ee", ""},
            Rule {"ff", ""},
            Rule {"gg", ""},
            Rule {"hh", ""},
            Rule {"ii", ""},
            Rule {"jj", ""},
            Rule {"kk", ""},
            Rule {"ll", ""},
            Rule {"mm", ""},
            Rule {"nn", ""},
            Rule {"oo", ""},

            Rule {"bab", "aba"},
            Rule {"ca", "ac"},
            Rule {"da", "ad"},
            Rule {"ea", "ae"},
            Rule {"fa", "af"},
            Rule {"ga", "ag"},
            Rule {"ha", "ah"},
            Rule {"ia", "ai"},
            Rule {"ja", "aj"},
            Rule {"ka", "ak"},
            Rule {"la", "al"},
            Rule {"ma", "am"},
            Rule {"na", "an"},
            Rule {"oa", "ao"},
            Rule {"cbc", "bcb"},
            Rule {"db", "bd"},
            Rule {"eb", "be"},
            Rule {"fb", "bf"},
            Rule {"gb", "bg"},
            Rule {"hb", "bh"},
            Rule {"ib", "bi"},
            Rule {"jb", "bj"},
            Rule {"kb", "bk"},
            Rule {"lb", "bl"},
            Rule {"mb", "bm"},
            Rule {"nb", "bn"},
            Rule {"ob", "bo"},
            Rule {"dcd", "cdc"},
            Rule {"ec", "ce"},
            Rule {"fc", "cf"},
            Rule {"gc", "cg"},
            Rule {"hc", "ch"},
            Rule {"ic", "ci"},
            Rule {"jc", "cj"},
            Rule {"kc", "ck"},
            Rule {"lc", "cl"},
            Rule {"mc", "cm"},
            Rule {"nc", "cn"},
            Rule {"oc", "co"},
            Rule {"ede", "ded"},
            Rule {"fd", "df"},
            Rule {"gd", "dg"},
            Rule {"hd", "dh"},
            Rule {"id", "di"},
            Rule {"jd", "dj"},
            Rule {"kd", "dk"},
            Rule {"ld", "dl"},
            Rule {"md", "dm"},
            Rule {"nd", "dn"},
            Rule {"od", "do"},
            Rule {"fef", "efe"},
            Rule {"ge", "eg"},
            Rule {"he", "eh"},
            Rule {"ie", "ei"},
            Rule {"je", "ej"},
            Rule {"ke", "ek"},
            Rule {"le", "el"},
            Rule {"me", "em"},
            Rule {"ne", "en"},
            Rule {"oe", "eo"},
            Rule {"gfg", "fgf"},
            Rule {"hf", "fh"},
            Rule {"if", "fi"},
            Rule {"jf", "fj"},
            Rule {"kf", "fk"},
            Rule {"lf", "fl"},
            Rule {"mf", "fm"},
            Rule {"nf", "fn"},
            Rule {"of", "fo"},
            Rule {"hgh", "ghg"},
            Rule {"ig", "gi"},
            Rule {"jg", "gj"},
            Rule {"kg", "gk"},
            Rule {"lg", "gl"},
            Rule {"mg", "gm"},
            Rule {"ng", "gn"},
            Rule {"og", "go"},
            Rule {"ihi", "hih"},
            Rule {"jh", "hj"},
            Rule {"kh", "hk"},
            Rule {"lh", "hl"},
            Rule {"mh", "hm"},
            Rule {"nh", "hn"},
            Rule {"oh", "ho"},
            Rule {"jij", "iji"},
            Rule {"ki", "ik"},
            Rule {"li", "il"},
            Rule {"mi", "im"},
            Rule {"ni", "in"},
            Rule {"oi", "io"},
            Rule {"kjk", "jkj"},
            Rule {"lj", "jl"},
            Rule {"mj", "jm"},
            Rule {"nj", "jn"},
            Rule {"oj", "jo"},
            Rule {"lkl", "klk"},
            Rule {"mk", "km"},
            Rule {"nk", "kn"},
            Rule {"ok", "ko"},
            Rule {"mlm", "lml"},
            Rule {"nl", "ln"},
            Rule {"ol", "lo"},
            Rule {"nmn", "mnm"},
            Rule {"om", "mo"},
            Rule {"ono", "non"}
        };

        std::vector<Rule> confluent = knuthbendix<TestType>(rules);

        /* CHECK(check_confluent(confluent)); */
        CHECK(confluent.size() == 211);
    }

    SECTION("Libsemi-group test 027") {
        std::vector<Rule> rules {
            Rule {"bb", "c"},
            Rule {"cac", "aba"}
        };

        std::vector<Rule> confluent = knuthbendix<TestType>(rules);

        /* CHECK(check_confluent(confluent)); */
        CHECK(confluent.size() == 6);
    }

    SECTION("Libsemi-group test 028") {
        std::vector<Rule> rules {
            Rule {"ab", ""},
            Rule {"aab", ""}
        };

        std::vector<Rule> confluent = knuthbendix<TestType>(rules);

        /* CHECK(check_confluent(confluent)); */
        CHECK(confluent.size() == 2);
    }

    SECTION("Libsemi-group test 029") {
        std::vector<Rule> rules {
            Rule {"aA", ""},
            Rule {"Aa", ""}
        };

        std::vector<Rule> confluent = knuthbendix<TestType>(rules);

        /* CHECK(check_confluent(confluent)); */
        CHECK(confluent.size() == 2);
    }

    SECTION("Libsemi-group test 030") {
        std::vector<Rule> rules {
            Rule {"a", ""}
        };

        std::vector<Rule> confluent = knuthbendix<TestType>(rules);

        CHECK(confluent.size() == 1);
    }

    SECTION("Libsemi-group test 031") {
        std::vector<Rule> rules {
            Rule {"ab", "c"},
            Rule {"bc", "d"},
            Rule {"cd", "y"},
            Rule {"dy", "a"},
            Rule {"ya", "b"}
        };

        std::vector<Rule> confluent = knuthbendix<TestType>(rules);

        /* CHECK(check_confluent(confluent)); */
        CHECK(confluent.size() == 24);
    }

    SECTION("Libsemi-group test 032") {
        std::vector<Rule> rules {
            Rule {"ab", ""},
            Rule {"ba", ""},
            Rule {"cd", ""},
            Rule {"dc", ""},
            Rule {"ee", ""},

            Rule {"aaaa", "bbb"},
            Rule {"cc", "d"},
            Rule {"db", "e"}
        };

        std::vector<Rule> confluent = knuthbendix<TestType>(rules);

        /* CHECK(check_confluent(confluent)); */
        CHECK(confluent.size() == 32);
    }

    SECTION("Libsemi-group test 033") {
        std::vector<Rule> rules {
            Rule {"aa", ""}
        };

        std::vector<Rule> confluent = knuthbendix<TestType>(rules);

        CHECK(confluent.size() == 1);
    }

    SECTION("Libsemi-group test 034") {
        std::vector<Rule> rules {
            Rule {"bbb", ""},
            Rule {"dddd", ""},
            Rule {"bdbd", ""},
            Rule {"ad", "a"},
            Rule {"aa", "a"},
            Rule {"ba", "a"},
            Rule {"da", "a"}
        };

        std::vector<Rule> confluent = knuthbendix<TestType>(rules);

        /* CHECK(check_confluent(confluent)); */
        CHECK(confluent.size() == 14);
    }

    SECTION("Libsemi-group test 049") {
        std::vector<Rule> rules {
            Rule {"aa", ""},
            Rule {"bb", ""},
            Rule {"cc", ""},
            Rule {"dd", ""},
            Rule {"ee", ""},
            Rule {"ff", ""},
            Rule {"gg", ""},
            Rule {"hh", ""},

            Rule {"bab", "aba"},
            Rule {"ca", "ac"},
            Rule {"da", "ad"},
            Rule {"ea", "ae"},
            Rule {"fa", "af"},
            Rule {"ga", "ag"},
            Rule {"ha", "ah"},
            Rule {"cbc", "bcb"},
            Rule {"db", "bd"},
            Rule {"eb", "be"},
            Rule {"fb", "bf"},
            Rule {"gb", "bg"},
            Rule {"hb", "bh"},
            Rule {"dcd", "cdc"},
            Rule {"ec", "ce"},
            Rule {"fc", "cf"},
            Rule {"gc", "cg"},
            Rule {"hc", "ch"},
            Rule {"ede", "ded"},
            Rule {"fd", "df"},
            Rule {"gd", "dg"},
            Rule {"hd", "dh"},
            Rule {"fef", "efe"},
            Rule {"ge", "eg"},
            Rule {"he", "eh"},
            Rule {"gfg", "fgf"},
            Rule {"hf", "fh"},
            Rule {"hgh", "ghg"}
        };

        std::vector<Rule> confluent = knuthbendix<TestType>(rules);

        /* CHECK(check_confluent(confluent)); */
        CHECK(confluent.size() == 57);
    }
}
