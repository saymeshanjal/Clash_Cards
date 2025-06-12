#include <bits/stdc++.h>
using namespace std;
using ll = long long;

const ll MOD = 998244353;

int n_global;
int E_global;
int r_global;

struct Outcome {
    int winner; // 0: tie, 1: attacker win, 2: defender win
    int a_mask;
    int b_mask;
    int a_energy;
    int b_energy;
};

map<ll, Outcome> dp;

ll get_key(bool attacker_alex, int a_mask, int a_energy, int b_mask, int b_energy) {
    ll key = 0;
    key |= (ll(attacker_alex) << 63);
    key |= (ll(a_mask) << 45);
    key |= (ll(a_energy) << 30);
    key |= (ll(b_mask) << 15);
    key |= (ll(b_energy));
    return key;
}

Outcome simulate_round(bool attacker_alex, int a_mask, int a_energy, int b_mask, int b_energy) {
    ll key = get_key(attacker_alex, a_mask, a_energy, b_mask, b_energy);
    if (dp.find(key) != dp.end()) {
        return dp[key];
    }

    Outcome tie_outcome;
    tie_outcome.winner = 0;
    tie_outcome.a_mask = a_mask;
    tie_outcome.b_mask = b_mask;
    tie_outcome.a_energy = a_energy;
    tie_outcome.b_energy = b_energy;

    if (a_mask == 0 && b_mask == 0) {
        dp[key] = tie_outcome;
        return tie_outcome;
    }

    vector<int> possible_x;
    if (attacker_alex) {
        for (int card = 1; card <= n_global; ++card) {
            if ((a_mask & (1 << (card -1))) && (a_energy >= card)) {
                possible_x.push_back(card);
            }
        }
    } else {
        for (int card = 1; card <= n_global; ++card) {
            if ((b_mask & (1 << (card -1))) && (b_energy >= card)) {
                possible_x.push_back(card);
            }
        }
    }

    if (possible_x.empty()) {
        Outcome def_win = tie_outcome;
        def_win.winner = 2;
        dp[key] = def_win;
        return def_win;
    }

    for (int x : possible_x) {
        int new_a_mask = a_mask;
        int new_a_energy = a_energy;
        int new_b_mask = b_mask;
        int new_b_energy = b_energy;

        if (attacker_alex) {
            new_a_mask &= ~(1 << (x - 1));
            new_a_energy -= x;
        } else {
            new_b_mask &= ~(1 << (x - 1));
            new_b_energy -= x;
        }

        vector<int> possible_y;
        if (attacker_alex) {
            for (int y = x + 1; y <= n_global; ++y) {
                if ((b_mask & (1 << (y - 1))) && (b_energy >= y)) {
                    possible_y.push_back(y);
                }
            }
        } else {
            for (int y = x + 1; y <= n_global; ++y) {
                if ((a_mask & (1 << (y - 1))) && (a_energy >= y)) {
                    possible_y.push_back(y);
                }
            }
        }

        if (possible_y.empty()) {
            Outcome res = tie_outcome;
            if (attacker_alex) {
                res.a_mask = new_a_mask;
                res.a_energy = new_a_energy;
            } else {
                res.b_mask = new_b_mask;
                res.b_energy = new_b_energy;
            }
            res.winner = 1;
            dp[key] = res;
            return res;
        }

        bool all_defense_lead_to_win = true;
        for (int y : possible_y) {
            int next_a_mask = new_a_mask;
            int next_a_energy = new_a_energy;
            int next_b_mask = new_b_mask;
            int next_b_energy = new_b_energy;

            if (attacker_alex) {
                next_b_mask &= ~(1 << (y - 1));
                next_b_energy -= y;
            } else {
                next_a_mask &= ~(1 << (y - 1));
                next_a_energy -= y;
            }

            Outcome next = simulate_round(!attacker_alex, next_a_mask, next_a_energy, next_b_mask, next_b_energy);
            if (next.winner != 1) {
                all_defense_lead_to_win = false;
                break;
            }
        }

        if (all_defense_lead_to_win) {
            Outcome res = tie_outcome;
            res.a_mask = new_a_mask;
            res.b_mask = new_b_mask;
            res.a_energy = new_a_energy;
            res.b_energy = new_b_energy;
            res.winner = 1;
            dp[key] = res;
            return res;
        }
    }

    Outcome def_win = tie_outcome;
    def_win.winner = 2;
    dp[key] = def_win;
    return def_win;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    int t;
    cin >> t;
    while (t--) {
        dp.clear();
        cin >> n_global >> E_global >> r_global;

        ll alex_wins = 0;
        ll boris_wins = 0;
        ll ties = 0;

        int total_cards = n_global;
        int alex_cards = n_global / 2;
        int mask_limit = 1 << n_global;

        for (int mask = 0; mask < mask_limit; ++mask) {
            if (__builtin_popcount(mask) != alex_cards) continue;

            int current_a_mask = mask;
            int current_b_mask = ((1 << n_global) - 1) ^ mask;
            int a_energy = E_global;
            int b_energy = E_global;

            int alex_rounds = 0;
            int boris_rounds = 0;
            bool valid_round = true;

            for (int round = 0; round < r_global; ++round) {
                if (current_a_mask == 0 && current_b_mask == 0) {
                    break;
                }

                Outcome res = simulate_round(true, current_a_mask, a_energy, current_b_mask, b_energy);

                if (res.winner == 1) {
                    alex_rounds++;
                } else if (res.winner == 2) {
                    boris_rounds++;
                }

                current_a_mask = res.a_mask;
                current_b_mask = res.b_mask;
                a_energy = res.a_energy;
                b_energy = res.b_energy;
            }

            if (alex_rounds > boris_rounds) {
                alex_wins = (alex_wins + 1) % MOD;
            } else if (boris_rounds > alex_rounds) {
                boris_wins = (boris_wins + 1) % MOD;
            } else if (alex_rounds == boris_rounds && (alex_rounds + boris_rounds) > 0) {
                ties = (ties + 1) % MOD;
            }
        }

        cout << alex_wins << " " << boris_wins << " " << ties << "\n";
    }

    return 0;
}