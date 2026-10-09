#include <gtest/gtest.h>
#include <set>
#include <string>
#include <vector>
#include <map>

#include "condensation.hpp"
#include "automats.hpp"

using std::vector;

bool NonDetAndDet(const std::string& word, const Automat& a, const DeterminedAutomat& b) {
    return a.CheckWord(word) == b.CheckWord(word);
}

bool NonDetAndNonDet(const std::string& word, const Automat& a, const Automat& b) {
    return a.CheckWord(word) == b.CheckWord(word);
}

bool EqualTwoVectors(const vector<int>& a, const vector<int>& b) {
    for (int i = 0; i < a.size(); ++i) {
        for (int j = i + 1; j < a.size(); ++j) {
            if ((a[i] == a[j]) !=  (b[i] == b[j])) {
                return false;
            }
        }
    }
    return true;
}

TEST(CondensationTest, ThreeComponents) {
    vector<vector<int>> g(3, vector<int>());
    g[0].push_back(1);
    g[0].push_back(2);
    g[1].push_back(2);
    EXPECT_TRUE(EqualTwoVectors(Condensation(g), vector<int>({1, 2, 3})));
}

TEST(CondensationTest, OneComponent) {
    vector<vector<int>> g(3, vector<int>());
    g[0].push_back(1);
    g[0].push_back(2);
    g[1].push_back(2);
    g[1].push_back(0);
    g[2].push_back(0);
    g[2].push_back(1);
    EXPECT_TRUE(EqualTwoVectors(Condensation(g), vector<int>({1, 1, 1})));
}

TEST(CondensationTest, TwoComponents) {
    vector<vector<int>> g(6, vector<int>());
    g[0].push_back(1);
    g[0].push_back(2);
    g[1].push_back(2);
    g[1].push_back(0);
    g[2].push_back(0);
    g[2].push_back(1);

    g[2].push_back(3);

    g[3].push_back(4);
    g[3].push_back(5);
    g[4].push_back(5);
    g[4].push_back(3);
    g[5].push_back(3);
    g[5].push_back(4);
    EXPECT_TRUE(EqualTwoVectors(Condensation(g), vector<int>({1, 1, 1, 2, 2, 2})));
}

TEST(EpsGraphTest, NoEpsilons) {
    size_t n = 3;
    std::vector<std::vector<std::set<char>>> g(n, std::vector<std::set<char>>(n));
    g[0][1].insert('a');
    g[1][2].insert('b');
    g[2][0].insert('c');
    Automat nfa(n, g, 0, {2}, {'a', 'b', 'c'});
    auto eps_g = nfa.EpsGraph();
    ASSERT_EQ(eps_g.size(), n);
    for (size_t i = 0; i < n; ++i) {
        EXPECT_TRUE(eps_g[i].empty());
    }
}

TEST(EpsGraphTest, TwoEpsilons) {
    size_t n = 3;
    std::vector<std::vector<std::set<char>>> g(n, std::vector<std::set<char>>(n));
    g[0][1].insert('1');
    g[1][2].insert('1');

    Automat nfa(n, g, 0, {2}, {'a'});
    auto eps_g = nfa.EpsGraph();

    std::vector<std::vector<int>> expected = {{1}, {2}, {}};

    EXPECT_EQ(eps_g, expected);
}

TEST(EpsGraphTest, Cycles) {
    size_t n = 3;
    std::vector<std::vector<std::set<char>>> g(n, std::vector<std::set<char>>(n));
    g[0][1].insert('1');
    g[1][0].insert('1');
    g[2][2].insert('1');

    Automat nfa(n, g, 0, {1}, {'a'});
    auto eps_g = nfa.EpsGraph();
    std::vector<std::vector<int>> expected = {{1}, {0}, {2}};
    EXPECT_EQ(eps_g, expected);
}

TEST(NonEpsilonAutomatTest, TestEquivalence_1) {
    size_t n = 4;
    std::vector<std::vector<std::set<char>>> g(n, std::vector<std::set<char>>(n));
    g[0][1].insert('1');
    g[0][2].insert('1');
    g[1][3].insert('1');
    g[2][3].insert('1');

    g[1][1].insert('a');
    g[2][2].insert('b');

    std::vector<char> alphabet = {'a', 'b'};
    Automat original_nfa(n, g, 0, {3}, alphabet);

    Automat clean_nfa = original_nfa;
    clean_nfa.DeleteAllEpsilons();

    vector<std::string> a({"", "a", "aa", "aaa", "aaaa", "b", "bb", "bbb", "bbbbb", "ab", "ababababab", "aawsergfzsdfvsdfhs", "sixsevengazan"});

    for (auto e : a) {
        EXPECT_EQ(clean_nfa.CheckWord(e), original_nfa.CheckWord(e));
    }
}

TEST(NonEpsilonAutomatTest, TestEquivalence_2) {
    size_t n = 4;
    std::vector<std::vector<std::set<char>>> g(n, std::vector<std::set<char>>(n));
    
    g[0][1].insert('1');
    g[1][0].insert('1');
    
    g[1][2].insert('a');
    g[2][3].insert('b');

    Automat original_nfa(n, g, 0, {3}, {'a', 'b'});
    Automat clean_nfa = original_nfa;
    clean_nfa.DeleteEpsCircles();

    vector<std::string> a({"", "a", "b", "ab", "ba", "sixsevengazan"});

    for (auto e : a) {
        EXPECT_EQ(clean_nfa.CheckWord(e), original_nfa.CheckWord(e));
    }
}

TEST(MakeDeterminedTest, TestEquivalence_1) {
    size_t n = 4;
    std::vector<std::vector<std::set<char>>> g(n, std::vector<std::set<char>>(n));
    g[0][1].insert('1');
    g[0][2].insert('1');
    g[1][3].insert('1');
    g[2][3].insert('1');

    g[1][1].insert('a');
    g[2][2].insert('b');

    Automat original_nfa(n, g, 0, {3}, {'a', 'b'});

    DeterminedAutomat dfa = original_nfa.MakeDetermined();

    vector<std::string> a({"", "a", "aa", "aaa", "aaaa", "b", "bb", "bbb", "bbbbb", "ab", "ababababab", "aawsergfzsdfvsdfhs", "sixsevengazan"});

    for (auto e : a) {
        EXPECT_EQ(dfa.CheckWord(e), original_nfa.CheckWord(e));
    }
}

TEST(MakeDeterminedTest, TestEquivalence_2) {
    size_t n = 4;
    std::vector<std::vector<std::set<char>>> g(n, std::vector<std::set<char>>(n));
    
    g[0][1].insert('1');
    g[1][0].insert('1');
    
    g[1][2].insert('a');
    g[2][3].insert('b');

    Automat original_nfa(n, g, 0, {3}, {'a', 'b'});
    DeterminedAutomat dfa = original_nfa.MakeDetermined();

    vector<std::string> a({"", "a", "b", "ab", "ba", "sixsevengazan"});

    for (auto e : a) {
        EXPECT_EQ(dfa.CheckWord(e), original_nfa.CheckWord(e));
    }
}



