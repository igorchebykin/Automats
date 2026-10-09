#include "automats.hpp"
#include "condensation.hpp"

using std::vector;

DeterminedAutomat::DeterminedAutomat(
    size_t _state_number,
    std::map<int, std::map<char, int>> _g,
    int _start_point,
    std::set<int> _finish_points,
    vector<char> _alphabet
) {
    state_number = _state_number;
    g = _g;
    start_point = _start_point;
    finish_points = _finish_points;
    alphabet = _alphabet;
}

Automat::Automat(
    size_t _state_number,
    std::vector<std::vector<std::set<char>>> _g,
    int _start_point,
    std::set<int> _finish_points,
    vector<char> _alphabet
) {
    state_number = _state_number;
    g = _g;
    start_point = _start_point;
    finish_points = _finish_points;
    alphabet = _alphabet;
}

vector<vector<int>> Automat::EpsGraph() {
    vector<vector<int>> eps_g(state_number);
    vector<vector<bool>> check(state_number, vector<bool>(state_number, false));
    for (int i = 0; i < static_cast<int>(state_number); ++i) {
        for (int j = 0; j < static_cast<int>(state_number); ++j) {
            for (auto e : g[i][j]) {
                if (e == 'E' && check[i][j] == false) {
                    eps_g[i].push_back(j);
                    check[i][j] = true;
                }
            }
        }
    }
    return eps_g;
}

void Automat::DeleteEpsCircles() {
    auto eps_graph = EpsGraph();
    auto colors = Condensation(eps_graph);
    int color_count = 0;
    for (int e : colors) {
        color_count = std::max(color_count, e + 1);
    }
    vector<vector<std::set<char>>> new_g(color_count, vector<std::set<char>>(color_count));
    for (int i = 0; i < static_cast<int>(state_number); ++i) {
        for (int j = 0; j < static_cast<int>(state_number); ++j) {
            for (auto e : g[i][j]) {
                new_g[colors[i]][colors[j]].insert(e);
            }
        }
    }
    start_point = colors[start_point];
    state_number = color_count;
    g = new_g;
    std::set<int> new_finish_points;
    for (auto e : finish_points) {
        new_finish_points.insert(colors[e]);
    }
    finish_points = new_finish_points;
}

void Automat::DeleteEpsilonEdge(int u, int v) {
    if (finish_points.count(v)) {
        finish_points.insert(u);
    }
    g[u][v].erase('E');
    for (int i = 0; i < static_cast<int>(state_number); ++i) {
        for (auto e : g[v][i]) {
            g[u][i].insert(e);
        }
    }
}

void Automat::EpsDfs(
    int s,
    vector<bool>& check,
    vector<vector<int>>& epsilon_g
) {
    check[s] = true;
    for (auto e : epsilon_g[s]) {
        if (!check[e]) {
            EpsDfs(e, check, epsilon_g);
        }
    }
    for (auto e : epsilon_g[s]) {
        DeleteEpsilonEdge(s, e);
    }
}

void Automat::DeleteAllEpsilons() {
    DeleteEpsCircles();
    vector<bool> check(state_number, false);
    auto eps_graph = EpsGraph();
    for (int i = 0; i < static_cast<int>(state_number); ++i) {
        if (!check[i]) {
            EpsDfs(i, check, eps_graph);
        }
    }
}

vector<std::map<char, int>> Automat::AllDestinations() {
    vector<std::map<char, int>> result(state_number);
    for (int i = 0; i < static_cast<int>(state_number); ++i) {
        for (int j = 0; j < static_cast<int>(state_number); ++j) {
            for (auto e : g[i][j]) {
                result[i][e] = (result[i][e] | (1 << j));
            }
        }
    }
    return result;
}

bool Automat::ISNewFinish(int v) {
    for (auto e : finish_points) {
        if ((1 << e) & v) {
            return true;
        }
    }
    return false;
}

DeterminedAutomat Automat::MakeDetermined() {
    DeleteAllEpsilons();
    auto all_destinations = AllDestinations();
    vector<bool> check((1 << state_number), false);
    std::set<int> new_finishes;
    std::queue<int> q;
    std::map<int, std::map<char, int>> new_g;
    q.push(1 << start_point);
    check[1 << start_point] = true;
    while (!q.empty()) {
        int v = q.front();
        q.pop();
        if (ISNewFinish(v)) {
            new_finishes.insert(v);
        }
        std::map<char, int> v_destinations;
        for (int i = 0; i < static_cast<int>(state_number); ++i) {
            if ((v & (1 << i)) == 0) {
                continue;
            }
            for (auto e : alphabet) {
                v_destinations[e] = (v_destinations[e] | all_destinations[i][e]);
            }
        }
        for (auto [e, u] : v_destinations) {
            if (u == 0) {
                continue;
            }
            new_g[v][e] = u;
            if (!check[u]) {
                check[u] = true;
                q.push(u);
            }
        }
    }
    return DeterminedAutomat(
        static_cast<int>(1 << state_number),
        new_g,
        (1 << start_point),
        new_finishes,
        alphabet
    );
}

bool DeterminedAutomat::CheckWord(const std::string& word) const {
    int current_state = start_point;
    int i = 0;
    int n = word.size();
    while (i < n) {
        bool flag = false;
        if (g.find(current_state) == g.end()) {
            return false;
        }
        for (auto [letter, next_state] : g.at(current_state)) {
            if (word[i] == letter) {
                ++i;
                current_state = next_state;
                flag = true;
                break;
            }
        }
        if (!flag) {
            return false;
        }
    }
    if (finish_points.count(current_state)) {
        return true;
    }
    return false;
}

std::set<int> Automat::GetClosure(const std::set<int>& states) const {
    std::set<int> closure = states;
    std::queue<int> q;
    for (int s : states) {
        q.push(s);
    }
    while (!q.empty()) {
        int u = q.front();
        q.pop();

        for (int v = 0; v < static_cast<int>(state_number); ++v) {
            if (g[u][v].count('E') && closure.count(v) == 0) {
                closure.insert(v);
                q.push(v);
            }
        }
    }
    return closure;
}

bool Automat::CheckWord(const std::string& word) const {
    std::set<int> current_states = GetClosure({start_point});
    for (char symbol : word) {
        std::set<int> next_states;
        for (int u : current_states) {
            for (int v = 0; v < static_cast<int>(state_number); ++v) {
                if (g[u][v].count(symbol)) {
                    next_states.insert(v);
                }
            }
        }
        current_states = GetClosure(next_states);
        if (current_states.empty()) {
            return false;
        }
    }
    for (int state : current_states) {
        if (finish_points.count(state)) {
            return true;
        }
    }
    return false;
}

void DeterminedAutomat::Print() const {
    std::cout << "===== ИНФОРМАЦИЯ О ДКА =====" << "\n";
    std::cout << "Всего состояний: " << state_number << "\n";
    std::cout << "Стартовое состояние: " << start_point << "\n";

    std::cout << "Финальные состояния: ";
    for (int finish : finish_points) {
        std::cout << finish << " ";
    }
    std::cout << "\n";

    std::cout << "Таблица переходов:" << "\n";
    if (g.empty()) {
        std::cout << "  (Переходы отсутствуют)" << "\n";
    } else {
        for (const auto& [from, transitions] : g) {
            for (const auto& [symbol, to] : transitions) {
                std::cout << "  Состояние " << from 
                          << " --'" << symbol << "'--> " 
                          << "Состояние " << to << "\n";
            }
        }
    }
    std::cout << "============================" << "\n";
    
}

void Automat::Print() const {
    std::cout << "===== ИНФОРМАЦИЯ О НКА0-1 =====" << "\n";
    std::cout << "Количество состояний: " << state_number << "\n";
    std::cout << "Стартовое состояние: " << start_point << "\n";

    std::cout << "Терминальные (финальные) состояния: { ";
    for (int finish : finish_points) {
        std::cout << finish << " ";
    }
    std::cout << "}" << "\n";

    std::cout << "Список переходов:" << "\n";
    bool has_transitions = false;

    for (size_t u = 0; u < state_number; ++u) {
        for (size_t v = 0; v < state_number; ++v) {
            if (!g[u][v].empty()) {
                has_transitions = true;
                std::cout << "  Состояние " << u << " -> Состояние " << v << " по символам: { ";
                for (char symbol : g[u][v]) {
                    std::cout << "'" << symbol << "' ";
                }
                std::cout << "}" << "\n";
            }
        }
    }

    if (!has_transitions) {
        std::cout << "  (Переходы отсутствуют)" << "\n";
    }
    std::cout << "============================" << "\n";
}

BadAutomat::BadAutomat(
    size_t _state_number,
    std::vector<std::vector<std::set<std::string>>> _g,
    int _start_point,
    std::set<int> _finish_points,
    vector<char> _alphabet
) {
    state_number = _state_number;
    g = _g;
    start_point = _start_point;
    finish_points = _finish_points;
    alphabet = _alphabet;
}

Automat BadAutomat::ToAutomat() const {
    int new_state_count = static_cast<int>(state_number);
    vector<vector<std::set<char>>> new_g;

    struct Chain {
        int u;
        int v;
        std::string s;
    };
    vector<Chain> chains;

    for (int i = 0; i < static_cast<int>(state_number); ++i) {
        for (int j = 0; j < static_cast<int>(state_number); ++j) {
            for (const auto& s : g[i][j]) {
                if (s.empty()) {
                    continue;
                }
                if (s.size() == 1) {
                    chains.push_back({i, j, s});
                } else {
                    int prev = i;
                    for (size_t k = 0; k + 1 < s.size(); ++k) {
                        int mid = new_state_count++;
                        chains.push_back({prev, mid, std::string(1, s[k])});
                        prev = mid;
                    }
                    chains.push_back({prev, j, std::string(1, s.back())});
                }
            }
        }
    }

    new_g.assign(new_state_count,
                 vector<std::set<char>>(new_state_count));

    for (const auto& c : chains) {
        new_g[c.u][c.v].insert(c.s[0]);
    }

    return Automat(
        static_cast<size_t>(new_state_count),
        new_g,
        start_point,
        finish_points,
        alphabet
    );
}

DeterminedAutomat BadAutomat::MakeDetermined() const {
    Automat nka = ToAutomat();
    return nka.MakeDetermined();
}

void BadAutomat::Print() const {
    std::cout << "===== ИНФОРМАЦИЯ О НКА =====" << "\n";
    std::cout << "Количество состояний: " << state_number << "\n";
    std::cout << "Стартовое состояние: " << start_point << "\n";

    std::cout << "Финальные состояния: { ";
    for (int finish : finish_points) {
        std::cout << finish << " ";
    }
    std::cout << "}" << "\n";

    std::cout << "Список переходов:" << "\n";
    bool has_transitions = false;

    for (size_t u = 0; u < state_number; ++u) {
        for (size_t v = 0; v < state_number; ++v) {
            if (!g[u][v].empty()) {
                has_transitions = true;
                std::cout << "  Состояние " << u
                          << " -> Состояние " << v
                          << " по строкам: { ";
                for (const auto& s : g[u][v]) {
                    std::cout << "\"" << s << "\" ";
                }
                std::cout << "}" << "\n";
            }
        }
    }

    if (!has_transitions) {
        std::cout << "  (Переходы отсутствуют)" << "\n";
    }
    std::cout << "==============================================" << "\n";
}