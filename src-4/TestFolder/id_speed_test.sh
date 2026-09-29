#!/bin/bash

NAIVE=../id_query_naive
INDEXED=../id_query_indexed
BINSORT=../id_query_binsort
DATA="../20000records.tsv"

echo "Comparing lookup speed: ID query"
echo "================================"
echo

# Positions in the dataset, NOT counting the header
positions=(1 2500 5000 7500 10000 12500 15000 17500 20000)

printf "%-10s %-15s %-15s %-15s\n" \
    "Position" "Naive (us)" "Indexed (us)" "Binsort (us)"

printf "%-10s %-15s %-15s %-15s\n" \
    "--------" "----------" "------------" "------------"

naive_total=0
indexed_total=0
binsort_total=0
count=0

for pos in "${positions[@]}"
do
    # +1 because line 1 is the TSV header
    line=$(sed -n "$((pos + 1))p" "$DATA")

    # OSM ID = column 4
    osm_id=$(printf "%s\n" "$line" | cut -f4)

    # Run all three implementations
    naive_output=$(printf "%s\n" "$osm_id" | "$NAIVE" "$DATA")
    indexed_output=$(printf "%s\n" "$osm_id" | "$INDEXED" "$DATA")
    binsort_output=$(printf "%s\n" "$osm_id" | "$BINSORT" "$DATA")

    # Extract ONLY query time
    naive_time=$(printf "%s\n" "$naive_output" |
        grep "Query time:" |
        awk '{print $3}' |
        sed 's/us//')

    indexed_time=$(printf "%s\n" "$indexed_output" |
        grep "Query time:" |
        awk '{print $3}' |
        sed 's/us//')

    binsort_time=$(printf "%s\n" "$binsort_output" |
        grep "Query time:" |
        awk '{print $3}' |
        sed 's/us//')

    printf "%-10s %-15s %-15s %-15s\n" \
        "$pos" "$naive_time" "$indexed_time" "$binsort_time"

    naive_total=$((naive_total + naive_time))
    indexed_total=$((indexed_total + indexed_time))
    binsort_total=$((binsort_total + binsort_time))

    count=$((count + 1))
done

# Calculate averages
naive_average=$(awk "BEGIN {printf \"%.2f\", $naive_total / $count}")
indexed_average=$(awk "BEGIN {printf \"%.2f\", $indexed_total / $count}")
binsort_average=$(awk "BEGIN {printf \"%.2f\", $binsort_total / $count}")

echo
echo "Average lookup time"
echo "==================="

printf "%-15s %10s us\n" "Naive:" "$naive_average"
printf "%-15s %10s us\n" "Indexed:" "$indexed_average"
printf "%-15s %10s us\n" "Binsort:" "$binsort_average"