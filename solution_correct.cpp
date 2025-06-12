#include <bits/stdc++.h>
using namespace std;

const int MOD = 998244353;


/**
 * @brief Simulates r rounds of the game for a fixed card distribution between Alex and Boris.
 *
 * In each round, both players play alternatively starting with Alex. A player plays the smallest
 * possible valid card that is strictly greater than the last played card and within energy budget.
 *
 * The player who cannot play a valid card loses the round. If both fail at the same step, it's a draw.
 *
 * @param alex_hand The initial hand (set of cards) for Alex.
 * @param boris_hand The initial hand (set of cards) for Boris.
 * @param E The initial energy given to both players at the start of each round.
 * @param r The number of rounds to simulate.
 * @param a_wins Reference to the variable to store Alex's round wins.
 * @param b_wins Reference to the variable to store Boris's round wins.
 * @param ties Reference to the variable to store number of ties.
 */
void simulateGame(const set<int>& alex_hand, const set<int>& boris_hand, int E, int r, int& a_wins, int& b_wins, int& ties) {
    int a_energy = E, b_energy = E;
    auto a_cards = alex_hand;
    auto b_cards = boris_hand;
    a_wins = 0, b_wins = 0, ties = 0;

    for (int round = 0; round < r; ++round) {
        int last_played = 0;
        int current_player = 0; // 0: Alex, 1: Boris
        bool game_over = false;
        int winner = -1; // -1: draw, 0: Alex, 1: Boris

        while (!game_over) {
            if (current_player == 0) {
                // Alex's turn: play smallest possible card to conserve energy
                int best_card = -1;
                for (int card : a_cards) {
                    if (card > last_played && card <= a_energy) {
                        if (best_card == -1 || card < best_card) {
                            best_card = card;
                        }
                    }
                }
                if (best_card != -1) {
                    a_cards.erase(best_card);
                    a_energy -= best_card;
                    last_played = best_card;
                } else {
                    if (winner == -1) winner = 1;
                    else winner = -1;
                    game_over = true;
                }
            } else {
                // Boris's turn: play smallest possible card to conserve energy
                int best_card = -1;
                for (int card : b_cards) {
                    if (card > last_played && card <= b_energy) {
                        if (best_card == -1 || card < best_card) {
                            best_card = card;
                        }
                    }
                }
                if (best_card != -1) {
                    b_cards.erase(best_card);
                    b_energy -= best_card;
                    last_played = best_card;
                } else {
                    if (winner == -1) winner = 0;
                    else winner = -1;
                    game_over = true;
                }
            }
            if (!game_over) {
                current_player = 1 - current_player;
            }
        }

        if (winner == 0) a_wins++;
        else if (winner == 1) b_wins++;
        else ties++;
    }
}

/**
 * @brief Handles one or more test cases: reads input, tries all possible card distributions,
 * and computes how many of them result in Alex winning, Boris winning, or a tie.
 */

void solve() {
    int t;
    cin >> t;
    while (t--) {
        int n, E, r;
        cin >> n >> E >> r;

        vector<int> cards(n);
        for (int i = 0; i < n; ++i) {
            cards[i] = i + 1;
        }

        int alex_wins = 0, boris_wins = 0, ties = 0;

        vector<bool> mask(n, false);
        fill(mask.begin(), mask.begin() + n/2, true);
        do {
            set<int> alex_hand, boris_hand;
            for (int i = 0; i < n; ++i) {
                if (mask[i]) {
                    alex_hand.insert(cards[i]);
                } else {
                    boris_hand.insert(cards[i]);
                }
            }

            int a_wins = 0, b_wins = 0, round_ties = 0;
            simulateGame(alex_hand, boris_hand, E, r, a_wins, b_wins, round_ties);

            if (a_wins > b_wins) alex_wins++;
            else if (b_wins > a_wins) boris_wins++;
            else ties++;
        } while (prev_permutation(mask.begin(), mask.end()));

        cout << alex_wins << " " << boris_wins << " " << ties << '\n';
    }
}
/**
 * @brief Entry point of the program. Initializes IO and runs the solver.
 */

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    solve();
    return 0;
}
