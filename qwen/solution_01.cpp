#include <bits/stdc++.h>
using namespace std;

typedef long long ll;
const ll MOD = 998244353;

enum Outcome { ALEX_WIN, BORIS_WIN, TIE };

struct State {
    int a_mask, b_mask;
    int a_energy, b_energy;
    int required;
    bool turn; // false: Alex's turn to start, true: Boris's turn to start
    bool operator<(const State& other) const {
        if (a_mask != other.a_mask) return a_mask < other.a_mask;
        if (b_mask != other.b_mask) return b_mask < other.b_mask;
        if (a_energy != other.a_energy) return a_energy < other.a_energy;
        if (b_energy != other.b_energy) return b_energy < other.b_energy;
        if (required != other.required) return required < other.required;
        return turn < other.turn;
    }
};

map<State, pair<Outcome, pair<int, int>>> memo;

pair<Outcome, pair<int, int>> simulate_round(int a_mask, int b_mask, int a_energy, int b_energy, int required, bool turn) {
    State s = {a_mask, b_mask, a_energy, b_energy, required, turn};
    if (memo.count(s)) return memo[s];

    vector<int> a_playable, b_playable;
    for (int i = 0; i < 16; ++i) {
        if ((a_mask >> i) & 1) {
            int card = i + 1;
            if (card > required && card <= a_energy) {
                a_playable.push_back(card);
            }
        }
    }
    for (int i = 0; i < 16; i++) {
        if ((b_mask >> i) & 1) {
            int card = i + 1;
            if (card > required && card <= b_energy) {
                b_playable.push_back(card);
            }
        }
    }

    if (turn == 0) {
        if (a_playable.empty()) {
            if (a_mask == 0 && b_mask == 0) {
                return memo[s] = {TIE, {0, 0}};
            } else {
                return memo[s] = {BORIS_WIN, {0, 0}};
            }
        }
        Outcome best_outcome = BORIS_WIN;
        int best_a = 0, best_b = 0;
        for (int card : a_playable) {
            int new_mask = a_mask & ~(1 << (card - 1));
            int new_energy = a_energy - card;
            auto res = simulate_round(b_mask, new_mask, b_energy, new_energy, card, true);
            Outcome outcome = res.first;
            if (outcome == BORIS_WIN) {
                best_outcome = ALEX_WIN;
                best_a = card;
                best_b = 0;
                break;
            } else if (outcome == TIE && best_outcome != ALEX_WIN) {
                best_outcome = TIE;
                best_a = card;
                best_b = 0;
            }
        }
        if (best_outcome == ALEX_WIN) {
            return memo[s] = {ALEX_WIN, {best_a, 0}};
        } else if (best_outcome == TIE) {
            return memo[s] = {TIE, {best_a, 0}};
        } else {
            return memo[s] = {BORIS_WIN, {0, 0}};
        }
    } else {
        if (b_playable.empty()) {
            if (a_mask == 0 && b_mask == 0) {
                return memo[s] = {TIE, {0, 0}};
            } else {
                return memo[s] = {ALEX_WIN, {0, 0}};
            }
        }
        Outcome best_outcome = ALEX_WIN;
        int best_a = 0, best_b = 0;
        for (int card : b_playable) {
            int new_mask = b_mask & ~(1 << (card - 1));
            int new_energy = b_energy - card;
            auto res = simulate_round(a_mask, new_mask, a_energy, new_energy, card, false);
            Outcome outcome = res.first;
            if (outcome == ALEX_WIN) {
                best_outcome = BORIS_WIN;
                best_b = card;
                best_a = 0;
                break;
            } else if (outcome == TIE && best_outcome != BORIS_WIN) {
                best_outcome = TIE;
                best_b = card;
                best_a = 0;
            }
        }
        if (best_outcome == BORIS_WIN) {
            return memo[s] = {BORIS_WIN, {0, best_b}};
        } else if (best_outcome == TIE) {
            return memo[s] = {TIE, {0, best_b}};
        } else {
            return memo[s] = {ALEX_WIN, {0, 0}};
        }
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int t;
    cin >> t;
    while (t--) {
        int n, E, r;
        cin >> n >> E >> r;
        int total = 0;
        ll alex_wins = 0, boris_wins = 0, ties = 0;
        int half = n / 2;
        vector<int> cards;
        for (int i = 1; i <= n; i++) cards.push_back(i);
        vector<int> mask_list;
        for (int mask = 0; mask < (1 << n); mask++) {
            if (__builtin_popcount(mask) == half) {
                mask_list.push_back(mask);
            }
        }
        for (int a_mask : mask_list) {
            memo.clear();
            int b_mask = ((1 << n) - 1) ^ a_mask;
            vector<int> a_win(r, 0), b_win(r, 0);
            vector<int> curr_a_mask = a_mask;
            vector<int> curr_b_mask = b_mask;
            int curr_a_energy = E;
            int curr_b_energy = E;
            vector<int> round_a_win(r, 0), round_b_win(r, 0);
            for (int round = 0; round < r; round++) {
                auto res = simulate_round(curr_a_mask, curr_b_mask, curr_a_energy, curr_b_energy, 0, false);
                Outcome outcome = res.first;
                int a_card = res.second.first;
                int b_card = res.second.second;
                if (outcome == ALEX_WIN) {
                    round_a_win[round] = 1;
                } else if (outcome == BORIS_WIN) {
                    round_b_win[round] = 1;
                } else {
                    // tie, no change
                }
                if (a_card != 0) {
                    curr_a_energy -= a_card;
                    curr_a_mask &= ~(1 << (a_card - 1));
                }
                if (b_card != 0) {
                    curr_b_energy -= b_card;
                    curr_b_mask &= ~(1 << (b_card - 1));
                }
            }
            int a_total = 0, b_total = 0;
            for (int i = 0; i < r; i++) {
                a_total += round_a_win[i];
                b_total += round_b_win[i];
            }
            if (a_total > b_total) {
                alex_wins = (alex_wins + 1) % MOD;
            } else if (b_total > a_total) {
                boris_wins = (boris_wins + 1) % MOD;
            } else {
                ties = (ties + 1) % MOD;
            }
        }
        cout << alex_wins << " " << boris_wins << " " << ties << endl;
    }
    return 0;
}