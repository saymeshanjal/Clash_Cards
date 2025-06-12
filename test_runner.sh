#!/bin/bash

INPUT_DIR="tests"
OUTPUT_DIR="tests"
TMP_OUTPUT="temp_output.txt"
EXEC_NAME="exec_temp"
JSON_LOG="testcase-performance.json"
REQUIREMENTS="requirements.json"
GTIME="/opt/homebrew/bin/gtime"

if [ $# -ne 1 ]; then
    echo "Usage: $0 <source_file.cpp>"
    exit 1
fi

source_file="$1"

if [ ! -f "$REQUIREMENTS" ]; then
    echo "Missing requirements.json file."
    exit 1
fi

# Read limits from JSON
time_limit=$(jq '.time' "$REQUIREMENTS")
space_limit=$(jq '.space' "$REQUIREMENTS")

if [ -z "$time_limit" ] || [ -z "$space_limit" ]; then
    echo "Invalid requirements.json"
    exit 1
fi

# Compile source
compiler="g++"
if ! $compiler -std=c++17 "$source_file" -o "$EXEC_NAME"; then
    echo "Compilation failed."
    exit 1
fi

echo "[" > "$JSON_LOG"
first_entry=true

echo "Running test cases with time=${time_limit}s, space=${space_limit}KB..."

passed=0
failed=0
crashed=0
total=0

for input_file in "$INPUT_DIR"/*.in; do
    test_name=$(basename "$input_file" .in)
    output_file="$OUTPUT_DIR/$test_name.out"

    if [ ! -f "$output_file" ]; then
        echo "$test_name: Missing output file, skipping."
        continue
    fi

    ((total++))

    runtime=0
    mem_kb=null
    verdict=""
    perf_file=$(mktemp)

    $GTIME -f "%U %M" -o "$perf_file" timeout --foreground "$time_limit" ./"$EXEC_NAME" < "$input_file" > "$TMP_OUTPUT" 2>/dev/null
    exit_code=$?

    if [ -s "$perf_file" ]; then
        read runtime mem_kb < "$perf_file"
        # If memory result is 0, treat it as unavailable
        if [ "$mem_kb" -eq 0 ]; then
            mem_kb=null
        fi
    fi
    rm -f "$perf_file"

    if [ $exit_code -eq 124 ]; then
        verdict="TIMEOUT"
        ((failed++))
    elif [ $exit_code -ne 0 ]; then
        verdict="CRASH"
        ((crashed++))
    elif (( $(echo "$runtime > $time_limit" | bc -l) )); then
        verdict="TIMEOUT"
        ((failed++))
    elif [[ "$mem_kb" != "null" && "$mem_kb" -gt "$space_limit" ]]; then
        verdict="MEMORY"
        ((failed++))
    elif diff -q "$TMP_OUTPUT" "$output_file" > /dev/null; then
        verdict="PASS"
        ((passed++))
    else
        verdict="FAIL"
        ((failed++))
    fi

    echo "$test_name: $verdict (time=${runtime}s, mem=${mem_kb}KB)"

    if [ "$first_entry" = true ]; then
        first_entry=false
    else
        echo "," >> "$JSON_LOG"
    fi

    echo "  {\"testcase_id\": \"$test_name\", \"verdict\": \"$verdict\", \"time\": $runtime, \"space\": $mem_kb}" >> "$JSON_LOG"
done

echo "]" >> "$JSON_LOG"
rm -f "$TMP_OUTPUT" "$EXEC_NAME"

echo ""
echo "Test Summary: $passed passed / $failed failed / $crashed crashed / $total total."
echo "Performance log: $JSON_LOG"
