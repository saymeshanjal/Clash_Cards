#include <bits/stdc++.h>
using namespace std;
using ll = long long;

const int MOD = 998244353;

int n, E, r;
map<tuple<int, int, int, int, int>, int> memo_round; // turn, a_mask, b_mask, last_played, e_left
map<tuple<int, int, int, int, int>, pair<int, int>> memo_game; // a_mask, b_mask, e_a, e_b, rem

// Function to determine if current player can win the round
int can_win_round(int turn, int a_mask, int b_mask, int last_played, int e_a, int e_b) {
    auto key = make_tuple(turn, a_mask, b_mask, last_played, e_a, e_b);
    if (memo_round.count(key)) return memo_round[key];

    int &res = memo_round[key];

    int curr_mask = (turn == 0) ? a_mask : b_mask;
    int curr_energy = (turn == 0) ? e_a : e_b;
    int other_mask = (turn == 0) ? b_mask : a_mask;
    int other_energy = (turn == 0) ? e_b : e_a;

    bool can_play = false;
    for (int i = 1; i <= n; ++i) {
        if ((curr_mask & (1 << (i - 1))) && i > last_played && i <= curr_energy) {
            can_play = true;
            int new_mask = curr_mask ^ (1 << (i - 1));
            int new_energy = curr_energy - i;
            int new_last = i;

            int new_a_mask = (turn == 0) ? new_mask : a_mask;
            int new_b_mask = (turn == 1) ? new_mask : b_mask;
            int new_e_a = (turn == 0) ? new_energy : e_a;
            int new_e_b = (turn == 1) ? new_energy : e_b;

            int outcome = can_win_round(1 - turn, new_a_mask, new_b_mask, new_last, new_e_a, new_e_b);
            if (outcome == (turn == 0 ? 1 : -1)) {
                res = (turn == 0 ? 1 : -1);
                return res;
            }
        }
    }

    if (!can_play) {
        res = (turn == 0 ? -1 : 1); // current player loses
    } else {
        res = (turn == 0 ? -1 : 1); // all moves lead to loss
    }

    return res;
}

// Function to simulate the entire game for 'rem' remaining rounds
pair<int, int> simulate_game(int a_mask, int b_mask, int e_a, int e_b, int rem) {
    if (rem == 0) return {0, 0};
    auto key = make_tuple(a_mask, b_mask, e_a, e_b, rem);
    if (memo_game.count(key)) return memo_game[key];

    int winner = can_win_round(0, a_mask, b_mask, 0, e_a, e_b);
    pair<int, int> res;
    if (winner == 1) res = {1, 0};
    else if (winner == -1) res = {0, 1};
    else res = {0, 0};

    pair<int, int> future = simulate_game(a_mask, b_mask, e_a, e_b, rem - 1);
    res.first += future.first;
    res.second += future.second;

    return memo_game[key] = res;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);

    int t;
    cin >> t;

    while (t--) {
        cin >> n >> E >> r;

        int total = 0, alex = 0, boris = 0, tie = 0;

        int total_dists = 0;
        for (int mask = 0; mask < (1 << n); ++mask) {
            if (__builtin_popcount(mask) != n / 2) continue;

            int a_mask = mask;
            int b_mask = ((1 << n) - 1) ^ a_mask;

            int a_energy = E;
            int b_energy = E;

            memo_game.clear();

            pair<int, int> result = simulate_game(a_mask, b_mask, a_energy, b_energy, r);

            if (result.first > result.second) {
                alex = (alex + 1) % MOD;
            } else if (result.second > result.first) {
                boris = (boris + 1) % MOD;
            } else {
                tie = (tie + 1) % MOD;
            }
        }

        cout << alex << " " << boris << " " << tie << "\n";
    }

    return 0;
}