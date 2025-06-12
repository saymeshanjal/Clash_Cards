# Card Clash

This project provides a comprehensive test generation and validation system for the **Card Clash** problem, which involves a combinatorial card game simulation with energy constraints and strategic minimax-based decision making.

---

## Project Structure

```
.
card_clash/
├── qwen/                      # Directory containing Qwen's failed solutions
│   ├── solution_01.cpp        # First failed solution
│   ├── solution_02.cpp        # Second failed solution
│   ├── ...
│   ├── solution_16.cpp        # 16th failed solution
│   └── conversations.md       # Links to conversations that generated these solutions
├── solution_correct.cpp       # Correct (optimal) solution implementation
├── solution_bf.cpp           # Brute force solution (if applicable)
├── prompt.md                 # Problem statement and requirements
├── README.md                 # Project documentation
├── test_generator.sh         # Test case generator (generates 40-50 test cases + valid outputs)
├── test_runner.sh            # Script to run tests
└── tests/                    # Directory containing test cases
│   ├── test_sample.in/out                        # Sample test case for basic verification
│   ├── test_01.in/out to test_04.in/out          # Small manual/edge test cases
│   ├── test_big.in/out                           # Large manual test case
│   └── test_gen_01.in/out to test_gen_40.in/out  # Generated test cases
```


##  Usage Instructions

###  1. Generate Tests

```bash
bash test_generator.sh
```

This script:
- Deletes old test files in `tests/input/` and `tests/output/`
- Creates new valid test cases (with varying `n`, `E`, and `r`)
- Runs your reference/brute-force solution to generate expected outputs

---

### 2. Run Solution on Tests

```bash
bash test_runner.sh solution.cpp
```

This:
- Compiles `solution.cpp`
- Runs the binary on each test case from `tests/input/`
- Compares the output to the expected output from `tests/output/`
- Displays `PASS` or `FAIL` for each test case

---

### 3. Validate Input Format

You can manually test if an input file is valid by:

```bash
g++ -o validator validator.cpp
./validator < tests/input/01.txt
```


## Problem Constraints Recap

- `t` test cases
- For each test:
  - Even `n` (2 ≤ n ≤ 30)
  - Energy `E` (0 ≤ E ≤ 500)
  - Rounds `r` (1 ≤ r ≤ 5)
- Total configurations ≤ C(n, n/2)

---

## Test Categories

We include:
- Small cases (n ≤ 6)
- Medium cases (n = 10–16)
- Edge cases (E = 0, E very high)
- Tie scenarios
- All win/loss configurations

---


## Requirements

- g++ ≥ 9.0
- bash
- POSIX-compliant system (Linux, macOS, WSL)
