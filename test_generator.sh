#!/bin/bash

# Number of test cases to generate
NUM_TESTS=10

# File names
GENERATOR=generator
SOLUTION=solution
INPUT_DIR=tests/input
OUTPUT_DIR=tests/output

# Cleanup
echo "[INFO] Cleaning up old test cases..."
rm -rf tests
mkdir -p $INPUT_DIR
mkdir -p $OUTPUT_DIR

# Compile the solution
echo "[INFO] Compiling solution..."
g++ -O2 -std=c++17 solution.cpp -o $SOLUTION || { echo "[ERROR] Compilation failed"; exit 1; }

# Optional: Compile generator
if [[ -f generator.cpp ]]; then
    echo "[INFO] Compiling generator..."
    g++ -O2 -std=c++17 generator.cpp -o $GENERATOR || { echo "[ERROR] Generator compilation failed"; exit 1; }
fi

# Generate test cases and outputs
echo "[INFO] Generating test cases..."
for ((i = 1; i <= NUM_TESTS; ++i)); do
    INPUT_FILE=$INPUT_DIR/input$i.txt
    OUTPUT_FILE=$OUTPUT_DIR/output$i.txt

    if [[ -f $GENERATOR ]]; then
        ./$GENERATOR > $INPUT_FILE
    else
        # Fallback: simple random generator inline
        T=$((RANDOM % 5 + 1))
        echo $T > $INPUT_FILE
        for ((j = 0; j < T; ++j)); do
            N=$((2 * (RANDOM % 8 + 1)))  # even n <= 16
            E=$((RANDOM % 501))
            R=$((RANDOM % 5 + 1))
            echo "$N $E $R" >> $INPUT_FILE
        done
    fi

    # Run the solution
    echo "[INFO] Generating output for test $i"
    ./$SOLUTION < $INPUT_FILE > $OUTPUT_FILE
done

echo "[DONE] Generated $NUM_TESTS test cases in '$INPUT_DIR' and '$OUTPUT_DIR'."
