#include <iostream>

#include "condensation.hpp"
#include "automats.hpp"

using std::vector;
int state_number;
int start_point;
std::set<int> finish_points;
vector<char> alphabet;
vector<vector<std::set<std::string>>> g;

void ReadAlphabet() {
    std::cout << "Введите число символов в алфавите\n";
    int n;
    std::cin >> n;
    std::cout << "Введите символы в алфавите без пробелов\n";
    for (int i = 0; i < n; ++i) {
        char x;
        std::cin >> x;
        alphabet.push_back(x);
    }
}

void ReadStates() {
    std::cout << "Введите число состояний\n";
    std::cin >> state_number;
    g.resize(state_number, vector<std::set<std::string>>(state_number));
    std::cout << "Введите номер старта (0-индексация)\n";
    std::cin >> start_point;
    std::cout << "Введите число завершающих состояний\n";
    int finish_count;
    std::cin >> finish_count;
    std::cout << "Введите все завершающие состояния (0-индексация)\n";
    for (int i = 0; i < finish_count; ++i) {
        int x;
        std::cin >> x;
        finish_points.insert(x);
    }
}

void ReadTransitions() {
    std::cout << "Введите число переходов\n";
    int trans_number;
    std::cin >> trans_number;
    for (int i = 0; i < trans_number; ++i) {
        std::cout << "Введите вершины\n";
        int u, v;
        std::cin >> u >> v;
        std::cout << "Введите слово (epsilon = 1)\n";
        std::string s;
        std::cin >> s;
        g[u][v].insert(s);
        std::cout << std::endl;
    }
}

int main() {
    ReadAlphabet();
    ReadStates();
    ReadTransitions();
    BadAutomat bad_auto(state_number, g, start_point, finish_points, alphabet);
    DeterminedAutomat det = bad_auto.MakeDetermined();
    det.Print();
    std::cout << std::endl;
    std::cout << "Сколько слов хотите чекнуть?\n";
    int word_count;
    std::cin >> word_count;
    while (word_count--) {
        std::cout << "Введите слово:\n";
        std::string w;
        std::cin >> w;
        std::cout << (det.CheckWord(w) ? "True" : "False");
        std::cout << std::endl;
    }
}