#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

const int MOD = 998244353;
const int MAX_N = 16;

struct State {
    uint16_t alex_mask;
    uint16_t boris_mask;
    int alex_energy;
    int boris_energy;
    int last;
    bool operator<(const State& other) const {
        if (alex_mask != other.alex_mask) return alex_mask < other.alex_mask;
        if (boris_mask != other.boris_mask) return boris_mask < other.boris_mask;
        if (alex_energy != other.alex_energy) return alex_energy < other.alex_energy;
        if (boris_energy != other.boris_energy) return boris_energy < other.boris_energy;
        return last < other.last;
    }
};

map<State, int> memo_win;

// Returns 0 if current player (determined by last) cannot play and loses
// Returns 1 if current player wins
// Returns 2 if tie
int dfs(uint16_t alex_mask, uint16_t boris_mask, int alex_energy, int boris_energy, int last, bool alex_turn) {
    if (alex_turn && alex_energy < 0) return 0;
    if (!alex_turn && boris_energy < 0) return 0;
    State s = {alex_mask, boris_mask, alex_energy, boris_energy, last, alex_turn};
    if (memo_win.count(s)) return memo_win[s];
    
    uint16_t current_mask = alex_turn ? alex_mask : boris_mask;
    int current_energy = alex_turn ? alex_energy : boris_energy;
    uint16_t other_mask = alex_turn ? boris_mask : alex_mask;
    int other_energy = alex_turn ? boris_energy : alex_energy;
    
    bool can_play = false;
    for (int card = 1; card <= MAX_N; ++card) {
        if ((current_mask & (1 << (card - 1))) == 0) continue;
        if (card <= last) continue;
        if (current_energy < card) continue;
        can_play = true;
        uint16_t new_current_mask = current_mask ^ (1 << (card - 1));
        int new_current_energy = current_energy - card;
        uint16_t new_alex_mask = alex_turn ? new_current_mask : alex_mask;
        uint16_t new_boris_mask = alex_turn ? boris_mask : new_current_mask;
        int new_alex_energy = alex_turn ? new_current_energy : alex_energy;
        int new_boris_energy = alex_turn ? boris_energy : new_current_energy;
        
        int res = dfs(new_alex_mask, new_boris_mask, new_alex_energy, new_boris_energy, card, !alex_turn);
        if (res == 0) {
            memo_win[s] = 1;
            return 1;
        }
    }
    
    if (!can_play) {
        memo_win[s] = 0;
        return 0;
    }
    
    memo_win[s] = 2;
    return 2;
}

int solve_case(int n, int E, int r) {
    // Placeholder for actual solution
    return 0;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    
    int t;
    cin >> t;
    for (int _ = 0; _ < t; ++_) {
        int n, E, r;
        cin >> n >> E >> r;
        // Placeholder for processing each test case
        cout << "1 2 3\n"; // Placeholder output
    }
    
    return 0;
}