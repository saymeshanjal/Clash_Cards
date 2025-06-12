#include <bits/stdc++.h>
using namespace std;

const int MOD = 998244353;

// Structure to store the result of a round simulation
struct RoundResult {
    int winner; // 0: Alex wins, 1: Boris wins, 2: tie
    unsigned short alex_mask;
    unsigned short boris_mask;
    int e_a;
    int e_b;
};

// Memoization cache for the simulate_round function
map<tuple<bool, unsigned short, unsigned short, int, int>, RoundResult> memo;

// Utility function to check if a card is present in a mask
bool has_card(unsigned short mask, int card) {
    return (mask >> (card - 1)) & 1;
}

// Simulate a single round of the game
RoundResult simulate_round(bool attacker_is_alex, unsigned short alex_mask, unsigned short boris_mask, int e_a, int e_b) {
    auto key = make_tuple(attacker_is_alex, alex_mask, boris_mask, e_a, e_b);
    if (memo.count(key)) return memo[key];

    // Determine attacker and defender
    unsigned short attacker_mask = attacker_is_alex ? alex_mask : boris_mask;
    unsigned short defender_mask = attacker_is_alex ? boris_mask : alex_mask;
    int attacker_energy = attacker_is_alex ? e_a : e_b;
    int defender_energy = attacker_is_alex ? e_b : e_a;

    // Check if attacker can play any card
    bool can_play = false;
    for (int card = 1; card <= 16; ++card) {
        if (has_card(attacker_mask, card) && card <= attacker_energy) {
            can_play = true;
            break;
        }
    }

    // Base case: attacker cannot play any card
    if (!can_play) {
        RoundResult res;
        res.winner = attacker_is_alex ? 1 : 0;
        res.alex_mask = alex_mask;
        res.boris_mask = boris_mask;
        res.e_a = e_a;
        res.e_b = e_b;
        memo[key] = res;
        return res;
    }

    // Try all possible cards attacker can play
    for (int x = 1; x <= 16; ++x) {
        if (!has_card(attacker_mask, x) || x > attacker_energy)
            continue;

        // Update attacker's mask and energy
        unsigned short new_attacker_mask = attacker_mask & ~(1 << (x - 1));
        int new_attacker_energy = attacker_energy - x;

        // Update global masks based on attacker
        unsigned short new_alex_mask = attacker_is_alex ? new_attacker_mask : alex_mask;
        unsigned short new_boris_mask = attacker_is_alex ? boris_mask : new_attacker_mask;

        // Defender's possible responses
        vector<int> responses;
        for (int y = x + 1; y <= 16; ++y) {
            if (has_card(defender_mask, y) && y <= defender_energy) {
                responses.push_back(y);
            }
        }

        // Defender cannot respond
        if (responses.empty()) {
            RoundResult res;
            res.winner = 0; // Attacker wins
            res.alex_mask = attacker_is_alex ? new_attacker_mask : alex_mask;
            res.boris_mask = attacker_is_alex ? boris_mask : new_attacker_mask;
            res.e_a = attacker_is_alex ? new_attacker_energy : e_a;
            res.e_b = attacker_is_alex ? e_b : defender_energy;
            memo[key] = res;
            return res;
        }

        // Try all possible responses from defender
        bool all_defense_lead_to_attacker_win = true;
        for (int y : responses) {
            // Update defender's mask and energy
            unsigned short new_defender_mask = defender_mask & ~(1 << (y - 1));
            int new_defender_energy = defender_energy - y;

            // Update global masks based on attacker
            unsigned short new_alex_mask = attacker_is_alex ? new_attacker_mask : new_defender_mask;
            unsigned short new_boris_mask = attacker_is_alex ? new_defender_mask : new_attacker_mask;

            // Recursive call
            RoundResult subgame = simulate_round(!attacker_is_alex, new_alex_mask, new_boris_mask, new_attacker_energy, new_defender_energy);

            if (subgame.winner != 0) {
                all_defense_lead_to_attacker_win = false;
                break;
            }
        }

        if (all_defense_lead_to_attacker_win) {
            RoundResult res;
            res.winner = 0; // Attacker wins
            res.alex_mask = attacker_is_alex ? new_attacker_mask : alex_mask;
            res.boris_mask = attacker_is_alex ? boris_mask : new_attacker_mask;
            res.e_a = attacker_is_alex ? new_attacker_energy : e_a;
            res.e_b = attacker_is_alex ? e_b : defender_energy;
            memo[key] = res;
            return res;
        }
    }

    // If no winning move found, check for tie
    bool all_cards_played = true;
    for (int card = 1; card <= 16; ++card) {
        if (has_card(alex_mask, card) || has_card(boris_mask, card)) {
            all_cards_played = false;
            break;
        }
    }

    if (all_cards_played) {
        RoundResult res;
        res.winner = 2; // Tie
        res.alex_mask = alex_mask;
        res.boris_mask = boris_mask;
        res.e_a = e_a;
        res.e_b = e_b;
        memo[key] = res;
        return res;
    }

    // Otherwise, defender wins
    RoundResult res;
    res.winner = 1;
    res.alex_mask = alex_mask;
    res.boris_mask = boris_mask;
    res.e_a = e_a;
    res.e_b = e_b;
    memo[key] = res;
    return res;
}

int main() {
    int t;
    cin >> t;

    for (int _ = 0; _ < t; ++_) {
        int n, E, r;
        cin >> n >> E >> r;

        long long alex_wins = 0, boris_wins = 0, ties = 0;

        // Generate all valid card distributions
        int total_cards = n;
        int alex_cards_count = n / 2;

        vector<int> cards;
        for (int i = 1; i <= n; ++i)
            cards.push_back(i);

        do {
            unsigned short alex_mask = 0;
            unsigned short boris_mask = 0;

            for (int i = 0; i < n; ++i) {
                if (i < alex_cards_count)
                    alex_mask |= 1 << (cards[i] - 1);
                else
                    boris_mask |= 1 << (cards[i] - 1);
            }

            // Simulate r rounds
            int alex_round_wins = 0, boris_round_wins = 0;

            unsigned short curr_alex_mask = alex_mask;
            unsigned short curr_boris_mask = boris_mask;
            int curr_e_a = E;
            int curr_e_b = E;

            for (int round = 0; round < r; ++round) {
                RoundResult res = simulate_round(true, curr_alex_mask, curr_boris_mask, curr_e_a, curr_e_b);

                if (res.winner == 0)
                    alex_round_wins++;
                else if (res.winner == 1)
                    boris_round_wins++;

                curr_alex_mask = res.alex_mask;
                curr_boris_mask = res.boris_mask;
                curr_e_a = res.e_a;
                curr_e_b = res.e_b;
            }

            if (alex_round_wins > boris_round_wins)
                alex_wins = (alex_wins + 1) % MOD;
            else if (boris_round_wins > alex_round_wins)
                boris_wins = (boris_wins + 1) % MOD;
            else
                ties = (ties + 1) % MOD;

        } while (next_permutation(cards.begin(), cards.end()));

        cout << alex_wins << " " << boris_wins << " " << ties << "\n";
    }

    return 0;
}