#include <bits/stdc++.h>
using namespace std;

typedef long long ll;
const int MOD = 998244353;

const int MAX_N = 16;

// Precompute factorials and inverse factorials for combination calculations
ll fact[20], inv_fact[20];
ll modpow(ll a, ll b, ll m) {
    ll res = 1;
    while (b > 0) {
        if (b & 1) res = (res * a) % m;
        a = (a * a) % m;
        b >>= 1;
    }
    return res;
}
void precompute_factorials(int max_n, ll mod) {
    fact[0] = 1;
    for (int i = 1; i <= max_n; ++i)
        fact[i] = (fact[i-1] * i) % mod;
    inv_fact[max_n] = modpow(fact[max_n], mod-2, mod);
    for (int i = max_n-1; i >=0; --i)
        inv_fact[i] = (inv_fact[i+1] * (i+1)) % mod;
}
ll comb(int n, int k, ll mod) {
    if (k < 0 || k > n) return 0;
    return fact[n] * inv_fact[k] % mod * inv_fact[n-k] % mod;
}

// Memoization for the round simulation
map<tuple<bool, int, int, int, int, int>, tuple<char, int, int, int, int>> round_memo;

// Simulates a single round's steps recursively
tuple<char, int, int, int, int> simulate_round_step(
    bool is_alice_turn, int last_val,
    int mask_a, int e_a, int mask_b, int e_b) {
    
    tuple<bool, int, int, int, int, int> key = 
        {is_alice_turn, last_val, mask_a, e_a, mask_b, e_b};
    if (round_memo.count(key)) return round_memo[key];
    
    // Check if both masks are empty
    if (mask_a == 0 && mask_b == 0) {
        return round_memo[key] = {'T', mask_a, e_a, mask_b, e_b};
    }
    
    // Current player's turn
    if (is_alice_turn) {
        // Alice must play a card > last_val
        vector<int> candidates;
        for (int card = 1; card <= MAX_N; ++card) {
            if ((mask_a & (1 << (card-1))) && card <= e_a && card > last_val) {
                candidates.push_back(card);
            }
        }
        if (candidates.empty()) {
            // Alice freezes, Boris wins the round
            return round_memo[key] = {'B', mask_a, e_a, mask_b, e_b};
        }
        
        // Try each candidate and choose the best outcome for Alice
        char best_outcome = 'B'; // Worst case
        int best_mask_a = mask_a, best_e_a = e_a;
        int best_mask_b = mask_b, best_e_b = e_b;
        
        for (int card : candidates) {
            int new_mask_a = mask_a & ~(1 << (card-1));
            int new_e_a = e_a - card;
            
            // Boris's turn to respond
            auto [outcome, ra, rea, rb, reb] = simulate_round_step(
                false, card, new_mask_a, new_e_a, mask_b, e_b);
                
            if (outcome == 'T' && mask_a == (1 << (card-1)) && mask_b == 0) {
                // Special case: both masks empty after this play
                // But Boris has nothing to play
                outcome = 'A';
            }
            
            if (outcome == 'A' || outcome == 'B' || outcome == 'T') {
                // Now check what happens after this move
                if (outcome == 'T') {
                    // Check if both masks are now empty
                    bool a_empty = (ra == 0);
                    bool b_empty = (rb == 0);
                    if (a_empty && b_empty) {
                        outcome = 'T';
                    } else {
                        // Continue normally
                    }
                }
                
                // If outcome is 'T', proceed to see
                if (outcome == 'T') {
                    // Check if both masks are now empty
                    if ((ra == 0) && (rb == 0)) {
                        return round_memo[key] = {'T', 0, new_e_a, 0, e_b};
                    }
                }
                
                // Proceed to next steps
                auto next_outcome = outcome;
                int nma = ra, nea = rea, nmb = rb, neb = reb;
                
                if (next_outcome == 'A' || next_outcome == 'T') {
                    // If outcome is 'A' or 'T', compare and keep the best for Alice
                    if (next_outcome == 'A') {
                        best_outcome = 'A';
                        best_mask_a = ra;
                        best_e_a = rea;
                        best_mask_b = rb;
                        best_e_b = reb;
                        break; // Alice prefers to win immediately
                    } else if (best_outcome != 'A') {
                        best_outcome = 'T';
                        best_mask_a = ra;
                        best_e_a = rea;
                        best_mask_b = rb;
                        best_e_b = reb;
                    }
                }
            }
        }
        
        return round_memo[key] = {best_outcome, best_mask_a, best_e_a, best_mask_b, best_e_b};
        
    } else {
        // Boris's turn
        vector<int> candidates;
        for (int card = 1; card <= MAX_N; ++card) {
            if ((mask_b & (1 << (card-1))) && card <= e_b && card > last_val) {
                candidates.push_back(card);
            }
        }
        if (candidates.empty()) {
            // Boris freezes, Alex wins
            return round_memo[key] = {'A', mask_a, e_a, mask_b, e_b};
        }
        
        // Try each candidate and choose the best outcome for Boris
        char best_outcome = 'A'; // Worst case for Boris
        int best_mask_a = mask_a, best_e_a = e_a;
        int best_mask_b = mask_b, best_e_b = e_b;
        
        for (int card : candidates) {
            int new_mask_b = mask_b & ~(1 << (card-1));
            int new_e_b = e_b - card;
            
            // Alice's turn to respond
            auto [outcome, ra, rea, rb, reb] = simulate_round_step(
                true, card, mask_a, e_a, new_mask_b, new_e_b);
                
            if (outcome == 'A' || outcome == 'B' || outcome == 'T') {
                if (outcome == 'B') {
                    // Boris prefers to win
                    return round_memo[key] = {'B', ra, rea, rb, reb};
                } else if (outcome == 'T') {
                    best_outcome = 'T';
                    best_mask_a = ra;
                    best_e_a = rea;
                    best_mask_b = rb;
                    best_e_b = reb;
                }
            }
        }
        
        if (best_outcome == 'A') {
            // All options lead to Alex winning
            return round_memo[key] = {'A', best_mask_a, best_e_a, best_mask_b, best_e_b};
        } else if (best_outcome == 'T') {
            return round_memo[key] = {'T', best_mask_a, best_e_a, best_mask_b, best_e_b};
        } else {
            // Default to best case
            return round_memo[key] = {best_outcome, best_mask_a, best_e_a, best_mask_b, best_e_b};
        }
    }
}

// Memoization for the main DFS function
map<tuple<int, int, int, int, int>, pair<int, int>> dfs_memo;

// Main recursive function to compute the number of wins for each player in remaining rounds
pair<int, int> dfs(int mask_a, int e_a, int mask_b, int e_b, int remaining_r) {
    if (remaining_r == 0) return {0, 0};
    
    tuple<int, int, int, int, int> key = {mask_a, e_a, mask_b, e_b, remaining_r};
    if (dfs_memo.count(key)) return dfs_memo[key];
    
    // Play one round and get outcome
    char outcome;
    int new_mask_a, new_e_a, new_mask_b, new_e_b;
    tie(outcome, new_mask_a, new_e_a, new_mask_b, new_e_b) = simulate_round_step(
        true, 0, mask_a, e_a, mask_b, e_b);
    
    // Recursive call for remaining rounds
    auto [next_a, next_b] = dfs(new_mask_a, new_e_a, new_mask_b, new_e_b, remaining_r - 1);
    
    int curr_a = 0, curr_b = 0;
    if (outcome == 'A') curr_a++;
    else if (outcome == 'B') curr_b++;
    // else 'T' adds 0
    
    pair<int, int> result = {curr_a + next_a, curr_b + next_b};
    return dfs_memo[key] = result;
}

int main() {
    precompute_factorials(20, MOD);
    
    int t;
    cin >> t;
    
    while (t--) {
        int n, E, r;
        cin >> n >> E >> r;
        
        ll alex_wins = 0, boris_wins = 0, ties = 0;
        int total_dists = comb(n, n/2, MOD);
        
        // Iterate all possible masks for Alex (n choose n/2 distributions)
        for (int mask = 0; mask < (1 << n); ++mask) {
            // Check if mask has exactly n/2 bits set
            int cnt = __builtin_popcount(mask);
            if (cnt != n/2) continue;
            
            // Reset memoization tables for each distribution
            round_memo.clear();
            dfs_memo.clear();
            
            // Simulate the game for this distribution
            auto [a, b] = dfs(mask, E, ~mask & ((1 << n) - 1), E, r);
            
            if (a > b) alex_wins = (alex_wins + 1) % MOD;
            else if (b > a) boris_wins = (boris_wins + 1) % MOD;
            else ties = (ties + 1) % MOD;
        }
        
        cout << alex_wins << " " << boris_wins << " " << ties << endl;
    }
    
    return 0;
}