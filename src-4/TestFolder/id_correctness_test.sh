#!/bin/bash

NAIVE=../id_query_naive
INDEXED=../id_query_indexed
BINSORT=../id_query_binsort
DATA="../20000records.tsv"

echo "Testing correctness: ID query"
echo "============================="
echo

printf "%-10s %-12s %-20s %-10s %-10s %-10s\n" \
    "Position" "OSM ID" "Expected" "Naive" "Indexed" "Binsort"

printf "%-10s %-12s %-20s %-10s %-10s %-10s\n" \
    "--------" "------" "--------" "-----" "-------" "-------"

passed=0
failed=0


run_test() {
    local position="$1"
    local osm_id="$2"
    local expected="$3"

    naive_output=$(printf "%s\n" "$osm_id" | "$NAIVE" "$DATA")
    indexed_output=$(printf "%s\n" "$osm_id" | "$INDEXED" "$DATA")
    binsort_output=$(printf "%s\n" "$osm_id" | "$BINSORT" "$DATA")

    if printf "%s\n" "$naive_output" | grep -Fq "$expected"; then
        naive_result="PASS"
    else
        naive_result="FAIL"
    fi

    if printf "%s\n" "$indexed_output" | grep -Fq "$expected"; then
        indexed_result="PASS"
    else
        indexed_result="FAIL"
    fi

    if printf "%s\n" "$binsort_output" | grep -Fq "$expected"; then
        binsort_result="PASS"
    else
        binsort_result="FAIL"
    fi

    if [ "$naive_result" = "PASS" ] &&
       [ "$indexed_result" = "PASS" ] &&
       [ "$binsort_result" = "PASS" ]; then
        passed=$((passed + 1))
    else
        failed=$((failed + 1))
    fi

    printf "%-10s %-12s %-20s %-10s %-10s %-10s\n" \
        "$position" \
        "$osm_id" \
        "$expected" \
        "$naive_result" \
        "$indexed_result" \
        "$binsort_result"
}


# 5 hard-coded records spread through the dataset

run_test 1     2202162  "France"
run_test 5000  1079422  "Sainte-Eusoye"
run_test 10000 122806   "Badevel"
run_test 15000 30113853 "NU"
run_test 19999 3219806  "Pierre-Perthuis"


echo
echo "Correctness summary"
echo "==================="
printf "%-10s %s\n" "Passed:" "$passed"
printf "%-10s %s\n" "Failed:" "$failed"