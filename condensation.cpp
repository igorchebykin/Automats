#include <iostream>
#include <vector>
#include "condensation.hpp"

using std::vector;

void DfsForOrder(int v, vector<vector<int>>& reversed_graph,
                 vector<int>& answer, vector<int>& used) {
  used[v] = 1;
  for (auto u : reversed_graph[v]) {
    if (used[u] == 0) {
      DfsForOrder(u, reversed_graph, answer, used);
    }
  }
  answer.push_back(v);
}

void DfsForComponents(int v, int color, vector<vector<int>>& graph,
                      vector<int>& used) {
  used[v] = color;
  for (auto u : graph[v]) {
    if (used[u] == -1) {
      DfsForComponents(u, color, graph, used);
    }
  }
}

vector<int> Condensation(vector<vector<int>> graph) {
    int n = graph.size();
    int m = 0;
    vector<vector<int>> reversed_graph(n, vector<int>());
    for (int i = 0; i < n; ++i) {
        for (int e : graph[i]) {
            ++m;
            reversed_graph[e].push_back(i);
        }
    }
    vector<int> used(n, 0);
    vector<int> answer;

    for (int i = 0; i < n; ++i) {
        if (used[i] == 0) {
            DfsForOrder(i, reversed_graph, answer, used);
        }
    }
    used.assign(n, -1);
    int color = 0;
    for (int i = static_cast<int>(answer.size() - 1); i >= 0; --i) {
        int e = answer[i];
        if (used[e] == -1) {
            DfsForComponents(e, color, graph, used);
            ++color;
        }
    }
    vector<int> solution(n, 0);
    for (int i = 0; i < n; ++i) {
        solution[i] = color - used[i] - 1;
    }
    return solution;
}
