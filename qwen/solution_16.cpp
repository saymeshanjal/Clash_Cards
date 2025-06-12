#include <bits/stdc++.h>
using namespace std;

const int MOD = 998244353;

// Function to count the number of set bits in a bitmask
int count_bits(int mask) {
    return __builtin_popcount(mask);
}

// Function to generate all combinations of n choose k
vector<int> generate_combinations(int n, int k) {
    vector<int> res;
    int mask = (1 << k) - 1;
    mask <<= (n - k);
    do {
        res.push_back(mask);
        if (mask == 0) break;
        int x = mask & -mask;
        int y = mask + x;
        int z = mask ^ y;
        mask = y | (((z >> 2) / x) & ~mask);
    } while (count_bits(mask) == k);
    return res;
}

// Memoization structure for the simulate_round function
struct State {
    int turn;
    int mask_a;
    int mask_b;
    int e_a;
    int e_b;
    bool operator<(const State& other) const {
        return tie(turn, mask_a, mask_b, e_a, e_b) < tie(other.turn, other.mask_a, other.mask_b, other.e_a, other.e_b);
    }
};

map<State, int> memo;

// Simulate a single round and return the outcome (1 for Alex win, -1 for Boris win, 0 for draw)
int simulate_round(int n, int mask_a, int mask_b, int e_a, int e_b, int turn) {
    State state = {turn, mask_a, mask_b, e_a, e_b};
    if (memo.count(state)) return memo[state];

    int winner = 0;
    if (turn == 0) {
        // Alex's turn to initiate
        bool can_play = false;
        for (int x = 1; x <= n; ++x) {
            if ((mask_a & (1 << (x - 1))) && e_a >= x) {
                can_play = true;
                break;
            }
        }
        if (!can_play) {
            memo[state] = -1; // Alex can't play, Boris wins
            return -1;
        }

        int best = -1;
        for (int x = 1; x <= n; ++x) {
            if ((mask_a & (1 << (x - 1))) == 0 || e_a < x) continue;
            int new_mask_a = mask_a & ~(1 << (x - 1));
            int new_e_a = e_a - x;
            vector<int> responses;
            for (int y = x + 1; y <= n; ++y) {
                if ((mask_b & (1 << (y - 1))) && e_b >= y) {
                    responses.push_back(y);
                }
            }
            if (responses.empty()) {
                memo[state] = 1;
                return 1;
            }

            int min_resp = 1;
            for (int y : responses) {
                int new_mask_b = mask_b & ~(1 << (y - 1));
                int new_e_b = e_b - y;
                int res = simulate_round(n, new_mask_a, new_mask_b, new_e_a, new_e_b, 1);
                if (res < min_resp) min_resp = res;
            }
            if (min_resp == 1) {
                memo[state] = 1;
                return 1;
            }
            if (best < min_resp) best = min_resp;
        }
        memo[state] = best;
        return best;
    } else {
        // Boris's turn to initiate
        bool can_play = false;
        for (int x = 1; x <= n; ++x) {
            if ((mask_b & (1 << (x - 1))) && e_b >= x) {
                can_play = true;
                break;
            }
        }
        if (!can_play) {
            memo[state] = 1; // Boris can't play, Alex wins
            return 1;
        }

        int best = 1;
        for (int x = 1; x <= n; ++x) {
            if ((mask_b & (1 << (x - 1))) == 0 || e_b < x) continue;
            int new_mask_b = mask_b & ~(1 << (x - 1));
            int new_e_b = e_b - x;
            vector<int> responses;
            for (int y = x + 1; y <= n; ++y) {
                if ((mask_a & (1 << (y - 1))) && e_a >= y) {
                    responses.push_back(y);
                }
            }
            if (responses.empty()) {
                memo[state] = -1;
                return -1;
            }

            int max_resp = -1;
            for (int y : responses) {
                int new_mask_a = mask_a & ~(1 << (y - 1));
                int new_e_a = e_a - y;
                int res = simulate_round(n, new_mask_a, new_mask_b, new_e_a, new_e_b, 0);
                if (res > max_resp) max_resp = res;
            }
            if (max_resp == -1) {
                memo[state] = -1;
                return -1;
            }
            if (best > max_resp) best = max_resp;
        }
        memo[state] = best;
        return best;
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);

    int t;
    cin >> t;
    for (int _ = 0; _ < t; ++_) {
        int n, E, r;
        cin >> n >> E >> r;

        int k = n / 2;
        vector<int> all_masks;
        for (int mask = 0; mask < (1 << n); ++mask) {
            if (__builtin_popcount(mask) == k) {
                all_masks.push_back(mask);
            }
        }

        long long alex_wins = 0;
        long long boris_wins = 0;
        long long ties = 0;

        for (int mask_a : all_masks) {
            int mask_b = ((1 << n) - 1) ^ mask_a;

            vector<int> round_results;
            for (int round = 0; round < r; ++round) {
                memo.clear();
                int result = simulate_round(n, mask_a, mask_b, E, E, 0);
                if (result == 1) {
                    round_results.push_back(1); // Alex wins
                } else if (result == -1) {
                    round_results.push_back(-1); // Boris wins
                } else {
                    round_results.push_back(0); // Draw
                }
            }

            int alex_count = 0;
            int boris_count = 0;
            for (int res : round_results) {
                if (res == 1) alex_count++;
                else if (res == -1) boris_count++;
            }

            if (alex_count > boris_count) {
                alex_wins++;
            } else if (boris_count > alex_count) {
                boris_wins++;
            } else {
                ties++;
            }
        }

        alex_wins %= MOD;
        boris_wins %= MOD;
        ties %= MOD;

        cout << alex_wins << " " << boris_wins << " " << ties << endl;
    }

    return 0;
}