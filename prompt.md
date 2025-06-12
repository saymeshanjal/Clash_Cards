# Card Clash

### Problem Description

Alex and Boris are playing a two-player, turn-based card game. The game is played with `n` unique cards labeled `1` through `n`, where `n` is even. Each player receives exactly `n/2` cards. Each card is assigned to either Alex or Boris, and **all such distinct assignments** are considered.

The game proceeds over **`r` rounds**. In every round, **Alex always plays first**. Cards once played are removed from the game permanently. Each player starts with the same amount of **energy `E`**, and **energy is not replenished** across rounds.



### Round Rules

* The round proceeds with alternate turns, starting with **Alex**.
* The player whose turn it is must:

  * **Play a card** from their remaining hand whose value is **strictly greater** than the last card played **and** whose cost (value) does not exceed their **remaining energy**.
  * **Or freeze**, meaning they cannot make a valid move (due to card strength or insufficient energy), and immediately **lose the round**.
* When a player plays a card:

  * It is discarded permanently.
  * The cost is subtracted from their remaining energy.
* The round continues with players alternating turns until one player freezes.
* If **both players** are unable to make a move when it is their turn, the round ends in a **draw**.


### Game Rules

* Unused cards and remaining energy are **retained** between rounds.
* The player who wins **more rounds** out of `r` is declared the **overall winner**.
* If both win the same number of rounds, the match is a **tie**.


### Optimal Strategy

This is a **perfect information game**:

* Both players know the full state: each other’s cards, remaining energy, and all past moves.
* Both players play **optimally**, attempting to **maximize their round wins** across all rounds.
* This means using **minimax strategy** to simulate and respond to all possible future move sequences — **not just greedy play**.


### Input Format

```
t
n1 E1 r1
n2 E2 r2
...
nt Et rt
```

* `t` — number of test cases (1 ≤ t ≤ 150)
* For each test case:

  * `n` (even, 2 ≤ n ≤ 16) — number of cards in total
  * `E` (0 ≤ E ≤ 500) — starting energy for each player
  * `r` (1 ≤ r ≤ 5) — number of rounds

---

### Output Format

For each test case, print 3 space-separated integers:

```
alex_wins boris_wins ties
```

* Number of card distributions where Alex wins more rounds
* Number where Boris wins more rounds
* Number of ties

Output each value modulo `998244353`.

---

### Example

#### Input

```
2
4 10 1
6 15 2
```

#### Output

```
3 3 0
5 6 9
```

### Sample Input 2:
```
1
4 0 2
```

### Sample Output 2:
```
0 6 0
```


### Sample Input 3:
```
1
6 0 2
```

### Sample Output 3:
```
0 20 0
```


### Sample Input 4:
```
1
4 1 1
```

### Sample Output 4:
```
3 3 0
```


### Notes

* Once a card is used, it’s removed from the game.
* Energy is deducted and **carries over**.
* Card distributions are unique based on assignment of each card to a player.
* The game is **not symmetric** - Alex always starts.
* Players evaluate all move sequences to play optimally.

Give me the solution of the above problem in C++14 code.