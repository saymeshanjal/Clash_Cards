#include <bits/stdc++.h>
using namespace std;

typedef long long ll;

const int MOD = 998244353;

struct Result {
    int outcome; // 0: Alex wins round, 1: Boris, 2: draw
    int played_a;
    int played_b;
};

class GameSimulator {
public:
    map<tuple<int, int, int, int, int>, Result> memo;

    Result simulate_exchange(int a_mask, int b_mask, int a_energy, int b_energy, int turn) {
        tuple<int, int, int, int, int> key = make_tuple(a_mask, b_mask, a_energy, b_energy, turn);
        if (memo.count(key)) return memo[key];
        Result res;
        res.outcome = 2;
        res.played_a = res.played_b = 0;
        if (a_mask == 0 && b_mask == 0) {
            memo[key] = res;
            return res;
        }
        if (turn == 0) {
            bool alex_can_play = false;
            for (int x = 1; x <= 16; ++x) {
                if ((a_mask & (1 << (x-1))) && x <= a_energy) {
                    alex_can_play = true;
                    break;
                }
            }
            if (!alex_can_play) {
                res.outcome = 1;
                memo[key] = res;
                return res;
            }
            Result best;
            best.outcome = -1;
            for (int x = 1; x <= 16; ++x) {
                if ((a_mask & (1 << (x-1))) == 0 || x > a_energy) continue;
                int new_a_mask = a_mask & ~(1 << (x-1));
                int new_a_energy = a_energy - x;
                vector<int> possible_y;
                for (int y = x + 1; y <= 16; ++y) {
                    if ((b_mask & (1 << (y-1))) && y <= b_energy) {
                        possible_y.push_back(y);
                    }
                }
                if (possible_y.empty()) {
                    res.outcome = 0;
                    res.played_a = 1 << (x-1);
                    res.played_b = 0;
                    memo[key] = res;
                    return res;
                } else {
                    Result best_y;
                    best_y.outcome = 1;
                    for (int y : possible_y) {
                        int new_b_mask = b_mask & ~(1 << (y-1));
                        int new_b_energy = b_energy - y;
                        Result sub = simulate_exchange(new_a_mask, new_b_mask, new_a_energy, new_b_energy, 1);
                        sub.played_a |= (1 << (x-1));
                        sub.played_b |= (1 << (y-1));
                        if (sub.outcome < best_y.outcome) {
                            best_y = sub;
                        }
                    }
                    if (best.outcome == -1 || best_y.outcome > best.outcome) {
                        best = best_y;
                        best.played_a |= (1 << (x-1));
                    } else if (best_y.outcome == best.outcome) {
                        if ((best.played_a | best_y.played_a) != best.played_a) {
                            best = best_y;
                        }
                    }
                }
            }
            if (best.outcome != -1) {
                memo[key] = best;
                return best;
            }
        } else {
            bool boris_can_play = false;
            for (int y = 1; y <= 16; ++y) {
                if ((b_mask & (1 << (y-1))) && y <= b_energy) {
                    boris_can_play = true;
                    break;
                }
            }
            if (!boris_can_play) {
                res.outcome = 0;
                memo[key] = res;
                return res;
            }
            Result best;
            best.outcome = -1;
            for (int y = 1; y <= 16; ++y) {
                if ((b_mask & (1 << (y-1))) == 0 || y > b_energy) continue;
                int new_b_mask = b_mask & ~(1 << (y-1));
                int new_b_energy = b_energy - y;
                vector<int> possible_x;
                for (int x = y + 1; x <= 16; ++x) {
                    if ((a_mask & (1 << (x-1))) && x <= a_energy) {
                        possible_x.push_back(x);
                    }
                }
                if (possible_x.empty()) {
                    res.outcome = 1;
                    res.played_b = 1 << (y-1);
                    res.played_a = 0;
                    memo[key] = res;
                    return res;
                } else {
                    Result best_x;
                    best_x.outcome = 0;
                    for (int x : possible_x) {
                        int new_a_mask = a_mask & ~(1 << (x-1));
                        int new_a_energy = a_energy - x;
                        Result sub = simulate_exchange(new_a_mask, new_b_mask, new_a_energy, new_b_energy, 0);
                        sub.played_a |= (1 << (x-1));
                        sub.played_b |= (1 << (y-1));
                        if (sub.outcome > best_x.outcome) {
                            best_x = sub;
                        }
                    }
                    if (best.outcome == -1 || best_x.outcome < best.outcome) {
                        best = best_x;
                        best.played_b |= (1 << (y-1));
                    } else if (best_x.outcome == best.outcome) {
                        if ((best.played_b | best_x.played_b) != best.played_b) {
                            best = best_x;
                        }
                    }
                }
            }
            if (best.outcome != -1) {
                memo[key] = best;
                return best;
            }
        }
        memo[key] = res;
        return res;
    }

    tuple<int, int, int> simulate_round(int a_mask, int b_mask, int a_energy, int b_energy) {
        Result res = simulate_exchange(a_mask, b_mask, a_energy, b_energy, 0);
        int sum_a = 0, sum_b = 0;
        for (int i = 0; i < 16; ++i) {
            if (res.played_a & (1 << i)) sum_a += i + 1;
            if (res.played_b & (1 << i)) sum_b += i + 1;
        }
        return make_tuple(res.outcome, sum_a, sum_b);
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int t;
    cin >> t;
    while (t--) {
        int n, E, rounds;
        cin >> n >> E >> rounds;
        int alex_wins = 0, boris_wins = 0, ties = 0;
        int half = n / 2;
        vector<int> dist;
        for (int mask = 0; mask < (1 << n); ++mask) {
            if (__builtin_popcount(mask) != half) continue;
            GameSimulator sim;
            vector<int> alex_cards;
            int a_mask = 0, b_mask = 0;
            for (int i = 0; i < n; ++i) {
                if (mask & (1 << i)) {
                    a_mask |= (1 << i);
                } else {
                    b_mask |= (1 << i);
                }
            }
            int total_rounds = rounds;
            int curr_rounds = 0;
            int a_energy = E, b_energy = E;
            int aw = 0, bw = 0, dr = 0;
            bool valid = true;
            for (int r = 0; r < rounds; ++r) {
                int oc, sa, sb;
                tie(oc, sa, sb) = sim.simulate_round(a_mask, b_mask, a_energy, b_energy);
                if (oc == 0) aw++;
                else if (oc == 1) bw++;
                else dr++;
                int new_a_mask = a_mask;
                int new_b_mask = b_mask;
                for (int i = 0; i < n; ++i) {
                    if ((a_mask & (1 << i)) && ((sim.memo.begin()->second).played_a & (1 << i))) {
                        new_a_mask &= ~(1 << i);
                    }
                    if ((b_mask & (1 << i)) && ((sim.memo.begin()->second).played_b & (1 << i))) {
                        new_b_mask &= ~(1 << i);
                    }
                }
                a_mask = new_a_mask;
                b_mask = new_b_mask;
                a_energy -= sa;
                b_energy -= sb;
                if (a_energy < 0 || b_energy < 0) {
                    valid = false;
                    break;
                }
            }
            if (!valid) continue;
            if (aw > bw) alex_wins++;
            else if (bw > aw) boris_wins++;
            else ties++;
        }
        cout << (alex_wins % MOD) << " " << (boris_wins % MOD) << " " << (ties % MOD) << endl;
    }
    return 0;
}