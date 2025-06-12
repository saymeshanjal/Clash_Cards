#include <bits/stdc++.h>
using namespace std;

const int MOD = 998244353;

int n, E, r;
int max_mask;

map<tuple<int, int, int, int, int, int>, tuple<int, int, int>> dp_round; // Memoization for round outcomes

// Function to compute the result of a round
tuple<int, int, int, int, int> play_round(int a_cards, int b_cards, int eA, int eB) {
    static map<tuple<int, int, int, int>, tuple<int, int, int, int, int>> memo;
    tuple<int, int, int, int> state = make_tuple(a_cards, b_cards, eA, eB);
    if (memo.count(state)) return memo[state];
    
    // Check if either player can start the round
    bool a_can_play = false;
    for (int card = 1; card <= n; ++card) {
        if ((a_cards & (1 << (card - 1))) && eA >= card) {
            a_can_play = true;
            break;
        }
    }
    if (!a_can_play) {
        return memo[state] = make_tuple(1, 0, 0, 0, 0); // Boris wins the round
    }
    
    bool b_can_play = false;
    for (int card = 1; card <= n; ++card) {
        if ((b_cards & (1 << (card - 1))) && eB >= card) {
            b_can_play = true;
            break;
        }
    }
    if (!b_can_play) {
        return memo[state] = make_tuple(0, 0, 0, 0, 0); // Alex wins the round
    }
    
    // Try all possible plays for Alex
    vector<int> a_possible;
    for (int card = 1; card <= n; ++card) {
        if ((a_cards & (1 << (card - 1))) && eA >= card) {
            a_possible.push_back(card);
        }
    }
    if (a_possible.empty()) {
        return memo[state] = make_tuple(1, 0, 0, 0, 0); // Boris wins
    }
    
    for (int x : a_possible) {
        int new_a_cards = a_cards & ~(1 << (x - 1));
        int new_eA = eA - x;
        
        // Boris must play a y > x
        vector<int> b_possible;
        for (int card = x + 1; card <= n; ++card) {
            if ((b_cards & (1 << (card - 1))) && eB >= card) {
                b_possible.push_back(card);
            }
        }
        if (b_possible.empty()) {
            return memo[state] = make_tuple(0, x, 0, 0, 0); // Alex wins this round, played x
        }
        
        for (int y : b_possible) {
            int new_b_cards = b_cards & ~(1 << (y - 1));
            int new_eB = eB - y;
            
            // Now Boris starts the next exchange
            auto [result, played_a1, played_b1, played_a2, played_b2] = play_round(new_b_cards, new_a_cards, new_eB, new_eA);
            if (result == 1) {
                // Boris wins this sub-exchange, so Alex loses the round
                continue;
            } else {
                // Alex wins the round
                return memo[state] = make_tuple(0, x, y, played_a1 | played_a2, played_b1 | played_b2);
            }
        }
    }
    
    // If no play leads to Alex winning, he loses
    return memo[state] = make_tuple(1, 0, 0, 0, 0);
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    
    int t;
    cin >> t;
    while (t--) {
        cin >> n >> E >> r;
        max_mask = 1 << n;
        
        int alex_wins = 0, boris_wins = 0, ties = 0;
        
        for (int mask = 0; mask < max_mask; ++mask) {
            if (__builtin_popcount(mask) != n / 2) continue;
            
            int a_cards = mask;
            int b_cards = ((1 << n) - 1) & ~mask;
            
            vector<tuple<int, int, int, int, int, int>> round_results;
            int current_a_cards = a_cards;
            int current_b_cards = b_cards;
            int current_eA = E;
            int current_eB = E;
            
            int a_rounds = 0, b_rounds = 0;
            bool valid = true;
            
            for (int round = 0; round < r; ++round) {
                auto res = play_round(current_a_cards, current_b_cards, current_eA, current_eB);
                int winner = get<0>(res);
                if (winner == 0) {
                    a_rounds++;
                } else if (winner == 1) {
                    b_rounds++;
                } else {
                    valid = false;
                    break;
                }
                int x = get<1>(res);
                int y = get<2>(res);
                int played_a = get<3>(res);
                int played_b = get<4>(res);
                
                current_a_cards &= ~((1 << (x - 1)) | played_a);
                current_b_cards &= ~((1 << (y - 1)) | played_b);
                current_eA -= x;
                for (int card = 1; card <= n; ++card) {
                    if (played_a & (1 << (card - 1))) {
                        current_eA -= card;
                    }
                }
                current_eB -= y;
                for (int card = 1; card <= n; ++card) {
                    if (played_b & (1 << (card - 1))) {
                        current_eB -= card;
                    }
                }
            }
            
            if (!valid) {
                ties++;
                continue;
            }
            
            if (a_rounds > b_rounds) {
                alex_wins++;
            } else if (b_rounds > a_rounds) {
                boris_wins++;
            } else {
                ties++;
            }
        }
        
        cout << alex_wins % MOD << " " << boris_wins % MOD << " " << ties % MOD << "\n";
    }
    
    return 0;
}