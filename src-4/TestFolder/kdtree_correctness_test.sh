#!/bin/bash

KD=../coord_query_kdtree
DATA="../20000records.tsv"

passed=0
failed=0

echo "Testing correctness: KD-TREE"
echo "============================"
echo

printf "%-5s %-8s %-13s %-13s %-13s %-13s %-6s\n" \
    "Test" "Type" "Expected lon" "Expected lat" "Returned lon" "Returned lat" "Result"

printf "%-5s %-8s %-13s %-13s %-13s %-13s %-6s\n" \
    "----" "----" "------------" "------------" "------------" "------------" "------"


run_test() {
    local test_number="$1"
    local type="$2"
    local query_lon="$3"
    local query_lat="$4"
    local expected_lon="$5"
    local expected_lat="$6"

    output=$(printf "%s %s\n" "$query_lon" "$query_lat" | "$KD" "$DATA")

    result_line=$(printf "%s\n" "$output" | grep '^(' | head -1)

    returned_lon=$(printf "%s\n" "$result_line" |
        sed -E 's/.*\((-?[0-9.]+),(-?[0-9.]+)\)$/\1/')

    returned_lat=$(printf "%s\n" "$result_line" |
        sed -E 's/.*\((-?[0-9.]+),(-?[0-9.]+)\)$/\2/')

    # Small tolerance because the C program prints fewer decimals
    match=$(awk \
        -v elon="$expected_lon" \
        -v elat="$expected_lat" \
        -v rlon="$returned_lon" \
        -v rlat="$returned_lat" '
        BEGIN {
            tolerance = 0.000001

            dlon = elon - rlon
            dlat = elat - rlat

            if (dlon < 0) dlon = -dlon
            if (dlat < 0) dlat = -dlat

            if (dlon <= tolerance && dlat <= tolerance)
                print 1
            else
                print 0
        }')

    if [ "$match" -eq 1 ]; then
        result="PASS"
        passed=$((passed + 1))
    else
        result="FAIL"
        failed=$((failed + 1))
    fi

    printf "%-5s %-8s %-13s %-13s %-13s %-13s %-6s\n" \
        "$test_number" \
        "$type" \
        "$expected_lon" \
        "$expected_lat" \
        "$returned_lon" \
        "$returned_lat" \
        "$result"
}


# ---------------------------------------------------------
# 3 exact-coordinate tests
# ---------------------------------------------------------

run_test 1 "exact" \
    1.8753098 46.7995347 \
    1.8753098 46.7995347

run_test 2 "exact" \
    6.9360077 47.5014236 \
    6.9360077 47.5014236

run_test 3 "exact" \
    3.7892891 47.4323246 \
    3.7892891 47.4323246


# ---------------------------------------------------------
# 2 offset-coordinate tests
#
# Query coordinates are deliberately different from
# the expected nearest record.
# ---------------------------------------------------------

run_test 4 "offset" \
    2.3024263 49.5247802 \
    2.3232587 49.5234873

run_test 5 "offset" \
    -79.9617342 55.8318588 \
    -79.9363141 55.8507461


echo
echo "Correctness summary"
echo "==================="
printf "%-10s %s\n" "Passed:" "$passed"
printf "%-10s %s\n" "Failed:" "$failed"