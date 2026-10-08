#pragma once

#include <vector>
#include <set>
#include <map>
#include <queue>
#include <algorithm>
#include <cstddef>

#include "condensation.hpp"

using std::vector;

class DeterminedAutomat {   
public:
    size_t state_number;
    std::map<int, std::map<char, int>> g;
    int start_point;
    std::set<int> finish_points;
    vector<char> alphabet;

public:
    DeterminedAutomat(size_t _state_number,
                      std::map<int, std::map<char, int>> _g,
                      int _start_point,
                      std::set<int> _finish_points,
                      vector<char> _alphabet);
    
    bool CheckWord(const std::string& word) const;
    void Print() const;
};

class Automat {
public:
    size_t state_number;
    std::vector<std::vector<std::set<char>>> g;
    int start_point;
    std::set<int> finish_points;
    vector<char> alphabet;

    std::vector<std::vector<int>> EpsGraph();

    void DeleteEpsCircles();

    void DeleteEpsilonEdge(int u, int v);

    void EpsDfs(int s, vector<bool>& check,
                std::vector<std::vector<int>>& epsilon_g);

    void DeleteAllEpsilons();

    vector<std::map<char, int>> AllDestinations();

    bool ISNewFinish(int v);

    std::set<int> GetClosure(const std::set<int>& states) const;

public:
    Automat(size_t _state_number,
            std::vector<std::vector<std::set<char>>> _g,
            int _start_point,
            std::set<int> _finish_points,
            vector<char> _alphabet
    );

    DeterminedAutomat MakeDetermined();

    bool CheckWord(const std::string& word) const;

    void Print() const;
};