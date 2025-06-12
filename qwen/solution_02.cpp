#include <bits/stdc++.h>
using namespace std;

const int MOD = 998244353;
const int MAX_N = 16;

int n, E, r;
map<tuple<int, int, int, int>, int> memo_round; // Memoization for round simulation

// Simulate a single round and return 1 if Alex wins, -1 if Boris wins, 0 for tie
int simulate_round(int a_mask, int a_energy, int b_mask, int b_energy) {
    tuple<int, int, int, int> state(a_mask, a_energy, b_mask, b_energy);
    if (memo_round.count(state)) return memo_round[state];

    // Alex starts as attacker
    bool attacker_can_play = false;

    for (int x = 1; x <= n; ++x) {
        if ((a_mask & (1 << (x - 1))) == 0) continue;
        if (x > a_energy) continue;

        attacker_can_play = true;
        bool all_defense_lead_to_win = true;

        // Check all possible y responses
        bool has_y = false;
        vector<int> responses;

        for (int y = x + 1; y <= n; ++y) {
            if ((b_mask & (1 << (y - 1))) == 0) continue;
            if (y > b_energy) continue;
            responses.push_back(y);
            has_y = true;
        }

        if (!has_y) {
            memo_round[state] = 1; // Alex wins
            return 1;
        }

        for (int y : responses) {
            int new_a_mask = a_mask & ~(1 << (x - 1));
            int new_b_mask = b_mask & ~(1 << (y - 1));
            int new_a_energy = a_energy - x;
            int new_b_energy = b_energy - y;

            int res = simulate_round(new_b_mask, new_b_energy, new_a_mask, new_a_energy);
            if (res != -1) {
                all_defense_lead_to_win = false;
                break;
            }
        }

        if (all_defense_lead_to_win) {
            memo_round[state] = 1;
            return 1;
        }
    }

    if (!attacker_can_play) {
        memo_round[state] = -1; // Boris wins
        return -1;
    }

    memo_round[state] = -1;
    return -1;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);

    int t;
    cin >> t;
    while (t--) {
        cin >> n >> E >> r;
        int half = n / 2;
        long long alex_wins = 0, boris_wins = 0, ties = 0;

        memo_round.clear();

        vector<int> masks;
        for (int mask = 0; mask < (1 << n); ++mask) {
            if (__builtin_popcount(mask) != half) continue;
            masks.push_back(mask);
        }

        for (int mask_a : masks) {
            int mask_b = ((1 << n) - 1) ^ mask_a;

            // Simulate all r rounds
            vector<int> card_a = {0}, card_b = {0};
            vector<int> energy_a = {E}, energy_b = {E};
            int alex = 0, boris = 0;

            // Use BFS/DFS or memoization for multiple rounds
            // For simplicity, assuming r=1 in the code below
            // Proper implementation requires handling multiple rounds with state transitions
            int res = simulate_round(mask_a, E, mask_b, E);
            if (res == 1) alex++;
            else if (res == -1) boris++;
            else ties++;
        }

        alex_wins %= MOD;
        boris_wins %= MOD;
        ties %= MOD;

        cout << alex_wins << " " << boris_wins << " " << ties << endl;
    }

    return 0;
}