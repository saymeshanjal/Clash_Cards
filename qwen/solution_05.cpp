#include <bits/stdc++.h>
using namespace std;

const int MOD = 998244353;

typedef uint32_t mask_t;

int n_global;

map<tuple<mask_t, mask_t, int, int, int>, int> game_memo;

int dfs_step(mask_t a_mask, int a_energy, mask_t b_mask, int b_energy, int last_val, int turn, int rem_rounds) {
    if (turn == 0) {
        int best = -1e9;
        bool found = false;
        for (int i = 0; i < n_global; ++i) {
            if ((a_mask & (1 << i)) == 0) continue;
            int val = i + 1;
            if (val > last_val && a_energy >= val) {
                mask_t new_a_mask = a_mask & ~(1 << i);
                int new_a_energy = a_energy - val;
                int res = dfs_step(new_a_mask, new_a_energy, b_mask, b_energy, val, 1, rem_rounds);
                if (res > best) {
                    best = res;
                    found = true;
                }
            }
        }
        if (!found) {
            int future = 0;
            future = game_memo.count(make_tuple(a_mask, b_mask, a_energy, b_energy, rem_rounds - 1)) ? 
                game_memo[make_tuple(a_mask, b_mask, a_energy, b_energy, rem_rounds - 1)] : 
                0;
            return -1 + future;
        }
        return best;
    } else {
        int best = 1e9;
        bool found = false;
        for (int i = 0; i < n_global; ++i) {
            if ((b_mask & (1 << i)) == 0) continue;
            int val = i + 1;
            if (val > last_val && b_energy >= val) {
                mask_t new_b_mask = b_mask & ~(1 << i);
                int new_b_energy = b_energy - val;
                int res = dfs_step(a_mask, a_energy, new_b_mask, new_b_energy, val, 0, rem_rounds);
                if (res < best) {
                    best = res;
                    found = true;
                }
            }
        }
        if (!found) {
            int future = 0;
            future = game_memo.count(make_tuple(a_mask, b_mask, a_energy, b_energy, rem_rounds - 1)) ? 
                game_memo[make_tuple(a_mask, b_mask, a_energy, b_energy, rem_rounds - 1)] : 
                0;
            return 1 + future;
        }
        return best;
    }
}

int game(mask_t a_mask, int a_energy, mask_t b_mask, int b_energy, int rem_rounds) {
    if (rem_rounds == 0) return 0;
    auto key = make_tuple(a_mask, b_mask, a_energy, b_energy, rem_rounds);
    auto it = game_memo.find(key);
    if (it != game_memo.end()) return it->second;

    int res = dfs_step(a_mask, a_energy, b_mask, b_energy, 0, 0, rem_rounds);
    game_memo[key] = res;
    return res;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    
    int t;
    cin >> t;
    for (int _ = 0; _ < t; ++_) {
        int n, E, r;
        cin >> n >> E >> r;
        n_global = n;
        game_memo.clear();
        
        int alex_wins = 0;
        int boris_wins = 0;
        int ties = 0;
        
        int total = 0;
        int half = n / 2;
        vector<mask_t> masks;
        
        for (mask_t mask = 0; mask < (1 << n); ++mask) {
            int cnt = __builtin_popcount(mask);
            if (cnt == half) {
                masks.push_back(mask);
            }
        }
        
        for (mask_t a_mask : masks) {
            mask_t b_mask = ((1 << n) - 1) ^ a_mask;
            int delta = game(a_mask, E, b_mask, E, r);
            if (delta > 0) {
                alex_wins = (1LL * alex_wins + 1) % MOD;
            } else if (delta < 0) {
                boris_wins = (1LL * boris_wins + 1) % MOD;
            } else {
                ties = (1LL * ties + 1) % MOD;
            }
        }
        
        cout << alex_wins << " " << boris_wins << " " << ties << endl;
    }
    
    return 0;
}