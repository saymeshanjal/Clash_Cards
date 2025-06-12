#include <bits/stdc++.h>
using namespace std;

typedef long long ll;
const int MOD = 998244353;
const int MAX_N = 16;

int n, E, r;

bitset<MAX_N> all_cards_mask;

map<tuple<bitset<MAX_N>, bitset<MAX_N>, int, int>, int> memo_win;
map<tuple<bitset<MAX_N>, bitset<MAX_N>, int, int>, pair<bitset<MAX_N>, bitset<MAX_N>>> memo_masks;
map<tuple<bitset<MAX_N>, bitset<MAX_N>, int, int>, pair<int, int>> memo_energy;

int can_win(bitset<MAX_N> init_mask, bitset<MAX_N> resp_mask, int init_energy, int resp_energy) {
    tuple<bitset<MAX_N>, bitset<MAX_N>, int, int> key = make_tuple(init_mask, resp_mask, init_energy, resp_energy);
    if (memo_win.count(key)) return memo_win[key];

    // Check if init can play any card
    bool can_play = false;
    for (int i = 0; i < n; ++i) {
        if (init_mask.test(i) && (i + 1) <= init_energy) {
            can_play = true;
            break;
        }
    }
    if (!can_play) {
        memo_win[key] = 2; // responder wins
        return 2;
    }

    // Check if both masks are empty
    if (init_mask.count() == 0 && resp_mask.count() == 0) {
        memo_win[key] = 0; // draw
        return 0;
    }

    // Try all possible x
    for (int x_bit = 0; x_bit < n; ++x_bit) {
        if (!init_mask.test(x_bit)) continue;
        int x = x_bit + 1;
        if (x > init_energy) continue;

        bitset<MAX_N> new_init_mask = init_mask;
        new_init_mask.reset(x_bit);
        int new_init_energy = init_energy - x;

        // Check if resp can respond
        bool all_resp_win = true;
        bool any_resp_loss = false;

        for (int y_bit = 0; y_bit < n; ++y_bit) {
            if (!resp_mask.test(y_bit)) continue;
            int y = y_bit + 1;
            if (y <= resp_energy && y > x) {
                bitset<MAX_N> new_resp_mask = resp_mask;
                new_resp_mask.reset(y_bit);
                int new_resp_energy = resp_energy - y;

                int outcome = can_win(new_resp_mask, new_init_mask, new_resp_energy, new_init_energy);
                if (outcome != 1) {
                    all_resp_win = false;
                }
                if (outcome == 2) {
                    any_resp_loss = true;
                }
            }
        }

        if (all_resp_win) {
            memo_win[key] = 1;
            return 1;
        }

        if (!any_resp_loss) {
            memo_win[key] = 1;
            return 1;
        }
    }

    memo_win[key] = 2;
    return 2;
}

pair<bitset<MAX_N>, bitset<MAX_N>> get_final_masks(bitset<MAX_N> init_mask, bitset<MAX_N> resp_mask, int init_energy, int resp_energy) {
    tuple<bitset<MAX_N>, bitset<MAX_N>, int, int> key = make_tuple(init_mask, resp_mask, init_energy, resp_energy);
    if (memo_masks.count(key)) return memo_masks[key];

    int outcome = can_win(init_mask, resp_mask, init_energy, resp_energy);
    if (outcome == 0) {
        memo_masks[key] = make_pair(init_mask, resp_mask);
        return make_pair(init_mask, resp_mask);
    }

    for (int x_bit = 0; x_bit < n; ++x_bit) {
        if (!init_mask.test(x_bit)) continue;
        int x = x_bit + 1;
        if (x > init_energy) continue;

        bitset<MAX_N> new_init_mask = init_mask;
        new_init_mask.reset(x_bit);
        int new_init_energy = init_energy - x;

        bool found = false;
        for (int y_bit = 0; y_bit < n; ++y_bit) {
            if (!resp_mask.test(y_bit)) continue;
            int y = y_bit + 1;
            if (y <= resp_energy && y > x) {
                bitset<MAX_N> new_resp_mask = resp_mask;
                new_resp_mask.reset(y_bit);
                int new_resp_energy = resp_energy - y;

                int next_outcome = can_win(new_resp_mask, new_init_mask, new_resp_energy, new_init_energy);
                if (outcome == 1 && next_outcome == 2) {
                    auto p = get_final_masks(new_resp_mask, new_init_mask, new_resp_energy, new_init_energy);
                    memo_masks[key] = make_pair(p.second, p.first);
                    return memo_masks[key];
                }
                if (outcome == 2 && next_outcome != 1) {
                    auto p = get_final_masks(new_resp_mask, new_init_mask, new_resp_energy, new_init_energy);
                    memo_masks[key] = make_pair(p.second, p.first);
                    return memo_masks[key];
                }
            }
        }

        if (found) break;
    }

    memo_masks[key] = make_pair(init_mask, resp_mask);
    return memo_masks[key];
}

pair<int, int> get_final_energy(bitset<MAX_N> init_mask, bitset<MAX_N> resp_mask, int init_energy, int resp_energy) {
    tuple<bitset<MAX_N>, bitset<MAX_N>, int, int> key = make_tuple(init_mask, resp_mask, init_energy, resp_energy);
    if (memo_energy.count(key)) return memo_energy[key];

    int outcome = can_win(init_mask, resp_mask, init_energy, resp_energy);
    if (outcome == 0) {
        memo_energy[key] = make_pair(init_energy, resp_energy);
        return make_pair(init_energy, resp_energy);
    }

    for (int x_bit = 0; x_bit < n; ++x_bit) {
        if (!init_mask.test(x_bit)) continue;
        int x = x_bit + 1;
        if (x > init_energy) continue;

        bitset<MAX_N> new_init_mask = init_mask;
        new_init_mask.reset(x_bit);
        int new_init_energy = init_energy - x;

        bool found = false;
        for (int y_bit = 0; y_bit < n; ++y_bit) {
            if (!resp_mask.test(y_bit)) continue;
            int y = y_bit + 1;
            if (y <= resp_energy && y > x) {
                bitset<MAX_N> new_resp_mask = resp_mask;
                new_resp_mask.reset(y_bit);
                int new_resp_energy = resp_energy - y;

                int next_outcome = can_win(new_resp_mask, new_init_mask, new_resp_energy, new_init_energy);
                if (outcome == 1 && next_outcome == 2) {
                    auto p = get_final_energy(new_resp_mask, new_init_mask, new_resp_energy, new_init_energy);
                    memo_energy[key] = make_pair(p.second, p.first);
                    return memo_energy[key];
                }
                if (outcome == 2 && next_outcome != 1) {
                    auto p = get_final_energy(new_resp_mask, new_init_mask, new_resp_energy, new_init_energy);
                    memo_energy[key] = make_pair(p.second, p.first);
                    return memo_energy[key];
                }
            }
        }

        if (found) break;
    }

    memo_energy[key] = make_pair(init_energy, resp_energy);
    return make_pair(init_energy, resp_energy);
}

int main() {
    int t;
    cin >> t;
    while (t--) {
        cin >> n >> E >> r;
        ll alex_wins = 0, boris_wins = 0, ties = 0;

        all_cards_mask = 0;
        for (int i = 0; i < n; ++i) {
            all_cards_mask.set(i);
        }

        int total_dists = 0;
        vector<bitset<MAX_N>> alex_masks;

        bitset<MAX_N> mask;
        function<void(int, int)> generate_masks = [&](int pos, int cnt) {
            if (cnt == n / 2) {
                alex_masks.push_back(mask);
                return;
            }
            if (pos >= n) return;
            mask.set(pos);
            generate_masks(pos + 1, cnt + 1);
            mask.reset(pos);
            generate_masks(pos + 1, cnt);
        };
        generate_masks(0, 0);

        for (auto a_mask : alex_masks) {
            bitset<MAX_N> b_mask = all_cards_mask & (~a_mask);

            vector<bitset<MAX_N>> a_round_masks(r + 1);
            vector<bitset<MAX_N>> b_round_masks(r + 1);
            vector<int> a_round_energy(r + 1);
            vector<int> b_round_energy(r + 1);

            a_round_masks[0] = a_mask;
            b_round_masks[0] = b_mask;
            a_round_energy[0] = E;
            b_round_energy[0] = E;

            int alex_rounds = 0;
            int boris_rounds = 0;

            for (int round = 1; round <= r; ++round) {
                bitset<MAX_N> a_m = a_round_masks[round - 1];
                bitset<MAX_N> b_m = b_round_masks[round - 1];
                int a_e = a_round_energy[round - 1];
                int b_e = b_round_energy[round - 1];

                int outcome = can_win(a_m, b_m, a_e, b_e);
                auto new_masks = get_final_masks(a_m, b_m, a_e, b_e);
                auto new_energy = get_final_energy(a_m, b_m, a_e, b_e);

                a_round_masks[round] = new_masks.first;
                b_round_masks[round] = new_masks.second;
                a_round_energy[round] = new_energy.first;
                b_round_energy[round] = new_energy.second;

                if (outcome == 1) {
                    alex_rounds++;
                } else if (outcome == 2) {
                    boris_rounds++;
                }
            }

            if (alex_rounds > boris_rounds) {
                alex_wins = (alex_wins + 1) % MOD;
            } else if (boris_rounds > alex_rounds) {
                boris_wins = (boris_wins + 1) % MOD;
            } else {
                ties = (ties + 1) % MOD;
            }
        }

        cout << alex_wins << " " << boris_wins << " " << ties << endl;
    }

    return 0;
}