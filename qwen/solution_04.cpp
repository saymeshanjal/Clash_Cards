#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const int MOD = 998244353;

// Define a structure to hold the result of a round simulation
struct RoundResult {
    int outcome; // 0: Alex wins, 1: Boris wins, 2: draw
    ll mask_a;
    ll mask_b;
    int e_a;
    int e_b;
};

map<tuple<ll, ll, int, int, int, bool>, RoundResult> memo;

vector<int> get_playable(ll mask, int prev_card, int energy) {
    vector<int> res;
    for (int i = 0; i < 16; ++i) {
        if ((mask >> i) & 1) {
            int card = i + 1;
            if (card > prev_card && energy >= card)
                res.push_back(card);
        }
    }
    return res;
}

RoundResult simulate_round(ll mask_a, ll mask_b, int ea, int eb, int prev_max, bool is_attacker_alex) {
    auto key = make_tuple(mask_a, mask_b, ea, eb, prev_max, is_attacker_alex);
    if (memo.count(key)) return memo[key];

    ll current_mask = is_attacker_alex ? mask_a : mask_b;
    int current_energy = is_attacker_alex ? ea : eb;
    vector<int> candidates = get_playable(current_mask, prev_max, current_energy);

    // Check if attacker can play any card
    if (candidates.empty()) {
        // Check if defender can play
        ll def_mask = is_attacker_alex ? mask_b : mask_a;
        int def_energy = is_attacker_alex ? eb : ea;
        bool def_can_play = false;
        for (int i = 0; i < 16; ++i) {
            if ((def_mask >> i) & 1) {
                int card = i + 1;
                if (card > prev_max && def_energy >= card) {
                    def_can_play = true;
                    break;
                }
            }
        }
        if (!def_can_play) {
            return memo[key] = {2, mask_a, mask_b, ea, eb};
        } else {
            return memo[key] = {1, mask_a, mask_b, ea, eb};
        }
    }

    // Try all possible moves
    RoundResult best = {1, mask_a, mask_b, ea, eb}; // Assume defender wins
    for (int x : candidates) {
        int idx = x - 1;
        ll new_mask_a = mask_a;
        ll new_mask_b = mask_b;
        int new_ea = ea;
        int new_eb = eb;
        if (is_attacker_alex) {
            new_mask_a &= ~(1LL << idx);
            new_ea -= x;
        } else {
            new_mask_b &= ~(1LL << idx);
            new_eb -= x;
        }
        RoundResult next = simulate_round(new_mask_a, new_mask_b, new_ea, new_eb, x, !is_attacker_alex);
        if (next.outcome == 1) {
            // Defender wins in this path, attacker loses
            continue;
        } else if (next.outcome == 0) {
            // Attacker wins, this is a better outcome
            best = {0, next.mask_a, next.mask_b, next.e_a, next.e_b};
            break;
        } else if (best.outcome != 0) {
            best = {2, next.mask_a, next.mask_b, next.e_a, next.e_b};
        }
    }

    if (best.outcome == 0) {
        if (is_attacker_alex) {
            return memo[key] = best;
        } else {
            return memo[key] = best;
        }
    }

    // Check for draw possibility
    for (int x : candidates) {
        int idx = x - 1;
        ll new_mask_a = mask_a;
        ll new_mask_b = mask_b;
        int new_ea = ea;
        int new_eb = eb;
        if (is_attacker_alex) {
            new_mask_a &= ~(1LL << idx);
            new_ea -= x;
        } else {
            new_mask_b &= ~(1LL << idx);
            new_eb -= x;
        }
        RoundResult next = simulate_round(new_mask_a, new_mask_b, new_ea, new_eb, x, !is_attacker_alex);
        if (next.outcome == 2) {
            return memo[key] = {2, next.mask_a, next.mask_b, next.e_a, next.e_b};
        }
    }

    return memo[key] = best;
}

vector<ll> generate_combinations(int n, int k) {
    vector<ll> res;
    for (ll mask = 0; mask < (1LL << n); ++mask) {
        if (__builtin_popcountll(mask) == k) {
            res.push_back(mask);
        }
    }
    return res;
}

int main() {
    int t;
    cin >> t;
    while (t--) {
        int n, E, r;
        cin >> n >> E >> r;
        int half = n / 2;
        vector<ll> masks = generate_combinations(n, half);
        ll alex_win = 0, boris_win = 0, tie_count = 0;

        for (ll amask : masks) {
            ll bmask = ((1LL << n) - 1) ^ amask;
            int total_rounds = r;
            int win_a = 0, win_b = 0;

            for (int round = 0; round < r; ++round) {
                memo.clear();
                RoundResult res = simulate_round(amask, bmask, E, E, 0, true);
                if (res.outcome == 0) {
                    win_a++;
                } else if (res.outcome == 1) {
                    win_b++;
                } else {
                    // Draw round
                }
                amask = res.mask_a;
                bmask = res.mask_b;
            }

            if (win_a > win_b) {
                alex_win++;
            } else if (win_b > win_a) {
                boris_win++;
            } else {
                tie_count++;
            }
        }

        cout << alex_win % MOD << " " << boris_win % MOD << " " << tie_count % MOD << endl;
    }

    return 0;
}