# KnuthBendix

This is a header-only c++20 library for running the [Knuth-Bendix
procedure](https://en.wikipedia.org/wiki/Knuth%E2%80%93Bendix_completion_algorithm)
with different rule lookup strategies. Lookup strategies must expose member
functions for inserting and removing rules, and rewriting strings. Three such
strategies are implemented by default: 
    - KnuthBendix::NaiveLookup which simply checks every active rule,
    - KnuthBendix::OrderedLookup which stores rules in reverse lexicographic order to find
      suffixes using a binary search
    - KnuthBendix::TrieLookup which can find prefixes in time linear to the length of the string

The implementation of the procedure itself is based on the code outlined in Computation with Finitely Presented Groups.

# Usage

```cpp
#include <vector>

#include <KnuthBendix/knuth_bendix.hpp>
#include <KnuthBendix/trie_lookup.hpp>

std::vector<KnuthBendix::Rule> initial_rules {
    KnuthBendix::Rule("aa", ""),
    KnuthBendix::Rule("bbb", ""),
    KnuthBendix::Rule("ababab", "")
};

std::vector<KnuthBendix::Rule> confluent = 
    KnuthBendix::knuthbendix<KnuthBendix::TrieLookup>(initial_rules);
```

Compile with:

```
g++ file.cpp -Ipath/to/KnuthBendix --std=c++20 -O3
```

# Building and installing

The examples and tests can be built and using `make examples` and `make
tests`. A compiler supporting the c++20 standard must be installed.

# Contributing

This is not being actively developed, but contributions are welcome.
