#!/bin/bash

# Define the output file
output_file="output.txt"

# Clear the output file if it exists
> "$output_file"

# Loop through each matching file and concatenate its contents
for file in P=*.txt; do
    if [[ -f "$file" ]]; then  # Check if it's a file
        cat "$file" >> "$output_file"
    fi
done

echo "Concatenation complete. Output written to $output_file."
