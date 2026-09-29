#!/bin/bash

KD=../coord_query_kdtree
NAIVE=../coord_query_naive
DATA="../20000records.tsv"

echo "Comparing lookup speed: KD-tree vs naive"
echo "========================================="
echo

# Positions in the dataset, NOT counting the header.
positions=(1 2500 5000 7500 10000 12500 15000 17500 20000)

printf "%-10s %-15s %-15s\n" "Position" "Naive (us)" "KD-tree (us)"
printf "%-10s %-15s %-15s\n" "--------" "----------" "------------"

# Totals used for calculating averages
naive_total=0
kd_total=0
count=0

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

    # Add lookup times to totals
    naive_total=$((naive_total + naive_time))
    kd_total=$((kd_total + kd_time))
    count=$((count + 1))
done

# Calculate averages
naive_average=$(awk "BEGIN {printf \"%.2f\", $naive_total / $count}")
kd_average=$(awk "BEGIN {printf \"%.2f\", $kd_total / $count}")

echo
echo "Average lookup time"
echo "==================="
printf "%-15s %10s us\n" "Naive:" "$naive_average"
printf "%-15s %10s us\n" "KD-tree:" "$kd_average"