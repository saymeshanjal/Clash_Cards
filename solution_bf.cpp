#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <map>

using namespace std;

// Define a struct to represent the complete state of the game at any point.
// This is necessary for the recursive solver and for memoization.
// We make it comparable (`operator<`) so it can be used as a key in std::map.
struct State {
    // Game resource states
    int alex_mask;
    int boris_mask;
    int alex_energy;
    int boris_energy;
    
    // Game progress states
    int alex_wins;
    int boris_wins;
    int last_card_played; // Value of the last card played in the current round
    int turn;             // 0 for Alex, 1 for Boris
    bool prev_player_froze; // Tracks if the opponent froze on their last turn, for draw detection

    // Comparison operator for std::map key
    bool operator<(const State& other) const {
        if (alex_mask != other.alex_mask) return alex_mask < other.alex_mask;
        if (boris_mask != other.boris_mask) return boris_mask < other.boris_mask;
        if (alex_energy != other.alex_energy) return alex_energy < other.alex_energy;
        if (boris_energy != other.boris_energy) return boris_energy < other.boris_energy;
        if (alex_wins != other.alex_wins) return alex_wins < other.alex_wins;
        if (boris_wins != other.boris_wins) return boris_wins < other.boris_wins;
        if (last_card_played != other.last_card_played) return last_card_played < other.last_card_played;
        if (turn != other.turn) return turn < other.turn;
        return prev_player_froze < other.prev_player_froze;
    }
};

// Global variables for the current test case
int N_CARDS, INITIAL_ENERGY, N_ROUNDS;
vector<int> ALEX_INITIAL_HAND, BORIS_INITIAL_HAND;
map<State, int> memo; // Memoization table to store results of solved states

// --- Core Recursive Solver ---
// This function determines the outcome of the game from a given state,
// assuming both players play optimally from that point on.
// It returns:
//  1 if Alex wins the game
//  0 if the game is a tie
// -1 if Boris wins the game
int solve_game_state(const State& s) {
    // --- Base Case: Game Over ---
    // If all rounds are completed, determine the final winner based on current scores.
    if (s.alex_wins + s.boris_wins >= N_ROUNDS) {
        if (s.alex_wins > s.boris_wins) return 1;
        if (s.boris_wins > s.alex_wins) return -1;
        return 0;
    }

    // --- Memoization Check ---
    if (memo.count(s)) {
        return memo[s];
    }

    // --- Recursive Step ---
    // Find all valid card plays for the current player
    vector<int> valid_moves_indices;
    if (s.turn == 0) { // Alex's turn
        for (int i = 0; i < ALEX_INITIAL_HAND.size(); ++i) {
            // Check if Alex has the card (using bitmask), if it's greater than the last, and if he has energy
            if ((s.alex_mask & (1 << i)) && ALEX_INITIAL_HAND[i] > s.last_card_played && ALEX_INITIAL_HAND[i] <= s.alex_energy) {
                valid_moves_indices.push_back(i);
            }
        }
    } else { // Boris's turn
        for (int i = 0; i < BORIS_INITIAL_HAND.size(); ++i) {
            if ((s.boris_mask & (1 << i)) && BORIS_INITIAL_HAND[i] > s.last_card_played && BORIS_INITIAL_HAND[i] <= s.boris_energy) {
                valid_moves_indices.push_back(i);
            }
        }
    }

    int best_outcome;

    // --- Player Logic (Minimax) ---
    if (s.turn == 0) { // Alex's turn: He wants to MAXIMIZE the outcome.
        best_outcome = -2; // Initialize to a value worse than a loss

        // Option 1: Alex plays a card (for each valid move)
        for (int card_idx : valid_moves_indices) {
            int card_val = ALEX_INITIAL_HAND[card_idx];
            State next_state = s;
            next_state.alex_mask &= ~(1 << card_idx); // Remove card from hand
            next_state.alex_energy -= card_val;
            next_state.last_card_played = card_val;
            next_state.turn = 1; // It becomes Boris's turn
            next_state.prev_player_froze = false;
            best_outcome = max(best_outcome, solve_game_state(next_state));
        }

        // Option 2: Alex freezes (this is always an option, forced or strategic)
        {
            // If Boris (the previous player) also just froze, the round is a draw.
            if (s.prev_player_froze) {
                // The round is a draw. Start the next round with no score change.
                State next_round_state = s;
                next_round_state.last_card_played = 0;
                next_round_state.turn = 0; // Alex starts the new round
                next_round_state.prev_player_froze = false; // Reset the freeze flag for the new round
                best_outcome = max(best_outcome, solve_game_state(next_round_state));
            } else {
                // Alex freezes, so Boris wins the round.
                State next_round_state = s;
                next_round_state.boris_wins++;
                next_round_state.last_card_played = 0;
                next_round_state.turn = 0; // Alex starts the new round
                next_round_state.prev_player_froze = true; // Set that Alex just froze
                best_outcome = max(best_outcome, solve_game_state(next_round_state));
            }
        }
    } else { // Boris's turn: He wants to MINIMIZE the outcome.
        best_outcome = 2; // Initialize to a value worse than a win for Alex

        // Option 1: Boris plays a card
        for (int card_idx : valid_moves_indices) {
            int card_val = BORIS_INITIAL_HAND[card_idx];
            State next_state = s;
            next_state.boris_mask &= ~(1 << card_idx);
            next_state.boris_energy -= card_val;
            next_state.last_card_played = card_val;
            next_state.turn = 0; // It becomes Alex's turn
            next_state.prev_player_froze = false;
            best_outcome = min(best_outcome, solve_game_state(next_state));
        }

        // Option 2: Boris freezes
        {
            // If Alex (the previous player) also just froze, the round is a draw.
            if (s.prev_player_froze) {
                State next_round_state = s;
                next_round_state.last_card_played = 0;
                next_round_state.turn = 0;
                next_round_state.prev_player_froze = false;
                best_outcome = min(best_outcome, solve_game_state(next_round_state));
            } else {
                // Boris freezes, so Alex wins the round.
                State next_round_state = s;
                next_round_state.alex_wins++;
                next_round_state.last_card_played = 0;
                next_round_state.turn = 0;
                next_round_state.prev_player_froze = true;
                best_outcome = min(best_outcome, solve_game_state(next_round_state));
            }
        }
    }

    // Store the result in the memoization table and return it.
    return memo[s] = best_outcome;
}

// --- Main Test Case Solver ---
void solve() {
    cin >> N_CARDS >> INITIAL_ENERGY >> N_ROUNDS;

    vector<int> all_cards(N_CARDS);
    iota(all_cards.begin(), all_cards.end(), 1); // Fills vector with 1, 2, ..., n

    long long alex_wins_total = 0;
    long long boris_wins_total = 0;
    long long ties_total = 0;
    const int MOD = 998244353;

    // Generate all combinations of cards for Alex. Boris gets the rest.
    vector<bool> v(N_CARDS);
    fill(v.begin(), v.begin() + N_CARDS / 2, true);

    do {
        ALEX_INITIAL_HAND.clear();
        BORIS_INITIAL_HAND.clear();
        for (int i = 0; i < N_CARDS; ++i) {
            if (v[i]) {
                ALEX_INITIAL_HAND.push_back(all_cards[i]);
            } else {
                BORIS_INITIAL_HAND.push_back(all_cards[i]);
            }
        }

        // For each distribution, clear memoization and run the recursive solver.
        memo.clear();
        State initial_state;
        initial_state.alex_mask = (1 << (N_CARDS / 2)) - 1; // Bitmask for a full hand
        initial_state.boris_mask = (1 << (N_CARDS / 2)) - 1; // Bitmask for a full hand
        initial_state.alex_energy = INITIAL_ENERGY;
        initial_state.boris_energy = INITIAL_ENERGY;
        initial_state.alex_wins = 0;
        initial_state.boris_wins = 0;
        initial_state.last_card_played = 0;
        initial_state.turn = 0; // Alex starts
        initial_state.prev_player_froze = false;

        int result = solve_game_state(initial_state);

        if (result == 1) {
            alex_wins_total++;
        } else if (result == -1) {
            boris_wins_total++;
        } else {
            ties_total++;
        }
    } while (prev_permutation(v.begin(), v.end()));

    cout << alex_wins_total % MOD << " " << boris_wins_total % MOD << " " << ties_total % MOD << "\n";
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    int t;
    cin >> t;
    while (t--) {
        solve();
    }
    return 0;
}
