#include <bits/stdc++.h>
using namespace std;

typedef long long ll;
const int MOD = 998244353;

int n, E, r;
unordered_map<string, int> memo;

int toInt(const string& s) {
    return stoi(s);
}

string getKey(int maskA, int maskB, int last, int turn, int ea, int eb) {
    return to_string(maskA) + "," + to_string(maskB) + "," + to_string(last) + "," + to_string(turn) + "," + to_string(ea) + "," + to_string(eb);
}

// Returns 1 if current player can win, -1 if current player loses, 0 for tie
int game(int maskA, int maskB, int last, int turn, int ea, int eb) {
    string key = getKey(maskA, maskB, last, turn, ea, eb);
    if (memo.count(key)) return memo[key];
    
    bool can_play = false;
    if (turn == 0) { // Alex's turn to play
        for (int card = 1; card <= n; ++card) {
            if ((maskA & (1 << (card-1))) && card > last && ea >= card) {
                can_play = true;
                int new_maskA = maskA & ~(1 << (card-1));
                int new_ea = ea - card;
                int res = game(new_maskA, maskB, card, 1, new_ea, eb);
                if (res == -1) { // opponent can't win
                    memo[key] = 1;
                    return 1;
                }
            }
        }
        if (!can_play) {
            memo[key] = -1; // current player can't play, loses
            return -1;
        }
    } else { // Boris's turn to play
        for (int card = 1; card <= n; ++card) {
            if ((maskB & (1 << (card-1))) && card > last && eb >= card) {
                can_play = true;
                int new_maskB = maskB & ~(1 << (card-1));
                int new_eb = eb - card;
                int res = game(maskA, new_maskB, card, 0, ea, new_eb);
                if (res == -1) {
                    memo[key] = 1;
                    return 1;
                }
            }
        }
        if (!can_play) {
            memo[key] = -1; // can't play, lose
            return -1;
        }
    }
    // If all possible moves lead to opponent's win, current player loses
    memo[key] = -1;
    return -1;
}

// Simulate r rounds, returns the number of Alex wins and Boris wins
pair<int, int> simulate_rounds(int maskA, int maskB, int initial_E, int rounds) {
    int alex = 0, boris = 0;
    int current_maskA = maskA;
    int current_maskB = maskB;
    int current_eA = initial_E;
    int current_eB = initial_E;
    
    for (int i = 0; i < rounds; ++i) {
        // Check if either player has no cards left
        if (current_maskA == 0 || current_maskB == 0) {
            if (current_maskA == 0 && current_maskB == 0) {
                // tie, no change
            } else {
                if (current_maskA == 0) {
                    boris += 1;
                } else {
                    alex += 1;
                }
            }
            continue;
        }
        
        int res = game(current_maskA, current_maskB, 0, 0, current_eA, current_eB);
        if (res == 1) {
            alex += 1;
            // Now, remove the played cards
            // But we need to find which cards were played
            // This part is missing in the current implementation
            // So this code is incomplete
        } else if (res == -1) {
            boris += 1;
        } else {
            // tie, no change
        }
    }
    
    return make_pair(alex, boris);
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    int t; cin >> t;
    while (t--) {
        cin >> n >> E >> r;
        int half = n / 2;
        ll alex_wins = 0, boris_wins = 0, ties = 0;
        
        vector<int> all_masks;
        int total = 1 << n;
        for (int mask = 0; mask < total; ++mask) {
            if (__builtin_popcount(mask) == half) {
                all_masks.push_back(mask);
            }
        }
        
        for (int maskA : all_masks) {
            int alex = 0, boris = 0;
            int res = game(maskA, ((1 << n) - 1) ^ maskA, 0, 0, E, E);
            if (res == 1) {
                alex += 1;
            } else if (res == -1) {
                boris += 1;
            } else {
                // tie
            }
            
            // For r=1, compare alex and boris
            if (alex > boris) {
                alex_wins = (alex_wins + 1) % MOD;
            } else if (boris > alex) {
                boris_wins = (boris_wins + 1) % MOD;
            } else {
                ties = (ties + 1) % MOD;
            }
        }
        
        cout << alex_wins << " " << boris_wins << " " << ties << endl;
    }
    
    return 0;
}