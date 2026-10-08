#include <iostream>
#include <vector>

using std::vector;

void DfsForOrder(int v, vector<vector<int>>& reversed_graph,
                 vector<int>& answer, vector<int>& used);

void DfsForComponents(int v, int color, vector<vector<int>>& graph,
                      vector<int>& used);

vector<int> Condensation(vector<vector<int>> graph);