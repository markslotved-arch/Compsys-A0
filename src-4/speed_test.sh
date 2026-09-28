#!/bin/bash

KD=./coord_query_kdtree
NAIVE=./coord_query_naive
DATA="20000records.tsv"

echo "Comparing lookup speed: KD-tree vs naive"
echo "========================================="
echo

# Positions in the dataset, NOT counting the header.
# First, several points through the middle, and last.
positions=(1 2500 5000 7500 10000 12500 15000 17500 20000)

printf "%-10s %-15s %-15s\n" "Position" "Naive (us)" "KD-tree (us)"
printf "%-10s %-15s %-15s\n" "--------" "----------" "------------"

for pos in "${positions[@]}"
do
    # +1 because line 1 is the TSV header
    line=$(sed -n "$((pos + 1))p" "$DATA")

    # Longitude = column 7
    # Latitude  = column 8
    lon=$(printf "%s\n" "$line" | cut -f7)
    lat=$(printf "%s\n" "$line" | cut -f8)

    # Run naive search
    naive_output=$(printf "%s %s\n" "$lon" "$lat" | "$NAIVE" "$DATA")

    # Run KD-tree search
    kd_output=$(printf "%s %s\n" "$lon" "$lat" | "$KD" "$DATA")

    # Extract ONLY the lookup time
    naive_time=$(printf "%s\n" "$naive_output" |
        grep "Query time:" |
        awk '{print $3}' |
        sed 's/us//')

    kd_time=$(printf "%s\n" "$kd_output" |
        grep "Query time:" |
        awk '{print $3}' |
        sed 's/us//')

    printf "%-10s %-15s %-15s\n" "$pos" "$naive_time" "$kd_time"
done